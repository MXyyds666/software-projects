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
*\*\file mqtt_client.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"
#include "lwipopts.h"

#include <stdio.h>
#include <string.h>


#if LWIP_DNS
#define HOST_NAME                          "awlpesy.iot.gz.baidubce.com"
#else
#define HOST_IP_ADDR0                      ((uint8_t)14U)
#define HOST_IP_ADDR1                      ((uint8_t)215U)
#define HOST_IP_ADDR2                      ((uint8_t)183U)
#define HOST_IP_ADDR3                      ((uint8_t)236U)
#endif

#define HOST_PORT                          (1883)

#define MSG_MAX_LEN                        (128)
#define MSG_TOPIC_LEN                      (50)
#define KEEPLIVE_TIME                      (60) /* In seconds, max. 65535 seconds */
#define MQTT_VERSION                       (4)

#define CLIENT_ID                           "n001"
#define USER_NAME                           "thingidp@awlpesy|n001|0|MD5"
#define PASSWORD                            "a5895bcfe642c80fe472debc352f9a1d"
#define TOPIC                               "$iot/n001/user/temp_hum"

#define MQTT_CLIENT_RECEIVE_TASK_STACK_SIZE (2048)
#define MQTT_CLIENT_PUBLISH_TASK_STACK_SIZE (2048)
#define MQTT_CLIENT_RECEIVE_TASK_PRIORITY   (4)
#define MQTT_CLIENT_PUBLISH_TASK_PRIORITY   (5)

#define USE_PRINTF                          (1)

#if USE_PRINTF
#define ERR_PRINTF                          printf("#err: %s, %d, %s\n", __FUNCTION__, __LINE__, __FILE__)
#else
#define ERR_PRINTF
#endif

typedef enum
{
    QOS0 = 0,
    QOS1,
    QOS2
} QoSType;

typedef enum
{
    MQTT_CONNECT_OK = 0,
    MQTT_CONNECT_NOK,
    MQTT_CONNECT_NOTACK
} ConnectType;

typedef struct __MQTTMessageType
{
    uint32_t qos;
    uint8_t  retained;
    uint8_t  dup;
    uint16_t id;
    uint8_t  type;
    void    *payload;
    int32_t  payloadlen;
} MQTTMessageType;

typedef struct __MQTTUserMsgType
{
    uint8_t  msgqos;               /* Message QoS */
    uint8_t  msg[MSG_MAX_LEN];     /* Messages */
    uint32_t msglenth;             /* Message Length */
    uint8_t  topic[MSG_TOPIC_LEN]; /* Topic */
    uint16_t packetid;             /* Message ID */
    uint8_t  valid;                /* Message valid flag */
} MQTTUserMsgType;


void mqtt_task_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __MQTT_CLIENT_H__ */


