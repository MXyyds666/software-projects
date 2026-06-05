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
*\*\file crc_dma.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/


#include "main.h"


DMA_InitType DMA_InitStructure;


uint32_t CRC_original_word_data[114] =
{
	0x00001021, 0x20423063, 0x408450a5, 0x60c670e7, 0x9129a14a, 0xb16bc18c,
	0xd1ade1ce, 0xf1ef1231, 0x32732252, 0x52b54294, 0x72f762d6, 0x93398318,
	0xa35ad3bd, 0xc39cf3ff, 0xe3de2462, 0x34430420, 0x64e674c7, 0x44a45485,
	0xa56ab54b, 0x85289509, 0xf5cfc5ac, 0xd58d3653, 0x26721611, 0x063076d7,
	0x569546b4, 0xb75ba77a, 0x97198738, 0xf7dfe7fe, 0xc7bc48c4, 0x58e56886,
	0x78a70840, 0x18612802, 0xc9ccd9ed, 0xe98ef9af, 0x89489969, 0xa90ab92b,
	0x4ad47ab7, 0x6a961a71, 0x0a503a33, 0x2a12dbfd, 0xfbbfeb9e, 0x9b798b58,
	0xbb3bab1a, 0x6ca67c87, 0x5cc52c22, 0x3c030c60, 0x1c41edae, 0xfd8fcdec,
	0xad2abd0b, 0x8d689d49, 0x7e976eb6, 0x5ed54ef4, 0x2e321e51, 0x0e70ff9f,
	0xefbedfdd, 0xcffcbf1b, 0x9f598f78, 0x918881a9, 0xb1caa1eb, 0xd10cc12d,
	0xe16f1080, 0x00a130c2, 0x20e35004, 0x40257046, 0x83b99398, 0xa3fbb3da,
	0xc33dd31c, 0xe37ff35e, 0x129022f3, 0x32d24235, 0x52146277, 0x7256b5ea,
	0x95a88589, 0xf56ee54f, 0xd52cc50d, 0x34e224c3, 0x04817466, 0x64475424,
	0x4405a7db, 0xb7fa8799, 0xe75ff77e, 0xc71dd73c, 0x26d336f2, 0x069116b0,
	0x76764615, 0x5634d94c, 0xc96df90e, 0xe92f99c8, 0xb98aa9ab, 0x58444865,
	0x78066827, 0x18c008e1, 0x28a3cb7d, 0xdb5ceb3f, 0xfb1e8bf9, 0x9bd8abbb,
	0x4a755a54, 0x6a377a16, 0x0af11ad0, 0x2ab33a92, 0xed0fdd6c, 0xcd4dbdaa,
	0xad8b9de8, 0x8dc97c26, 0x5c644c45, 0x3ca22c83, 0x1ce00cc1, 0xef1fff3e,
	0xdf7caf9b, 0xbfba8fd9, 0x9ff86e17, 0x7e364e55, 0x2e933eb2, 0x0ed11ef0
};
uint8_t CRC_original_byte_data[456] =
{
	0x00, 0x00, 0x10, 0x21, 0x20, 0x42, 0x30, 0x63, 0x40, 0x84, 0x50, 0xa5, 0x60, 0xc6, 0x70, 0xe7, 0x91, 0x29, 0xa1, 0x4a, 0xb1, 0x6b, 0xc1, 0x8c,
	0xd1, 0xad, 0xe1, 0xce, 0xf1, 0xef, 0x12, 0x31, 0x32, 0x73, 0x22, 0x52, 0x52, 0xb5, 0x42, 0x94, 0x72, 0xf7, 0x62, 0xd6, 0x93, 0x39, 0x83, 0x18,
	0xa3, 0x5a, 0xd3, 0xbd, 0xc3, 0x9c, 0xf3, 0xff, 0xe3, 0xde, 0x24, 0x62, 0x34, 0x43, 0x04, 0x20, 0x64, 0xe6, 0x74, 0xc7, 0x44, 0xa4, 0x54, 0x85,
	0xa5, 0x6a, 0xb5, 0x4b, 0x85, 0x28, 0x95, 0x09, 0xf5, 0xcf, 0xc5, 0xac, 0xd5, 0x8d, 0x36, 0x53, 0x26, 0x72, 0x16, 0x11, 0x06, 0x30, 0x76, 0xd7,
	0x56, 0x95, 0x46, 0xb4, 0xb7, 0x5b, 0xa7, 0x7a, 0x97, 0x19, 0x87, 0x38, 0xf7, 0xdf, 0xe7, 0xfe, 0xc7, 0xbc, 0x48, 0xc4, 0x58, 0xe5, 0x68, 0x86,
	0x78, 0xa7, 0x08, 0x40, 0x18, 0x61, 0x28, 0x02, 0xc9, 0xcc, 0xd9, 0xed, 0xe9, 0x8e, 0xf9, 0xaf, 0x89, 0x48, 0x99, 0x69, 0xa9, 0x0a, 0xb9, 0x2b,
	0x4a, 0xd4, 0x7a, 0xb7, 0x6a, 0x96, 0x1a, 0x71, 0x0a, 0x50, 0x3a, 0x33, 0x2a, 0x12, 0xdb, 0xfd, 0xfb, 0xbf, 0xeb, 0x9e, 0x9b, 0x79, 0x8b, 0x58,
	0xbb, 0x3b, 0xab, 0x1a, 0x6c, 0xa6, 0x7c, 0x87, 0x5c, 0xc5, 0x2c, 0x22, 0x3c, 0x03, 0x0c, 0x60, 0x1c, 0x41, 0xed, 0xae, 0xfd, 0x8f, 0xcd, 0xec,
	0xad, 0x2a, 0xbd, 0x0b, 0x8d, 0x68, 0x9d, 0x49, 0x7e, 0x97, 0x6e, 0xb6, 0x5e, 0xd5, 0x4e, 0xf4, 0x2e, 0x32, 0x1e, 0x51, 0x0e, 0x70, 0xff, 0x9f,
	0xef, 0xbe, 0xdf, 0xdd, 0xcf, 0xfc, 0xbf, 0x1b, 0x9f, 0x59, 0x8f, 0x78, 0x91, 0x88, 0x81, 0xa9, 0xb1, 0xca, 0xa1, 0xeb, 0xd1, 0x0c, 0xc1, 0x2d,
	0xe1, 0x6f, 0x10, 0x80, 0x00, 0xa1, 0x30, 0xc2, 0x20, 0xe3, 0x50, 0x04, 0x40, 0x25, 0x70, 0x46, 0x83, 0xb9, 0x93, 0x98, 0xa3, 0xfb, 0xb3, 0xda,
	0xc3, 0x3d, 0xd3, 0x1c, 0xe3, 0x7f, 0xf3, 0x5e, 0x12, 0x90, 0x22, 0xf3, 0x32, 0xd2, 0x42, 0x35, 0x52, 0x14, 0x62, 0x77, 0x72, 0x56, 0xb5, 0xea,
	0x95, 0xa8, 0x85, 0x89, 0xf5, 0x6e, 0xe5, 0x4f, 0xd5, 0x2c, 0xc5, 0x0d, 0x34, 0xe2, 0x24, 0xc3, 0x04, 0x81, 0x74, 0x66, 0x64, 0x47, 0x54, 0x24,
	0x44, 0x05, 0xa7, 0xdb, 0xb7, 0xfa, 0x87, 0x99, 0xe7, 0x5f, 0xf7, 0x7e, 0xc7, 0x1d, 0xd7, 0x3c, 0x26, 0xd3, 0x36, 0xf2, 0x06, 0x91, 0x16, 0xb0,
	0x76, 0x76, 0x46, 0x15, 0x56, 0x34, 0xd9, 0x4c, 0xc9, 0x6d, 0xf9, 0x0e, 0xe9, 0x2f, 0x99, 0xc8, 0xb9, 0x8a, 0xa9, 0xab, 0x58, 0x44, 0x48, 0x65,
	0x78, 0x06, 0x68, 0x27, 0x18, 0xc0, 0x08, 0xe1, 0x28, 0xa3, 0xcb, 0x7d, 0xdb, 0x5c, 0xeb, 0x3f, 0xfb, 0x1e, 0x8b, 0xf9, 0x9b, 0xd8, 0xab, 0xbb,
	0x4a, 0x75, 0x5a, 0x54, 0x6a, 0x37, 0x7a, 0x16, 0x0a, 0xf1, 0x1a, 0xd0, 0x2a, 0xb3, 0x3a, 0x92, 0xed, 0x0f, 0xdd, 0x6c, 0xcd, 0x4d, 0xbd, 0xaa,
	0xad, 0x8b, 0x9d, 0xe8, 0x8d, 0xc9, 0x7c, 0x26, 0x5c, 0x64, 0x4c, 0x45, 0x3c, 0xa2, 0x2c, 0x83, 0x1c, 0xe0, 0x0c, 0xc1, 0xef, 0x1f, 0xff, 0x3e,
	0xdf, 0x7c, 0xaf, 0x9b, 0xbf, 0xba, 0x8f, 0xd9, 0x9f, 0xf8, 0x6e, 0x17, 0x7e, 0x36, 0x4e, 0x55, 0x2e, 0x93, 0x3e, 0xb2, 0x0e, 0xd1, 0x1e, 0xf0
};

