/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "fifo.h"
#include "can.h"
#include "usb.h"
#include "protocol.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t ID_FLAG = 0;
uint16_t ID_TEMP = 0;
uint16_t SEND_ID = 0xF81;
uint8_t ID_CMD = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE BEGIN EXPORTED_VARIABLES */
#define ID_CMD_READ        0x00U
#define ID_CMD_WRITE       0x01U
#define ID_CMD_WRITE_FAIL  0x81U
#define CAN_ID_DEFAULT     0x0781U
#define CAN_ID_MIN         0x0001U
#define CAN_ID_MAX         0x07FFU
#define CAN_ID_FLASH_PAGE_SIZE  0x800U
#define CAN_ID_FLASH_MAGIC      0xA55AU

static uint32_t Get_CAN_ID_StorageAddress(void)
{
  uint32_t flash_size_kb;

  flash_size_kb = (uint32_t)(*(__IO uint16_t *)FLASHSIZE_BASE);
  return FLASH_BASE + (flash_size_kb * 1024U) - CAN_ID_FLASH_PAGE_SIZE;
}

static uint64_t Build_CAN_ID_StorageWord(uint16_t can_id)
{
  uint64_t value;

  value = (uint64_t)CAN_ID_FLASH_MAGIC;
  value |= ((uint64_t)can_id << 16);
  value |= ((uint64_t)((uint16_t)(can_id ^ 0xFFFFU)) << 32);
  value |= ((uint64_t)0xFFFFU << 48);
  return value;
}

uint32_t Read_CAN_ID(void);

uint32_t Read_ID(void) {
  return Read_CAN_ID();
}

uint32_t Read_CAN_ID(void) {
  uint32_t storage_addr;
  uint64_t raw;
  uint16_t magic;
  uint16_t value;
  uint16_t inverse;

  storage_addr = Get_CAN_ID_StorageAddress();
  raw = *(uint64_t *)storage_addr;

  magic = (uint16_t)(raw & 0xFFFFU);
  value = (uint16_t)((raw >> 16) & 0xFFFFU);
  inverse = (uint16_t)((raw >> 32) & 0xFFFFU);

  if ((magic != CAN_ID_FLASH_MAGIC) || ((uint16_t)(value ^ inverse) != 0xFFFFU)) {
    return 0xFFFFFFFFU;
  }

  return value;
}

static uint16_t Normalize_CAN_ID(uint32_t can_id)
{
  if ((can_id < CAN_ID_MIN) || (can_id > CAN_ID_MAX)) {
    return CAN_ID_DEFAULT;
  }

  return (uint16_t)can_id;
}

HAL_StatusTypeDef Save_CAN_ID(uint32_t can_id) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t storage_addr;
    uint32_t page_index;
    uint64_t data_to_write;

    storage_addr = Get_CAN_ID_StorageAddress();
    page_index = (storage_addr - FLASH_BASE) / CAN_ID_FLASH_PAGE_SIZE;

    // 1. 解锁 Flash
    if (HAL_FLASH_Unlock() != HAL_OK) {
        return HAL_ERROR;
    }

    // 2. 擦除最后一页 (第63页)
    // 注意：擦除是以“页”为单位的，这一页的其他数据也会被清空
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = FLASH_BANK_1;
    EraseInitStruct.Page        = page_index;
    EraseInitStruct.NbPages     = 1;       // 只擦除1页

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        status = HAL_ERROR;
    }

    // 3. 写入数据 (G4 必须写入 64 位)
    data_to_write = Build_CAN_ID_StorageWord((uint16_t)can_id);
    
    if ((status == HAL_OK) && (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, storage_addr, data_to_write) != HAL_OK)) {
        status = HAL_ERROR;
    }

    // 4. 上锁
    HAL_FLASH_Lock();
    return status;
}

void ID_Check(void)
{
  uint8_t data[5] = {0};
  uint16_t current_id;
  uint16_t original_id;
  uint8_t response_cmd;

  if (!ID_FLAG)
    return;

  original_id = Normalize_CAN_ID(Read_CAN_ID());
  current_id = original_id;
  response_cmd = ID_CMD;

  data[0] = 0xA5;
  data[1] = 0xA5;

  switch (ID_CMD)
  {
    case ID_CMD_READ:
      response_cmd = ID_CMD_READ;
      break;

    case ID_CMD_WRITE:
      if ((ID_TEMP < CAN_ID_MIN) || (ID_TEMP > CAN_ID_MAX)) {
        response_cmd = ID_CMD_WRITE_FAIL;
        current_id = original_id;
        break;
      }

      if (Save_CAN_ID(ID_TEMP) != HAL_OK) {
        response_cmd = ID_CMD_WRITE_FAIL;
        current_id = original_id;
        break;
      }

      current_id = Normalize_CAN_ID(Read_CAN_ID());
      if (current_id != ID_TEMP) {
        response_cmd = ID_CMD_WRITE_FAIL;
        current_id = original_id;
        break;
      }

      response_cmd = ID_CMD_WRITE;
      break;

    default:
      response_cmd = ID_CMD_WRITE_FAIL;
      current_id = original_id;
      break;
  }

  SEND_ID = current_id;
  data[2] = response_cmd;
  data[3] = (uint8_t)((current_id >> 8) & 0xFF);
  data[4] = (uint8_t)(current_id & 0xFF);

  CDC_Transmit_FS(data, 5);
  ID_FLAG = 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_Device_Init();
  MX_FDCAN1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	FDCAN_ConfigFilters();
	
	USR_FIFO_INIT();
	
	HAL_TIM_Base_Start_IT(&htim2);
	
	SEND_ID = Read_CAN_ID();
	if (SEND_ID == 0xFFFF) {
			// 说明没存过，使用默认 ID
			SEND_ID = 0x781;
			Save_CAN_ID(SEND_ID);
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		USB_Pure_Loopback_Test();
		// 1. 负责把 USB 发给电脑
		
//		//上位机发送数据回环测试
//		USB_Protocol_Parse_Loopback();
		
		ID_Check();
		
    /* 1. 协议解析层：从 USB FIFO 识别并提取 65 字节合法包 */
    USB_Protocol_Parse_And_Bridge();
    
    /* 2. 逻辑网桥层：控制 CAN 的一发一收节奏 */
    CAN_Bridge_Sequence_Process();
		
    /* 3. USB 驱动桥：将需要发给 PC 的数据真正通过硬件发出去 */
    USB_CDC_TX_Bridge(); 
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t cnt = 0;
	if (htim->Instance == TIM2)
	{
		if(++cnt == 20)
		{
			cnt = 0;
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
