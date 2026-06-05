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
*\*\file dual_role_app.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include <string.h>
#include "log.h"
#include "dual_role_app.h"
#include "usbh_core.h"
#include "usbh_user.h"
#include "usbd_user.h"
#include "usbd_desc.h"
#include "usbh_msc_core.h"
#include "usbd_msc_core.h"
#include "usbhs_conf.h"
#include "ff.h"       /* FATFS */
#include "n32h47x_48x_gpio.h"


#define IMAGE_BUFFER_SIZE  512

#define BF_TYPE 0x4D42          /* "MB" */

#define BI_RGB       0          /* No compression - straight BGR data */
#define BI_RLE8      1          /* 8-bit run-length compression */
#define BI_RLE4      2          /* 4-bit run-length compression */
#define BI_BITFIELDS 3          /* RGB bitmap with RGB masks */

extern uint8_t USB_HOST_USER_AppState;
uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
extern MassStorageParameter_TypeDef USBH_MSC_Param; 

uint8_t filenameString[15] = { 0 };
FATFS fatfs;
FIL file;
DRD_StateMachine drd;
uint8_t line_idx = 0;
__IO uint8_t wait_user_input = 0;
uint8_t Enum_Done = 0;
uint8_t *DRD_main_menu[] = {
  (uint8_t *)
    "      1 - Host Demo                                                          \r\n",
  (uint8_t *)
    "      2 - Device Demo                                                        \r\n",
  (uint8_t *)
    "      3 - Credits                                                            \r\n\r\n",
};

uint8_t *DRD_HOST_menu[] = {
  (uint8_t *)
    "      1 - Explore Flash content                                              \r\n",
  (uint8_t *)
    "      2 - Write File to disk                                                 \r\n",
  (uint8_t *)
    "      3 - Read File from disk                                                \r\n",
  (uint8_t *)
    "      4 - Return                                                             \r\n",
};

uint8_t *DRD_DEVICE_menu[] = {
  (uint8_t *)
    "      1 - Return                                                             \r\n",
  (uint8_t *)
    "                                                                             \r\n",
  (uint8_t *)
    "                                                                             \r\n",
};

uint8_t writeTextBuff[] = "N32 Connectivity line Host Demo application using FAT_FS ";
uint8_t *readTextBuff[sizeof(writeTextBuff)] = {0};

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_CORE_MODULE USBHS_Core __ALIGN_END;

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;
#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_CORE_MODULE USBHS_Core __ALIGN_END;

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST USB_FS_Host __ALIGN_END;


static void DRD_Application(void);
static void DRD_SelectItem(uint8_t ** menu, uint8_t item);
static uint8_t Explore_Disk(char *path, uint8_t recu_level);
static void Toggle_Leds(void);

/**
*\*\name    DRD_Init.
*\*\fun     Dual role Demo initialization.
*\*\param   none
*\*\return  none.
**/
void DRD_Init(void)
{
    log_info("> Initializing dual role demo....\r\n");
    DRD_SelectItem(DRD_main_menu, 0);
    log_info("> Demo Initialized \r\n");
    log_info("> Use KeyButton to Select dual role run mode.\r\n");
    USB_BSP_Init();
    USBH_Init(&USBHS_Core, &USB_Host, &USBH_MSC_cb, &USER_Host_MSC_cb);

    USB_BSP_mDelay(100);
    
    DRD_UNLOCK();
}

/**
*\*\name    DRD_Process.
*\*\fun     Dual role demo task.
*\*\param   none
*\*\return  none.
**/
void DRD_Process(void)
{
    if (drd.state == DRD_HOST)
    {
        if (HOST_IsDeviceConnected(&USBHS_Core))
        {
            /* Host Task handler */
            USBH_Main_Process(&USBHS_Core, &USB_Host);
        }
    }
    DRD_Application();
}

