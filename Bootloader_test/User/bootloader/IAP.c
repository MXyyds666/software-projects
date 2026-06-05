/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : IAP.c
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "IAP.h"
#include <string.h>
#include "Parse_ComData.h"
#include "at32f45x_board.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

extern uint32_t g_DEVICE_ID;

#define  IAP_HEADMARK   0xAABB			//特征参数
#define IAP_BUF_SIZE    256   			//
u8 IAP_Buffer[IAP_BUF_SIZE+2] = {0};

IAP_CTRL_MAP_T IAP_Ctrl_Map = {0};
IAP_CTRL_MAP_T IAP_Ctrl_Map_Default = {
	IAP_HEADMARK,
	0,             
	0,  
	2,   						//默认非静默模式
	0,
	0
};

IAP_STATE iap_state;
BOOTLOADINFO Bootload_Info = {0};//2022-09-03
uint8_t g_UpData_BootB = 0;

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Save_IAP_CtrlData
* Description   : IAP控制数据保存
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
u8 Save_IAP_CtrlData(void)
{
	u8 s_result = 0;
	
	//2022-08-25  __disable_irq();   //关闭总中断
	
	s_result = Flash_Prepare(ADD_IAPDATA, sizeof(IAP_CTRL_MAP_T));
	if(!s_result)
		s_result = Flash_Write_Word(ADD_IAPDATA,(u8*)&IAP_Ctrl_Map, sizeof(IAP_CTRL_MAP_T));
	
	//2022-08-25  __enable_irq();		//开放总中断
	
	return s_result;
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
	
	//2022-08-25  __disable_irq();   //关闭总中断
	
	s_result = Flash_Prepare(ID_INFO_Addr, 4);
	if(!s_result)
		s_result = Flash_Write_Word(ID_INFO_Addr,(u8*)&id, 4);
	
	//2022-08-25  __enable_irq();		//开放总中断
	
	return s_result;
}

