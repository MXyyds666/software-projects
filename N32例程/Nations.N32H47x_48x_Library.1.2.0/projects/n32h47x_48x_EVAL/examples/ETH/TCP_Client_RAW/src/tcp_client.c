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


uint8_t FirstInitFlag = 1U;
static struct tcp_pcb *tcp_raw_pcb = NULL;

uint8_t aPollSendBuf[] = "This is a message sent periodically by the client...\n";


static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_err_callback(void *arg, err_t err);
static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err);


/**
*\*\name    tcp_poll_callback.
*\*\fun     Poll callback function.
*\*\param   arg
*\*\         - Additional argument to pass to the callback function
*\*\param   tpcb
*\*\         - tcp pcb
*\*\return  An error code
**/
static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb)
{
    LWIP_UNUSED_ARG(arg);
    
    if (tpcb == NULL)
    {
        return ERR_VAL;
    }
    
    /* Send a message to the server periodically */
    tcp_write(tpcb, aPollSendBuf, sizeof(aPollSendBuf), 1); 
    
    return ERR_OK;
}

/**
*\*\name    tcp_recv_callback.
*\*\fun     Recv callback function.
*\*\param   arg
*\*\         - Additional argument to pass to the callback function
*\*\param   tpcb
*\*\         - The connection pcb which received data
*\*\param   p
*\*\         - The received data (or NULL when the connection has been closed!)
*\*\param   err
*\*\         - An error code if there has been an error receiving
*\*\return  An error code
**/
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    err_t retErr = ERR_OK;
    
    LWIP_UNUSED_ARG(arg);
    
    if (tpcb == NULL)
    {
        return ERR_VAL;
    }
    
    /* Valid data received, sent back to remote */
    if (p != NULL)
    {
        /* Update the receive data window */
        tcp_recved(tpcb, p->tot_len);

        /* Echo the received data */
        tcp_write(tpcb, p->payload, p->tot_len, 1);
        
        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    }
    /* Received successfully, but packet is empty */
    else if (err == ERR_OK)
    {
        printf("The server disconnected! Close current pcb!!\n");
        retErr = tcp_close(tpcb);
        tcp_raw_pcb = NULL;
        printf("Try to reconnect to the server...\n");
        /* Try to reconnect to the server */
        tcp_client_init();
    }

    return retErr;
}

/**
*\*\name    tcp_err_callback.
*\*\fun     Err callback function.
*\*\param   arg
*\*\         - Additional argument to pass to the callback function
*\*\param   err
*\*\         - Error code to indicate why the pcb has been closed
 *             ERR_ABRT: aborted through tcp_abort or by a TCP timer
 *             ERR_RST: the connection was reset by the remote host
*\*\return  none
**/
static void tcp_err_callback(void *arg, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    
    printf("Connect error! Close current pcb!!\n");
    /* Connection failed, release current pcb */
    tcp_close(tcp_raw_pcb);
    
    printf("Try to reconnect to the server...\n");
    /* Try to reconnect to the server */
    tcp_client_init();
}

/**
*\*\name    tcp_connect_callback.
*\*\fun     Connect callback function.
*\*\param   arg
*\*\         - Additional argument to pass to the callback function
*\*\param   tpcb
*\*\         - The connection pcb which is connected
*\*\param   err
*\*\         - An unused error code, always ERR_OK currently
*\*\return  An error code
**/
static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    
    if ((err != ERR_OK) || (tpcb == NULL))
    {
        return ERR_VAL;
    }
    
    printf("Connect successful!!\n");

    /* Registers a callback function for periodic processing */
    tcp_poll(tpcb, tcp_poll_callback, 10);
    
    /* Registers a callback function for receiving */
    tcp_recv(tpcb, tcp_recv_callback);

    return ERR_OK;
}

/**
*\*\name    tcp_client_init.
*\*\fun     Initialize tcp client by RAW API.
*\*\param   none
*\*\return  none
**/
void tcp_client_init(void)
{
    ip4_addr_t RemoteIpAddr;
    
    /* Set the server IP address */
    IP4_ADDR(&RemoteIpAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    
    /* Creates a new TCP protocol control block */
    tcp_raw_pcb = tcp_new();
    
    if (FirstInitFlag)
    {
        FirstInitFlag = 0U;
        printf("Server IP: %d.%d.%d.%d\n", REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
        printf("Try to connect to the server...\n");
    }
    
    /* Try to connect to the server */
    tcp_connect(tcp_raw_pcb, &RemoteIpAddr, REMOTE_PORT, tcp_connect_callback);
    
    /* Registers a callback function for error handling */
    tcp_err(tcp_raw_pcb, tcp_err_callback);
}