/**
*\*\name    DRD_ProbeKey.
*\*\fun     Probe the key button state
*\*\param   state : key button state
*\*\return  none.
**/
void DRD_ProbeKey(Button_TypeDef state)
{
    /* Explorer running */
    if ((wait_user_input == 1) && (state != BUTTON_NULL))
    {
        wait_user_input = 2;
        return;
    }

    /* Critical process ongoing : do not interrupt */
    if (DRD_IS_LOCKED())
    {
        return;
    }

    if ((state == BUTTON_UP) && (drd.select > 0))
    {
        drd.select--;
    }
    else if (((state == BUTTON_DOWN) && (drd.select < 2)) ||
       ((drd.state == DRD_HOST) && (state == BUTTON_DOWN) &&
        (drd.select < 3)))
    {

        if (!((drd.state == DRD_DEVICE) && drd.select == 0))
        {
            drd.select++;
        }
    }
    else if (state == BUTTON_SEL)
    {
        drd.select |= 0x80;
    }
}

/**
*\*\name    DisplayStringLine.
*\*\fun     printf string
*\*\param   ptr : string pointer
*\*\return  none.
**/
void DisplayStringLine(uint8_t *ptr)
{
    /* Send the string character by character */
    log_info((char *)ptr);
}

/**
*\*\name    CDC_SelectItem.
*\*\fun     manage the menu
*\*\param   menu: menu table
*\*\param   item : selected item to be highlighted
*\*\return  0.
*\*\
**/
static void DRD_SelectItem(uint8_t ** menu, uint8_t item)
{
    switch (item)
    {
        case 0:
            log_info(" -->");
            DisplayStringLine(menu[0]);
            DisplayStringLine(menu[1]);
            DisplayStringLine(menu[2]);
            break;
        
        case 1:
            DisplayStringLine(menu[0]);
            log_info(" -->");
            DisplayStringLine(menu[1]);
            DisplayStringLine(menu[2]);
            break;
        
        case 2:
            DisplayStringLine(menu[0]);
            DisplayStringLine(menu[1]);
            log_info(" -->");
            DisplayStringLine(menu[2]);
            break;
        
        case 3:
            DisplayStringLine(menu[1]);
            DisplayStringLine(menu[2]);
            log_info(" -->");
            DisplayStringLine(menu[3]);
            break;
        
        case 4:
            DisplayStringLine(menu[2]);
            DisplayStringLine(menu[3]);
            log_info(" -->");
            DisplayStringLine(menu[4]);
            break;
        
        default:
            DisplayStringLine(menu[0]);
            DisplayStringLine(menu[1]);
            DisplayStringLine(menu[2]);
            break;
    }
}