s32 IAP_JumpTo(u32 ApplicationAddress)
{	
	u32 JumpAddress;
	
	FORM_FRAME_DATA s_frame_data = {g_DEVICE_ID, 0x20, PF_RESPONSE, 0, 5, 8};//初始化一个自定义的 CAN 数据帧结构体 (用于回复响应)
	memset(s_frame_data.pdata, 0xFF, 8); 

	//判断地址区域是否有固件
	if (((*(vu32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{ 
		//获取用户程序主函数地址
		JumpAddress = *(vu32*) (ApplicationAddress + 4);
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(vu32*) ApplicationAddress);
		
		//关闭所有中断		
//		InitAllPeripherals(FALSE); 
//		__disable_irq(); 
//		NVIC_SystemReset();
		//跳转到用户主函数
//		CAN_PushFrame(s_frame_data);
//		Can_Check_Send();
		(*((void(*)(void))JumpAddress))();
		return 1;
	}
	return 0;
}

/*****************************************************************
* Function Name : IAP_Init
* Description   : IAP相关变量初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void IAP_Reset(void)
{
	memset(&iap_state,0,sizeof(iap_state));
}

u8 Table_x[12][2] = {{"01"},{"02"},{"03"},{"04"},{"05"},{"06"},{"07"},{"08"},{"09"},{"10"},{"11"},{"12"}};
u8 Table_y[12][3] = {{"Jan"},{"Feb"},{"Mar"},{"Apr"},{"May"},{"Jun"},{"Jul"},{"Aug"},{"Sep"},{"Oct"},{"Nov"},{"Dec"}};

void IAP_Init(void)
{
//	u8 i = 0;
//	unsigned char codeDataStr[]=__DATE__;		//获取编译的日期
//	unsigned char codeTimeStr[]=__TIME__;		//获取编译的时间
//	static u8 s_default_sw[32] = "MC_Boot_Sofeware_Version_is_1001";	//版本号
	Flash_Read_Byte(ADD_IAPDATA, (u8*)&IAP_Ctrl_Map, sizeof(IAP_CTRL_MAP_T));	//读取IAP相关信息
//	Flash_Read_Byte(BOOT_INFO_Addr, (u8*)&Bootload_Info, sizeof(BOOTLOADINFO));//2022-09-03
	IAP_Ctrl_Map.Be_IAP = 0;//2022-08-30  应答不了固件长度
	if(IAP_Ctrl_Map.IAP_HeadMark != IAP_HEADMARK)		//检查读取的IAP控制参数是否合法，不合法则恢复为默认参数
	{
		memcpy((u8*)&IAP_Ctrl_Map, (u8*)&IAP_Ctrl_Map_Default, sizeof(IAP_CTRL_MAP_T));
	}
	/*2022-09-03 
	for(i=0;i<12;i++)
	{
		if(!memcmp(Table_y[i], codeDataStr, 3))//比较两个参数的前3个字节，相等时返回0
		{
			memcpy((u8*)&IAP_Ctrl_Map.Boot_Compile_Date[4], Table_x[i], 2);//日期：月
			break;
		}
	}
	memcpy((u8*)&IAP_Ctrl_Map.Boot_Compile_Date[0], &codeDataStr[7], 4);//日期：年
	memcpy((u8*)&IAP_Ctrl_Map.Boot_Compile_Date[6], &codeDataStr[4], 2);//日期：日
	if(IAP_Ctrl_Map.Boot_Compile_Date[6] == 0x20)
		IAP_Ctrl_Map.Boot_Compile_Date[6] = 0x30;
	memcpy((u8*)&IAP_Ctrl_Map.Boot_Compile_Time[0], codeTimeStr, 8);//时间
	
	memcpy((u8*)&IAP_Ctrl_Map.Boot_SW_Version[0], s_default_sw, 32);//BOOT软件版本
	*/
	//转化时间等BOOT信息
//	for(i=0;i<12;i++)//2022-09-03 
//	{
//		if(!memcmp(Table_y[i], codeDataStr, 3))//比较两个参数的前3个字节，相等时返回0
//		{
//			memcpy((u8*)&Bootload_Info.Boot_Compile_Date[4], Table_x[i], 2);//日期：月
//			break;
//		}
//	}
//	memcpy((u8*)&Bootload_Info.Boot_Compile_Date[0], &codeDataStr[7], 4);//日期：年
//	memcpy((u8*)&Bootload_Info.Boot_Compile_Date[6], &codeDataStr[4], 2);//日期：日
//	if(Bootload_Info.Boot_Compile_Date[6] == 0x20)
//		Bootload_Info.Boot_Compile_Date[6] = 0x30;
//	memcpy((u8*)&Bootload_Info.Boot_Compile_Time[0], codeTimeStr, 8);//时间	
//	memcpy((u8*)&Bootload_Info.Boot_SW_Version[0], s_default_sw, 32);//BOOT软件版本
	
	Save_IAP_CtrlData();
//	Save_BOOT_Info();//2022-09-03
}

/*****************************************************************
* Function Name : IAP_Request_Data
* Description   : 请求一包IAP数据
* Input         : framedata:请求数据相关地址
* Output        : None
* Notes         :
******************************************************************/
void IAP_Request_Data(FORM_FRAME_DATA* framedata, u8 cmd)
{
	if(cmd == 1)  //数据请求
	{
		framedata->pdata[0] = 17;
		framedata->pdata[1] = 1;                      //可发送的数据包数
		framedata->pdata[2] = iap_state.IAPIndex;     //数据包编号
		framedata->pdata[3] = 0xFF;
	}
	else if(cmd == 2)
	{
		framedata->pdata[0] = 19;
	}
	framedata->pf = PF_GE_PROTOCOL;
	framedata->pdata[4] = 0;
	framedata->pdata[5] = 0;
	framedata->pdata[6] = PF_BOOT_CMD;
	framedata->pdata[7] = 0;
	CAN_PushFrame(*framedata); //请求一包数据
}

/*****************************************************************
* Function Name : IAP_Handle
* Description   : IAP数据处理函数
* Input         : extid:数据的扩展ID pdata:数据的地址
* Output        : None
* Notes         :
******************************************************************/
void IAP_Handle(CAN_EXT_ID extid ,u8* pdata)
{
	u16 i = 0;
	u8 result = IAP_OK;
	u32 s_temp32 = 0;
	static u8 s_iap_buff[1024] = {0};  //IAP数据缓存
	u16 s_data_lenth = 0;
	u32 s_TopStack_Addr;
	
	FORM_FRAME_DATA s_frame_data = {g_DEVICE_ID, extid.params.CAN_SD, PF_RESPONSE, 0, 5, 8};
	memset(s_frame_data.pdata, 0xFF, 8);  
	
	if(iap_state.IAP_Busy) //防止重复进入
	{
		iap_state.IAP_Busy = 0;
		s_frame_data.pdata[0] = 2; //0肯定 1否定 2拒绝
		CAN_PushFrame(s_frame_data);
		return;
	}
	iap_state.IAP_Busy = 1;
	
	switch(extid.params.CAN_PF){
		
	case PF_BOOT_CMD:										//0xE0升级控制命令，步骤1
	case PF_BOOT_UPDATA_B:							//229 升级BOOTB
		switch(pdata[0]) {	
				
		case IAP_BEGIN:  									//02 开始IAP命令，传入固件大小
			if(!IAP_Ctrl_Map.Be_IAP) {			//没有进入固件升级时进入，擦除flash
				IAP_Reset();									//
				iap_state.IAP_CRC = 0;
				IAP_Ctrl_Map.Be_IAP = 1;
				g_Variable.Be_IAP = 1;
				memcpy(&iap_state.IAP_FileSize, &pdata[1], 4); //取出固件大小(这里会将高低字节调换位置)
				iap_state.IAP_FileSize -= 4;
				
				
				//判断数据大小是否合法
				result = IAPERROR_SIZE;
				if(g_UpData_BootB == 0)
				{
					if(iap_state.IAP_FileSize>0 && iap_state.IAP_FileSize<= Max_PROGRAM_SIZE)
						result = Flash_Prepare(ADD_FORMAL, iap_state.IAP_FileSize); //擦除flash
				}
				else
				{
					if(iap_state.IAP_FileSize>0 && iap_state.IAP_FileSize<= MAX_BOOT_B_SIZE)
						result = Flash_Prepare(ADD_FORMAL, iap_state.IAP_FileSize); //擦除flash
				}

				__disable_irq();//2022-08-25 
				IAP_Ctrl_Map.Be_Rseset = 0;		//2022-08-30 清重启标志						
				Save_IAP_CtrlData();			//将升级状态写入FLASH
				__enable_irq();//2022-08-25  
		  }
			break;

		case IAP_CHECK:  //03 校验码计算方式，当固件包发送完成时，再调用CRC检验命令
			memcpy(&s_temp32, &pdata[1], 2);  //取出校验码
			if(s_temp32 != (iap_state.IAP_CRC&0x0000ffff)) //校验不正确
				result = IAPERROR_FORM;
			break;

		case IAP_RESET: //重启指令
			if(!g_Variable.Be_IAP) {//01 当前次IAP过程中不接受中断命令
				if(IAP_Ctrl_Map.Be_Rseset == 1) {//已经重启过不再重启,但应答 2022-08-30
					//g_Variable.Be_IAP = 1;
					s_frame_data.ps = extid.params.CAN_SD;
					s_frame_data.pdata[0] = 0; //0肯定 1否定 2拒绝
					CAN_PushFrame(s_frame_data);
					Can_Check_Send();
				}								
				else if(IAP_Ctrl_Map.Be_Rseset == 0) {//只重启一次2022-08-30
					IAP_Ctrl_Map.Be_Rseset = 1;
					IAP_Ctrl_Map.Rseset_CMD_SRCID = extid.params.CAN_SD;
					Save_IAP_CtrlData();
					//g_Variable.Be_Reset = 1;

					// 2022-08-25----------------------------
					//g_Variable.Be_IAP = 1;
					s_frame_data.ps = extid.params.CAN_SD;
					s_frame_data.pdata[0] = 0; //0肯定 1否定 2拒绝
					CAN_PushFrame(s_frame_data);
					Can_Check_Send();

					__disable_irq(); 
					delay_ms(10);
					NVIC_SystemReset(); 
					// 2022-08-25------end----------------------
				}
			}	
			break;

		default:
			result = IAPERROR_FORM;
			break;
		}
		
		if(pdata[0] != IAP_RESET) {
			if(result != IAP_OK) {
				IAP_Reset();
				s_frame_data.pdata[0] = 1; //0肯定 1否定 2拒绝
			}
			else 
				s_frame_data.pdata[0] = 0; //0肯定 1否定 2拒绝
			CAN_PushFrame(s_frame_data);
		}
		break;

	case PF_GE_DATA:      								//235(0xEB) IAP数据 步骤3
		if(iap_state.IAP_GroupSize == 0) {	//数据为空，错误
			result = IAPERROR_FORM;
			break;
		}
		
		if(pdata[0] == iap_state.IAPIndex) { //数据索引是否正确，不正确时，要求重新发送
			if(iap_state.IAPPageNb == 1 && iap_state.IAPIndex == 1) { //第一组的第一包数据
				memcpy((u8*)&s_TopStack_Addr, &pdata[1], 4);		 	//获取栈顶地址  
				if (!((s_TopStack_Addr & 0x2FFE0000 ) == 0x20000000)) {//判断栈顶地址是否合法
					result = IAPERROR_FORM;
					break;
				}
			}

			iap_state.IAPIndex++;	//准备下一包的索引
			if(pdata[0] == iap_state.IAPPackNb) {//一组中的最后一包 
				if(pdata[0] == 147)	//判断是否是最后一包
					s_data_lenth = 2;	//最后一节可能不满7字节
				else {           		//固件发送完成
					if(iap_state.IAP_SavePos < iap_state.IAP_FileSize)//计算最后一包应有的实际有效数据长度
						s_data_lenth = iap_state.IAP_FileSize - iap_state.IAP_SavePos;
					else
						s_data_lenth = 0;
				}
				//组装应答报文，反馈当前包序和已接收长度
				s_frame_data.pdata[3] = pdata[0];
				memcpy(&s_frame_data.pdata[1], &iap_state.IAP_GroupSize, 2); 
				IAP_Request_Data(&s_frame_data, 2); //该组数据发送完成应答
			}
			else {
				s_data_lenth = 7;
				IAP_Request_Data(&s_frame_data, 1); //请求下一包数据
			}
			//缓存数据：将收到的7字节数据存入缓冲区对应位置
			memcpy(&s_iap_buff[(pdata[0]-1)*7], &pdata[1], s_data_lenth);
			//更新总接收字数
			iap_state.IAP_SavePos += s_data_lenth;	//已接收的数据计数
			
			//临时长度清零，准备后续FLASH写入判断
			if(pdata[0] == iap_state.IAPPackNb)
				s_data_lenth = 0; 
			//FLASH写入逻辑：当一组数据全部收齐，且尚未写完整个文件时
			if(pdata[0] == iap_state.IAPPackNb && iap_state.IAP_WritePos<iap_state.IAP_FileSize) {//写入Flash
				if(pdata[0] == 147)//确定本次写入FLASH的总长度
					s_data_lenth = 1024;
				else {
					s_data_lenth = iap_state.IAP_FileSize - iap_state.IAP_WritePos;
				}
				
				//将缓冲区中的数据正式写入 Flash（起始地址 = ADD_FORMAL + 已写入偏移）
				if(g_UpData_BootB == 0)
					result = Flash_Write_Word(ADD_FORMAL+iap_state.IAP_WritePos, s_iap_buff, s_data_lenth);
				else if(g_UpData_BootB == 1)
					result = Flash_Write_Word(BOOT_B_FORMAL+iap_state.IAP_WritePos, s_iap_buff, s_data_lenth);
				iap_state.IAP_WritePos += s_data_lenth; //已写入的数据计数
				
				//计算校验码
				for(i=0; i<s_data_lenth; i++) {
					if(iap_state.IAP_CRC_cnt<iap_state.IAP_FileSize) {
						iap_state.IAP_CRC_cnt++;
						iap_state.IAP_CRC += s_iap_buff[i];	 	//累加校验和
					}
				}
			}
		}
		else {
			IAP_Request_Data(&s_frame_data, 1); //重新请求上一包数据
		}
		break;
		
	case PF_GE_PROTOCOL:  //236(0xEC)  IAP报文数量 1k为1组，步骤2
		if(pdata[0] == 16) {
			memset(s_iap_buff, 0, 1024); 					//将1KB的FLASH清除
			iap_state.IAPIndex = 1;							//设置索引为1，准备接收这包数据
			iap_state.IAPPageNb++;							
			iap_state.IAPPackNb = pdata[3];                 //获取该组数据包数量
			memcpy(&iap_state.IAP_GroupSize, &pdata[1], 2); //获取报文字节数
			IAP_Request_Data(&s_frame_data, 1);             //请求第一包数据
			Can_Check_Send();//2022-08-30 
		}
		break;
		
	default:
		break;
	}
	iap_state.IAP_Busy = 0;
}

/*******************************END OF FILE************************/
////修改版本，每组接收1784字节
//void IAP_Handle(CAN_EXT_ID extid ,u8* pdata)
//{
//	u16 i = 0;
//	u8 result = IAP_OK;
//	u32 s_temp32 = 0;
//	static u8 s_iap_buff[1784] = {0};  // 修改点1：缓冲区扩大至 1784
//	u16 s_data_lenth = 0;
//	u32 s_TopStack_Addr;
//	
//	FORM_FRAME_DATA s_frame_data = {g_DEVICE_ID, extid.params.CAN_SD, PF_RESPONSE, 0, 5, 8};
//	memset(s_frame_data.pdata, 0xFF, 8);  
//	
//	if(iap_state.IAP_Busy) 
//	{
//		iap_state.IAP_Busy = 0;
//		s_frame_data.pdata[0] = 2; 
//		CAN_PushFrame(s_frame_data);
//		return;
//	}
//	iap_state.IAP_Busy = 1;
//	
//	switch(extid.params.CAN_PF){
//		
//	case PF_BOOT_CMD:
//		switch(pdata[0]) {	
//				
//		case IAP_BEGIN: 
//			if(!IAP_Ctrl_Map.Be_IAP) {
//				IAP_Reset();
//				iap_state.IAP_CRC = 0;
//				IAP_Ctrl_Map.Be_IAP = 1;
//				g_Variable.Be_IAP = 1;
//				memcpy(&iap_state.IAP_FileSize, &pdata[1], 4); 
//				iap_state.IAP_FileSize -= 4;
//				if(iap_state.IAP_FileSize>0 && iap_state.IAP_FileSize<= Max_PROGRAM_SIZE)
//					result = Flash_Prepare(ADD_FORMAL, iap_state.IAP_FileSize+2048); // 修改点2：增加擦除余量边界
//				else
//					result = IAPERROR_SIZE;

//				__disable_irq();
//				IAP_Ctrl_Map.Be_Rseset = 0;						
//				Save_IAP_CtrlData();
//				__enable_irq();  
//		  }
//			break;

//		case IAP_CHECK: 
//			memcpy(&s_temp32, &pdata[1], 2); 
//			if(s_temp32 != (iap_state.IAP_CRC&0x0000ffff)) 
//				result = IAPERROR_FORM;
//			break;

//		case IAP_RESET: 
//            // ... (重启逻辑保持不变)
//			break;

//		default:
//			result = IAPERROR_FORM;
//			break;
//		}
//		
//		if(pdata[0] != IAP_RESET) {
//			s_frame_data.pdata[0] = (result != IAP_OK) ? 1 : 0;
//			CAN_PushFrame(s_frame_data);
//		}
//		break;

//	case PF_GE_DATA:      
//		if(iap_state.IAP_GroupSize == 0) {
//			result = IAPERROR_FORM;
//			break;		
//		}
//		
//		if(pdata[0] == iap_state.IAPIndex) { 
//			if(iap_state.IAPPageNb == 1 && iap_state.IAPIndex == 1) { 
//				memcpy((u8*)&s_TopStack_Addr, &pdata[1], 4);		 	
//				if (!((s_TopStack_Addr & 0x2FFE0000 ) == 0x20000000)) {
//					result = IAPERROR_FORM;
//					break;
//				}
//			}

//			iap_state.IAPIndex++;
//            
//            // 修改点3：移除 147 硬编码，根据当前包序和组大小动态计算包长度
//			if(pdata[0] == iap_state.IAPPackNb) { // 一组中的最后一包 
//                // 最后一包的长度 = 组总长度 - 之前所有包已占用的长度
//                s_data_lenth = iap_state.IAP_GroupSize - (pdata[0] - 1) * 7;
//                
//				// 组装反馈
//				s_frame_data.pdata[3] = pdata[0];
//				memcpy(&s_frame_data.pdata[1], &iap_state.IAP_GroupSize, 2); 
//				IAP_Request_Data(&s_frame_data, 2); 
//			}
//			else {
//				s_data_lenth = 7;
//				IAP_Request_Data(&s_frame_data, 1); 
//			}

//			// 存入缓存
//			memcpy(&s_iap_buff[(pdata[0]-1)*7], &pdata[1], s_data_lenth);
//			iap_state.IAP_SavePos += s_data_lenth;
//			
//			// 修改点4：当收到组最后一包时，整组写入 Flash
//			if(pdata[0] == iap_state.IAPPackNb && iap_state.IAP_WritePos < iap_state.IAP_FileSize) {
//                // 写入长度即为该组收到的总长度
//                u16 write_len = iap_state.IAP_GroupSize;
//				
//				result = Flash_Write_Word(ADD_FORMAL + iap_state.IAP_WritePos, s_iap_buff, write_len);
//				iap_state.IAP_WritePos += write_len; 
//				
//				// 计算校验码（针对本组写入的内容）
//				for(i=0; i < write_len; i++) {
//					if(iap_state.IAP_CRC_cnt < iap_state.IAP_FileSize) {
//						iap_state.IAP_CRC_cnt++;
//						iap_state.IAP_CRC += s_iap_buff[i];
//					}
//				}
//			}
//		}
//		else {
//			IAP_Request_Data(&s_frame_data, 1); 
//		}
//		break;
//		
//	case PF_GE_PROTOCOL:  
//		if(pdata[0] == 16) {
//			memset(s_iap_buff, 0, 1784); 					// 修改点5：清除 1784 字节缓存
//			iap_state.IAPIndex = 1;							
//			iap_state.IAPPageNb++;							
//			iap_state.IAPPackNb = pdata[3];                 
//			memcpy(&iap_state.IAP_GroupSize, &pdata[1], 2); 
//			IAP_Request_Data(&s_frame_data, 1);             
//			Can_Check_Send();
//		}
//		break;
//		
//	default:
//		break;
//	}
//	iap_state.IAP_Busy = 0;
//}
