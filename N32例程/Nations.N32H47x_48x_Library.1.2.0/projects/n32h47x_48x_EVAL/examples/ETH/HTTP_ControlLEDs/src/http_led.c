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
*\*\file http_led.c
*\*\author Nations
*\*\version v1.1.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "http_led.h"

#include "lwip/tcp.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#define HTTP_RESPONSE "<html>"\
"<head><title>N32 MCU HTTP Example</title></head>"\
/*Set the font to "Times New Roman"*/\
"<style>.custom-font {font-family: Times New Roman, sans-serif;}</style>"\
/*Define an elastic container in which child elements will be centered horizontally and vertically,\
and a distance of 10 pixels between this container and other elements around it*/\
"<style>.led-container {display: flex;justify-content: center;align-items: center;margin: 10px;}</style>"\
/*Define an element of class name .led that is a circle with a height and width of 50 pixels, with the text\
centered internally and with a line height of 50 pixels as well, making the text vertically centered*/\
"<style>.led {width: 50px;height: 50px;border-radius: 50px;text-align: center;line-height: 50px;}</style>"\
/*Defines the background color of an element with class name .led1*/\
"<style>.led1 {background-color: %s;}</style>"\
/*Defines the background color of an element with class name .led2*/\
"<style>.led2 {background-color: %s;}</style>"\
/*Define the style of an element with a class name of .control-btn: the left margin is 15 pixels,\
the font family is "Times New Roman", and the width is 40 pixels*/\
"<style>.control-btn {margin-left: 15px;font-family: Times New Roman, sans-serif;width: 40px;}</style>"\
"<body>"\
/*First-level text headings, centered, font "Times New Roman*/\
"<h1 align=\"center\" class=\"custom-font\">LEDs Control</h1>"\
"<h2 align=\"center\" class=\"custom-font\">Welcome to N32 MCU LwIP HTTP Server!</h2>"\
/*Text, centered, font "Times New Roman"*/\
"<p align=\"center\" class=\"custom-font\">This is a small test page.</p>"\
"<p align=\"center\" class=\"custom-font\">You can control the on/off of each LED with the buttons below.</p>"\
/*This is the start tag for an HTML element containing a <div> element with the class name "led-container"*/\
"<div class=\"led-container\">"\
/*Displays a "LED1" text of size 5, followed by a space, and applies a style called "custom-font"*/\
"<font size=\"5\" class=\"custom-font\">LED1&nbsp</font>"\
/*Defines an element (LED) that associates the styles of the class names led and led1*/\
"<div class=\"led led1\"> </div>"\
/*Defines the form data submission method as "post", defines the target URL for form data submission,\
names the form element as "ledform", and sets the element's top margin to zero*/\
"<form method=post action=\"%s\" name=\"ledform\" style=\"margin-block-end: 0em;\">"\
/*Creates a button element that is associated with a style with the class name "control-btn"*/\
"<button class=\"control-btn\">%s</button>"\
"</form>"\
"</div>"\
"<div class=\"led-container\">"\
"<font size=\"5\" class=\"custom-font\">LED2&nbsp</font>"\
"<div class=\"led led2\"> </div>"\
"<form method=post action=\"%s\" name=\"ledform\" style=\"margin-block-end: 0em;\">"\
"<button class=\"control-btn\">%s</button>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"

#define HTTP_RESPONSE_FORMAT(led1Color, led2Color, led1Action, led1Ctrl, led2Action, led2Ctrl) HTTP_RESPONSE, led1Color, led2Color, led1Action, led1Ctrl, led2Action, led2Ctrl

