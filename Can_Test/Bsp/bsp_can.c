/**
 * @file bsp_can.c
 * @author baishuaijie (baishuaijie17@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-16
 * 
 * 
 * 
 */
#include "bsp_can.h"
#include "at32f45x_gpio.h"

/**
 * @brief CAN1外设初始化
 * 
 * @return error_status 
 *         ERROR：初始化错误
 *         SUCCES：初始化成功
 */
error_status Bsp_Can1_Init(void)
{
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
  if(crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
  {
    return ERROR;
  }

  /* 时钟使能 */
  crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);
  crm_can_clock_select(CRM_CAN1, CRM_CAN_CLOCK_SOURCE_PCLK);

  can_software_reset(CAN1, TRUE);                                       //开启软件复位
  Can_Boardrate_Set(CAN1, 1000);                                         //设置波特率
  can_fd_iso_mode_enable(CAN1, FALSE);                                  //设置CAN FD的ISO标准，仅CAN FD时生效
  can_stb_transmit_mode_set(CAN1, CAN_STB_TRANSMIT_BY_FIFO);            //设置发送缓冲区
  can_retransmission_limit_set(CAN1, CAN_RE_TRANS_TIMES_0);             //设置重传次数
  can_rearbitration_limit_set(CAN1, CAN_RE_ARBI_TIMES_0);               //设置仲裁限制重发值
  can_rxbuf_warning_set(CAN1, 3);                                       //设置溢出报警值
  can_mode_set(CAN1, CAN_MODE_COMMUNICATE);                             //正常通信模式
  can_software_reset(CAN1, FALSE);                                      //解除软件复位

#if(USE_CAN_IT)
  /* CAN中断配置 */
  nvic_irq_enable(CAN1_RX_IRQn, 0, 0);
  nvic_irq_enable(CAN1_ERR_IRQn, 0, 0);
  can_interrupt_enable(CAN1, CAN_RAFIE_INT, TRUE);                      //使能FIFO报警中断
  can_interrupt_enable(CAN1, CAN_RFIE_INT, TRUE);                       //使能接收FIFO非空中断
  can_interrupt_enable(CAN1, CAN_ROIE_INT, TRUE);                       //使能接收溢出中断
  can_interrupt_enable(CAN1, CAN_RIE_INT, TRUE);                        //使能全局接收中断
  can_interrupt_enable(CAN1, CAN_BEIE_INT, TRUE);                       //使能总线错误中断
#endif

  return SUCCESS;
}

 /**
  * @brief 过滤器配置
  * 
  * @param can_x            ：选择CAN外设
  * @param id               ：ID号    
  * @param mask_id          ：屏蔽的ID号，由于是1忽略，0屏蔽，所以需要取反
  * @param can_filter_num   ：选择使用哪个过滤器，范围0~15
  */
void Bsp_Can_Filter_config(can_type* can_x, uint32_t id, uint32_t mask_id,can_filter_type can_filter_num)
{
  can_filter_config_type can_filter_struct;

  can_software_reset(can_x, TRUE);                                  //开启软件复位

  can_filter_default_para_init(&can_filter_struct);
  can_filter_struct.mask_para.id_type = FALSE;                       
  can_filter_struct.code_para.id_type = CAN_ID_STANDARD;            //选择帧类型
  can_filter_struct.mask_para.id = mask_id;                         //指定位为0过滤，1忽略
  can_filter_struct.code_para.id = id;                              //ID
  can_filter_struct.mask_para.data_length = 0x0F;                    //指定位为0过滤，1忽略
  can_filter_struct.code_para.data_length = 0x8;                    //数据长度
  can_filter_struct.mask_para.frame_type = TRUE;
  can_filter_struct.code_para.frame_type = CAN_FRAME_DATA;          //帧内容
  can_filter_struct.mask_para.recv_frame = TRUE;
  can_filter_struct.code_para.recv_frame = CAN_RECV_NORMAL;         //接收什么模式的帧
  can_filter_struct.mask_para.fd_format = FALSE;
  can_filter_struct.code_para.fd_format = CAN_FORMAT_CLASSIC;       //帧标准
  can_filter_struct.mask_para.fd_rate_switch = FALSE;
  can_filter_struct.code_para.fd_rate_switch = CAN_BRS_OFF;         //是否自动变换波特率
  can_filter_struct.mask_para.fd_error_state = TRUE;
  can_filter_struct.code_para.fd_error_state = CAN_ESI_ACTIVE;      //错误类型
  can_filter_set(can_x, can_filter_num, &can_filter_struct);

  can_filter_enable(can_x, can_filter_num, TRUE);                   //使能指定FIFO
  can_receive_all_enable(can_x, FALSE);                             //关闭接收所有帧
  can_software_reset(can_x, FALSE);                                 //关闭软件复位
}

/**
 * @brief 设置CAN的波特率
 * 
 * @param can_x ：选择哪个CAN外设
 * @param baudrate ：设置波特率，单位为Kbit/s
 */