/**
*\*\name    TestCrc32AndDma.
*\*\fun     Test to calculate CRC32 value by transferring data through DMA.
*\*\param   burst_mode
*\*\          - 0
*\*\          - 1
*\*\param   burst_mode_len
*\*\          - 2
*\*\          - 3
*\*\          - 4
*\*\          - 5
*\*\          - 6
*\*\          - 7
*\*\return  none
**/
void TestCrc32AndDma(uint32_t burst_mode, uint32_t burst_mode_len)
{									 
		uint32_t CRC32_Value = 0;	
		uint32_t temp_crc_32_code=0;
	
		/*Memory->CRC32DAT*/	
		DMA_InitStructure.PeriphAddr=(uint32_t)&CRC->CRC32DAT;
		DMA_InitStructure.MemAddr=(uint32_t)CRC_original_word_data;		
		DMA_InitStructure.Direction=DMA_DIR_PERIPH_DST;
		DMA_InitStructure.Mem2Mem=DMA_M2M_ENABLE;
		DMA_InitStructure.MemDataSize=DMA_MEM_DATA_WIDTH_WORD;
		DMA_InitStructure.PeriphDataSize=DMA_PERIPH_DATA_WIDTH_WORD;
		DMA_InitStructure.MemoryInc=DMA_MEM_INC_ENABLE;
		DMA_InitStructure.PeriphInc=DMA_PERIPH_INC_DISABLE;
		DMA_InitStructure.CircularMode=DMA_MODE_NORMAL;	
		DMA_InitStructure.Priority=DMA_PRIORITY_VERY_HIGH;
		DMA_InitStructure.BufSize=114;			
		DMA_InitStructure.BURST_BYPASS=ENABLE;					
						
		if(burst_mode_len>1)
		{
				printf("burst_mode_len=%d,burst_mode=%d.\r\n",burst_mode_len,burst_mode);						
				DMA_InitStructure.BURST_BYPASS=DISABLE;					
				if(burst_mode_len==2)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_2;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}					
				else if(burst_mode_len==3)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_3;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==4)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_4;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==5)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_5;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==6)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_6;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==7)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_7;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else
				{
						printf("Burst mode length error.\r\n");							
				}							
		}
			
		DMA_Init(DMA1_CH1,&DMA_InitStructure);
		if(DMA_InitStructure.BURST_BYPASS==ENABLE)
		{
				DMA1->BURSTEN=DMA_BURST_BYPASS_ENABLE;
		}
		else
		{
				DMA1->BURSTEN=DMA_BURST_BYPASS_DISABLE;        
		}	
		DMA_EnableChannel(DMA1_CH1, ENABLE);
		while(!DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1));
		DMA_DeInit(DMA1_CH1);	

		CRC32_Value = CRC32_GetCrc();

		printf("The CRC 32 code after Memory->CRC32DAT DMA transmission is %08x.\r\n",CRC32_GetCrc());
		if(CRC32_Value != 0x379E9F06)
		{
				printf("The CRC 32 code computed through DMA transmission is error,it should be 0x379E9F06.\r\n");					
		}

		/* CRC.CRC32D->Memory */	
		DMA_InitStructure.PeriphAddr=(uint32_t)&CRC->CRC32D;
		DMA_InitStructure.MemAddr=(uint32_t)&temp_crc_32_code;		
		DMA_InitStructure.Direction=DMA_DIR_PERIPH_SRC;
		DMA_InitStructure.Mem2Mem=DMA_M2M_ENABLE;
		DMA_InitStructure.MemDataSize=DMA_MEM_DATA_WIDTH_WORD;
		DMA_InitStructure.PeriphDataSize=DMA_PERIPH_DATA_WIDTH_WORD;
		DMA_InitStructure.MemoryInc=DMA_MEM_INC_ENABLE;
		DMA_InitStructure.PeriphInc=DMA_PERIPH_INC_ENABLE;
		DMA_InitStructure.CircularMode=DMA_MODE_NORMAL;	
		DMA_InitStructure.Priority=DMA_PRIORITY_VERY_HIGH;
		DMA_InitStructure.BufSize=1;			
		
		DMA_Init(DMA1_CH1,&DMA_InitStructure);
		DMA_EnableChannel(DMA1_CH1, ENABLE);
		while(!DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1));
		DMA_DeInit(DMA1_CH1);			

		printf("\"temp_crc_32_code\" is %08x after CRC32DAT->Memory DMA transmission.\r\n",temp_crc_32_code);		
		if(temp_crc_32_code!=0x379E9F06)
		{
				printf("CRC32DAT->Memory DMA transmission error,\"temp_crc_32_code\" should be 0x379E9F06.\r\n");				
		}		

}

