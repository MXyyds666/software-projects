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
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "main.h"


#define iir_coeffa_size   2
#define iir_coeffb_size   3
#define iir_d0            105
#define iir_d1            106
#define iir_gain          1
#define input_array_size  100

/*
    array of filter (IIR) coefficients A (feedback coefficients) in Q1.15 format
    IIR filter order is 2 order, the output data is multiply by 32767
    the sampling frequency is 16KHz and the cut-off frequency is 500Hz
*/
static int16_t iir_coeffa[iir_coeffa_size] = {
    -12412, 28242,
};

/*
    array of filter (FIR) coefficients B (feed-forward taps) in Q1.15 format
    IIR filter order is 2 order, the output data is multiply by 32767
    the sampling frequency is 16KHz and the cut-off frequency is 500Hz
*/
/* array of filter(IIR) coefficients B (feed-forward taps) in Q1.15 format */
static int16_t iir_coeffb[iir_coeffb_size] = {
    138, 277, 138,
};


/* array of output data to preload in Q1.15 format */
static int16_t output_data[iir_coeffa_size] = {
    0, 0
};

/*
    array of input values in -32767~32768
    the data is calculated by using 'x[i]=25000*(sin(2*pi*500/16000*i)+0.2*sin(2*pi*5*500/16000*i));' i goes from 1 to 100
    the useful signal frequency is 500Hz, and the noise signal frequency is 2500Hz
*/
static int16_t input_data[input_array_size] = {
    9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000, 27297, 21184,
    15883, 14142, 14865, 14186, 9035, 0, -9035, -14186, -14865, -14142,
    -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142, -14865, -14186,
    -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184, 27297, 30000,
    27297, 21184, 15883, 14142, 14865, 14186, 9035, 0, -9035, -14186,
    -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184, -15883, -14142,
    -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142, 15883, 21184,
    27297, 30000, 27297, 21184, 15883, 14142, 14865, 14186, 9035, 0,
    -9035, -14186, -14865, -14142, -15883, -21184, -27297, -30000, -27297, -21184,
    -15883, -14142, -14865, -14186, -9035, 0, 9035, 14186, 14865, 14142,
};

/* array of calculated filtered data by computer */
const int16_t reference_data[input_array_size - iir_coeffb_size + 1] = {
    1079, 2044, 3204, 4541, 6122, 8005, 10128, 12287, 14228, 15784,
    16927, 17711, 18146, 18135, 17514, 16178, 14178, 11694, 8918, 5948,
    2775, -615, -4130, -7546, -10600, -13121, -15086, -16554, -17548, -17979,
    -17698, -16615, -14791, -12419, -9704, -6754, -3568, -141, 3426, 6906,
    10030, 12624, 14660, 16197, 17255, 17745, 17516, 16478, 14694, 12355,
    9667, 6738, 3570, 156, -3402, -6876, -9996, -12589, -14625, -16163,
    -17222, -17715, -17489, -16455, -14673, -12338, -9653, -6727, -3561, -149,
    3407, 6879, 9999, 12590, 14625, 16162, 17222, 17714, 17488, 16453,
    14672, 12336, 9652, 6726, 3560, 148, -3408, -6880, -9999, -12590,
    -14625, -16163, -17222, -17714, -17488, -16453, -14672, -12336,
};

/* array of calculated filtered data by mcu */
static int16_t calculated_data[input_array_size - iir_coeffb_size + 1] = {0};

/* error between reference_data and calculated_data */
volatile int16_t error[input_array_size - iir_coeffb_size + 1] = {0};

void FAMC_Init(void);

/**
*\*\name main.
*\*\fun Main program.
*\*\return none
**/
int main(void)
{
    uint16_t i;
    /* FAMC_Init all configured peripherals */
    FAMC_Init();
    
    /* get calculation result */
    for(i = 0; i < input_array_size - iir_coeffb_size + 1; i++) 
    {
        while(FMAC_GetFlagStatus(FMAC_FLAG_YBUFEF) != RESET);
        calculated_data[i] = FMAC_ReadData();
        error[i] = reference_data[i] - calculated_data[i];
    }
    /* finish calculation */
    FMAC_FinishCalculate();

    while(1) 
    {
    }
}

/**
*\*\name    FAMC_Init.
*\*\fun     FMAC Initialization.
*\*\param   none
*\*\return  none
**/
void FAMC_Init(void)
{
    FMAC_InitType FMAC_InitStruct;
    FMAC_PreLoadType  FMAC_PreLoadStruct;
    
    RCC_EnableAHBPeriphClk( RCC_AHB_PERIPHEN_FMAC, ENABLE);

    /* initialize the FAC filter parameter struct */
    FMAC_StructInit(&FMAC_InitStruct);

    /* configure Coefficient buffer */
    FMAC_InitStruct.CoeffBufBaseAddress        = 0;
    FMAC_InitStruct.CoeffBufSize               = iir_coeffa_size + iir_coeffb_size;

    /* configure input buffer */
    FMAC_InitStruct.InputBufBaseAddress        = iir_coeffa_size + iir_coeffb_size;
    FMAC_InitStruct.InputBufSize               = iir_coeffb_size + iir_d0;
    FMAC_InitStruct.InputBufThreshold          = FMAC_THRESHOLD1;

    /* configure output buffer */
    FMAC_InitStruct.OutBufBaseAddress          = iir_coeffa_size + iir_coeffb_size + iir_coeffb_size + iir_d0;
    FMAC_InitStruct.OutBufSize                 = iir_coeffa_size + iir_d1;
    FMAC_InitStruct.OutBufThreshold            = FMAC_THRESHOLD1;
    FMAC_InitStruct.Limit                      = FMAC_LIMIT_ENABLE;
    FMAC_Init(&FMAC_InitStruct);

    FMAC_PreLoadStruct.pCoeffA                 = iir_coeffa;
    FMAC_PreLoadStruct.CoeffASize              = iir_coeffa_size;
    FMAC_PreLoadStruct.pCoeffB                 = iir_coeffb;
    FMAC_PreLoadStruct.CoeffBSize              = iir_coeffb_size;
    FMAC_PreLoadStruct.pInputData              = input_data;
    FMAC_PreLoadStruct.InputDataSize           = iir_coeffb_size + iir_d0;
    FMAC_PreLoadStruct.pOutputData             = output_data;
    FMAC_PreLoadStruct.OutputDataSize          = iir_coeffa_size;
    /* preload X0 X1 Y buffer with coefficient */
    FMAC_PreloadBufferData(&FMAC_PreLoadStruct);

    FMAC_InitStruct.Func                       = FMAC_FUNC_IIR_DIRECT_FORM_1;
    FMAC_InitStruct.P                          = iir_coeffb_size;
    FMAC_InitStruct.Q                          = iir_coeffa_size;
    FMAC_InitStruct.R                          = iir_gain;
    FMAC_ConfigParam(&FMAC_InitStruct);
    /* start filter calculation */
    FMAC_Enable(ENABLE);
}

