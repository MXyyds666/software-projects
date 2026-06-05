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
*\*\file usb_cdc_vcp.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifdef USB_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif                          /* USB_INTERNAL_DMA_ENABLED */


#include "usbd_cdc_vcp.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "misc.h"

LINE_CODING linecoding = 
{
    115200,                       /* baud rate */
    0x00,                         /* stop bits-1 */
    0x00,                         /* parity - none */
    0x08                          /* nb. of bits 8 */
};


USART_InitType USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN
 * transfer management. */
extern uint8_t APP_Rx_Buffer[]; /* Write CDC received data in this buffer.
                                 * These data will be sent over USB IN endpoint
                                 * in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;  /* Increment this pointer or roll it back to
                                 * start address when writing received data in
                                 * the buffer APP_Rx_Buffer. */

/* Private function prototypes ----------------------------------------------- */
static uint16_t VCP_Init(void);
static uint16_t VCP_DeInit(void);
static uint16_t VCP_Ctrl(uint32_t Cmd, uint8_t * Buf, uint32_t Len);
static uint16_t VCP_DataTx(void);
static uint16_t VCP_DataRx(uint8_t * Buf, uint32_t Len);

static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops = 
{
    VCP_Init,
    VCP_DeInit,
    VCP_Ctrl,
    VCP_DataTx,
    VCP_DataRx
};

/**
*\*\name    CDC_COMM_GPIO_Configuration.
*\*\fun     Configures the CDC GPIO ports.
*\*\param   none
*\*\return  none
**/
void CDC_COMM_GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(CDC_COMM_GPIO_CLK, ENABLE);
    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);
    
    /* Configure CDC COMM Tx as alternate function */
    GPIO_InitStructure.Pin            = CDC_COMM_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = CDC_COMM_Tx_GPIO_AF;
    GPIO_InitPeripheral(CDC_COMM_GPIO, &GPIO_InitStructure);

    /* Configure CDC COMM Rx as alternate function */
    GPIO_InitStructure.Pin            = CDC_COMM_RxPin;
    GPIO_InitStructure.GPIO_Alternate = CDC_COMM_Rx_GPIO_AF;
    GPIO_InitPeripheral(CDC_COMM_GPIO, &GPIO_InitStructure);
}

