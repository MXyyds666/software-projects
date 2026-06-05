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
*\*\file tcp_client.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "tcp_client.h"
#include "lwip/tcp.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"


#if USE_SOCKET

#include <lwip/sockets.h>

char RecvBuff[BUFFER_LEN]; /* receive buffer */
char SendBuff[BUFFER_LEN]; /* send buffer */

static void tcp_client_socket_thread(void *arg);

/**
*\*\name    tcp_client_socket_thread.
*\*\fun     TCP client task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void tcp_client_socket_thread(void *arg)
{
    int Sock = -1;
    int Len = 0;
    ip4_addr_t IpAddr;
    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;
    socklen_t AddrSize = sizeof(ClientAddr);
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    
    /* Set the server IP address */
    IP4_ADDR(&IpAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    printf("Server IP: %d.%d.%d.%d\n", REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    printf("Server Port: %d\n", REMOTE_PORT);
    
    /* Set the server address information */
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = IpAddr.addr;
    ServerAddr.sin_port = htons(REMOTE_PORT);
    
    while (1)
    {
        /* Creates a socket */
        Sock = socket(AF_INET, SOCK_STREAM, 0);
        if (Sock < 0)
        {
            printf("socket error\n");
            vTaskDelay(1000);
            continue;
        }
        
        /* Connect to a remote server with a specified IP address and port */
        if (connect(Sock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) != 0)
        {
            printf("connect failed!\n");
            closesocket(Sock);
            vTaskDelay(1000);
            continue;
        }
        
        /* Get local address and port information */
        getsockname(Sock, (struct sockaddr *)&ClientAddr, &AddrSize);
        printf("The server is connected from local %s:%d\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
        
        while (1)
        {
            memset(&RecvBuff, 0, sizeof(RecvBuff));
            memset(&SendBuff, 0, sizeof(SendBuff));
            
            /* Blocking Receive Data */
            Len = recv(Sock, RecvBuff, sizeof(RecvBuff), 0);
            /* If valid data is successfully received, process it */
            if (Len > 0)
            {
                /* Fill the send buffer with received data and send it */
                Len = snprintf(SendBuff, sizeof(SendBuff), "Hi, this is N32 MCU! you say: '%s'", RecvBuff);
                /* Sends the data to the remote */
                Len = send(Sock, SendBuff, Len, 0);
                if (Len <= 0)
                {
                    printf("send error\n");
                }
            }
            /* If the connection closes, it jumps out of the loop and tries to reconnect */
            else if (Len == 0)
            {
                printf("Server disconnected\n");
                closesocket(Sock);
                vTaskDelay(1000);
                break;
            }
            /* Receive error */
            else
            {
                printf("recv error\n");
            }
        }
    }
}

#endif /* USE_SOCKET */

#if USE_NETCONN

static void tcp_client_netconn_thread(void *arg);

/**
*\*\name    tcp_client_netconn_thread.
*\*\fun     TCP client task entry function, programmed via NETCONN API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void tcp_client_netconn_thread(void *arg)
{
    err_t Err;
    struct netconn *Conn = NULL;
    
    u16_t Len;
    void *pData = NULL;
    struct netbuf *pBuf = NULL;
    
    ip4_addr_t ServerAddr;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Set the server IP address */
    IP4_ADDR(&ServerAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    printf("Server IP: %d.%d.%d.%d\n", REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    printf("Server Port: %d\n", REMOTE_PORT);
    
    while (1)
    {
        /* Create a connection structure */
        Conn = netconn_new(NETCONN_TCP);
        LWIP_ERROR("Invalid Conn\n", (Conn != NULL), while (1););
        
        /* Connect to a remote server with a specified IP address and port */
        Err = netconn_connect(Conn, &ServerAddr, REMOTE_PORT);
        /* Connected successfully, ready to receive data, send data */
        if (Err == ERR_OK)
        {
            printf("The server is connected from local %s:%d\n", ipaddr_ntoa(&Conn->pcb.tcp->local_ip), Conn->pcb.tcp->local_port);
            
            while (1)
            {
                /* Blocking Receive Data */
                Err = netconn_recv(Conn, &pBuf);
                /* If valid data is successfully received, process it */
                if (Err == ERR_OK)
                {
                    /* Get the data pointer and length */
                    netbuf_data(pBuf, &pData, &Len);
                    
                    /* Sends the received data to the remote */
                    if (netconn_write(Conn, pData, Len, NETCONN_COPY) != ERR_OK)
                    {
                        printf("netconn_write error\n");
                    }
                    
                    /* Clears the allocated pBuf */
                    netbuf_delete(pBuf);
                }
                /* If the connection closes, it jumps out of the loop and tries to reconnect */
                else if (Err == ERR_CLSD)
                {
                    printf("Server disconnected\n");
                    /* Close connection */
                    netconn_close(Conn);
                    /* Discard connection identifier */
                    netconn_delete(Conn);
                    break;
                }
                /* Receive error, print error code */
                else
                {
                    printf("netconn_recv error, error code: %d\n", Err);
                }
            }
        }
        /* Connection fails, attempts to reconnect after a delay */
        /* The number of reconnections allowed is determined by MEMP_NUM_NETCONN */
        else
        {
            printf("Connect failed\n");
            netconn_close(Conn);
            vTaskDelay(1000);
            continue;
        }
    }
}
#endif /* USE_NETCONN */

/**
*\*\name    tcp_client_init.
*\*\fun     Initialize tcp client by creating a task thread.
*\*\param   none
*\*\return  none
**/
void tcp_client_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    printf("Set USE_NETCONN to 1 to use the NETCONN API\n");
    printf("Set USE_NETCONN to 0 to use the SOCKET API\n");
    
    /* Create a TCP client task thread */
#if USE_NETCONN
    sys_thread_new("tcp_client_netconn_thread", /* Task name */
                   tcp_client_netconn_thread,   /* Task entry function */
                   NULL,                        /* Task entry function parameter */
                   TCP_CLIENT_TASK_STACK_SIZE,  /* Task stack size */
                   TCP_CLIENT_TASK_PRIORITY);   /* Task priority */
#endif /* USE_NETCONN */

#if USE_SOCKET
    sys_thread_new("tcp_client_socket_thread",  /* Task name */
                   tcp_client_socket_thread,    /* Task entry function */
                   NULL,                        /* Task entry function parameter */
                   TCP_CLIENT_TASK_STACK_SIZE,  /* Task stack size */
                   TCP_CLIENT_TASK_PRIORITY);   /* Task priority */
#endif /* USE_SOCKET */
}

