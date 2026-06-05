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

#include "main.h"
#include "tcp_server.h"

#include <lwip/opt.h>
#include <lwip/arch.h>
#include "tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "netif/ethernet.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/etharp.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"

struct netif sNetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

__IO uint32_t TimeBase = 0;
__IO int ReceiveDataFlag = 0;


/**
*\*\name    sys_now.
*\*\fun     In NO_SYS mode, this function is a must.
*\*\        Used for timing base of LwIP.
*\*\param   none
*\*\return  none
**/
u32_t sys_now(void)
{
    return TimeBase;
}

/**
*\*\name    NetworkInitialize.
*\*\fun     Network initialization, including add network interfaces,
*\*\        Initialize and configure ETH, Initialize LwIP stack, etc.
*\*\param   none
*\*\return  none 
**/
void NetworkInitialize(void)
{
    /* IP addresses initialization */
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    
    /* Initilialize the LwIP stack without RTOS */
    lwip_init();

    /* add the network interface (IPv4/IPv6) without RTOS */
    netif_add(&sNetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* Registers the default network interface */
    netif_set_default(&sNetif);

    if (netif_is_link_up(&sNetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&sNetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&sNetif);
    }
}

/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none
**/
int main(void)
{
    RCC_ClocksType clk;
    
    log_init();
    
    log_info("ETH TCP server (no RTOS) demo go...\r\n\r\n");
    
    /* Get clock frequency value */
    RCC_GetClocksFreqValue(&clk);
    
    /* Configure SysTick to generate a 1ms time base */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(clk.HclkFreq / 1000);
    NVIC_SetPriority(SysTick_IRQn, 1);
    
    /* Network initialization, adding network interfaces */
    NetworkInitialize();
    
    /* Link successful, print local IP information */
    if (netif_is_up(&sNetif))
    {
        log_info("Network connection successful\n");
        log_info("Local IP: %s\n", ipaddr_ntoa(&sNetif.ip_addr));
        log_info("Netmask: %s\n", ipaddr_ntoa(&sNetif.netmask));
        log_info("Gateway: %s\n", ipaddr_ntoa(&sNetif.gw));
    }
    else
    {
        log_info("Network not connected\n");
        while (1);
    }
    
    /* Initialize TCP server task */
    tcp_server_init();
    
    while (1)
    {
        if (ReceiveDataFlag)
        {
            ReceiveDataFlag = 0;
            
            /* Receive data and LwIP process */
            ethernetif_input(&sNetif);
        }
        
        /* Handling LwIP timeout */
        sys_check_timeouts();
    }
}

