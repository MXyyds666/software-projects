/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file dht11.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dht11.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/* Creating task handle */
static TaskHandle_t DHT11TaskHandle = NULL;

DHT11DataType sDHT11Data;

extern uint32_t SystemCoreClock;
extern QueueHandle_t MQTTDataQueueHandle;

static void dht11_thread(void);


/**
*\*\name    DWT_DelayUs.
*\*\fun     DWT delay in us.
*\*\param   us
*\*\         - Number of microseconds
*\*\return  none
**/
void DWT_DelayUs(uint32_t us)
{
    uint32_t tempStartVal;
    uint32_t tempDelayVal;
    uint32_t tempNowVal;
    uint32_t tempIntervalVal;

    tempStartVal    = DWT_CYCCNT;
    tempDelayVal    = (SystemCoreClock / 1000000) * us;
    tempIntervalVal = 0;
    
    while (tempIntervalVal < tempDelayVal)
    {
        tempNowVal = DWT_CYCCNT;
        if (tempNowVal > tempStartVal)
        {
            tempIntervalVal = tempNowVal - tempStartVal;
        }
        else
        {
            tempIntervalVal = 0xFFFFFFFF - tempStartVal + tempNowVal;
        }
    }
}

/**
*\*\name    DWT_DelayMs.
*\*\fun     DWT delay in ms.
*\*\param   ms
*\*\         - Number of milliseconds
*\*\return  none
**/
void DWT_DelayMs(uint32_t ms)
{
    DWT_DelayUs(1000 * ms);
}

/**
*\*\name    DWT_DelayDisable.
*\*\fun     Disable DWT function.
*\*\param   none
*\*\return  none
**/
void DWT_DelayDisable(void)
{
    CPU_DELAY_DISABLE();
}

/**
*\*\name    DWT_DelayEnable.
*\*\fun     Enable DWT function.
*\*\param   none
*\*\return  none
**/
void DWT_DelayEnable(void)
{
    CPU_DELAY_INTI();
}

/**
*\*\name    DHT11_GPIO_ClockConfig.
*\*\fun     Configure the data signal pin clock.
*\*\param   GPIOx
*\*\         - GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOF/GPIOG/GPIOH
*\*\return  none
**/
void DHT11_GPIO_ClockConfig(GPIO_Module *GPIOx)
{
    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    }
    else if (GPIOx == GPIOD)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    }
    else if (GPIOx == GPIOE)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
    }
    else if (GPIOx == GPIOF)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
    }
    else if (GPIOx == GPIOG)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
    }
    else if (GPIOx == GPIOH)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
    }
    else
    {
        /* Do nothing */
    }
}

/**
*\*\name    DHT11_GPIO_INPUT.
*\*\fun     Set the data signal pin to input mode.
*\*\param   none
*\*\return  none
**/
void DHT11_GPIO_INPUT(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Current   = GPIO_DS_2mA;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(DHT11_PORT, &GPIO_InitStructure);
}

/**
*\*\name    DHT11_GPIO_OUTPUT.
*\*\fun     Set the data signal pin to output push pull mode.
*\*\param   none
*\*\return  none
**/
void DHT11_GPIO_OUTPUT(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current   = GPIO_DS_2mA;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(DHT11_PORT, &GPIO_InitStructure);
}

/**
*\*\name    DHT11_Start.
*\*\fun     Send the DHT11 start signal and detect the response signal.
*\*\param   none
*\*\return  SUCCESS or ERROR
**/
ErrorStatus DHT11_Start(void)
{
    uint32_t timeout = 0;
    
    /* Pull down bus at least 18ms */
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    DWT_DelayMs(20);
    
    /* Set to input mode after 30us of pull-up */
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    DWT_DelayUs(30);
    
    /* Set to input mode */
    DHT11_GPIO_INPUT();
    
    /* Determines whether the slave has a low-level response signa */
    if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
    {
        return ERROR;
    }
    
    /* Previous DHT11 responds low and waits for DHT11 to pull the bus high (80us) */
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
    {
        timeout++;
        /* ACK timeout */
        if (timeout > 1000)
        {
            return ERROR;
        }
        DWT_DelayUs(1);
    }
    
    timeout = 0;
    /* Previous DHT11 pulled the bus high and waits for DHT11 to pull the bus low (80us) */
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
    {
        timeout++;
        /* ACK timeout */
        if (timeout > 1000)
        {
            return ERROR;
        }
        DWT_DelayUs(1);
    }
    
    return SUCCESS;
}

