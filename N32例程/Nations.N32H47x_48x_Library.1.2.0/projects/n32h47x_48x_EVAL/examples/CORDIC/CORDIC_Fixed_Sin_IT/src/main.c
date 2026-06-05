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

#include "n32h47x_48x_cordic.h"
#include "n32h47x_48x_it.h"
#include "log.h"


/* Size of data array */
#define ARRAY_SIZE        (64U)

/* Max residual error in Q1.31 format */
#define MAX_RESIDUAL_ERROR_19 ((int32_t)0x00001000) /* Max residual error for sines, with 6 cycle precision: \
                                                       2^-19 max residual error, ie 31-19=12 LSB, ie <0x1000 */
#define MAX_RESIDUAL_ERROR_18 ((int32_t)0x00002000) /* Max residual error for sines, with 6 cycle precision: \
                                                       2^-18 max residual error, ie 31-18=13 LSB, ie <0x2000 */

__IO uint32_t wArrayIndex = 0;
__IO uint32_t rArrayIndex = 0;

CORDIC_InitType CORDIC_InitStructure;

/* Array of input data in Q1.31 format, regularly incremented from 0 to 2*pi */
static int32_t aInData[ARRAY_SIZE] =
{
    0x00000000, 0x04000000, 0x08000000, 0x0C000000,
    0x10000000, 0x14000000, 0x18000000, 0x1C000000,
    0x20000000, 0x24000000, 0x28000000, 0x2C000000,
    0x30000000, 0x34000000, 0x38000000, 0x3C000000,
    0x40000000, 0x44000000, 0x48000000, 0x4C000000,
    0x50000000, 0x54000000, 0x58000000, 0x5C000000,
    0x60000000, 0x64000000, 0x68000000, 0x6C000000,
    0x70000000, 0x74000000, 0x78000000, 0x7C000000,
    0x80000000, 0x84000000, 0x88000000, 0x8C000000,
    0x90000000, 0x94000000, 0x98000000, 0x9C000000,
    0xA0000000, 0xA4000000, 0xA8000000, 0xAC000000,
    0xB0000000, 0xB4000000, 0xB8000000, 0xBC000000,
    0xC0000000, 0xC4000000, 0xC8000000, 0xCC000000,
    0xD0000000, 0xD4000000, 0xD8000000, 0xDC000000,
    0xE0000000, 0xE4000000, 0xE8000000, 0xEC000000,
    0xF0000000, 0xF4000000, 0xF8000000, 0xFC000000
};

/* Array of reference result in Q1.31 format */
static int32_t aRefResult[ARRAY_SIZE] =
{
    0x00000000, 0x0C8BD35E, 0x18F8B83C, 0x25280C5D,
    0x30FBC54D, 0x3C56BA70, 0x471CECE6, 0x5133CC94,
    0x5A827999, 0x62F201AC, 0x6A6D98A4, 0x70E2CBC6,
    0x7641AF3C, 0x7A7D055B, 0x7D8A5F3F, 0x7F62368F,
    0x80000000, 0x7F62368F, 0x7D8A5F3F, 0x7A7D055B,
    0x7641AF3C, 0x70E2CBC6, 0x6A6D98A4, 0x62F201AC,
    0x5A827999, 0x5133CC94, 0x471CECE6, 0x3C56BA70,
    0x30FBC54D, 0x25280C5D, 0x18F8B83C, 0x0C8BD35E,
    0x00000000, 0xF3742CA2, 0xE70747C4, 0xDAD7F3A3,
    0xCF043AB3, 0xC3A94590, 0xB8E3131A, 0xAECC336C,
    0xA57D8667, 0x9D0DFE54, 0x9592675C, 0x8F1D343A,
    0x89BE50C4, 0x8582FAA5, 0x8275A0C1, 0x809DC971,
    0x80000000, 0x809DC971, 0x8275A0C1, 0x8582FAA5,
    0x89BE50C4, 0x8F1D343A, 0x9592675C, 0x9D0DFE54,
    0xA57D8667, 0xAECC336C, 0xB8E3131A, 0xC3A94590,
    0xCF043AB3, 0xDAD7F3A3, 0xE70747C4, 0xF3742CA2
};