int SendLen = 0;
LEDx_FlagType ledFlag;
static char htmlData[1500];
static char htmlHeader[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

static void http_server_thread(void *arg);
static void http_led_gpio_init(GPIO_Module* GPIOx, uint16_t Pin);
static void http_html_initial_package(void);
static void http_led_control_task(struct netconn *conn, char *p, u16_t len);

/**
*\*\name    http_led_control_task.
*\*\fun     LED control task entry function.
*\*\param   conn
*\*\         - The netconn from which to receive data
*\*\param   p
*\*\         - Pointer to a void pointer where to store the data pointer
*\*\param   len
*\*\         - The length of the data is stored
*\*\return  none
**/
static void http_led_control_task(struct netconn *conn, char *p, u16_t len)
{
    err_t Err = ERR_OK;
    
    /* Check if it is an HTTP GET command packet */
    if ((len >= 5) && (p[0] == 'G') && (p[1] == 'E') && (p[2] == 'T') && (p[3] == ' ') && (p[4] == '/'))
    {
        /* Sends the HTTP HTML response header */
        Err = netconn_write(conn, htmlHeader, sizeof(htmlHeader) - 1, NETCONN_COPY);
        if (Err != ERR_OK)
        {
            printf("netconn_write error, err code: %d\n", Err);
        }
        
        /* Sends HTTP HTML data */
        Err = netconn_write(conn, htmlData, SendLen, NETCONN_COPY);
        if (Err != ERR_OK)
        {
            printf("netconn_write error, err code: %d\n", Err);
        }
    }
    /* Check if it is an HTTP POST command packet */
    else if ((len >= 8) && (p[0] == 'P') && (p[1] == 'O') && (p[2] == 'S') && (p[3] == 'T'))
    {
        /* Request to turn on LED1: POST /led1on */
        if ((p[9] == '1') && (p[10] == 'o') && (p[11] == 'n'))
        {
            /* Currently LED2 is turned on */
            if (ledFlag.LED2_On)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "blue", "led1off", "OFF", "led2off", "OFF"));
                ledFlag.LED1_On  = 1;
                ledFlag.LED1_Off = 0;
                ledFlag.LED2_On  = 1;
                ledFlag.LED2_Off = 0;
                HTTP_LED1_ON;
            }
            /* Currently LED2 is turned off */
            else if (ledFlag.LED2_Off)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "#fff", "led1off", "OFF", "led2on", "ON"));
                ledFlag.LED1_On  = 1;
                ledFlag.LED1_Off = 0;
                ledFlag.LED2_On  = 0;
                ledFlag.LED2_Off = 1;
                HTTP_LED1_ON;
            }
        }
        /* Request to turn off LED1: POST /led1off */
        else if ((p[9] == '1') && (p[10] == 'o') && (p[11] == 'f'))
        {
            /* Currently LED2 is turned on */
            if (ledFlag.LED2_On)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "blue", "led1on", "ON", "led2off", "OFF"));
                ledFlag.LED1_On  = 0;
                ledFlag.LED1_Off = 1;
                ledFlag.LED2_On  = 1;
                ledFlag.LED2_Off = 0;
                HTTP_LED1_OFF;
            }
            /* Currently LED2 is turned off */
            else if (ledFlag.LED2_Off)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "#fff", "led1on", "ON", "led2on", "ON"));
                ledFlag.LED1_On  = 0;
                ledFlag.LED1_Off = 1;
                ledFlag.LED2_On  = 0;
                ledFlag.LED2_Off = 1;
                HTTP_LED1_OFF;
            }
        }
        /* Request to turn on LED2: POST /led2on */
        else if ((p[9] == '2') && (p[10] == 'o') && (p[11] == 'n'))
        {
            /* Currently LED1 is turned on */
            if (ledFlag.LED1_On)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "blue", "led1off", "OFF", "led2off", "OFF"));
                ledFlag.LED1_On  = 1;
                ledFlag.LED1_Off = 0;
                ledFlag.LED2_On  = 1;
                ledFlag.LED2_Off = 0;
                HTTP_LED2_ON;
            }
            /* Currently LED1 is turned off */
            else if (ledFlag.LED1_Off)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "blue", "led1on", "ON", "led2off", "OFF"));
                ledFlag.LED1_On  = 0;
                ledFlag.LED1_Off = 1;
                ledFlag.LED2_On  = 1;
                ledFlag.LED2_Off = 0;
                HTTP_LED2_ON;
            }
        }
        /* Request to turn off LED2: POST /led2off */
        else if ((p[9] == '2') && (p[10] == 'o') && (p[11] == 'f'))
        {
            /* Currently LED1 is turned on */
            if (ledFlag.LED1_On)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "#fff", "led1off", "OFF", "led2on", "ON"));
                ledFlag.LED1_On  = 1;
                ledFlag.LED1_Off = 0;
                ledFlag.LED2_On  = 0;
                ledFlag.LED2_Off = 1;
                HTTP_LED2_OFF;
            }
            /* Currently LED1 is turned off */
            else if (ledFlag.LED1_Off)
            {
                /* Packaged LEDs message data is then sent to the remote */
                SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "#fff", "led1on", "ON", "led2on", "ON"));
                ledFlag.LED1_On  = 0;
                ledFlag.LED1_Off = 1;
                ledFlag.LED2_On  = 0;
                ledFlag.LED2_Off = 1;
                HTTP_LED2_OFF;
            }
        }
        else
        {
            /* Do nothing */
        }
        
        /* Sends the HTTP HTML response header */
        Err = netconn_write(conn, htmlHeader, sizeof(htmlHeader) - 1, NETCONN_COPY);
        if (Err != ERR_OK)
        {
            printf("netconn_write error, err code: %d\n", Err);
        }
        
        /* Sends HTTP HTML data */
        Err = netconn_write(conn, htmlData, SendLen, NETCONN_COPY);
        if (Err != ERR_OK)
        {
            printf("netconn_write error, err code: %d\n", Err);
        }
    }
}

