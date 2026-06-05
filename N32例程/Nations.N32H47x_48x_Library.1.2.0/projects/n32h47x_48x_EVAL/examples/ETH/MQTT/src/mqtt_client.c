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
*\*\file mqtt_client.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "mqtt_client.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include <lwip/sockets.h>
#include "lwip/sys.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "transport.h"
#include "MQTTPacket.h"
#include "bsp_cjson.h"
#include "dht11.h"


extern QueueHandle_t MQTTDataQueueHandle;


/* Global variable definition */
int MQTTSock = -1;
MQTTUserMsgType MQTTUserMsg;
__IO uint8_t noMQTTMsgExchange = 1;


/* Private function declaration */
static void mqtt_client_publish_thread(void *arg);
static void mqtt_client_receive_thread(void *arg);


/**
*\*\name    MQTT_ProcessUserMessage.
*\*\fun     Processing user messages.
*\*\param   pUserMsg
*\*\         - Pointer to the structure that save the user message
*\*\return  none
**/
void MQTT_ProcessUserMessage(MQTTUserMsgType *pUserMsg)
{
    /* This demo handles data just to print the message */
    /* User can add other processing within this function */
    printf("*****Receive subscribed messages*****\n");
    
    switch (pUserMsg->msgqos)
    {
        case 0:
            printf("Message QoS: QoS0\n");
            break;
        case 1:
            printf("Message QoS: QoS1\n");
            break;
        case 2:
            printf("Message QoS: QoS2\n");
            break;
        default:
            printf("Message QoS error!\n");
            break;
    }
    
    printf("Message Topic: %s\n", pUserMsg->topic);
    printf("Message Content: %s\n", pUserMsg->msg);
    printf("Message Length: %d\n", pUserMsg->msglenth);
    
    /* Calls the cJSON function to process data */
    cJSON_Process(pUserMsg->msg);

    /* Indicates that the message is invalid */
    pUserMsg->valid = 0;
}

/**
*\*\name    MQTT_SaveUserMessage.
*\*\fun     Save received messages as user messages.
*\*\param   pTopicName
*\*\         - Pointer to structure with topic name
*\*\param   pMsg
*\*\         - Pointer to structure for received messages
*\*\param   pUserMsg
*\*\         - Pointer to the structure that save the user message
*\*\return  none
**/
void MQTT_SaveUserMessage(MQTTString *pTopicName, MQTTMessageType *pMsg, MQTTUserMsgType *pUserMsg)
{
    /* Messages QoS */
    pUserMsg->msgqos = pMsg->qos;
    /* Saving messages */
    memcpy(pUserMsg->msg, pMsg->payload, pMsg->payloadlen);
    pUserMsg->msg[pMsg->payloadlen] = 0;
    /* Saving message length */
    pUserMsg->msglenth = pMsg->payloadlen;
    /* Message Topic */
    memcpy((char *)pUserMsg->topic, pTopicName->lenstring.data, pTopicName->lenstring.len);
    pUserMsg->topic[pTopicName->lenstring.len] = 0;
    /* Messages ID */
    pUserMsg->packetid = pMsg->id;
    /* Indicates that the message is valid */
    pUserMsg->valid = 1;
}