/**
*\*\name    Key_ReadIOPin_continuous.
*\*\fun     Key status read function.
*\*\param   none.
*\*\return  button id.
*\*\
**/
Button_TypeDef Key_ReadIOPin_continuous(void)
{
    Button_TypeDef enKey = BUTTON_NULL;
    Button_TypeDef present_key = BUTTON_NULL;
    
    if(KEY_Press_Status_Get(KEY_BUTTON_UP_PORT, KEY_BUTTON_UP_PIN))
    {
        enKey = BUTTON_UP;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_DOWN_PORT, KEY_BUTTON_DOWN_PIN))
    {
        enKey = BUTTON_DOWN;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_LEFT_PORT, KEY_BUTTON_LEFT_PIN))
    {
        enKey = BUTTON_LEFT;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_RIGHT_PORT, KEY_BUTTON_RIGHT_PIN))
    {
        enKey = BUTTON_RIGHT;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_SEL_PORT, KEY_BUTTON_SEL_PIN))
    {
        enKey = BUTTON_SEL;
    }
    else
    {
        enKey = BUTTON_NULL;
    }
    
    if(enKey != BUTTON_NULL)
    {
        present_key = enKey;
        
        USB_BSP_mDelay(20);
        
        if(KEY_Press_Status_Get(KEY_BUTTON_UP_PORT, KEY_BUTTON_UP_PIN))
        {
            enKey = BUTTON_UP;
        }
        else if(KEY_Press_Status_Get(KEY_BUTTON_DOWN_PORT, KEY_BUTTON_DOWN_PIN))
        {
            enKey = BUTTON_DOWN;
        }
        else if(KEY_Press_Status_Get(KEY_BUTTON_LEFT_PORT, KEY_BUTTON_LEFT_PIN))
        {
            enKey = BUTTON_LEFT;
        }
        else if(KEY_Press_Status_Get(KEY_BUTTON_RIGHT_PORT, KEY_BUTTON_RIGHT_PIN))
        {
            enKey = BUTTON_RIGHT;
        }
        else if(KEY_Press_Status_Get(KEY_BUTTON_SEL_PORT, KEY_BUTTON_SEL_PIN))
        {
            enKey = BUTTON_SEL;
        }
        else
        {
            enKey = BUTTON_NULL;
        }
        
        if(present_key != enKey)
        {
            enKey = BUTTON_NULL;
        }
    }
    if(enKey != BUTTON_NULL)
    {
        while(1)
        {
            if(KEY_Press_Status_Get(KEY_BUTTON_UP_PORT, KEY_BUTTON_UP_PIN))
            {
                present_key = BUTTON_UP;
            }
            else if(KEY_Press_Status_Get(KEY_BUTTON_DOWN_PORT, KEY_BUTTON_DOWN_PIN))
            {
                present_key = BUTTON_DOWN;
            }
            else if(KEY_Press_Status_Get(KEY_BUTTON_LEFT_PORT, KEY_BUTTON_LEFT_PIN))
            {
                present_key = BUTTON_LEFT;
            }
            else if(KEY_Press_Status_Get(KEY_BUTTON_RIGHT_PORT, KEY_BUTTON_RIGHT_PIN))
            {
                present_key = BUTTON_RIGHT;
            }
            else if(KEY_Press_Status_Get(KEY_BUTTON_SEL_PORT, KEY_BUTTON_SEL_PIN))
            {
                present_key = BUTTON_SEL;
            }
            else
            {
                present_key = BUTTON_NULL;
                break;
            }
        }
    }

    return enKey;
}

