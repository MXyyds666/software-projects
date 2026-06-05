#include "bl_can.h"

//设备ID
extern uint32_t g_DEVICE_ID;

#define MAX_CAN_TXFRM_NUM    25

can_txbuf_type m_TxBuff[MAX_CAN_TXFRM_NUM];

volatile uint8_t m_TxFrm_Head = 0;
volatile uint8_t m_TxFrm_Tail = 0;
volatile uint8_t m_bCANCheckingSend= 0;		//正在确认数据发送

CAN_EXT_ID can_extid;

/*****************************************************************
* Function Name : CAN_Form_Exid
* Description   : 构建扩展ID
* Input         : s_id:原地址 ps:特定PDU pf:PDU格式 dp:数据页 p:优先级
* Output        : u32构建成的扩展ID
* Notes         :
******************************************************************/
static uint32_t CAN_Form_Exid(u8 s_id, u8 ps, u8 pf, u8 dp, u8 p)
{	
	can_extid.params.CAN_SD = s_id; //源地址
	can_extid.params.CAN_PS = ps;   //特定PDU 群扩展域
	can_extid.params.CAN_PF = pf;   //PDU格式域 PDU1:0~239 PDU2:240_255
	can_extid.params.CAN_DP = dp;   //数据页 0或1
	can_extid.params.CAN_EDP = 0;   //扩展数据页 默认为0
	can_extid.params.CAN_P = p;     //优先级 0~7
	return can_extid.Cache;
}

/*****************************************************************
* Function Name : CanFilterConfig
* Description   : 初始化过滤器
* Input         : id_to_receive:接收数据的ID fifo:选择的FIFO
				: filter_number:选择的过滤器编号
* Output        : None
* Notes         :
******************************************************************/	
void CanFilterConfig(u8 id_to_receive, u8 filter_number)
{
	uint32_t id = CAN_Form_Exid(id_to_receive, 0, 0, 0, 0);
	uint32_t mask = CAN_Form_Exid(0xFF, 0, 0, 0, 0);

//	id= id << 3 | CAN_ID_EXTENDED | CAN_FRAME_DATA;
//	mask = mask << 3 | CAN_ID_EXTENDED;

	can_filter_config_type can_filter_struct;
	
	can_software_reset(CAN1, TRUE);                                  //开启软件复位

  can_filter_default_para_init(&can_filter_struct);
  can_filter_struct.mask_para.id_type = FALSE;                       
  can_filter_struct.code_para.id_type = CAN_ID_EXTENDED;            //选择帧类型
  can_filter_struct.mask_para.id = ~mask;	                         	//指定位为0过滤，1忽略
  can_filter_struct.code_para.id = id;                              //ID
  can_filter_struct.mask_para.data_length = 0x0F;                    //指定位为0过滤，1忽略
  can_filter_struct.code_para.data_length = 0x8;                    //数据长度
  can_filter_struct.mask_para.frame_type = TRUE;										//TRUE忽略，FALSE过滤
  can_filter_struct.code_para.frame_type = CAN_FRAME_DATA;          //帧内容
  can_filter_struct.mask_para.recv_frame = TRUE;
  can_filter_struct.code_para.recv_frame = CAN_RECV_NORMAL;         //接收什么模式的帧
  can_filter_struct.mask_para.fd_format = FALSE;
  can_filter_struct.code_para.fd_format = CAN_FORMAT_CLASSIC;       //帧标准
  can_filter_struct.mask_para.fd_rate_switch = FALSE;
  can_filter_struct.code_para.fd_rate_switch = CAN_BRS_OFF;         //是否自动变换波特率
  can_filter_struct.mask_para.fd_error_state = TRUE;
  can_filter_struct.code_para.fd_error_state = CAN_ESI_ACTIVE;      //错误类型
  can_filter_set(CAN1, (can_filter_type)filter_number, &can_filter_struct);

  can_filter_enable(CAN1, (can_filter_type)filter_number, TRUE);                   //使能指定FIFO
  can_receive_all_enable(CAN1, FALSE);                             //关闭接收所有帧
  can_software_reset(CAN1, FALSE);                                 //关闭软件复位
}