/**
*\*\name    http_server_netconn_thread.
*\*\fun     HTTP server task entry function.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void http_server_thread(void *arg)
{
    err_t Err;
    struct netconn *Conn = NULL;
    struct netconn *NewConn = NULL;
    
    u16_t Len;
    char *pData = NULL;
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
            Err = netconn_recv(NewConn, &pBuf);
            if (Err == ERR_OK)
            {
                /* Get the data pointer and length */
                netbuf_data(pBuf, (void **)&pData, &Len);
                
                /* Performs LED switching and submits relevant information to the web page */
                http_led_control_task(NewConn, pData, Len);
                
                /* Close connection */
                netconn_close(NewConn);
                /* Clears the allocated pBuf */
                netbuf_delete(pBuf);
            }
            /* Discard connection identifier */
            netconn_delete(NewConn);
        }
        else
        {
            printf("netconn_accept err, err code: %d\n", Err);
            /* Close connection */
            netconn_close(Conn);
            /* Discard connection identifier */
            netconn_delete(Conn);
        }
    }
}

/**
*\*\name    http_html_initial_package.
*\*\fun     Packs the initial HTML page data based on the state of the LEDs.
*\*\param   GPIOx
*\*\param   Pin
*\*\return  none
**/
static void http_html_initial_package(void)
{
    memset(htmlData, 0, sizeof(htmlData));
    memset(&ledFlag, 0, sizeof(ledFlag));
    /* LEDs initial state: LED1 on, LEN2 on */
    if (IS_HTTP_LED1_ON && IS_HTTP_LED2_ON)
    {
        /* Record LEDs status flag */
        ledFlag.LED1_On = 1;
        ledFlag.LED2_On = 1;
        /* Packaged LEDs message data is then sent to the remote */
        SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "blue", "led1off", "OFF", "led2off", "OFF"));
    }
    /* LEDs initial state: LED1 on, LEN2 off */
    else if (IS_HTTP_LED1_ON && IS_HTTP_LED2_OFF)
    {
        /* Record LEDs status flag */
        ledFlag.LED1_On  = 1;
        ledFlag.LED2_Off = 1;
        /* Packaged LEDs message data is then sent to the remote */
        SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("red", "#fff", "led1off", "OFF", "led2on", "ON"));
    }
    /* LEDs initial state: LED1 off, LEN2 on */
    else if (IS_HTTP_LED1_OFF && IS_HTTP_LED2_ON)
    {
        /* Record LEDs status flag */
        ledFlag.LED1_Off = 1;
        ledFlag.LED2_On  = 1;
        /* Packaged LEDs message data is then sent to the remote */
        SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "blue", "led1on", "ON", "led2off", "OFF"));
    }
    /* LEDs initial state: LED1 off, LEN2 off */
    else if (IS_HTTP_LED1_OFF && IS_HTTP_LED2_OFF)
    {
        /* Record LEDs status flag */
        ledFlag.LED1_Off = 1;
        ledFlag.LED2_Off = 1;
        /* Packaged LEDs message data is then sent to the remote */
        SendLen = snprintf(htmlData, sizeof(htmlData), HTTP_RESPONSE_FORMAT("#fff", "#fff", "led1on", "ON", "led2on", "ON"));
    }
    else
    {
        /* Do nothing */
    }
}

/**
*\*\name    http_led_gpio_init.
*\*\fun     Configures LED GPIO.
*\*\param   GPIOx
*\*\param   Pin
*\*\return  none
**/
static void http_led_gpio_init(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    }
    else if (GPIOx == GPIOD)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    }
    else
    {
        /* Do nothing */
    }
    /* Configure the GPIO pin */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
}

/**
*\*\name    http_led_control_init.
*\*\fun     Initialize HTTP web control LEDs.
*\*\param   none
*\*\return  none
**/
void http_led_control_init(void)
{
    /*
    LED2: PB4 is multiplexed with the debug interface function,
    configuration to enable SWD does not affect the LED2 function.
    */
    GPIO_ConfigPinRemap(0, 0, GPIO_RMP_SWJ_SWD);
    
    /* GPIO initialization for LED1 */
    http_led_gpio_init(HTTP_LED1_GPIO, HTTP_LED1_PIN);
    
    /* GPIO initialization for LED2 */
    http_led_gpio_init(HTTP_LED2_GPIO, HTTP_LED2_PIN);
    
    /* Users can turn on the LEDs here, they are off by default */
    HTTP_LED1_OFF;
    HTTP_LED2_ON;
    
    /* Packing initial data */
    http_html_initial_package();
    
    /* Create a HTTP server task thread */
    sys_thread_new("http_server_thread",        /* Task name */
                   http_server_thread,          /* Task entry function */
                   NULL,                        /* Task entry function parameter */
                   HTTP_SERVER_TASK_STACK_SIZE, /* Task stack size */
                   HTTP_SERVER_TASK_PRIORITY);  /* Task priority */
}