/**
*\*\name    DRD_Application.
*\*\fun     Dual role demo state machine
*\*\param   ptr : string pointer
*\*\return  none.
**/
static void DRD_Application(void)
{
    static uint8_t prev_select = 0;
    uint16_t bytesWritten, bytesToWrite, bytesRead, bytesToRead;
    FRESULT res;

    DRD_ProbeKey(Key_ReadIOPin_continuous());
    
    switch (drd.state)
    {
        case DRD_IDLE:
            DRD_SelectItem(DRD_main_menu, 0);
            drd.state = DRD_WAIT;
            drd.select = 0;
            break;
        
        case DRD_WAIT:
            if (drd.select != prev_select)
            {
                if((drd.select & 0x7F) == 0x02)
                {
                    drd.select = 0x00;
                }
                prev_select = drd.select;
                DRD_SelectItem(DRD_main_menu, drd.select & 0x7F);
                /* Handle select item */
                if (drd.select & 0x80)
                {
                    drd.select &= 0x7F;
                    switch (drd.select)
                    {
                        case 0:
                            drd.state = DRD_HOST;
                            drd.Host_state = DRD_HOST_IDLE;
                            break;

                        case 1:
                            drd.state = DRD_DEVICE;
                            drd.Device_state = DRD_DEVICE_IDLE;
                            break;

                        case 2:
                            drd.state = DRD_IDLE;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        case DRD_HOST:
            switch (drd.Host_state)
            {
                case DRD_HOST_IDLE:
                    DRD_LOCK();
                    /* Init HS Core : Demo start in host mode */
                    log_info("> Initializing USB Host High speed...\r\n");

                    USBH_Init(&USBHS_Core, &USB_Host, &USBH_MSC_cb, &USER_Host_MSC_cb);

                    drd.Host_state = DRD_HOST_WAIT;
                    DRD_UNLOCK();
                    break;

                case DRD_HOST_WAIT:
                    if (!HOST_IsDeviceConnected(&USBHS_Core))
                    {
                        DRD_HandleDisconnect();
                        log_info("Please, connect a device and try again.\r\n");
                    }
                    if (Enum_Done == 1)
                    {
                        log_info("> USB Host High speed initialized.\r\n");
                        /* Initialises the File System */
                        if (f_mount(&fatfs, "", 0) != FR_OK)
                        {
                            /* efs initialisation fails */
                            log_info("> Cannot initialize File System.\r\n");
                        }
                        log_info("> File System initialized.\r\n");
                        log_info("> Disk capacity : %lu Bytes\r\n", USBH_MSC_Param.MSCapacity * USBH_MSC_Param.MSPageLength);
                        DRD_SelectItem(DRD_HOST_menu, 0);
                        drd.select = 0;
                        Enum_Done = 2;
                    }
                    if (Enum_Done == 2)
                    {
                        if (drd.select != prev_select)
                        {
                            prev_select = drd.select;
                            USB_EnableGlobalInt(&USBHS_Core, DISABLE);
                            DRD_SelectItem(DRD_HOST_menu, drd.select & 0x7F);
                            USB_EnableGlobalInt(&USBHS_Core, ENABLE);

                            /* Handle select item */
                            if (drd.select & 0x80)
                            {
                                drd.select &= 0x7F;
                                switch (drd.select)
                                {
                                    case 0:
                                        DRD_LOCK();
                                        Explore_Disk("0:/", 1);
                                        line_idx = 0;
                                        DRD_UNLOCK();
                                        break;
                                    case 1:
                                        /* Writes a text file, N32.TXT in the disk */
                                        log_info("> Writing File to disk flash ...\r\n");
                                        if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
                                        {
                                            log_info("> Disk flash is write protected \r\n");
                                            USB_HOST_USER_AppState = USH_USER_FS_DRAW;
                                            break;
                                        }
                                        DRD_LOCK();
                                        /* Register work area for logical drives */
                                        f_mount(&fatfs, "", 0);

                                        if (f_open(&file, "0:N32.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
                                        {
                                            /* Write buffer to file */
                                            bytesToWrite = sizeof(writeTextBuff);
                                            res = f_write(&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);
                                            if ((bytesWritten == 0) || (res != FR_OK))  /* EOF or Error */
                                            {
                                                log_info("> N32.TXT CANNOT be writen.\r\n");
                                            }
                                            else
                                            {
                                                log_info("> 'N32.TXT' file created\r\n");
                                            }

                                            /* close file and filesystem */
                                            f_close(&file);
                                        }
                                        DRD_UNLOCK();
                                        break;
                                    case 2:
                                        if (f_mount(&fatfs, "", 0) != FR_OK)
                                        {
                                            /* fat_fs initialisation fails */
                                            break;
                                        }
                                        if (f_open(&file, "0:N32.TXT", FA_READ) == FR_OK)
                                        {
                                            /* Read buffer to file */
                                            bytesToRead = f_size(&file);
                                            res = f_read(&file, readTextBuff, bytesToRead, (void *)&bytesRead);
                                            if ((bytesRead == 0) || (res != FR_OK))  /* EOF or Error */
                                            {
                                                log_info("> N32.TXT CANNOT be read.\r\n");
                                            }
                                            else
                                            {
                                                log_info("> 'N32.TXT' file read\r\n");
                                                log_info((char *)readTextBuff);
                                                log_info("\r\n");
                                            }
                                            /* close file and filesystem */
                                            f_close(&file);
                                        }
                                        else
                                        {
                                            log_info("> Read file failed.\r\n");
                                        }
                                        break;

                                    case 3:
                                        DRD_HandleDisconnect();
                                        f_mount(NULL, "", 0);
                                        USB_StopHost(&USBHS_Core);

                                        /* Manage User disconnect operations */
                                        USB_Host.usr_cb->DeviceDisconnected();

                                        /* Re-Initilaize Host for new Enumeration */
                                        USBH_DeInit(&USBHS_Core, &USB_Host);
                                        USB_Host.usr_cb->DeInit();
                                        USB_Host.class_cb->DeInit(&USBHS_Core, &USB_Host.device_prop);
                                        log_info("> Use KeyButton to Select drd.\n");
                                        break;

                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            break;
                
        case DRD_DEVICE:
            switch (drd.Device_state)
            {
                case DRD_DEVICE_IDLE:
                    DRD_LOCK();
                    USBD_Init(&USBHS_Core, &USBD_desc, &USBD_MSC_cb, &USER_Device_MSC_cb);
                    drd.Device_state = DRD_DEVICE_WAIT;
                    drd.select = 0;
                    DRD_UNLOCK();
                    break;

                case DRD_DEVICE_WAIT:
                    if (drd.select != prev_select)
                    {
                        prev_select = drd.select;
                        DRD_SelectItem(DRD_DEVICE_menu, drd.select & 0x7F);
                        /* Handle select item */
                        if (drd.select & 0x80)
                        {
                            drd.select &= 0x7F;
                            switch (drd.select)
                            {
                                case 0:
                                    drd.state = DRD_IDLE;
                                    drd.select = 0;
                                    log_info("> Device application closed.\n");
                                    USBDEV_DevDisconnect(&USBHS_Core);
                                    USB_StopDevice(&USBHS_Core);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                break;
                default:
                    break;
            }
        break;
        default:
        break;
    }
}

/**
*\*\name    Explore_Disk.
*\*\fun     explore disk
*\*\param   path: file path.
*\*\param   recu_level: level.
*\*\return  none.
*\*\
**/
static uint8_t Explore_Disk(char *path, uint8_t recu_level)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;
    char tmp[14];


    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        while (HOST_IsDeviceConnected(&USBHS_Core))
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
            {
                break;
            }
            if (fno.fname[0] == '.')
            {
                continue;
            }
            fn = fno.fname;
            strcpy(tmp, fn);
            line_idx++;
            if (line_idx > 12)
            {
                line_idx = 0;
                wait_user_input = 1;
                log_info("Press USER KEY to continue...\n");
                while ((HOST_IsDeviceConnected(&USBHS_Core)) && (wait_user_input != 2))
                {
                    Toggle_Leds();
                    if(KEY_Press_Status_Get(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN))
                    {
                        while(KEY_Press_Status_Get(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN));
                        break;
                    }
                }
            }
            wait_user_input = 0;

            if (recu_level == 1)
            {
                log_info("   |__");
            }
            else if (recu_level == 2)
            {
                log_info("   |   |__");
            }
            if ((fno.fattrib & AM_MASK) == AM_DIR)
            {
                strcat(tmp, "\n");
                log_info((void *)tmp);
            }
            else
            {
                strcat(tmp, "\n");
                log_info((void *)tmp);
            }
            if (((fno.fattrib & AM_MASK) == AM_DIR) && (recu_level == 1))
            {
                Explore_Disk(fn, 2);
            }
        }
    }
    return res;
}

/**
*\*\name    DRD_HandleDisconnect.
*\*\fun     deinit demo and astart again the enumeration.
*\*\param   none.
*\*\return  none.
*\*\
**/
void DRD_HandleDisconnect(void)
{
    drd.state = DRD_IDLE;
    USBH_DeInit(&USBHS_Core, &USB_Host);
    Enum_Done = 0;
    DRD_UNLOCK();
}

/**
*\*\name    Toggle_Leds.
*\*\fun     Toggle leds to shows user input state.
*\*\param   none.
*\*\return  none.
*\*\
**/
static void Toggle_Leds(void)
{
    static uint32_t i;
    if (i++ == 0x10000u)
    {
        LED_Blink(LED1_PORT, LED1_PIN);
        i = 0u;
    }
}

