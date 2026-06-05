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
*\*\file tcp_server.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "tcp_server.h"
#include "lwip/tcp.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#if USE_SOCKET

#include <lwip/sockets.h>

#define RECV_DATA_LEN                 (1024)
#define SOCKET_LISTEN_BACKLOG         (5)

extern void * pvPortMalloc( size_t xWantedSize );


static void tcp_server_socket_thread(void *arg);

/**
*\*\name    tcp_server_socket_thread.
*\*\fun     TCP server task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void tcp_server_socket_thread(void *arg)
{
    int Sock = -1;
    int SockNew = -1;
    int Flag = 1;
    int Len = 0;
    char *pData = NULL;
    
    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;
    socklen_t SinSize = sizeof(struct sockaddr_in);
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    
    /* Requests a section of memory for receiving data */
    pData = (char *)pvPortMalloc(RECV_DATA_LEN);
    if (pData == NULL)
    {
        printf("No memory\n");
        while (1);
    }
    
    /* Creates a socket */
    Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Sock < 0)
    {
        printf("socket error\n");
        while (1);
    }
    
    /* Set the server address information */
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(LOCAL_PORT);
    
    /* Bind local port and IP address */
    if (bind(Sock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) != 0)
    {
        printf("Unable to bind\n");
        closesocket(Sock);
        free(pData);
        while (1);
    }
    
    printf("Local port: %d\n", LOCAL_PORT);
    printf("Start listening...\n");
    
    /* Tell connection to go into listening mode */
    if (listen(Sock, SOCKET_LISTEN_BACKLOG) != 0)
    {
        printf("listen error\n");
        closesocket(Sock);
        free(pData);
        while (1);
    }
    
    while (1)
    {
        /* Accept a new connection, return a new socket */
        SockNew = accept(Sock, (struct sockaddr *)&ClientAddr, &SinSize);
        if (SockNew >= 0)
        {
            printf("Client %s:%d gets online\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
            /* Sets options for the new socket */
            setsockopt(SockNew, IPPROTO_TCP, TCP_NODELAY, (void *)&Flag, sizeof(int));
            
            while (1)
            {
                /* Blocking Receive Data */
                Len = read(SockNew, pData, RECV_DATA_LEN);
                /* If valid data is successfully received, process it */
                if (Len > 0)
                {
                    /* Sends the received data to the remote */
                    write(SockNew, pData, Len);
                    free(pData);
                }
                /* The client disconnects, jumps out of the loop, and accepts a new connection */
                else if (Len == 0)
                {
                    printf("Client disconnected\n");
                    break;
                }
                /* Receive error */
                else
                {
                    printf("read error\n");
                }
            }
            /* Close the current accept socket */
            closesocket(SockNew);
            SockNew = -1;
        }
    }
}

#endif /* USE_SOCKET */

#if USE_NETCONN

static void tcp_server_netconn_thread(void *arg);

/**
*\*\name    tcp_server_netconn_thread.
*\*\fun     TCP server task entry function, programmed via NETCONN API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void tcp_server_netconn_thread(void *arg)
{
    err_t Err;
    struct netconn *Conn = NULL;
    struct netconn *NewConn = NULL;
    
    u16_t Len;
    void *pData = NULL;
    struct netbuf *pBuf = NULL;

    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
#if LWIP_IPV6
    /* Create a new connection identifier */
    Conn = netconn_new(NETCONN_TCP_IPV6);
    LWIP_ERROR("Invalid Conn\n", (Conn != NULL), while (1););
    /* Bind local port and IP address */
    netconn_bind(Conn, IP6_ADDR_ANY, LOCAL_PORT);
#else /* LWIP_IPV6 */
    /* Create a new connection identifier */
    Conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("Invalid Conn\n", (Conn != NULL), while (1););
    /* Bind local port and IP address */
    netconn_bind(Conn, IP_ADDR_ANY, LOCAL_PORT);
#endif /* LWIP_IPV6 */
    
    printf("Local port: %d\n", LOCAL_PORT);
    printf("Start listening...\n");

    /* Tell connection to go into listening mode */
    netconn_listen(Conn);
    
    while (1)
    {
        /* Accept a new connection */
        Err = netconn_accept(Conn, &NewConn);
        if (Err == ERR_OK)
        {
            printf("Client %s:%d gets online\n", ipaddr_ntoa(&NewConn->pcb.tcp->remote_ip), NewConn->pcb.tcp->remote_port);
            
            /* Receive data */
            while (netconn_recv(NewConn, &pBuf) == ERR_OK)
            {
                do {
                    /* Get the data pointer and length */
                    netbuf_data(pBuf, &pData, &Len);
                    /* Sends the received data to the remote */
                    if (netconn_write(NewConn, pData, Len, NETCONN_COPY) != ERR_OK)
                    {
                        printf("netconn_write error!!\n");
                    }
                } while (netbuf_next(pBuf) >= 0);
                
                /* Clears the allocated pBuf */
                netbuf_delete(pBuf);
            }
            
            /* Close connection */
            netconn_close(NewConn);
            /* Discard connection identifier */
            netconn_delete(NewConn);
        }
    }
}
#endif /* USE_NETCONN */

/**
*\*\name    tcp_server_init.
*\*\fun     Initialize tcp server by creating a task thread.
*\*\param   none
*\*\return  none
**/
void tcp_server_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    printf("Set USE_NETCONN to 1 to use the NETCONN API\n");
    printf("Set USE_NETCONN to 0 to use the SOCKET API\n");
    /* Create a TCP server task thread */
#if USE_NETCONN
    sys_thread_new("tcp_server_netconn_thread", /* Task name */
                   tcp_server_netconn_thread,   /* Task entry function */
                   NULL,                        /* Task entry function parameter */
                   TCP_SERVER_TASK_STACK_SIZE,  /* Task stack size */
                   TCP_SERVER_TASK_PRIORITY);   /* Task priority */
#endif /* USE_NETCONN */

#if USE_SOCKET
    sys_thread_new("tcp_server_socket_thread",  /* Task name */
                   tcp_server_socket_thread,    /* Task entry function */
                   NULL,                        /* Task entry function parameter */
                   TCP_SERVER_TASK_STACK_SIZE,  /* Task stack size */
                   TCP_SERVER_TASK_PRIORITY);   /* Task priority */
#endif /* USE_SOCKET */
}