/**
*\*\name    TestCrc16AndDma.
*\*\fun     Test to calculate CRC16 and LRC value by transferring data through DMA.
*\*\param   burst_mode
*\*\          - 0
*\*\          - 1
*\*\param   burst_mode_len
*\*\          - 2
*\*\          - 3
*\*\          - 4
*\*\          - 5
*\*\          - 6
*\*\          - 7
*\*\return  none
**/
void TestCrc16AndDma(uint32_t burst_mode, uint32_t burst_mode_len)
{
		uint16_t CRC16_Value= 0;
		uint8_t  LRC_Value= 0xAA;
		uint16_t temp_crc_16_code=0;
		uint8_t  temp_lrc_code=0xAA;
	
		/*CRC 16 and LRC big endian test wirh 0 initial value*/			
		__CRC16_SetCrc(0x00);	
		__CRC16_SetLRC(0x00);
		__CRC16_SetCleanEnable();	
		__CRC16_SetBigEndianFmt();

				
		/*Memory->CRC.CRC16DAT*/	
		DMA_InitStructure.PeriphAddr=(uint32_t)&CRC->CRC16DAT;
		DMA_InitStructure.MemAddr=(uint32_t)CRC_original_byte_data;		
		DMA_InitStructure.Direction=DMA_DIR_PERIPH_DST;
		DMA_InitStructure.Mem2Mem=DMA_M2M_ENABLE;
		DMA_InitStructure.MemDataSize=DMA_MEM_DATA_WIDTH_BYTE;
		DMA_InitStructure.PeriphDataSize=DMA_PERIPH_DATA_WIDTH_BYTE;
		DMA_InitStructure.MemoryInc=DMA_MEM_INC_ENABLE;
		DMA_InitStructure.PeriphInc=DMA_PERIPH_INC_DISABLE;
		DMA_InitStructure.CircularMode=DMA_MODE_NORMAL;	
		DMA_InitStructure.Priority=DMA_PRIORITY_VERY_HIGH;
		DMA_InitStructure.BufSize=456;			
		DMA_InitStructure.BURST_BYPASS=ENABLE;	
		if(burst_mode_len>1)
		{
				printf("burst_mode_len=%d,burst_mode=%d.\r\n",burst_mode_len,burst_mode);							
				DMA_InitStructure.BURST_BYPASS=DISABLE;					
				if(burst_mode_len==2)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_2;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}					
				else if(burst_mode_len==3)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_3;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==4)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_4;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==5)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_5;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==6)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_6;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else if(burst_mode_len==7)
				{			
						DMA_InitStructure.BURST_LEN=DMA_BURST_LEN_7;		
						if(burst_mode==0)
						{			
								DMA_InitStructure.BURST_MODE=DMA_BURST_MODE_ALMOST;				    	
						}			
						else if(burst_mode==1)
						{
								 DMA_InitStructure.BURST_MODE=DMA_CHCFG1_BURST_DYNAMIC;																	
						}
						else	
						{
								printf("Burst mode error.\r\n");
						}									
				}	
				else
				{
						printf("Burst mode length error.\r\n");							
				}							
		}

		DMA_Init(DMA1_CH1,&DMA_InitStructure);
		if(DMA_InitStructure.BURST_BYPASS==ENABLE)
		{
				DMA1->BURSTEN=DMA_BURST_BYPASS_ENABLE;
		}
		else
		{
				DMA1->BURSTEN=DMA_BURST_BYPASS_DISABLE;        
		}					
		DMA_EnableChannel(DMA1_CH1, ENABLE);
		while(!DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1));
		DMA_DeInit(DMA1_CH1);			

		CRC16_Value = __CRC16_GetCrc();
		LRC_Value = __CRC16_GetLRC();

		printf("The CRC 16 code after Memory->CRC.CRC16DAT DMA transmission is %04x.\r\n",__CRC16_GetCrc());
		if(CRC16_Value != 0x5268)
		{
				printf("The CRC 16 code computed through DMA transmission is error,it should be 0x5268.\r\n");					
		}
		
		printf("The LRC code after Memory->CRC.CRC16DAT DMA transmission is %02x.\r\n",__CRC16_GetLRC());
		if(LRC_Value != 0x00)
		{
				printf("The LRC code computed through DMA transmission is error,it should be 0x00.\r\n");					
		}				

		/* CRC.CRC16D->Memory */	
		DMA_InitStructure.PeriphAddr=(uint32_t)&CRC->CRC16D;
		DMA_InitStructure.MemAddr=(uint32_t)&temp_crc_16_code;		
		DMA_InitStructure.Direction=DMA_DIR_PERIPH_SRC;
		DMA_InitStructure.Mem2Mem=DMA_M2M_ENABLE;
		DMA_InitStructure.MemDataSize=DMA_MEM_DATA_WIDTH_HALFWORD;
		DMA_InitStructure.PeriphDataSize=DMA_PERIPH_DATA_WIDTH_HALFWORD;
		DMA_InitStructure.MemoryInc=DMA_MEM_INC_ENABLE;
		DMA_InitStructure.PeriphInc=DMA_PERIPH_INC_ENABLE;
		DMA_InitStructure.CircularMode=DMA_MODE_NORMAL;	
		DMA_InitStructure.Priority=DMA_PRIORITY_VERY_HIGH;
		DMA_InitStructure.BufSize=1;			
		
		DMA_Init(DMA1_CH1,&DMA_InitStructure);
		DMA_EnableChannel(DMA1_CH1, ENABLE);
		while(!DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1));
		DMA_DeInit(DMA1_CH1);		

		printf("\"temp_crc_16_code\" is %04x after CRC32DAT->Memory DMA transmission.\r\n",temp_crc_16_code);		
		if(temp_crc_16_code!=0x5268)
		{
				printf("CRC.CRC16R->Memory DMA transmission error,\"temp_crc_16_code\" should be 0x5268.\r\n");					
		}		

		/* CRC.LRC->Memory */	
		DMA_InitStructure.PeriphAddr=(uint32_t)&CRC->LRC;
		DMA_InitStructure.MemAddr=(uint32_t)&temp_lrc_code;		
		DMA_InitStructure.Direction=DMA_DIR_PERIPH_SRC;
		DMA_InitStructure.Mem2Mem=DMA_M2M_ENABLE;
		DMA_InitStructure.MemDataSize=DMA_MEM_DATA_WIDTH_BYTE;
		DMA_InitStructure.PeriphDataSize=DMA_PERIPH_DATA_WIDTH_BYTE;
		DMA_InitStructure.MemoryInc=DMA_MEM_INC_ENABLE;
		DMA_InitStructure.PeriphInc=DMA_PERIPH_INC_ENABLE;
		DMA_InitStructure.CircularMode=DMA_MODE_NORMAL;	
		DMA_InitStructure.Priority=DMA_PRIORITY_VERY_HIGH;
		DMA_InitStructure.BufSize=1;			
		
		DMA_Init(DMA1_CH1,&DMA_InitStructure);
		DMA_EnableChannel(DMA1_CH1, ENABLE);
		while(!DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1));
		DMA_DeInit(DMA1_CH1);		

		printf("\"temp_lrc_code\" is %04x after CRC.LRC->Memory DMA transmission.\r\n",temp_lrc_code);		
		if(temp_lrc_code!=0x00)
		{
				printf("CRC.LRC->Memory DMA transmission error,\"temp_lrc_code\" should be 0x00.\r\n");					
		}		

}

