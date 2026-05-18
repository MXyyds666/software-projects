/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Flash.c
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Flash_Read_Byte
* Description   : 读一个字节的数据
* Input         : Address：欲读取数据的起始地址 
                : Readbuff：数据存放的地址
								: Len: 要读取的字节数
* Output        : None
* Notes         :
******************************************************************/
void Flash_Read_Byte(u32 Address, u8* Readbuff, u32 Len)
{
	s32 i = 0;
	for(i=0;i<Len;i++)
	{
		Readbuff[i] = *(u8*)(Address+i);
	}
}

/*****************************************************************
* Function Name : Flash_Write_Word
* Description   : 按字长度写入数据
* Input         : Address：欲写入数据的起始地址 
                : Writebuff：数据存放的地址
			    : Len: 要写入的字节数
* Output        : 结果 0失败 1成功
* Notes         :
******************************************************************/
u8 Flash_Write_Word(u32 Address, const u8* Writebuff, u32 Len)
{
	u32 i = 0;
	
	flash_unlock();	
	for(i=0;i<Len;i+=4)
	{
		if(flash_word_program(Address+i, *(uint32_t*)(Writebuff+i)) != FLASH_OPERATE_DONE)
			break;
		if(*(u32*)(Address+i) != *(u32*)(Writebuff+i))
			break;
	}	
	flash_lock();
	
	if(i<Len)
		return 0;
	return 1;
}

/*****************************************************************
* Function Name : Flash_Prepare
* Description   : 擦除需要写入数据的Flash空间
* Input         : Address：欲擦除数据的起始地址 
			    : Len: 要擦除的字节数
* Output        : 结果 0失败 1成功
* Notes         :
******************************************************************/
u8 Flash_Prepare(u32 Address, u32 Len)
{
	u32 NbrOfPage = 0;
	u32 i = 0;
	
	flash_unlock();
	
	//计算页数
	NbrOfPage = (Len%PAGE_SIZE == 0? Len/PAGE_SIZE : Len/PAGE_SIZE+1);

	//擦除需要编程的页
	for(i=0; i<NbrOfPage; i++)
	{
		if(flash_sector_erase(Address + (PAGE_SIZE * i)) != FLASH_OPERATE_DONE)
			break;
	}
	flash_lock();
	
	if(i < NbrOfPage)
		return 1;
	return 0;
}

/*****************************************************************
* Function Name : Save_BOOT_Info
* Description   : bootload信息
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
u8 Save_ID_Info(uint32_t id)
{
	u8 s_result = 0;
	s_result = Flash_Prepare(ID_INFO_Addr, 4);
	if(!s_result)
		s_result = Flash_Write_Word(ID_INFO_Addr,(u8*)&id, 4);
	
	return s_result;
}