/**
*\*\name    VCP_Init.
*\*\fun     Initializes the configuration of usart port.
*\*\param   none
*\*\return  USBD_OK
**/
static uint16_t VCP_Init(void)
{
    NVIC_InitType NVIC_InitStructure;
    
    /* Enable CDC_COMM Clock */
    CDC_COMM_Periphclk(CDC_COMM_CLK, ENABLE);
    
    RCC_EnableAPB1PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* Configure CDC GPIO  */
    CDC_COMM_GPIO_Configuration();

    /* EVAL_COM1 default configuration */
    /* EVAL_COM1 configured as follow: - BaudRate = 115200 baud - Word Length = 8 
    * Bits - One Stop Bit - Parity Odd - Hardware flow control disabled -
    * Receive and transmit enabled */
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure and enable the USART */
    USART_Init(CDC_COMM, &USART_InitStructure);

    /* Enable the USART Receive interrupt */
    USART_ConfigInt(CDC_COMM, USART_INT_RXDNE, ENABLE);
    
    USART_Enable(CDC_COMM, ENABLE);

    /* Enable USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = CDC_COMM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return USBD_OK;
}

/**
*\*\name    VCP_DeInit.
*\*\fun     DeInitializes the different GPIO ports.
*\*\param   none
*\*\return  USBD_OK
**/
static uint16_t VCP_DeInit(void)
{
    return USBD_OK;
}

/**
*\*\name    VCP_Ctrl.
*\*\fun     Manage the CDC class requests.
*\*\param   Cmd: Command code
*\*\param   Buf: data to be sent or received
*\*\param   Len: data length in bytes
*\*\return  USBD_OK
**/
static uint16_t VCP_Ctrl(uint32_t Cmd, uint8_t * Buf, uint32_t Len)
{
    switch (Cmd)
    {
        case SEND_ENCAPSULATED_COMMAND:
            /* Not needed for this driver */
            break;

        case GET_ENCAPSULATED_RESPONSE:
            /* Not needed for this driver */
            break;

        case SET_COMM_FEATURE:
            /* Not needed for this driver */
            break;

        case GET_COMM_FEATURE:
            /* Not needed for this driver */
            break;

        case CLEAR_COMM_FEATURE:
            /* Not needed for this driver */
            break;

        case SET_LINE_CODING:
            linecoding.bitrate    = (uint32_t) (Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
            linecoding.format     = Buf[4];
            linecoding.paritytype = Buf[5];
            linecoding.datatype   = Buf[6];
            /* Set the new configuration */
            VCP_COMConfig(OTHER_CONFIG);
            break;

        case GET_LINE_CODING:
            Buf[0] = (uint8_t) (linecoding.bitrate);
            Buf[1] = (uint8_t) (linecoding.bitrate >> 8);
            Buf[2] = (uint8_t) (linecoding.bitrate >> 16);
            Buf[3] = (uint8_t) (linecoding.bitrate >> 24);
            Buf[4] = linecoding.format;
            Buf[5] = linecoding.paritytype;
            Buf[6] = linecoding.datatype;
            break;

        case SET_CONTROL_LINE_STATE:
            /* Not needed for this driver */
            break;

        case SEND_BREAK:
            /* Not needed for this driver */
            break;

        default:
            break;
    }

    return USBD_OK;
}

/**
*\*\name    VCP_DataTx.
*\*\fun     CDC received data to be send over USB IN endpoint are managed in this function.
*\*\param   none
*\*\return  USBD_OK
**/
static uint16_t VCP_DataTx(void)
{
    if (linecoding.datatype == 7)
    {
        APP_Rx_Buffer[APP_Rx_ptr_in] = USART_ReceiveData(CDC_COMM) & 0x7F;
    }
    else if (linecoding.datatype == 8)
    {
        APP_Rx_Buffer[APP_Rx_ptr_in] = USART_ReceiveData(CDC_COMM);
    }

    APP_Rx_ptr_in++;

    /* To avoid buffer overflow */
    if (APP_Rx_ptr_in == APP_RX_DATA_SIZE)
    {
        APP_Rx_ptr_in = 0;
    }

    return USBD_OK;
}

/**
*\*\name    VCP_DataRx.
*\*\fun     Data received from USB are sent to uart port.
*\*\param   Buf: buffer to be sent to uart port
*\*\param   Len: data length in bytes
*\*\return  USBD_OK
**/
static uint16_t VCP_DataRx(uint8_t * Buf, uint32_t Len)
{
    uint32_t i;

    for (i = 0; i < Len; i++)
    {
        USART_SendData(CDC_COMM, *(Buf + i));
        while (USART_GetFlagStatus(CDC_COMM, USART_FLAG_TXDE) == RESET);
    }

    return USBD_OK;
}

/**
*\*\name    VCP_COMConfig.
*\*\fun     Configure the COM Port with default values or values received from host.
*\*\param   Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG to set a configuration received from the host.
*\*\return  USBD_OK
**/
static uint16_t VCP_COMConfig(uint8_t Conf)
{
    if (Conf == DEFAULT_CONFIG)
    {
        USART_InitStructure.BaudRate            = 115200;
        USART_InitStructure.WordLength          = USART_WL_8B;
        USART_InitStructure.StopBits            = USART_STPB_1;
        USART_InitStructure.Parity              = USART_PE_NO;
        USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
        USART_InitStructure.OverSampling        = USART_16OVER;
        USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

        /* Configure and enable the USART */
        USART_Init(CDC_COMM, &USART_InitStructure);

        /* Enable the USART Receive interrupt */
        USART_ConfigInt(CDC_COMM, USART_INT_RXDNE, ENABLE);
    }
    else
    {
        /* set the Stop bit */
        switch (linecoding.format)
        {
            case 0:
                USART_InitStructure.StopBits = USART_STPB_1;
                break;
            case 1:
                USART_InitStructure.StopBits = USART_STPB_1_5;
                break;
            case 2:
                USART_InitStructure.StopBits = USART_STPB_2;
                break;
            default:
                VCP_COMConfig(DEFAULT_CONFIG);
                return (USBD_FAIL);
        }

        /* set the parity bit */
        switch (linecoding.paritytype)
        {
            case 0:
                USART_InitStructure.Parity = USART_PE_NO;
                break;
            case 1:
                USART_InitStructure.Parity = USART_PE_EVEN;
                break;
            case 2:
                USART_InitStructure.Parity = USART_PE_ODD;
                break;
            default:
                VCP_COMConfig(DEFAULT_CONFIG);
                return (USBD_FAIL);
        }

        /* set the data type : only 8bits and 9bits is supported */
        switch (linecoding.datatype)
        {
            case 0x07:
                /* With this configuration a parity (Even or Odd) should be set */
                USART_InitStructure.WordLength = USART_WL_8B;
                break;
            case 0x08:
                if (USART_InitStructure.Parity == USART_PE_NO)
                {
                    USART_InitStructure.WordLength = USART_WL_8B;
                }
                else
                {
                    USART_InitStructure.WordLength = USART_WL_9B;
                }
                break;
                
            default:
                VCP_COMConfig(DEFAULT_CONFIG);
                return (USBD_FAIL);
        }

        USART_InitStructure.BaudRate = linecoding.bitrate;
        USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
        USART_InitStructure.Mode = USART_MODE_RX | USART_MODE_TX;

        CDC_COMM_GPIO_Configuration();
        
        /* Configure and enable the USART */
        USART_Init(CDC_COMM, &USART_InitStructure);

        USART_Enable(CDC_COMM, ENABLE);
    }
    return USBD_OK;
}

/**
*\*\name    CDC_COMM_IRQHandler.
*\*\fun     This function handles CDC COMM exception.
*\*\param   none
*\*\return  none
**/
void CDC_COMM_IRQHandler(void)
{
    if (USART_GetFlagStatus(CDC_COMM, USART_FLAG_RXDNE) != RESET)
    {
        /* Send the received data to the PC Host */
        VCP_DataTx();
    }

    /* If overrun condition occurs, clear the ORE flag and recover communication */
    if (USART_GetFlagStatus(CDC_COMM, USART_FLAG_OREF) != RESET)
    {
        (void)USART_ReceiveData(CDC_COMM);
    }
}