/**
*\*\name    MQTT_ProcessPacket.
*\*\fun     Process incoming messages from server.
*\*\param   pkType
*\*\         - Type of received message
*\*\param   pbuf
*\*\         - Pointer to the buffer that holds the contents of the message
*\*\param   buflen
*\*\         - Length of buffer
*\*\return  none
**/
void MQTT_ProcessPacket(uint8_t pkType, uint8_t *pbuf, uint32_t buflen)
{
    MQTTMessageType msg;
    MQTTString receivedTopic;
    uint32_t len;
    
    switch (pkType)
    {
        case PUBLISH:
        {
            /* Deserialize PUBLISH Messages */
            if (MQTTDeserialize_publish(&msg.dup,(int*)&msg.qos, &msg.retained, &msg.id, &receivedTopic,
                   (unsigned char **)&msg.payload, &msg.payloadlen, pbuf, buflen) != 1)
            {
                ERR_PRINTF;
                return;
            }
            
            /* Save message to MQTTUserMsg */
            MQTT_SaveUserMessage(&receivedTopic, &msg, &MQTTUserMsg);
            
            /* Process data according to message QoS */
            /* For QOS0, no response messages are required */
            if (msg.qos == QOS0)
            {
                /* Process data */
                MQTT_ProcessUserMessage(&MQTTUserMsg);
                return;
            }
            /* For QOS1, a PUBACK response message is required */
            if (msg.qos == QOS1)
            {
                /* Serialize PUBACK Messages */
                len = MQTTSerialize_puback(pbuf, buflen, MQTTUserMsg.packetid);
                if (len == 0)
                {
                    ERR_PRINTF;
                    return;
                }
                /* Send data */
                if (transport_sendPacketBuffer(MQTTSock, pbuf, len) < 0)
                {
                    ERR_PRINTF;
                    return;
                }
                /* Process data */
                MQTT_ProcessUserMessage(&MQTTUserMsg);
                return;
            }
            /* For QOS2, a PUBREC response message is required */
            if (msg.qos == QOS2)
            {
                /* Serialize ACK Messages (PUBREC) */
                len = MQTTSerialize_ack(pbuf, buflen, PUBREC, 0, MQTTUserMsg.packetid);
                if (len == 0)
                {
                    ERR_PRINTF;
                    return;
                }
                /* Send data */
                transport_sendPacketBuffer(MQTTSock, pbuf, len);
            }
            break;
        }

        case PUBREL:
        {
            /* Deserialize ACK Messages (PUBREL) */
            if ((MQTTDeserialize_ack(&msg.type, &msg.dup, &msg.id, pbuf, buflen) != 1) ||
                    (msg.type != PUBREL) || (msg.id != MQTTUserMsg.packetid))
            {
                ERR_PRINTF;
                return;
            }
            /* Receive PUBREL, process and drop data */
            if (MQTTUserMsg.valid == 1)
            {
                /* Process data */
                MQTT_ProcessUserMessage(&MQTTUserMsg);
            }
            /* Serialize PUBCOMP Messages */
            len = MQTTSerialize_pubcomp(pbuf, buflen, msg.id);
            if (len == 0)
            {
                ERR_PRINTF;
                return;
            }
            /* Send data */
            transport_sendPacketBuffer(MQTTSock, pbuf, len);
            break;
        }

        case PUBACK:   break;
        case PUBREC:   break;
        case PUBCOMP:  break;
        case PINGRESP: break;
        default:       break;
    }
}

/**
*\*\name    MQTT_Publish.
*\*\fun     The client publishes (sends) an application message to the server.
*\*\param   sock
*\*\         - Valid sockets created when establishing a network connection
*\*\param   pTopic
*\*\         - Pointer to message topic
*\*\param   qos
*\*\         - QoS level for application message distribution
*\*\param   pMsg
*\*\         - Pointer to message contents
*\*\return  0 or error code
**/
int32_t MQTT_Publish(int32_t sock, char *pTopic, QoSType qos, uint8_t* pMsg)
{
    int8_t retain = 0;
    uint8_t aBuf[MSG_MAX_LEN];
    int32_t len;
    uint16_t packID;
    static uint16_t pubPacketID = 1;
    MQTTString TopicString = MQTTString_initializer;
    
    memset(aBuf, 0, sizeof(aBuf));

    /* Fill Topic */
    TopicString.cstring = (char *)pTopic;
    /* Fill Packet Identifier */
    if ((qos == QOS1) || (qos == QOS2))
    {
        packID = pubPacketID++;
    }
    else
    {
        packID = 0;
    }

    /* Serialize Publish message */
    len = MQTTSerialize_publish(aBuf, sizeof(aBuf), 0, qos, retain, packID, TopicString, (unsigned char*)pMsg, strlen((char *)pMsg));
    if (len <= 0)
    {
        /* Publish error */
        ERR_PRINTF;
        return -1;
    }
    /* Send Publish message */
    if(transport_sendPacketBuffer(MQTTSock, aBuf, len) < 0)
    {
        /* Send error */
        ERR_PRINTF;
        return -2;
    }
    
    return 0;
}

