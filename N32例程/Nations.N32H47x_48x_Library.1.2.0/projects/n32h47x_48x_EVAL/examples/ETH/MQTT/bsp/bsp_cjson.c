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
*\*\file bsp_cjson.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include <stdio.h>
#include <stdlib.h>
#include "bsp_cjson.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**
*\*\name    cJSON_Init.
*\*\fun     Create cJSON object, add key-value pair initial value.
*\*\param   none
*\*\return  none
**/
cJSON* cJSON_Init(void)
{
    /* JSON root node */
    cJSON *pRoot = NULL;

    /* Create Project */
    pRoot = cJSON_CreateObject();
    if (pRoot == NULL)
    {
        return NULL;
    }
    
    /* Adds initial value of key-value pairs */
    cJSON_AddStringToObject(pRoot, NAME, DEFAULT_NAME);
    cJSON_AddNumberToObject(pRoot, TEMP_NUM, DEFAULT_TEMP_NUM);
    cJSON_AddNumberToObject(pRoot, HUM_NUM, DEFAULT_HUM_NUM);
    
    return pRoot;
}

/**
*\*\name    cJSON_Update.
*\*\fun     Update cJSON format data.
*\*\param   object
*\*\         - Pointer to a cJSON object
*\*\param   string
*\*\         - Pointer to the item string
*\*\param   pData
*\*\         - Pointer to the data to update
*\*\return  UPDATE_SUCCESS or UPDATE_FAIL
**/
uint8_t cJSON_Update(const cJSON * const object, const char * const string, void *pData)
{
    /* JSON root node */
    cJSON *pRoot = NULL;
    uint8_t retStatus = UPDATE_FAIL;
    
    pRoot = cJSON_GetObjectItem(object, string);
    if (pRoot == NULL)
    {
        retStatus = UPDATE_FAIL;
    }

    if (cJSON_IsBool(pRoot))
    {
        int *b = (int *)pData;
        cJSON_GetObjectItem(object, string)->type = *b ? cJSON_True : cJSON_False;
        retStatus = UPDATE_SUCCESS;
    }
    else if (cJSON_IsString(pRoot))
    {
        cJSON_GetObjectItem(object, string)->valuestring = (char *)pData;
        retStatus = UPDATE_SUCCESS;
    }
    else if (cJSON_IsNumber(pRoot))
    {
        double *num = (double *)pData;
        cJSON_GetObjectItem(object, string)->valuedouble = (double)*num;
        retStatus = UPDATE_SUCCESS;
    }
    else
    {
        /* Do nothing */
    }
    
    return retStatus;
}

/**
*\*\name    cJSON_Process.
*\*\fun     Processes data in cJSON format.
*\*\param   pData
*\*\         - Pointer to data to be processed
*\*\return  none
**/
void cJSON_Process(void *pData)
{
    cJSON *pRoot    = NULL;
    cJSON *pName    = NULL;
    cJSON *pTempNum = NULL;
    cJSON *pHumNum  = NULL;
    
    printf("Start parsing JSON data...\n");
    
    /* Parsed into JSON */
    pRoot = cJSON_Parse((char *)pData);

    /* Get key-value pairs */
    pName    = cJSON_GetObjectItem(pRoot, NAME);
    pTempNum = cJSON_GetObjectItem(pRoot, TEMP_NUM);
    pHumNum  = cJSON_GetObjectItem(pRoot, HUM_NUM);

    printf("\tname: %s\n\ttemp_num: %.1f\n\thum_num: %.1f\n",
            pName->valuestring,
            pTempNum->valuedouble,
            pHumNum->valuedouble);

    /* Free Memory */
    cJSON_Delete(pRoot);
}

