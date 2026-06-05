/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Flash.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_H
#define FLASH_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define PAGE_SIZE            0x800   //Flash每页大小每页2K
#define Max_PROGRAM_SIZE     0x68000 //最大程序大小 416K
#define MAX_BOOT_B_SIZE			 0x8000	 //最大大小32K

//flash 地址划分
#define ADD_BASE         	0x08000000   //Flash基地址
#define ID_INFO_Addr   		0x08006000   //ID信息 2K 在Bootloader后第一页
#define ADD_IAPDATA      	0x08006800   //iap相关数据存储起始地址 2K 在Booloader后第二页
#define HAREWARE_DATA			0x08007000	 //硬件配置，大小4K
#define BOOT_B_FORMAL			0x08008000	 //BOOTB起始地址 2026.4.16
#define ADD_FORMAL       	0x08010000  //正式程序起始地址，BooTloaderA大小位32K，BooTloaderB大小位32K
#define PARAMETE_DATA			0x08070000	 //参数存储	64K

//针对536进行修改


/* Exported types ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Flash_Read_Byte(u32 Address, u8* Readbuff, u32 Len);
u8 Flash_Write_Word(u32 Address, const u8* Writebuff, u32 Len);
u8 Flash_Prepare(u32 Address, u32 Len);

#endif 

/*******************************END OF FILE************************/
