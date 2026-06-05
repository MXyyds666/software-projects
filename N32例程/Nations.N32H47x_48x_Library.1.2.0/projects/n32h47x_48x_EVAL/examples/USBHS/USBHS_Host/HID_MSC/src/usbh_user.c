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
*\*\file usbh_user.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "usbh_user.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keyboard.h"
#include "usb_host_user_print.h"
#include "n32h47x_48x_gpio.h"
#include "log.h"
#include "usbh_msc_core.h"
#include "usbh_msc_bot.h"
#include "string.h"
#include "ff.h"       /* FATFS */

extern USBH_HOST USB_Host;
extern USB_CORE_MODULE USBHS_Core;
extern MassStorageParameter_TypeDef USBH_MSC_Param;

uint8_t USB_HOST_USER_AppState = USH_USER_FS_INIT;
uint8_t filenameString[15]  = {0u};
FATFS fatfs;
FIL file;

uint8_t line_idx = 0u;


#define KYBRD_FIRST_COLUMN               (uint16_t)319
#define KYBRD_LAST_COLUMN                (uint16_t)7
#define KYBRD_FIRST_LINE                 (uint8_t)120
#define KYBRD_PAGE_LINE                  (uint8_t)50
#define KYBRD_LAST_LINE                  (uint8_t)200

uint8_t KeybrdCharXpos = 0;
uint16_t KeybrdCharYpos = 0;
extern int16_t x_loc, y_loc;
extern __IO int16_t prev_x, prev_y;

USBH_User_cb_TypeDef USER_cb = 
{
    USBH_USER_Init,
    USBH_USER_DeInit,
    USBH_USER_DeviceAttached,
    USBH_USER_ResetDevice,
    USBH_USER_DeviceDisconnected,
    USBH_USER_OverCurrentDetected,
    USBH_USER_DeviceSpeedDetected,
    USBH_USER_Device_DescAvailable,
    USBH_USER_DeviceAddressAssigned,
    USBH_USER_Configuration_DescAvailable,
    USBH_USER_Manufacturer_String,
    USBH_USER_Product_String,
    USBH_USER_SerialNum_String,
    USBH_USER_EnumerationDone,
    USBH_USER_UserInput,
    USBH_USER_MSC_App,
    USBH_USER_DeviceNotSupported,
    USBH_USER_UnrecoveredError
};

/*--------------- Print Messages ---------------*/
uint8_t MSG_HOST_INIT[] = "> Host Library Initialized\n";
uint8_t MSG_DEV_ATTACHED[] = "> Device Attached\n";
uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
uint8_t MSG_DEV_ENUMERATED[] = "> Enumeration completed\n";
uint8_t MSG_DEV_HIGHSPEED[] = "> High speed device detected\n";
uint8_t MSG_DEV_FULLSPEED[] = "> Full speed device detected\n";
uint8_t MSG_DEV_LOWSPEED[] = "> Low speed device detected\n";
uint8_t MSG_DEV_ERROR[] = "> Device fault \n";

uint8_t MSG_MSC_CLASS[] = "> Mass storage device connected\n";
uint8_t MSG_HID_CLASS[] = "> HID device connected\n";

uint8_t USB_HID_MouseStatus[] = "> HID Demo Device : Mouse\n";
uint8_t USB_HID_KeybrdStatus[] = "> HID Demo Device : Keyboard\n";

uint8_t MSG_DISK_SIZE[] = "> Size of the disk in MBytes: \n";
uint8_t MSG_LUN[] = "> LUN Available in the device:\n";
uint8_t MSG_ROOT_CONT[] = "> Exploring disk flash ...\n";
uint8_t MSG_WR_PROTECT[] = "> The disk is write protected\n";
uint8_t MSG_UNREC_ERROR[] = "> UNRECOVERED ERROR STATE\n";

static uint8_t Explore_Disk (char* path , uint8_t recu_level);


