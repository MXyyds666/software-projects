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
#define CAN_ID_FLASH_ADDR  0x0801F800  // 最后一页的起始地址

// 读取 CAN ID 的函数
uint32_t Read_CAN_ID(void) {
    return *(volatile uint32_t*)CAN_ID_FLASH_ADDR;
}

#define CAN_ID_FLASH_ADDR  0x0801F800  // 最后一页的起始地址

// 读取 CAN ID 的函数
uint32_t Read_ID(void) {
    return *(volatile uint32_t*)CAN_ID_FLASH_ADDR;
}

void Save_CAN_ID(uint32_t can_id) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    // 1. 解锁 Flash
    HAL_FLASH_Unlock();

    // 2. 擦除最后一页 (第63页)
    // 注意：擦除是以“页”为单位的，这一页的其他数据也会被清空
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = FLASH_BANK_1;
    EraseInitStruct.Page        = 63;      // 最后一页的索引
    EraseInitStruct.NbPages     = 1;       // 只擦除1页

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        // 擦除失败处理...
        HAL_FLASH_Lock();
        return;
    }

    // 3. 写入数据 (G4 必须写入 64 位)
    // 我们把 32 位的 CAN ID 拼成一个 64 位数据，高 32 位补 0 或放其他标志
    uint64_t data_to_write = (uint64_t)can_id; 
    
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, CAN_ID_FLASH_ADDR, data_to_write) != HAL_OK) {
        // 写入失败处理...
    }

    // 4. 上锁
    HAL_FLASH_Lock();
}

void ID_Check(void)
{
	if(!ID_FLAG)
		return;
	uint8_t data[5] = {0};
	
	data[0] =0xA5;
	data[1] =0xA5;
	
	switch(ID_CMD)
	{
		case 0:
			SEND_ID = Read_CAN_ID();
			break;
		case 1:
			Save_CAN_ID(ID_TEMP);
			break;
		default:
			break;
	}
	
	data[2] = ID_CMD;
	
	
	SEND_ID = Read_CAN_ID();
	
	data[3] =(SEND_ID >> 8) & 0xFF;
	data[4] =SEND_ID & 0xFF;
	
	CDC_Transmit_FS(data, 5);
	ID_FLAG = 0;
	if(ID_CMD == 1 || ID_CMD == 3)
		NVIC_SystemReset();
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
  /* USER CODE BEGIN 2 */
	FDCAN_ConfigFilters();
	
	USR_FIFO_INIT();
	
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