/**
*\*\name    MQTT_PingReq.
*\*\fun     Client sends PINGREQ message for Keep Alive processing.
*\*\param   sock
*\*\         - Valid sockets created when establishing a network connection
*\*\return  0 or error code
**/
int32_t MQTT_PingReq(int32_t sock)
{
    int32_t len;
    uint8_t aBuf[MSG_MAX_LEN];
    
    memset(aBuf, 0, sizeof(aBuf));

    /* Serialize PINGREQ Messages */
    len = MQTTSerialize_pingreq(aBuf, sizeof(aBuf));
    /* Send TCP data */
    if(transport_sendPacketBuffer(MQTTSock, aBuf, len) < 0)
    {
        /* Send error */
        ERR_PRINTF;
        return -1;
    }
    
    return 0;
}

/**
*\*\name    MQTT_Subscribe.
*\*\fun     Client sends a SUBSCRIBE message to server for creating subscription.
*\*\param   sock
*\*\         - Valid sockets created when establishing a network connection
*\*\param   pTopic
*\*\         - Pointer to message topic
*\*\param   qosMax
*\*\         - The maximum QoS level allowed for the server to send application
*\*\           messages to the client
*\*\return  error code
**/
int32_t MQTT_Subscribe(int32_t sock, char *pTopic, QoSType qosMax)
{
    static uint32_t PacketID = 1;
    uint16_t packetidbk = 0;
    int32_t conutbk = 0;
    uint8_t aBuf[MSG_MAX_LEN];
    MQTTString TopicString = MQTTString_initializer;
    int32_t len;
    int32_t req_qos, qosbk;

    fd_set readfd;
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);
    
    memset(aBuf, 0, sizeof(aBuf));

    /* Copy Topic */
    TopicString.cstring = (char *)pTopic;
    /* Maximum QoS request */
    req_qos = qosMax;

    /* Serialize Subscription Messages */
    len = MQTTSerialize_subscribe(aBuf, sizeof(aBuf), 0, PacketID++, 1, &TopicString, &req_qos);
    /* Send TCP data */
    if(transport_sendPacketBuffer(MQTTSock, aBuf, len) < 0)
    {
        /* Send error */
        ERR_PRINTF;
        return -1;
    }

    /* Wait for readable events */
    if(select(sock + 1, &readfd, NULL, NULL, &tv) <= 0)
    {
        /* Wait for timeout */
        ERR_PRINTF;
        return -2;
    }
    
    /* Check for readable events */
    if(FD_ISSET(sock, &readfd) == 0)
    {
        /* No readable event */
        ERR_PRINTF;
        return -3;
    }

    /* Wait for subscription return */
    if(MQTTPacket_read(aBuf, sizeof(aBuf), transport_getdata) != SUBACK)
    {
        /* No subscription return received */
        ERR_PRINTF;
        return -4;
    }

    /* Deserializes subscription response packets */
    if(MQTTDeserialize_suback(&packetidbk, 1, &conutbk, &qosbk, aBuf, sizeof(aBuf)) != 1)
    {
        /* Deserializes failure */
        ERR_PRINTF;
        return -5;
    }

    /* Check the correctness of returned data */
    if((qosbk == 0x80) || (packetidbk != (PacketID-1)))
    {
        /* The data is incorrect */
        ERR_PRINTF;
        return -6;
    }

    /* Subscription successful */
    return 0;
}

/**
*\*\name    MQTT_Connect.
*\*\fun     The MQTT client connects to the server, including sending CONNECT
*\*\        messages, receiving CONNACK messages, and determining whether the
*\*\        connection was successful.
*\*\param   none
*\*\return  One of MQTT_CONNECT_OK, MQTT_CONNECT_NOK, MQTT_CONNECT_NOTACK
**/
ConnectType MQTT_Connect(void)
{
    int len = 0;
    uint8_t aBuf[MSG_MAX_LEN];
    unsigned char sessionPresent, connack_rc;
    MQTTPacket_connectData sData = MQTTPacket_connectData_initializer;
    
    /* Set the sData */
    sData.clientID.cstring  = CLIENT_ID;
    sData.keepAliveInterval = KEEPLIVE_TIME;
    sData.username.cstring  = USER_NAME;
    sData.password.cstring  = PASSWORD;
    sData.MQTTVersion       = MQTT_VERSION;
    sData.cleansession      = 1;
    
    memset(aBuf, 0, sizeof(aBuf));
    
    /* Assemble message */
    len = MQTTSerialize_connect((unsigned char *)aBuf, sizeof(aBuf), &sData);
    /* Send message */
    transport_sendPacketBuffer(MQTTSock, aBuf, len);
    /* Wait for connection response */
    if (MQTTPacket_read(aBuf, sizeof(aBuf), transport_getdata) == CONNACK)
    {
        /* If the return code (connack_rc) of the CONNACK message is 0, the connection is successful */
        if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, aBuf, sizeof(aBuf)) != 1) || (connack_rc != 0))
        {
            printf("Cannot connect, err code: %d\n", connack_rc);
            return MQTT_CONNECT_NOK;
        }
        else
        {
            printf("Username and secret key authentication successful, MQTT connection successful!\n");
            return MQTT_CONNECT_OK;
        }
    }
    else
    {
        printf("MQTT connection does not respond!\n");
        return MQTT_CONNECT_NOTACK;
    }
}