/**
*\*\name    USBH_USER_Init.
*\*\fun     Printf the message for host lib initialization
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_Init(void)
{
    static uint8_t startup = 0;

    if (startup == 0)
    {
        startup = 1;
        log_info("> USB Host library started.\r\n");
    }
}

/**
*\*\name    USBH_USER_DeviceAttached.
*\*\fun     Printf the message for host lib initialization
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_DeviceAttached(void)
{
    log_info((void *)MSG_DEV_ATTACHED);
}

/**
*\*\name    USBH_USER_UnrecoveredError.
*\*\fun     Printf the message of unrecover error
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_UnrecoveredError(void)
{
    log_info((void *)MSG_UNREC_ERROR);
}

/**
*\*\name    USBH_USER_DeviceDisconnected.
*\*\fun     Printf the message of device disconnect event
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_DeviceDisconnected(void)
{
    log_info((void *)MSG_DEV_DISCONNECTED);
}

/**
*\*\name    USBH_USER_ResetDevice.
*\*\fun     Reset USB Device
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_ResetDevice(void)
{
  /* Users can do their application actions here for the USB-Reset */
}

/**
*\*\name    USBH_USER_DeviceSpeedDetected.
*\*\fun     Print the message for device speed
*\*\param   Devicespeed : Device Speed.
*\*\return  none.
*\*\
**/
void USBH_USER_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
    if (DeviceSpeed == HPCS_PRTSPD_HIGH_SPEED)
    {                                                
        log_info((void *)MSG_DEV_HIGHSPEED);
    }
    else if (DeviceSpeed == HPCS_PRTSPD_FULL_SPEED)
    {
        log_info((void *)MSG_DEV_FULLSPEED);
    }
    else if (DeviceSpeed == HPCS_PRTSPD_LOW_SPEED)
    {
        log_info((void *)MSG_DEV_LOWSPEED);
    }
    else
    {
        log_info((void *)MSG_DEV_ERROR);
    }
}

/**
*\*\name    USBH_USER_Device_DescAvailable.
*\*\fun     Print the message for device speed
*\*\param   Devicespeed : device descriptor.
*\*\return  none.
*\*\
**/
void USBH_USER_Device_DescAvailable(void *DeviceDesc)
{
    uint8_t temp[50];
    USBH_DevDesc_TypeDef *hs;
    hs = DeviceDesc;

    sprintf((char *)temp, "VID : %04lXh\n", (uint32_t) (*hs).idVendor);
    log_info((char *)temp);
    sprintf((char *)temp, "PID : %04lXh\n", (uint32_t) (*hs).idProduct);
    log_info((char *)temp); 
}

/**
*\*\name    USBH_USER_DeviceAddressAssigned.
*\*\fun     USB device is successfully assigned the Address
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_DeviceAddressAssigned(void)
{

}

/**
*\*\name    USBH_USER_Configuration_DescAvailable.
*\*\fun     Print the message for configuration descriptor
*\*\param   ConfDesc : Configuration descriptor.
*\*\param   itfDesc : interface descriptor.
*\*\param   epDesc : Endpoint descriptor.
*\*\return  none.
*\*\
**/
void USBH_USER_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc, USBH_InterfaceDesc_TypeDef * itfDesc, USBH_EpDesc_TypeDef * epDesc)
{
    USBH_InterfaceDesc_TypeDef *id;

    id = itfDesc;

    if ((*id).bInterfaceClass == 0x08)
    {
        log_info((void *)MSG_MSC_CLASS);
        USB_Host.class_cb = &USBH_MSC_cb;
        USB_HOST_USER_AppState = USH_USER_FS_INIT;
    }
    else if ((*id).bInterfaceClass == 0x03)
    {
        log_info((void *)MSG_HID_CLASS);
        USB_Host.class_cb = &USBH_HID_cb;
    }
    else
    {
        log_info("USB Class NOT supported");
    }
}

/**
*\*\name    USBH_USER_Manufacturer_String.
*\*\fun     Print the message for Manufacturer String
*\*\param   ManufacturerString : Manufacturer String of Device
*\*\return  none.
*\*\
**/
void USBH_USER_Manufacturer_String(void *ManufacturerString)
{
    char temp[100];
    sprintf(temp, "Manufacturer : %s\n", (char *)ManufacturerString);
    log_info((char *)temp);

}

/**
*\*\name    USBH_USER_Product_String.
*\*\fun     Print the message for Product String
*\*\param   ProductString : Product String of Device
*\*\return  none.
*\*\
**/
void USBH_USER_Product_String(void *ProductString)
{
    char temp[100];
    sprintf((char *)temp, "Product : %s\n", (char *)ProductString);
    log_info((char *)temp);
}

