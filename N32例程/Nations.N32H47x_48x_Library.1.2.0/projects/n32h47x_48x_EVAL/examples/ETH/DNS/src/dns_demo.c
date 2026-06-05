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
*\*\file dns_demo.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "dns_demo.h"
#include "lwip/tcp.h"

#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "FreeRTOS.h"
#include "task.h"


//#define HOST_NAME       "www.nationstech.com"
#define HOST_NAME       "www.example.com"


ip4_addr_t RemoteAddr;

static void dns_demo_thread(void *arg);
void dns_demo_found_callback(const char *name,const ip_addr_t *ipaddr, void *arg);

/**
*\*\name    dns_demo_found_callback.
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
void dns_demo_found_callback(const char *name,const ip_addr_t *ipaddr, void *arg)
{
    RemoteAddr.addr = ipaddr->addr;
}

/**
*\*\name    dns_demo_thread.
*\*\fun     DNS demo task entry function.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void dns_demo_thread(void *arg)
{
    err_t Err;
    
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    ip_addr_set_zero(&RemoteAddr);
    
#if USE_BLOCKING
    /* Blocking domain name resolution */
    Err = netconn_gethostbyname(HOST_NAME, &RemoteAddr);
    if (Err != ERR_OK)
    {
        printf("netconn_gethostbyname err, err code: %d\n", Err);
        while (1)
        {
            /* System can switch to other tasks */
            vTaskDelay(5);
        }
    }
#else
    ip4_addr_t dnsAddr;
    /* Non-blocking domain name resolution */
    Err = dns_gethostbyname(HOST_NAME, &dnsAddr, dns_demo_found_callback, NULL);
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
        while (1)
        {
            /* System can switch to other tasks */
            vTaskDelay(5);
        }
    }
#endif
    
    printf("Host name: %s, Host IP: %s\n", HOST_NAME, ipaddr_ntoa(&RemoteAddr));
    
    while (1)
    {
        /* System can switch to other tasks */
        vTaskDelay(5);
    }
}

/**
*\*\name    dns_demo_init.
*\*\fun     Initialize dns demo by creating a task thread.
*\*\param   none
*\*\return  none
**/
void dns_demo_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    printf("Set USE_BLOCKING to 1 to use the Blocking API\n");
    printf("Set USE_BLOCKING to 0 to use the Non-blocking API\n");
    
    /* Create a dns demo task thread */
    sys_thread_new("dns_demo_thread",        /* Task name */
                   dns_demo_thread,          /* Task entry function */
                   NULL,                     /* Task entry function parameter */
                   DNS_DEMO_TASK_STACK_SIZE, /* Task stack size */
                   DNS_DEMO_TASK_PRIORITY);  /* Task priority */
}