/**
*\*\name    MQTT_ClientInit.
*\*\fun     The MQTT client initialization, which consists of establishing a
*\*\        network connection, connecting to the MQTT server, and subscribing
*\*\        to messages.
*\*\param   none
*\*\return  none
**/
void MQTT_ClientInit(void)
{
    ip4_addr_t ServerAddr;
    
#if LWIP_DNS
    /* Blocking domain name resolution */
    err_t Err = netconn_gethostbyname(HOST_NAME, &ServerAddr);
    if (Err != ERR_OK)
    {
        printf("netconn_gethostbyname err, err code: %d\n", Err);
    }
#else
    /* Set the server IP address */
    IP4_ADDR(&ServerAddr, HOST_IP_ADDR0, HOST_IP_ADDR1, HOST_IP_ADDR2, HOST_IP_ADDR3);
#endif
    
    while (1)
    {
        printf("Start connect to the corresponding cloud platform server...\n");
        printf("Server IP: %s, Server Port: %d\n", ipaddr_ntoa(&ServerAddr), HOST_PORT);
        
        while (1)
        {
            /* Connect to server */
            MQTTSock = transport_open(ipaddr_ntoa(&ServerAddr), HOST_PORT);
            if (MQTTSock >= 0)
            {
                printf("Successful connection to the cloud platform server!\n");
                break;
            }
            else
            {
                printf("Failed to connect to the cloud platform server, wait 5 seconds and try to reconnect!\n");
                vTaskDelay(5000);
            }
        }
        
        printf("MQTT username and secret key authentication login...\n");
        /* If connection fails, reconnect to the server */
        if (MQTT_Connect() != MQTT_CONNECT_OK)
        {
            printf("MQTT username and secret key authentication login failed!\n");
            /* Close the link */
            transport_close(MQTTSock);
            /* Reconnect to the server */
            continue;
        }
        
        printf("Start subscribing to messages...\n");
        /* If subscription fails, reconnect to the server */
        if (MQTT_Subscribe(MQTTSock, (char *)TOPIC, QOS1) < 0)
        {
            printf("Client subscription message failed!\n");
            /* Close the link */
            transport_close(MQTTSock);
            /* Reconnect to the server */
            continue;
        }
        
        printf("Start a loop to receive subscribed messages...\n");
        break;
    }
}

/**
*\*\name    mqtt_client_publish_thread.
*\*\fun     MQTT client publish task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void mqtt_client_publish_thread(void *arg)
{
    char *p = NULL;
    uint8_t res1 = UPDATE_FAIL;
    uint8_t res2 = UPDATE_FAIL;
    BaseType_t xReturn = pdTRUE;
    DHT11DataType recvData;
    cJSON *pRoot = NULL;
    double TempValue, HumValue;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* cJSON initialization */
    pRoot = cJSON_Init();
    
    while (1)
    {
        /* Indicates no data exchange */
        noMQTTMsgExchange = 1;
        
        xReturn = xQueueReceive(MQTTDataQueueHandle, /* Message queue handle */
                                &recvData,           /* Content of messages sent */
                                portMAX_DELAY);      /* Wait time in ms */
        if (xReturn == pdTRUE)
        {
            TempValue = recvData.temperature;
            HumValue  = recvData.humidity;
            
            /* Update data */
            res1 = cJSON_Update(pRoot, TEMP_NUM, &TempValue);
            res2 = cJSON_Update(pRoot, HUM_NUM, &HumValue);
            if ((res1 == UPDATE_SUCCESS) && (res2 == UPDATE_SUCCESS))
            {
                p = cJSON_Print(pRoot);
                /* Publish messages */
                if (MQTT_Publish(MQTTSock, (char *)TOPIC, QOS1, (uint8_t *)p) < 0)
                {
                    printf("MQTT_Publish fail!\n");
                    /* Close the link */
                    transport_close(MQTTSock);
                    /* Reconnect to the server */
                    MQTT_ClientInit();
                }
                free(p);
                
                /* Indicates data exchange */
                noMQTTMsgExchange = 0;
            }
            else
            {
                printf("cJSON update fail!\n");
            }
        }
    }
}