/*****************************************************************
* Function Name : User_CAN_Init
* Description   : CAN初始化函数
* Input         : sate ： 是否开启中断
* Output        : NONE
* Notes         :
******************************************************************/
void User_CAN_Init(confirm_state sate)
{
	// BSP已实现
	if(sate)//如果初始化CAN
	{
		u8 id_receive[] = {g_DEVICE_ID, ID_METER, ID_PC, ID_BMS1, ID_BROADCAST};

		gpio_init_type gpio_init_struct;

		/*--------------------------- GPIO ---------------------------*/
		/* enable the gpio clock */
		crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

		/* GPIO引脚配置 */
		gpio_default_para_init(&gpio_init_struct);
		gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
		gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
		gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
		gpio_init_struct.gpio_pins = GPIO_PINS_11 | GPIO_PINS_12;
		gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
		gpio_init(GPIOA, &gpio_init_struct);

		gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE11, GPIO_MUX_9);
		gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE12, GPIO_MUX_9);

		/*--------------------------- CAN_TIME ---------------------------*/
		while(crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
		{
			//CAN必须要使用外部时钟源作为时钟才能正常通信
		}

		/* 时钟使能 */
		crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);
		crm_can_clock_select(CRM_CAN1, CRM_CAN_CLOCK_SOURCE_PCLK);

		can_software_reset(CAN1, TRUE);                                       //开启软件复位
																						
		can_bittime_type can_bittime_struct;
		can_bittime_default_para_init(&can_bittime_struct); 
		can_bittime_struct.bittime_div = 1;																		//配置波特率
		can_bittime_struct.ac_bts1_size = 144;
		can_bittime_struct.ac_bts2_size = 48;
		can_bittime_struct.ac_rsaw_size = 48;
		can_bittime_struct.fd_bts1_size = 0;
		can_bittime_struct.fd_bts2_size = 0;
		can_bittime_struct.fd_rsaw_size = 0;
		can_bittime_struct.fd_ssp_offset = 0;
		can_bittime_set(CAN1, &can_bittime_struct);

		can_fd_iso_mode_enable(CAN1, FALSE);                                  //设置CAN FD的ISO标准，仅CAN FD时生效
		can_stb_transmit_mode_set(CAN1, CAN_STB_TRANSMIT_BY_FIFO);            //设置发送缓冲区
		can_retransmission_limit_set(CAN1, CAN_RE_TRANS_TIMES_0);             //设置重传次数
		can_rearbitration_limit_set(CAN1, CAN_RE_ARBI_TIMES_0);               //设置仲裁限制重发值
		can_rxbuf_warning_set(CAN1, 3);                                       //设置溢出报警值
		can_mode_set(CAN1, CAN_MODE_COMMUNICATE);                             //正常通信模式
		can_software_reset(CAN1, FALSE);                                      //解除软件复位

		/* CAN中断配置 */
		nvic_irq_enable(CAN1_RX_IRQn, 0, 0);
		nvic_irq_enable(CAN1_ERR_IRQn, 0, 0);
		can_interrupt_enable(CAN1, CAN_RAFIE_INT, TRUE);                      //使能FIFO报警中断
		can_interrupt_enable(CAN1, CAN_RFIE_INT, TRUE);                       //使能接收FIFO非空中断
		can_interrupt_enable(CAN1, CAN_ROIE_INT, TRUE);                       //使能接收溢出中断
		can_interrupt_enable(CAN1, CAN_RIE_INT, TRUE);                        //使能全局接收中断
		can_interrupt_enable(CAN1, CAN_BEIE_INT, TRUE);                       //使能总线错误中断

		//CAN过滤器初始化
		for(uint8_t i= 0;i<(sizeof(id_receive)/sizeof(id_receive[0]));i++)
		{
			CanFilterConfig(id_receive[i], i);
		}
	}
	else
	{
		crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, FALSE);								 //关闭CAN时，关闭CAN的时钟
		can_interrupt_enable(CAN1, CAN_RAFIE_INT, FALSE);                      //关闭FIFO报警中断
		can_interrupt_enable(CAN1, CAN_RFIE_INT, FALSE);                       //关闭接收FIFO非空中断
		can_interrupt_enable(CAN1, CAN_ROIE_INT, FALSE);                       //关闭接收溢出中断
		can_interrupt_enable(CAN1, CAN_RIE_INT, FALSE);                        //关闭全局接收中断
		can_interrupt_enable(CAN1, CAN_BEIE_INT, FALSE);                       //关闭总线错误中断
	}
}

