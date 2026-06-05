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
*\*\file http_client.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "http_client.h"
#include "lwip/tcp.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"


#define HOST_NAME       "www.example.com"


/* Receive buffer */
char RecvBuff[BUFFER_LEN];


/* Send buffer */
#if USE_PHPSTUDY
/*
* Install PHPStudy to facilitate the creation of a web server on the PC side,
* the server address is the PC local address. You can use this to test the http
* client when it is not convenient to connect to an external network.
* PHPStudy, download path: https://www.xp.cn/download.html
*/
char SendBuff[] = "GET / HTTP/1.1\r\nHost: 192.168.0.24\r\nConnection: close\r\n\r\n\r\n\r\n";
#else
/*
* IP address of www.example.com: 93.184.216.34
* You can modify the hostname as desired, but care must be taken to modify its
* IP address accordingly.
*/
char SendBuff[] = "GET / HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n\r\n\r\n";
#endif


#if USE_SOCKET

#include "lwip/dns.h"
#include "lwip/netdb.h"
#include <lwip/sockets.h>

ip4_addr_t RemoteAddr;

void http_dns_found_callback(const char *name,const ip_addr_t *ipaddr, void *arg);
static void http_client_socket_thread(void *arg);

/**
*\*\name    http_dns_found_callback.
*\*\fun     Callback which is invoked when a hostname is found.
*\*\param   name
*\*\         - Pointer to the name that was looked up
*\*\param   ipaddr
*\*\         - Pointer to an ip_addr_t containing the IP address of the hostname,
*\*\           or NULL if the name could not be found (or on any other error)
*\*\param   arg
*\*\         - A user-specified callback argument passed to dns_gethostbyname
*\*\return  none
**/
void http_dns_found_callback(const char *name,const ip_addr_t *ipaddr, void *arg)
{
    RemoteAddr.addr = ipaddr->addr;
}

/**
*\*\name    http_client_socket_thread.
*\*\fun     HTTP client task entry function, programmed via SOCKET API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void http_client_socket_thread(void *arg)
{
    int Sock = -1;
    int Len = 0;
    struct sockaddr_in ServerAddr;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    
    ip_addr_set_zero(&RemoteAddr);
    
#if LWIP_DNS && (!USE_PHPSTUDY)
    ip4_addr_t dnsAddr;
    /* Non-blocking domain name resolution */
    err_t Err = dns_gethostbyname(HOST_NAME, &dnsAddr, http_dns_found_callback, NULL);
    /* Get IP from local resolver cache */
    if (Err == ERR_OK)
    {
        RemoteAddr.addr = dnsAddr.addr;
    }
    /* Get IP from DNS server */
    else if (Err == ERR_INPROGRESS)
    {
        while (RemoteAddr.addr == 0)
        {
            /* System can switch to other tasks */
            vTaskDelay(5);
        }
    }
    else
    {
        printf("dns_gethostbyname err, err code: %d\n", Err);
    }
#else
    /* Set the server IP address */
    IP4_ADDR(&RemoteAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
#endif
    
    /* Set the server address information */
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = RemoteAddr.addr;
    ServerAddr.sin_port = htons(REMOTE_PORT);
    
    printf("Server IP: %s\n", inet_ntoa(ServerAddr.sin_addr));
    printf("Server Port: %d\n", REMOTE_PORT);
    
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
        
        printf("\r\n\r\nThe server is connected\r\n\r\n");
        
        /* Send GET request to server */
        send(Sock, SendBuff, sizeof(SendBuff), 0);
        
        while (1)
        {
            memset(&RecvBuff, 0, sizeof(RecvBuff));
            
            /* Blocking Receive Data */
            Len = recv(Sock, RecvBuff, sizeof(RecvBuff), 0);
            /* If valid data is successfully received, process it */
            if (Len > 0)
            {
                RecvBuff[Len] = '\0';
                printf("%s", RecvBuff);
            }
            else
            {
                closesocket(Sock);
                vTaskDelay(5000);
                break;
            }
        }
    }
}

#endif /* USE_SOCKET */

#if USE_NETCONN

static void http_client_netconn_thread(void *arg);

/**
*\*\name    http_client_netconn_thread.
*\*\fun     HTTP client task entry function, programmed via NETCONN API.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void http_client_netconn_thread(void *arg)
{
    err_t Err;
    struct netconn *Conn = NULL;
    struct netbuf *pBuf = NULL;
    
    ip4_addr_t ServerAddr;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
#if LWIP_DNS && (!USE_PHPSTUDY)
    /* Blocking domain name resolution */
    Err = netconn_gethostbyname(HOST_NAME, &ServerAddr);
    if (Err != ERR_OK)
    {
        printf("netconn_gethostbyname err, err code: %d\n", Err);
    }
#else
    /* Set the server IP address */
    IP4_ADDR(&ServerAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
#endif
    
    printf("Server IP: %s\n", ipaddr_ntoa(&ServerAddr));
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
            printf("\r\n\r\nThe server is connected\r\n\r\n");
            
            memset(RecvBuff, 0, sizeof(RecvBuff));
            
            /* Send GET request to server */
            netconn_write(Conn, SendBuff, sizeof(SendBuff), NETCONN_COPY);
            
            while (1)
            {
                /* Blocking Receive Data */
                Err = netconn_recv(Conn, &pBuf);
                /* If valid data is successfully received, process it */
                if (Err == ERR_OK)
                {
                    /* No need netconn_connect here, since the netbuf contains the address */
                    if (netbuf_copy(pBuf, RecvBuff, sizeof(RecvBuff)) != pBuf->p->tot_len)
                    {
                        printf("netbuf_copy failed\n");
                    }
                    else
                    {
                        RecvBuff[pBuf->p->tot_len] = '\0';
                        printf("%s", RecvBuff);
                    }
                    /* Clears the allocated pBuf */
                    netbuf_delete(pBuf);
                }
                else
                {
                    /* Close connection */
                    netconn_close(Conn);
                    /* Discard connection identifier */
                    netconn_delete(Conn);
                    vTaskDelay(5000);
                    break;
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
*\*\name    http_client_init.
*\*\fun     Initialize http client by creating a task thread.
*\*\param   none
*\*\return  none
**/
void http_client_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    printf("Set USE_NETCONN to 1 to use the NETCONN API\n");
    printf("Set USE_NETCONN to 0 to use the SOCKET API\n");
    
    /* Create a http client task thread */
#if USE_NETCONN
    sys_thread_new("http_client_netconn_thread", /* Task name */
                   http_client_netconn_thread,   /* Task entry function */
                   NULL,                         /* Task entry function parameter */
                   HTTP_CLIENT_TASK_STACK_SIZE,  /* Task stack size */
                   HTTP_CLIENT_TASK_PRIORITY);   /* Task priority */
#endif /* USE_NETCONN */

#if USE_SOCKET
    sys_thread_new("http_client_socket_thread",  /* Task name */
                   http_client_socket_thread,    /* Task entry function */
                   NULL,                         /* Task entry function parameter */
                   HTTP_CLIENT_TASK_STACK_SIZE,  /* Task stack size */
                   HTTP_CLIENT_TASK_PRIORITY);   /* Task priority */
#endif /* USE_SOCKET */
}