/**
*\*\name    mqtt_client_receive_thread.
*\*\fun     MQTT client receive task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void mqtt_client_receive_thread(void *arg)
{
    uint32_t curTick;
    uint8_t aBuf[MSG_MAX_LEN];
    int32_t type;
    fd_set readfd;
    struct timeval tv;
    
    /* 10 microseconds Timeout */
    tv.tv_sec = 0;
    tv.tv_usec = 10;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Get the current tick as the heartbeat packet start time */
    curTick = xTaskGetTickCount();
    
    while (1)
    {
        memset(aBuf, 0, sizeof(aBuf));
        /* Indicates no data exchange */
        noMQTTMsgExchange = 1;
        
        FD_ZERO(&readfd);
        FD_SET(MQTTSock, &readfd);
        
        /* Wait for readable events */
        select(MQTTSock + 1, &readfd, NULL, NULL, &tv);
        /* Check if the MQTT server has data */
        if (FD_ISSET(MQTTSock, &readfd) != 0)
        {
            /* Read packet */
            type = MQTTPacket_read(aBuf, sizeof(aBuf), transport_getdata);
            if (type != -1)
            {
                /* Processing message data */
                MQTT_ProcessPacket(type, aBuf, sizeof(aBuf));
                /* Indicates data exchange */
                noMQTTMsgExchange = 0;
                /* Get the current tick as the heartbeat packet start time */
                curTick = xTaskGetTickCount();
            }
        }
        
        /* Send PING keep-alive commands to the server at regular intervals */
        if ((xTaskGetTickCount() - curTick) > ((KEEPLIVE_TIME)/2*1000))
        {
            /* Get the current tick as the heartbeat packet start time */
            curTick = xTaskGetTickCount();
            
            /* Check for data exchange */
            if (noMQTTMsgExchange == 0)
            {
                /* There is data exchange, no need to send PING messages */
                continue;
            }
            
            /* Send MQTT heartbeat packets */
            if (MQTT_PingReq(MQTTSock) < 0)
            {
                printf("Send the keep-alive command failed!\n");
                /* Close the link */
                transport_close(MQTTSock);
                /* Reconnect to the server */
                MQTT_ClientInit();
            }
            
            /* Indicates data exchange */
            noMQTTMsgExchange = 0;
        }
    }
}

/**
*\*\name    mqtt_task_init.
*\*\fun     Initialize mqtt task by creating mqtt client task thread.
*\*\param   none
*\*\return  none
**/
void mqtt_task_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Connect to server, subscribe messages */
    MQTT_ClientInit();
    
    /* Create a mqtt client receive task thread */
    sys_thread_new("mqtt_client_receive_thread",        /* Task name */
                   mqtt_client_receive_thread,          /* Task entry function */
                   NULL,                                /* Task entry function parameter */
                   MQTT_CLIENT_RECEIVE_TASK_STACK_SIZE, /* Task stack size */
                   MQTT_CLIENT_RECEIVE_TASK_PRIORITY);  /* Task priority */

    /* Create a mqtt client publish task thread */
    sys_thread_new("mqtt_client_publish_thread",        /* Task name */
                   mqtt_client_publish_thread,          /* Task entry function */
                   NULL,                                /* Task entry function parameter */
                   MQTT_CLIENT_PUBLISH_TASK_STACK_SIZE, /* Task stack size */
                   MQTT_CLIENT_PUBLISH_TASK_PRIORITY);  /* Task priority */
}

