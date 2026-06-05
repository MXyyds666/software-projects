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

#include <math.h>
#include <stdlib.h>

#include "n32h47x_48x_cordic.h"
#include "n32h47x_48x_it.h"
#include "log.h"


/* Size of data array */
#define ARRAY_SIZE         (64U)

/* Max residual error in Q1.31 format */
#define MAX_RESIDUAL_ERROR_19 (1.9073486323E-6f)    /* Max residual error for sines, with 6 cycle precision: \
                                                       2^-19 max residual error */
#define MAX_RESIDUAL_ERROR_18 (3.8146972656E-6f)    /* Max residual error for sines, with 6 cycle precision: \
                                                       2^-18 max residual error */

#define PI                 (3.1415926535897932f)


__IO uint32_t wArrayIndex = 0;
__IO uint32_t rArrayIndex = 0;

CORDIC_InitType CORDIC_InitStructure;

/* Array of input data in Q1.31 format */
static float aInData[ARRAY_SIZE];

/* Array of reference result in Q1.31 format */
static float aRefResult[ARRAY_SIZE];

/* Array of CORDIC calculated result in Q1.31 format */
static float aCalResult[ARRAY_SIZE];


void Cordic_IRQ_IRQHandler(void);


/**
*\*\name    RCC_Configuration.
*\*\fun     Initialization Configuration RCC.
*\*\param   none
*\*\return  none
**/
void RCC_Configuration(void)
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
void NVIC_Configuration(void)
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
uint32_t CheckResidualError(float VarA, float VarB, float MaxResErr)
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
    
    log_info("CORDIC_Float_Sin_IT demo go...\n");

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
    /* Select floating-point input */
    CORDIC_InitStructure.InSelect  = CORDIC_INPUT_FLOAT;
    /* Select floating-point output */
    CORDIC_InitStructure.OutSelect = CORDIC_OUTPUT_FLOAT;
    /* Configure CORDIC */
    CORDIC_Init(&CORDIC_InitStructure);

    /* Enable CORDIC interrupt */
    CORDIC_InterruptCmd(CORDIC_INT, ENABLE);
    
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        /* Random generation of input data */
        aInData[i] = -1.0f + ((float)(rand() % 1999)) * 0.001f;

        /* Software calculation results */
        aRefResult[i] = sinf(aInData[i] * PI);
    }
    
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