/**
*\*\name    USBH_USER_SerialNum_String.
*\*\fun     Print the message for SerialNum String
*\*\param   SerialNumString : SerialNum_String of device
*\*\return  none.
*\*\
**/
void USBH_USER_SerialNum_String(void *SerialNumString)
{
    uint8_t temp[100];
    sprintf((char *)temp, "Serial Number : %s\n", (char *)SerialNumString);
    log_info((char *)temp);
}

/**
*\*\name    USBH_USER_EnumerationDone.
*\*\fun     Print the message for Enumeration complete
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_EnumerationDone(void)
{
    /* Enumeration complete */
    log_info((void *)MSG_DEV_ENUMERATED);
}

/**
*\*\name    USBH_USER_DeviceNotSupported.
*\*\fun     Print the message for Device is not supported
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_DeviceNotSupported(void)
{
    log_info("> Device not supported.\n");

}

/**
*\*\name    USBH_USER_UserInput.
*\*\fun     User Action for application state entry
*\*\param   none.
*\*\return  USBH_USER_Status : User response for key button.
*\*\
**/
USBH_USER_Status USBH_USER_UserInput(void)
{
    USBH_USER_Status USBH_USER_status;

    USBH_USER_status = USBH_USER_NO_RESP;

    /* Key button is in polling mode to detect user action */
    if (KEY_Press_Status_Get(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN))
    {
        USBH_USER_status = USBH_USER_RESP_OK;
    }

    return USBH_USER_status;
}

/**
*\*\name    USBH_USER_OverCurrentDetected.
*\*\fun     Print the message for Device Overcurrent detection event
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_OverCurrentDetected(void)
{
    log_info("Overcurrent detected.\n");

}

/**
*\*\name    USER_MOUSE_Init.
*\*\fun     Init Mouse window
*\*\param   none.
*\*\return  none.
*\*\
**/
void USER_MOUSE_Init(void)
{
    log_info((void *)USB_HID_MouseStatus);
    log_info("\n\n\n\n\n\n\n\n");

    Mouse_ButtonRelease(0);
    Mouse_ButtonRelease(1);
    Mouse_ButtonRelease(2);
}

/**
*\*\name    USER_MOUSE_ProcessData.
*\*\fun     Process Mouse data
*\*\param   data : Mouse data to be displayed.
*\*\return  none.
*\*\
**/
void USER_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef * data)
{
    uint8_t idx = 1;
    static uint8_t b_state[3] = { 0, 0, 0 };

    if ((data->x != 0) && (data->y != 0))
    {
        Mouse_PositionUpdate(data->x, data->y);
    }

    for (idx = 0; idx < 3; idx++)
    {
        if (data->button & 1 << idx)
        {
            if (b_state[idx] == 0)
            {
                Mouse_ButtonPress(idx);
                b_state[idx] = 1;
            }
        }
        else
        {
            if (b_state[idx] == 1)
            {
                Mouse_ButtonRelease(idx);
                b_state[idx] = 0;
            }
        }
    }


}

/**
*\*\name    USER_KEYBRD_Init.
*\*\fun     Init Keyboard window
*\*\param   none.
*\*\return  none.
*\*\
**/
void USER_KEYBRD_Init(void)
{
    log_info((void *)USB_HID_KeybrdStatus);
    log_info("> Use Keyboard to tape characters: \n\n");
    log_info("\n\n\n\n\n\n");
}

/**
*\*\name    USER_KEYBRD_ProcessData.
*\*\fun     Process Keyboard data
*\*\param   data : Keyboard data to be displayed
*\*\return  none.
*\*\
**/
void USER_KEYBRD_ProcessData(uint8_t data)
{
    log_info("%c", (char)data);
}