void Can_Boardrate_Set(can_type* can_x, uint16_t baudrate)
{
  can_bittime_type can_bittime_struct;

  can_software_reset(can_x, TRUE);

   /* 获取默认时间配置 */
  can_bittime_default_para_init(&can_bittime_struct); 

  switch(baudrate)
  {
    case 1000:
        can_bittime_struct.bittime_div = 1;
        can_bittime_struct.ac_bts1_size = 144;
        can_bittime_struct.ac_bts2_size = 48;
        can_bittime_struct.ac_rsaw_size = 48;
        break;
    case 800:
        can_bittime_struct.bittime_div = 1;
        can_bittime_struct.ac_bts1_size = 180;
        can_bittime_struct.ac_bts2_size = 60;
        can_bittime_struct.ac_rsaw_size = 60;
        break;
    case 500:
        can_bittime_struct.bittime_div = 1;
        can_bittime_struct.ac_bts1_size = 288;
        can_bittime_struct.ac_bts2_size = 96;
        can_bittime_struct.ac_rsaw_size = 96;
        break;
    case 250:
        can_bittime_struct.bittime_div = 2;
        can_bittime_struct.ac_bts1_size = 288;
        can_bittime_struct.ac_bts2_size = 96;
        can_bittime_struct.ac_rsaw_size = 96;
        break;
    case 125:
        can_bittime_struct.bittime_div = 3;
        can_bittime_struct.ac_bts1_size = 384;
        can_bittime_struct.ac_bts2_size = 128;
        can_bittime_struct.ac_rsaw_size = 128;
        break;
    case 100:
        can_bittime_struct.bittime_div = 16;
        can_bittime_struct.ac_bts1_size = 90;
        can_bittime_struct.ac_bts2_size = 30;
        can_bittime_struct.ac_rsaw_size = 30;
        break;
    default:
        break;
  }
  can_bittime_struct.fd_bts1_size = 0;
  can_bittime_struct.fd_bts2_size = 0;
  can_bittime_struct.fd_rsaw_size = 0;
  can_bittime_struct.fd_ssp_offset = 0;
  can_bittime_set(can_x, &can_bittime_struct);

  can_software_reset(can_x, FALSE);
}

/**
 * @brief 发送数据函数
 * 
 * @param id ：ID号
 * @param data ：数据
 * @param num ：数据个数，小于等于8
 */
void Bsp_Can_Transmit_Classic_Standard(can_type* can_x, uint32_t id, uint8_t *data, uint8_t num)
{
  can_txbuf_type  can_txbuf_struct = {0};
  uint8_t i;

  can_txbuf_struct.id = id;
  can_txbuf_struct.id_type = CAN_ID_STANDARD;
  can_txbuf_struct.frame_type = CAN_FRAME_DATA;
  can_txbuf_struct.fd_format = CAN_FORMAT_CLASSIC;
  can_txbuf_struct.fd_rate_switch = CAN_BRS_OFF;
  // can_txbuf_struct.data_length = CAN_DLC_BYTES_8;
  can_txbuf_struct.data_length = (can_data_length_type) num;
  num = (num > 8) ? 8 : num;
  for(i= 0; i < num; i++)
  {
    can_txbuf_struct.data[i] = data[i];
  }
  
  while(can_txbuf_write(can_x, CAN_TXBUF_PTB, &can_txbuf_struct) != SUCCESS);

  /* transmit the primary transmit buffer */
  can_txbuf_transmit(can_x, CAN_TRANSMIT_PTB);
  while(can_flag_get(can_x, CAN_TPIF_FLAG) != SET);
  can_flag_clear(can_x, CAN_TPIF_FLAG);
}

/**
 * @brief 轮询接收CAN报文
 * 
 * @param can_x             ：指定使用的CAN外设
 * @param can_rxbuf_struct  ：存储接收的报文
 */
void Bsp_Can_Receive_Classic_Standard(can_type* can_x, can_rxbuf_type* can_rxbuf_struct)
{
    while(can_rxbuf_read(CAN1, can_rxbuf_struct) != SUCCESS)
    {
        //等待接收数据
    }
}

// void CAN1_RX_IRQHandler(void)
// {
//   can_rxbuf_type can_rxbuf_struct;

//   /* rx_buffer had data be received */
//   if(can_interrupt_flag_get(CAN1, CAN_RIF_FLAG) != RESET)
//   {
//     can_flag_clear(CAN1, CAN_RIF_FLAG);
//     while(ERROR != can_rxbuf_read(CAN1, &can_rxbuf_struct))
//     {
//         //执行相应操作
//         Bsp_Can_Transmit_Classic_Standard(CAN1, can_rxbuf_struct.id, can_rxbuf_struct.data,can_rxbuf_struct.data_length);       //测试样例
//     }
//   }

//   /* rx_buffer almost full */
//   if(can_interrupt_flag_get(CAN1, CAN_RAFIF_FLAG) != RESET)
//   {
//     can_flag_clear(CAN1, CAN_RAFIF_FLAG);
//   }

//   /* rx_buffer full */
//   if(can_interrupt_flag_get(CAN1, CAN_RFIF_FLAG) != RESET)
//   {
//     can_flag_clear(CAN1, CAN_RFIF_FLAG);
//   }

//   /* rx_buffer overflow */
//   if(can_interrupt_flag_get(CAN1, CAN_ROIF_FLAG) != RESET)
//   {
//     can_flag_clear(CAN1, CAN_ROIF_FLAG);
//   }
// }

void CAN1_ERR_IRQHandler(void)
{
  /* bus error */
  if(can_interrupt_flag_get(CAN1, CAN_BEIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_BEIF_FLAG);
  }
}