/**
*\*\name    DHT11_ReadByte.
*\*\fun     Reads data, one byte in length.
*\*\param   none
*\*\return  Returns 0 on timeout, returns valid data normally
**/
uint8_t DHT11_ReadByte(void)
{
    uint8_t i;
    uint8_t temp;
    uint32_t timeout = 0;
    
    for (i = 0; i < 8; i++)
    {
        /* Each bit begin with 54us low level,wait low level finish */
        while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
        {
            timeout++;
            /* ACK timeout */
            if (timeout > 1000)
            {
                return 0;
            }
            DWT_DelayUs(1);
        }
        
        /*  bit=0:High level is 23~27us
            bit=1:High level is 68~74us */
        /* Delay time must be between 27us and 68us*/
        DWT_DelayUs(40);
        
        /* bit=1 */
        if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
        {
            timeout = 0;
            while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
            {
                timeout++;
                /* ACK timeout */
                if (timeout > 1000)
                {
                    return 0;
                }
                DWT_DelayUs(1);
            }
            /* Get bit 1, MSB first*/
            temp |= (uint8_t)(0x01U << (7 - i));
        }
        /* bit=0 */
        else
        {
            /* Get bit 0, MSB first*/
            temp &= (uint8_t)(~(0x01U << (7 - i)));
        }
    }
    
    return temp;
}

/**
*\*\name    DHT11_Read.
*\*\fun     Reads DHT11 sensor data.
*\*\param   pData
*\*\         - Pointer to data of type DHT11DataType
*\*\return  SUCCESS or ERROR
**/
ErrorStatus DHT11_Read(DHT11DataType *pData)
{
    uint8_t sum;
    ErrorStatus es = ERROR;
    
    if (DHT11_Start() == SUCCESS)
    {
        /* Start reading data */
        pData->humi_high8bit = DHT11_ReadByte();
        pData->humi_low8bit  = DHT11_ReadByte();
        pData->temp_high8bit = DHT11_ReadByte();
        pData->temp_low8bit  = DHT11_ReadByte();
        pData->check_sum     = DHT11_ReadByte();
        
        /* Save actual data */
        pData->humidity    = (double)((pData->humi_high8bit * 10) + pData->humi_low8bit) / 10;
        pData->temperature = (double)((pData->temp_high8bit * 10) + pData->temp_low8bit) / 10;
        
        sum = pData->humi_high8bit + pData->humi_low8bit + pData->temp_high8bit + pData->temp_low8bit;
        
        /* Check that the read data is correct */
        if (pData->check_sum == sum)
        {
            es = SUCCESS;
        }
    }
    
    return es;
}

/**
*\*\name    dht11_thread.
*\*\fun     DHT11 task entry function.
*\*\param   none
*\*\return  none
**/
static void dht11_thread(void)
{
    ErrorStatus es = ERROR;
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    DHT11_GPIO_ClockConfig(DHT11_PORT);
    
    while (1)
    {
        /* Set to output mode */
        DHT11_GPIO_OUTPUT();
        
        /* Pull up first */
        GPIO_SetBits(DHT11_PORT, DHT11_PIN);
        
        memset(&sDHT11Data, 0, sizeof(sDHT11Data));
        
        DWT_DelayEnable();
        
        /* Read DHT11 data, send only if data is correct */
        es = DHT11_Read(&sDHT11Data);
        if (es != ERROR)
        {
            printf("Temperature: %.1f\nHumidity: %.1f\n", sDHT11Data.temperature, sDHT11Data.humidity);
            
            xQueueSend(MQTTDataQueueHandle, /* Message Queue Handle */
                       &sDHT11Data,         /* Content of messages sent */
                       0);                  /* Wait time in ms */
        }
        
        DWT_DelayDisable();
        
        /* Update data every 2 seconds */
        vTaskDelay(2000);
    }
}

/**
*\*\name    dht11_init.
*\*\fun     Initialize DHT11 by creating DHT11 task.
*\*\param   none
*\*\return  none
**/
void dht11_init(void)
{
    BaseType_t xReturn = pdFAIL;
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Creates a DHT11 Task */
    xReturn = xTaskCreate( /* Task entry function */
                          (TaskFunction_t)dht11_thread,
                           /* Task name */
                          (const char *)"dht11_thread",
                           /* Task stack size */
                          (uint16_t)DHT11_TASK_STACK_SIZE,
                           /* Task entry function parameter */
                          (void *)NULL,
                           /* Task priority */
                          (UBaseType_t)DHT11_TASK_PRIORITY,
                           /* Task handle pointer */
                          (TaskHandle_t *)&DHT11TaskHandle);
    if (xReturn == pdFAIL)
    {
        printf("Failure to create MQTT DHT11 task!\n");
        while (1)
        {
            /* System can switch to other tasks */
            vTaskDelay(5);
        }
    }
}