/**
*\*\name    USBH_USER_MSC_App.
*\*\fun     Init Mouse window
*\*\param   none.
*\*\return  none.
*\*\
**/
int USBH_USER_MSC_App(void)
{
    FRESULT res;
    uint8_t writeTextBuff[] = "Nations Connectivity line Host Demo application using FAT_FS   ";
    uint16_t bytesWritten, bytesToWrite;
    uint32_t u32DevConnectTmp = 0ul;
    uint64_t capacity;
    switch(USB_HOST_USER_AppState)
    {
        case USH_USER_FS_INIT:
            if (f_mount(&fatfs, "", 0u) != FR_OK )//register the work area of the volume
            {
                log_info("> Cannot initialize File System.\n");
                return(-1);
            }
            log_info("> File System initialized.\n");
            capacity = (uint64_t)(USBH_MSC_Param.MSCapacity) * (uint64_t)(USBH_MSC_Param.MSPageLength);
            log_info("> Disk capacity : %d * %d = %llu Bytes\n", USBH_MSC_Param.MSCapacity, USBH_MSC_Param.MSPageLength, capacity);
            if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
            {
                log_info((void *)MSG_WR_PROTECT);
            }

            USB_HOST_USER_AppState = USH_USER_FS_READLIST;
            break;

        case USH_USER_FS_READLIST:
            log_info((void *)MSG_ROOT_CONT);
            Explore_Disk("0:/", 1u);
            line_idx = 0u;
            USB_HOST_USER_AppState = USH_USER_FS_WRITEFILE;
            break;

        case USH_USER_FS_WRITEFILE:
            log_info("Press USER KEY to write file\n");
            USB_BSP_mDelay(100ul);

            /*Key in polling*/
            u32DevConnectTmp = HOST_IsDeviceConnected(&USBHS_Core);
            while((GPIO_ReadInputDataBit(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN) == SET) && u32DevConnectTmp)
            {
            }
            /* Writes a text file, N32.TXT in the disk*/
            log_info("> Writing File to disk flash ...\n");
            if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
            {
                log_info ( "> Disk flash is write protected \n");
                USB_HOST_USER_AppState = USH_USER_FS_IDLE;
                break;
            }

            /* Register work area for logical drives */
            f_mount(&fatfs, "", 0u);

            if(f_open(&file, "0:N32.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
            {
                /* Write buffer to file */
                bytesToWrite = (uint16_t)sizeof(writeTextBuff);
                res= f_write (&file, writeTextBuff, (UINT)bytesToWrite, (void *)&bytesWritten);

                if((bytesWritten == 0u) || (res != FR_OK)) /*EOF or Error*/
                {
                    log_info("> N32.TXT CANNOT be writen.\n");
                }
                else
                {
                    log_info("> 'N32.TXT' file created\n");
                }

                /*close file and filesystem*/
                f_close(&file);
                f_mount(NULL, "", 0u);
            }
            else
            {
                log_info ("> N32.TXT created in the disk\n");
            }

            USB_HOST_USER_AppState = USH_USER_FS_IDLE;
            break;

        case USH_USER_FS_IDLE:
            break;
        default: break;
    }
    return((int)0);
}

/**
*\*\name    Explore_Disk.
*\*\fun     explore disk
*\*\param   path: file path.
*\*\param   recu_level: level.
*\*\return  none.
*\*\
**/
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;
    char tmp[14];

    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        while(HOST_IsDeviceConnected(&USBHS_Core))
        {
            res = f_readdir(&dir, &fno);
            if ((res != FR_OK )|| (fno.fname[0] == 0))
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
            if(line_idx > 9u)
            {
                line_idx = 0u;
                log_info("Press USER KEY to continue...\r\n");

                /*Key in polling*/
                while((GPIO_ReadInputDataBit(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN) == SET) && HOST_IsDeviceConnected(&USBHS_Core))
                {
                }
            }

            if(recu_level == 1u)
            {
                log_info("   |__");
            }
            else if(recu_level == 2u)
            {
                log_info("   |   |__");
            }
            if((fno.fattrib & AM_DIR) == AM_DIR)
            {
                strcat(tmp, "\n");
                log_info((void *)tmp);
            }
            else
            {
                strcat(tmp, "\n");
                log_info((void *)tmp);
            }

            if(((fno.fattrib & AM_DIR) == AM_DIR)&&(recu_level == 1u))
            {
                Explore_Disk(fn, 2u);
            }
        }
    }
    return res;
}

/**
*\*\name    USBH_USER_DeInit.
*\*\fun     Deinit User state and associated variables
*\*\param   none.
*\*\return  none.
*\*\
**/
void USBH_USER_DeInit(void)
{
}
