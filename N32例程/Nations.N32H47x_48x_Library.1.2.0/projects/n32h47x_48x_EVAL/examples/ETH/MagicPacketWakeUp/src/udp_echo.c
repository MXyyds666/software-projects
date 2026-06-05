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
*\*\file udp_echo.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "udp_echo.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"


#if USE_SOCKET

#include <lwip/sockets.h>

char RecvBuff[BUFFER_LEN]; /* receive buffer */
char SendBuff[BUFFER_LEN]; /* send buffer */

static void udp_echo_socket_thread(void *arg);

/**
*\*\name    udp_echo_socket_thread.
*\*\fun     UDP echo task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void udp_echo_socket_thread(void *arg)
{
    int Sock = -1;
    int Len = 0;
    struct sockaddr_in RemoteAddr;
    struct sockaddr_in LocalAddr;
    socklen_t AddrSize;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    memset(&RemoteAddr, 0, sizeof(RemoteAddr));
    memset(&LocalAddr, 0, sizeof(LocalAddr));
    
    /* Set the server address information */
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_addr.s_addr = INADDR_ANY;
    LocalAddr.sin_port = htons(LOCAL_PORT);
    
    printf("Local port: %d\n", LOCAL_PORT);
    
    while (1)
    {
        /* Creates a socket */
        Sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (Sock < 0)
        {
            printf("socket error\n");
            vTaskDelay(1000);
            continue;
        }
        
        /* Bind socket to local information */
        if (bind(Sock, (struct sockaddr *)&LocalAddr, sizeof(struct sockaddr)) != 0)
        {
            printf("bind failed\n");
            closesocket(Sock);
            vTaskDelay(1000);
            continue;
        }
        
        while (1)
        {
            memset(&RecvBuff, 0, sizeof(RecvBuff));
            memset(&SendBuff, 0, sizeof(SendBuff));
            
            /* Blocking Receive Data */
            Len = recvfrom(Sock, RecvBuff, sizeof(RecvBuff), 0, (struct sockaddr*)&RemoteAddr, &AddrSize);
            if (Len < 0)
            {
                printf("recvfrom error\n");
            }
            
            printf("Receive from %s:%d\n", inet_ntoa(RemoteAddr.sin_addr), ntohs(RemoteAddr.sin_port));
            
            /* Fill the send buffer with received data and send it */
            Len = snprintf(SendBuff, sizeof(SendBuff), "Hi, this is N32 MCU! you say: '%s'", RecvBuff);
            /* Sends the data to the remote */
            Len = sendto(Sock, SendBuff, Len, 0, (struct sockaddr*)&RemoteAddr, AddrSize);
            if (Len < 0)
            {
                printf("sendto error\n");
            }
            
            printf("Receive: '%s'\n", RecvBuff);
        }
    }
}

#endif /* USE_SOCKET */

#if USE_NETCONN

static void udp_echo_netconn_thread(void *arg);

/**
*\*\name    udp_echo_netconn_thread.
*\*\fun     UDP echo task entry function, programmed via NETCONN API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void udp_echo_netconn_thread(void *arg)
{
    err_t Err;
    struct netconn *Conn = NULL;
    struct netbuf *pBuf = NULL;
    
    ip_addr_t *RemoteAddr;
    u16_t RemotePort;
    
    char aBuffer[128];
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
#if LWIP_IPV6
    Conn = netconn_new(NETCONN_UDP_IPV6);
    LWIP_ERROR("Invalid Conn\n", (Conn != NULL), while (1););
    netconn_bind(Conn, IP6_ADDR_ANY, LOCAL_PORT);
#else /* LWIP_IPV6 */
    Conn = netconn_new(NETCONN_UDP);
    LWIP_ERROR("Invalid Conn\n", (Conn != NULL), while (1););
    netconn_bind(Conn, IP_ADDR_ANY, LOCAL_PORT);
#endif /* LWIP_IPV6 */

    printf("Local port: %d\n", LOCAL_PORT);
    
    while (1)
    {
        /* Blocking Receive Data */
        Err = netconn_recv(Conn, &pBuf);
        /* If valid data is successfully received, process it */
        if (Err == ERR_OK)
        {
            memset(&aBuffer, 0, sizeof(aBuffer));
            
            /* Get remote IP address and port */
            RemoteAddr = netbuf_fromaddr(pBuf);
            RemotePort = netbuf_fromport(pBuf);
            printf("Receive from %s:%d\n", ipaddr_ntoa(RemoteAddr), RemotePort);
            
            /* No need netconn_connect here, since the netbuf contains the address */
            if (netbuf_copy(pBuf, aBuffer, sizeof(aBuffer)) != pBuf->p->tot_len)
            {
                printf("netbuf_copy failed\n");
            }
            else
            {
                aBuffer[pBuf->p->tot_len] = '\0';
                /* Sends the received data to the remote */
                Err = netconn_send(Conn, pBuf);
                if (Err != ERR_OK)
                {
                    printf("netconn_send failed: %d\n", (int)Err);
                }
                else
                {
                    printf("Receive: '%s'\n", aBuffer);
                }
            }
            /* Clears the allocated pBuf */
            netbuf_delete(pBuf);
        }
    }
}
#endif /* USE_NETCONN */

/**
*\*\name    udp_echo_init.
*\*\fun     Initialize udp echo by creating a task thread.
*\*\param   none
*\*\return  none
**/
void udp_echo_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    printf("Set USE_NETCONN to 1 to use the NETCONN API\n");
    printf("Set USE_NETCONN to 0 to use the SOCKET API\n");
    
    /* Create a UDP echo task thread */
#if USE_NETCONN
    sys_thread_new("udp_echo_netconn_thread", /* Task name */
                   udp_echo_netconn_thread,   /* Task entry function */
                   NULL,                      /* Task entry function parameter */
                   UDP_ECHO_TASK_STACK_SIZE,  /* Task stack size */
                   UDP_ECHO_TASK_PRIORITY);   /* Task priority */
#endif /* USE_NETCONN */

#if USE_SOCKET
    sys_thread_new("udp_echo_socket_thread",  /* Task name */
                   udp_echo_socket_thread,    /* Task entry function */
                   NULL,                      /* Task entry function parameter */
                   UDP_ECHO_TASK_STACK_SIZE,  /* Task stack size */
                   UDP_ECHO_TASK_PRIORITY);   /* Task priority */
#endif /* USE_SOCKET */
}