/*****************************************************************
* Function Name : CAN_PushFrame
* Description   : 发送数据压栈
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void CAN_PushFrame(FORM_FRAME_DATA frame_data)
{
	u8 s_data_len = frame_data.datalength;
	can_txbuf_type s_TxMessage = {0};
	u16 i = 0;
	s_TxMessage.frame_type = CAN_FRAME_DATA;
	s_TxMessage.id_type = CAN_ID_EXTENDED;

	while(s_data_len > 0)
	{
		if(s_data_len > CAN_MAX_FRM_LEN)
		{
			s_TxMessage.data_length = (can_data_length_type)CAN_MAX_FRM_LEN;
			s_data_len -= CAN_MAX_FRM_LEN;
			//多帧压栈，索引自增
		}
		else
		{
			s_TxMessage.data_length = (can_data_length_type)s_data_len;
			s_data_len = 0;
		}
		s_TxMessage.id = CAN_Form_Exid(frame_data.s_id, frame_data.ps, frame_data.pf, frame_data.dp, frame_data.p);
		memcpy(&s_TxMessage.data[0], &frame_data.pdata[i], s_TxMessage.data_length);
		memcpy(&m_TxBuff[m_TxFrm_Head], &s_TxMessage, sizeof(can_txbuf_type));
		i+=s_TxMessage.data_length;
		m_TxFrm_Head++;
		if(m_TxFrm_Head >= MAX_CAN_TXFRM_NUM)
			m_TxFrm_Head = 0;
		if(m_TxFrm_Head == m_TxFrm_Tail)
			m_TxFrm_Tail++;
	}
}

/*****************************************************************
* Function Name : Can_Check_Send
* Description   : CAN查询发送
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Can_Check_Send(void)
{
	can_transmit_status_type transmit_status_struct;
	//判断是否正在Check
	if(m_bCANCheckingSend == 1)
		return;
	m_bCANCheckingSend = 1;
	
	can_transmit_status_get(CAN1, &transmit_status_struct);
//	if(m_TxFrm_Head == m_TxFrm_Tail || (CAN1->MSR & ((uint32_t)0x00000100)) != 0) //队列为空 或 正在发送
	if(m_TxFrm_Head == m_TxFrm_Tail || (transmit_status_struct.current_tstat != CAN_TSTAT_IDLE) )//队列为空 或 正在发送
	{
		m_bCANCheckingSend = 0;
		return;
	}

	if(Bsp_Can_Transmit_Classic_Extended(CAN1, m_TxBuff[m_TxFrm_Tail].id, m_TxBuff[m_TxFrm_Tail].data, m_TxBuff[m_TxFrm_Tail].data_length) == SUCCESS)		//发送成功
	{
		if(++m_TxFrm_Tail == MAX_CAN_TXFRM_NUM)
			m_TxFrm_Tail = 0;
	}
	m_bCANCheckingSend = 0;
}

/*****************************************************************
* Function Name : USB_LP_CAN1_RX0_IRQHandler
* Description   : CAN接收中断函数
* Input         : None
* Output        : int
* Notes         :
******************************************************************/
void CAN1_RX_IRQHandler(void)
{
  can_rxbuf_type s_RxMessage = {0};

  /* rx_buffer had data be received */
  if(can_interrupt_flag_get(CAN1, CAN_RIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_RIF_FLAG);
    while(ERROR != can_rxbuf_read(CAN1, &s_RxMessage))
    {
			Parse_CAN_Frame(s_RxMessage);	//解析数据
    }
  }

  /* rx_buffer almost full */
  if(can_interrupt_flag_get(CAN1, CAN_RAFIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_RAFIF_FLAG);
  }

  /* rx_buffer full */
  if(can_interrupt_flag_get(CAN1, CAN_RFIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_RFIF_FLAG);
  }

  /* rx_buffer overflow */
  if(can_interrupt_flag_get(CAN1, CAN_ROIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_ROIF_FLAG);
  }
}