/* Array of CORDIC calculated result in Q1.31 format */
static int32_t aCalResult[ARRAY_SIZE];

void Cordic_IRQ_IRQHandler(void);

/**
*\*\name    RCC_Configuration.
*\*\fun     Initialization Configuration RCC.
*\*\param   none
*\*\return  none
**/
void RCC_Configuration()
{
    /* Enable CORDIC clock */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_CORDIC, ENABLE);
}

/**
*\*\name    NVIC_Configuration.
*\*\fun     Initialization Configuration NVIC.
*\*\param   none
*\*\return  none
**/
void NVIC_Configuration()
{
    NVIC_InitType NVIC_InitStructure;
    
    /* Configures the priority group */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Configures the Cordic_IRQn NVIC */
    NVIC_InitStructure.NVIC_IRQChannel                   = Cordic_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = NVIC_SUB_PRIORITY_0;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    CheckResidualError.
*\*\fun     Check delta between two values is below threshold.
*\*\param   VarA :
*\*\          - First input variable.
*\*\param   VarB :
*\*\          - Second input variable.
*\*\param   MaxResErr :
*\*\          - Maximum delta allowed between VarA and VarB.
*\*\return  status:
*\*\          - 1: Delta is below threshold
*\*\          - 0: Delta is above threshold
**/
uint32_t CheckResidualError(int32_t VarA, int32_t VarB, uint32_t MaxResErr)
{
    uint32_t status = 1U;
    if ((VarA - VarB) >= 0)
    {
        if ((VarA - VarB) > MaxResErr)
        {
            status = 0U;
        }
    }
    else
    {
        if ((VarB - VarA) > MaxResErr)
        {
            status = 0U;
        }
    }
    return status;
}

/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    uint32_t i;
    
    log_init();
    
    log_info("CORDIC_Fixed_Sin_IT demo go...\n");

    /* Clocks Configuration */
    RCC_Configuration();

    /* NVIC configuration */
    NVIC_Configuration();

    /* Module reset */
    CORDIC_DeInit();

    CORDIC_StructInit(&CORDIC_InitStructure);
    /* Select sine function */
    CORDIC_InitStructure.Function  = CORDIC_FUNCTION_SINE;
    /* Set max precision for Q1.31 sine */
    CORDIC_InitStructure.Precision = CORDIC_PRECISION_6CYCLES;
    /* Configure CORDIC */
    CORDIC_Init(&CORDIC_InitStructure);

    /* Enable CORDIC interrupt */
    CORDIC_InterruptCmd(CORDIC_INT, ENABLE);
    
    /* Write the array data to CORDIC_WDAT in order */
    for (wArrayIndex = 0; wArrayIndex < ARRAY_SIZE; wArrayIndex++)
    {
        /* Write data to be calculated */
        CORDIC_WriteData((uint32_t *)&aInData[wArrayIndex]);
    }
    
    /* Compare CORDIC results to the reference values */
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        /* Check the data results sequentially */
        if (CheckResidualError(aCalResult[i], aRefResult[i], MAX_RESIDUAL_ERROR_18) == 0U)
        {
            log_info("Error[%d]\n", i);
            while (1);
        }
    }

    /* Calculations meet expectations */
    log_info("CORDIC demo success!!!\n");

    while (1);
}

/**
*\*\name    Cordic_IRQ_IRQHandler.
*\*\fun     This function handles Cordic request.
*\*\param   none
*\*\return  none
**/
void Cordic_IRQ_IRQHandler(void)
{
    if (CORDIC_GetFlagStatus(CORDIC_FLAG_RRF) != RESET)
    {
        /* Read calculation results */
        /* The RRF flag will be cleared after reading the data register */
        CORDIC_ReadData((uint32_t *)&aCalResult[rArrayIndex++]);
    }
}
