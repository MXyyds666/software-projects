/**
 * @file motor.c
 * @author baishuaijie (baishuaijie17@qq.com)
 * @brief 四足关节电机从机控制实现
 * @version 0.1
 * @date 2026-03-18
 * 
 */
#include "motor.h"
#include "string.h"
#include "flash.h"

CAN_TASK_DATA g_CAN_TASK_FLAG = {0};	//任务函数运行切换标志位

//相关参数
//控制环路参数
TORQUE_POSITION_MIXTURE_DATA torque_position_data ={0};

extern uint32_t g_motor_id;         	//电机ID
//uint16_t g_encoder = 4096;            //编码器转一圈的计数值

//const float CNT_TO_RAD = TWO_PI / ENCODER_ONE_CYCLE;			//将cnt值转为rad的系数
const float CNT_TO_ANGLE = (360.0f / ENCODER_ONE_CYCLE) % 360.0f;			//将cnt值转为°的系数
const float CNT_S_TO_RAD_S = TWO_PI / ENCODER_ONE_CYCLE;		//将cnt/s转为rad/s
const float ANGLE_TO_CNT = ENCODER_ONE_CYCLE / 360.0f;			//将°转为cnt
const float RPM_TO_CNT_S = ENCODER_ONE_CYCLE / 60.0f;			//将rpm转为cnt/s
const float RAD_S2_TO_CNT_S2 = ENCODER_ONE_CYCLE /TWO_PI;		//将加速度的rad/s转为cnt/s
const float CNT_S2_TO_RAD_S2 = TWO_PI / ENCODER_ONE_CYCLE;		//将P/平方秒转为rad/平方秒
const float CNT_S_TO_RPM = 60.0f / ENCODER_ONE_CYCLE;			//将cnt/s转为rpm
const float RPM_TO_RAD_S = PI / 30.0f;							//将速度RPM转为rad/s
const float WRITE_TORQUE_COEFFICIENT = 1000000.0f / KT / RATED_CURRENT;	//写入扭矩系数，转换完电流千分比
const float WRITE_CURRENT_COEFFICIENT = 1000000.0f /RATED_CURRENT;	//写入电流系数，转换完千分比
const float READ_CURRENT_COEFFICIENT = RATED_CURRENT /1000000.0f;	//读取电流系数,转换为A

#if(USE_TEST_API)
void Write_Data(uint16_t idx,uint16_t sub,void* data)
{
    return;
}

void Read_Data(uint16_t idx,uint16_t sub,void *data)
{
    return;
}
#endif

//控制函数声明
static void torque_position_mixture_mode(uint8_t *data);	//控制环路中
static void postion_control_mode(uint8_t *data);			//中断
static void speed_control_mode(uint8_t *data);				//中断
static void current_possition_brake_mode(uint8_t *data);	//中断
static void handle_config_cmd(uint8_t *data, uint8_t len);	//中断
static void handle_query_cmd(uint8_t *data, uint8_t len);	//中断

//共用函数声明
static void send_feedback(uint8_t type);
static uint8_t motor_get_error(void);
/*============================================= 两大任务函数 ============================================*/
/**
 * @brief 广播消息处理函数
 * 
 * @param data 
 * @param len 
 */
static void handle_broadcast_cmd(uint8_t *data, uint8_t len)
{
	// 检查是否为查询ID指令
	if (data[0] == 0xFF && data[1] == 0xFF && data[2] == 0x00 && data[3] == 0x82) {
		uint32_t read_id = 0;
//		Motor_Read_Data(REDA_ID_CODE, 0, &read_id);//读取CAN的ID									//这样我替换成了FLASH对ID的操作
		read_id = *(volatile uint32_t*)ID_INFO_Addr;
		if(g_motor_id == read_id) {
			uint8_t tx[5] = {0xFF, 0xFF, 0x01, (g_motor_id >> 8) & 0xFF, g_motor_id & 0xFF};
			Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 5);
		}
		else {//读取失败返回长度4
			uint8_t tx[4] = {0x80, 0x80, 0x01, 0x80};
			Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 4);
		}
		return;
	}

	// 其他广播指令，先检查目标ID是否为本机
	uint16_t target_id = (data[0] << 8) | data[1];
	if (target_id != g_motor_id && target_id != 0x7F7F) // 0x7F7F用于重置ID
			return;

	if (data[2] != 0x00) // Byte2必须为0x00（控制器发送）
			return;

	uint8_t sub_cmd = data[3];
	switch (sub_cmd) {
		case 0x03: // 零点设置
			g_CAN_TASK_FLAG.can_task_flag = 1;
			memcpy(&g_CAN_TASK_FLAG.data, data, 4);
			g_CAN_TASK_FLAG.len = len;
//			handle_zero_set(data, uint8_t len);
			break;
		case 0x04: // ID设置
			g_CAN_TASK_FLAG.can_task_flag = 2;
			memcpy(&g_CAN_TASK_FLAG.data, data, 6);
			g_CAN_TASK_FLAG.len = len;
//			handle_id_set(data, len);
			break;
		case 0x05: // ID重置
			g_CAN_TASK_FLAG.can_task_flag = 3;
			memcpy(&g_CAN_TASK_FLAG.data, data, 6);
			g_CAN_TASK_FLAG.len = len;
//			handle_id_reset(data, len);
			break;
		default:
			break;
	}
}

/**
 * @brief 控制命令处理函数
 * 
 * @param data 
 * @param len 
 */
static void handle_unicast_cmd(uint8_t *data, uint8_t len)
{
	uint8_t mode = data[0] >> 5;//获取指令的控制模式

    switch (mode) {
		case MOTOR_MODE_MIXED: // 0x00：力位混控模式
			if (len >= 8)
				torque_position_mixture_mode(data);
			break;
		case MOTOR_MODE_POSITION: // 0x01：位置控制模式
			if (len >= 8)
				postion_control_mode(data);
			break;
		case MOTOR_MODE_SPEED: // 0x02：速度控制模式
			if (len >= 7) 
				speed_control_mode(data);
			break;
		case MOTOR_MODE_CURRENT: // 0x03：电流、力矩控制模式和刹车指令
			if (len >= 3)
				current_possition_brake_mode(data);
			break;
		case MOTOR_MODE_CONFIG: // 0x06：电机控制参数配置
			handle_config_cmd(data, len);
			break;
		case MOTOR_MODE_QUERY: // 0x07：电机控制参数查询
			handle_query_cmd(data, len);
			break;
		default:
			break;
    }
}
/*=======================================================================================================*/



/*===================================== 电机配置函数 Start =====================================*/
/**
 * @brief 零点设置函数
 * 
 * @param data 
 * @param len 
 * 
 * @note 已验证
 */
void handle_zero_set(uint8_t *data, uint8_t len)
{
	if(len != 4)
		return;
	uint8_t result = 0;         // 默认失败
	uint8_t actual_mode = 0;    //设置完成标志位
	uint16_t status = 0;        //状态标志位
	int time_out = 0;           //设置超时时间
	uint16_t controlword = 0;	//控制过程中的临时变量
	
	//操作相关变量
	uint8_t target_mode = 6;    	// Homing Mode
	int8_t method = 35;				//方法35，当前位置设置为0点
	int32_t offset = 0;				//偏移量0
	uint8_t pos_mod = 0;			//任务结束后回到的模式
	
	Motor_Write_Data(MODE_OPERATION_CODE, 0, &target_mode);
	time_out = 100;
	while(time_out--) {                   //等待模式切换确认
		Motor_Read_Data(MODE_OPERATION_DISPLAY, 0, &actual_mode);
		if(actual_mode == 6)
			break;
		delay_ms(1);
	}
	if(actual_mode == 6) {
		Motor_Write_Data(HOMING_METHOD_CODE, 0, &method);	//将当前位置直接定义为零点
		Motor_Write_Data(HOME_OFFSET, 0, &offset);	
		
		controlword = 0x0F;
		Motor_Write_Data(CONTROLWORD_CODE, 0, &controlword);//使能电机
		time_out = 200;
		while (time_out--) {                  //等待使能成功
			Motor_Read_Data(STATUSWORD_CODE, 0, &status);
			if ((status & 0x006F) == 0x0027) 
				break;
			delay_ms(1);
		}
		if ((status & 0x006F) == 0x0027) {
			controlword = 0x0F;
			Motor_Write_Data(CONTROLWORD_CODE, 0, &controlword);//触发回零
			delay_ms(5);
			controlword = 0x1F;
			Motor_Write_Data(CONTROLWORD_CODE, 0, &controlword);
			
			delay_ms(200);																						//设置回零后，可能需要保存
		}
	}
	//无论是否成功都要重置控制字并切回原模式
	controlword = 0x0F;
	Motor_Write_Data(CONTROLWORD_CODE, 0, &controlword); 
	if(result) {
		pos_mod = 1;
		Motor_Write_Data(MODE_OPERATION_CODE, 0, &pos_mod);  // 回零后，切换回正常位置模式
	}

	// 发送响应：ID相同，Byte2=0x01（电机发），Byte3=0x03（成功）或0x00（失败）
	uint8_t tx[4];
	tx[0] = data[0]; // 原ID高8位
	tx[1] = data[1]; // 原ID低8位
	tx[2] = 0x01;
	tx[3] = result ? 0x03 : 0x00;
	Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 4);
}

/**
 * @brief ID设置函数
 * 
 * @param data 
 * @param len 
 */
void handle_id_set(uint8_t *data, uint8_t len)
{
    if (len != 6)
		return;
    uint32_t new_id = (uint16_t)((data[4] << 8) | data[5]);
    uint16_t read_id = 0;
    uint8_t status = 0;
	
	//检查ID是否是否合法
	if(new_id == 0 || new_id >= 0x7FF) {
		status = 0;
	}
	else{
		Save_ID_Info(new_id);
		delay_ms(50);
//		Motor_Write_Data(REDA_ID_CODE, 0, &new_id);											//问题：0x351A对象字典的长度大小是8位，ID的范围是0x000~0x7FF，是否会有问题
		//写入ID后什么时候生效？？？
		//是否需要其他什么措施？？？
//		Motor_Read_Data(REDA_ID_CODE, 0, &read_id);				//读取电机ID，检查是否设置成功
		read_id = *(volatile uint32_t*)ID_INFO_Addr;
		if(read_id == new_id) {                    				//设置成功
			status =  1;
			g_motor_id = read_id;
			Bsp_Can_Filter_config(CAN1,g_motor_id,0x0,CAN_FILTER_NUM_1);//设置ID后，更新FIFO1，防止报文被过滤
		}
	}

    // 发送成功响应
    uint8_t tx[4];
    tx[0] = data[0];
    tx[1] = data[1];
    tx[2] = 0x01;
    tx[3] = status ? 0x04 : 0x00;
    Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 4);
}

/**
 * @brief ID重置函数
 * 
 * @param data 
 * @param len 
 */
void handle_id_reset(uint8_t *data, uint8_t len)
{
	if(len != 6)
		return;
	uint32_t reset_id = 0x01;
	uint32_t read_id = 0;

	Save_ID_Info(reset_id);
	delay_ms(5);
//    Motor_Write_Data(REDA_ID_CODE, 0, &reset_id);
    //重置ID后什么时候生效？？？
    //是否需要其他什么措施？？？
//	Motor_Read_Data(REDA_ID_CODE, 0, &read_id);//读取电机ID，检查是否设置成功
	read_id = *(volatile uint32_t*)ID_INFO_Addr;
    if(read_id == 0x01) {                      	//重置成功
		g_motor_id = 0x01;
        Bsp_Can_Filter_config(CAN1,g_motor_id,0x0,CAN_FILTER_NUM_1);//设置ID后，更新FIFO1，防止报文被过滤
    }
    else
		return ;
    uint8_t tx[6] = {0x7F, 0x7F, 0x01, 0x05, 0x7F, 0x7F};
    Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 6);
}

/*===================================== 电机配置函数 End =====================================*/

/*===================================== 电机控制函数 Start =====================================*/
static void torque_position_mixture_mode(uint8_t *data)
{
//	int32_t motor_pos_raw = 0, motor_spd_raw = 0;	//读取位置和速度原始值变量
	
	// 1.解析参数
	uint16_t kp_raw = ((data[0] & 0x1F) << 7) | (data[1] >> 1);
	uint16_t kd_raw = ((data[1] & 0x01) << 8) | data[2];
	uint16_t pos_raw = (data[3] << 8) | data[4];
	uint16_t spd_raw = (data[5] << 4) | (data[6] >> 4);
	uint16_t tor_raw = ((data[6] & 0x0F) << 8) | data[7];

	// 转换为物理量
	float kp = kp_raw * KP_COEFFICIENT;
	float kd = kd_raw * KD_COEFFICIENT;
	float pos_desire = pos_raw * POS_COEFFICIENT - 12.5f;	//单位rad
	float spd_desire = spd_raw * SPD_COEFFICIENT - 18.0f;	//单位rad/s
	float tor = tor_raw * TOR_COEFFICIENT - 180.0f;			//单位Nm
	
	//写入参数
	torque_position_data.Kp = kp;
	torque_position_data.Kd = kd;
	torque_position_data.pos = pos_desire;
	torque_position_data.spd = spd_desire;
	torque_position_data.tor = tor;

//	//控制环路内进行实际位置和速度的读取操作
//	Motor_Read_Data(POSITION_ACTUAL_VALUE, 0, &motor_pos_raw);							//问题三：这里读取的速度和位置的单位，不确定，影响下面的计算
//	Motor_Read_Data(VELOCITY_ACTUAL_VALUE, 0, &motor_spd_raw);							//以及这里这两个指令读取的时负载端的数据吗，这里按是负载端计算，没有算上减速比
//	//读取数据处理
//	float pos_current = motor_pos_raw * CNT_TO_RAD;				//默认脉冲进行转换，转换后单位rad
//	float spd_current = motor_spd_raw * RPM_TO_RAD_S;			//fix：默认读取速度是RPM，需要转为rad/s 
//	// CIA402中没有写入电流的索引值，只有扭矩的索引值，这里使用扭矩的目标值，代替电流值，由于使用了扭矩，故不需要除以KT
//	uint32_t motor_current = (kp * (pos_desire - pos_current) + 
//	                             kd * (spd_desire - spd_current) + tor) / KT;
//	int16_t tarque = motor_current * WRITE_TORQUE_COEFFICIENT;							//问题四：这里直接计算力矩，单位是否是千分之一
//	Motor_Write_Data(TARGET_TORQUE_CODE, 0, &tarque);
	// 返回类型1报文
	send_feedback(MSG_TYPE_CTRL1);
}

static void postion_control_mode(uint8_t *data)
{
	//判断当前模式
	uint8_t read_mode = 0;
	Motor_Read_Data(0x6061, 0, &read_mode);
	
	//数据拼接
	uint32_t pos_bits = ((uint32_t)(data[0] & 0x1F) << 27)	|
						((uint32_t)data[1] << 19)			|
						((uint32_t)data[2] << 11)           |
						((uint32_t)data[3] << 3) 			|
						((uint32_t)(data[4] >> 5) & 0x07);
	uint16_t speed_raw = ((uint16_t)(data[4] & 0x1F) << 10)	|
						 ((uint16_t)data[5] << 2)			|
						 ((uint16_t)(data[6] >> 6) & 0x03);
	uint16_t current_raw = ((uint16_t)(data[6] & 0x3F) << 6)| 
						   ((uint16_t)data[7] >> 2);
	uint8_t ret_type_pos = data[7] & 0x03; // 返回类型在Byte7低2位

	//进行转换
	float pos_deg = 0;
	memcpy(&pos_deg, &pos_bits, sizeof(float));     		//期望位置，单位°
	float speed_rpm = (float)speed_raw / 10.0f;				//期望速度，单位rpm
	float current_A = (float)current_raw / 10.0f;			//电流阈值，单位A
	
	int32_t pos_cnt = pos_deg * ANGLE_TO_CNT;				//单位cnt
	int32_t speed_cnt_s = speed_rpm * RPM_TO_CNT_S;			//单位cnt/s
	uint32_t current_a = current_A * 1000;					//单位mA
	
	
	Motor_Write_Data(TARGET_POSITION, 0, &pos_cnt);
	Motor_Write_Data(TARGET_VELOCITY, 0, &speed_cnt_s);
	Motor_Write_Data(APPLICATION_CURRENT_CODE, 0, &current_a);	//应用电流现在，暂认为原始单位就是mA

	if (ret_type_pos != 0) {
		if (ret_type_pos == 1) 
			send_feedback(MSG_TYPE_CTRL1);
		else if (ret_type_pos == 2) 
			send_feedback(MSG_TYPE_CTRL2);
		else if (ret_type_pos == 3) 
			send_feedback(MSG_TYPE_CTRL3);
	}
}

static void speed_control_mode(uint8_t *data)
{
	uint8_t ret_type = data[0] & 0x03;		//获取返回报文类型
	float speed = 0;
	
	//原始值
	uint32_t speed_raw = (data[1] << 24) |					//手册中未规定这里速度的单位，暂定RPM
						 (data[2] << 16) |
						 (data[3] << 8 ) |
							data[4];
	uint16_t current_raw = (data[5] << 8) |					//认为CANOPEN写入单位是mA	
							data[6];
					
	memcpy(&speed, &speed_raw, sizeof(float));
	float current_A = current_raw / 10.0f;		//单位A
	int32_t speed_cnt_s = speed * RPM_TO_CNT_S; //认为发送速度单位为RPM，转换后单位P/S
	uint32_t current_a = current_A * 1000;		//单位mA

	Motor_Write_Data(TARGET_VELOCITY, 0, &speed_cnt_s);
	Motor_Write_Data(APPLICATION_CURRENT_CODE, 0, &current_a);

	// 发送返回消息
	if (ret_type != 0) {
		if (ret_type == 1) send_feedback(MSG_TYPE_CTRL1);
		else if (ret_type == 2) send_feedback(MSG_TYPE_CTRL2);
		else if (ret_type == 3) send_feedback(MSG_TYPE_CTRL3);
	}
}

static void current_possition_brake_mode(uint8_t *data)
{
	uint8_t ret_type = data[0] &0x03;				//获取返回报文类型
	uint8_t ctrl_state = (data[0] >> 2) & 0x07;		// 预留控制位
	int16_t current_raw = (data[1] << 8) | data[2];// 期望电流/力矩

	float current_or_torque = current_raw / 100.0f;	// 写入值,单位A/Nm

	//根据控制模式，控制电机，写入控制参数
	switch(ctrl_state)
	{
		case CTRL_STATE_CURRENT: {// 电流控制模式
			int32_t current_a = current_or_torque * WRITE_CURRENT_COEFFICIENT;		//这里计算正确
			Motor_Write_Data(TARGET_TORQUE_CODE, 0, &current_a);
			break;
		}
		case CTRL_STATE_TORQUE: {//力矩控制模式
			int32_t torque = current_or_torque * WRITE_TORQUE_COEFFICIENT;
			Motor_Write_Data(TARGET_TORQUE_CODE, 0, &torque);
			break;
		}
		// 我司暂不支持接口
		// case CTRL_STATE_DAMPING://变阻尼控制模式
		// {
		//     break;
		// }
		// case CTRL_STATE_ENERGY://能耗制动控制模式
		// {
		//     break;
		// }
		// case CTRL_STATE_REGEN://再生制动控制模式
		// {
		//     break;
		// }
	}

	if (ret_type != 0) {
		if (ret_type == 1) send_feedback(MSG_TYPE_CTRL1);
		else if (ret_type == 2) send_feedback(MSG_TYPE_CTRL2);
		else if (ret_type == 3) send_feedback(MSG_TYPE_CTRL3);
	}
}

/**
 * @brief 配置函数
 * 
 * @param data 
 * @param len 
 */
static void handle_config_cmd(uint8_t *data, uint8_t len)
{
    uint8_t ret_type = data[0] & 0x03;      // 获取返回类型
    uint8_t config_code = data[1];          // 配置代码
    uint8_t res_control_status = (data[0] >> 2) & 0x07;
		uint16_t read_data = 0;
    uint8_t status = 0;                     // 默认失败

    // 控制预留位必须为0
    if(!res_control_status) {
        if (config_code == 0x01 && len== 4) { // 加速度配置
            uint16_t accel_raw = (data[2] << 8) | data[3];
			uint16_t accel = (accel_raw <= 2000)? accel_raw: 2000;
			accel /= 100;
			uint32_t rpm = accel * RAD_S2_TO_CNT_S2;					//加速度单位不明确，需要确定
			Motor_Write_Data(PROFILE_ACCELERATION, 0, &rpm);			//写入单位P/平方秒
			Motor_Read_Data(PROFILE_ACCELERATION, 0, &read_data);
			if(read_data == rpm)
				status = 1; // 成功
        }
        // 我司暂不支持接口
//		else if (config_code == 0x02 && len >= 6) { // 补偿系数与阻尼
//			 uint16_t comp_raw = unpack_uint16(&data[2]);
//			 uint16_t damp_raw = unpack_uint16(&data[4]);

//			 //调用接口，写入电机反馈系数、阻尼系数
//			 motor_set_param(CONFIG_COMP, comp_raw);
//			 motor_set_param(CONFIG_DAMP, damp_raw);
//			 status = 1;
//		}
    }
	else	//预留状态位不为0，直接返回，没有反馈值
	{
		return ;
	}
    if (ret_type == 1) { // 只有返回类型1才发送配置反馈
        uint8_t tx[3];
        // 调用接口，需要获取电机的错误信息发送再发送
		tx[0] = (MSG_TYPE_CONFIG << 5) | (motor_get_error() & 0x1F);
        tx[0] = MSG_TYPE_CONFIG << 5;
        tx[1] = config_code;
        tx[2] = status;
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 3);
    }
}

/**
 * @brief 查询处理函数
 * 
 * @param data 
 * @param len 
 */
static void handle_query_cmd(uint8_t *data, uint8_t len)
{
    uint8_t query_code = data[1];	//查询代码
    uint8_t tx[8] = {0};
    int32_t read_data1 = 0;       	//查询的数据1
	uint32_t read_data2 = 0;		//查询的数据2
    float send_data1 = 0;
	int16_t send_data2 = 0;
	tx[0] = (MSG_TYPE_QUERY << 5) | (motor_get_error() & 0x1F);
    tx[1] = query_code;

    if (query_code >= QUERY_POSITION && query_code <= QUERY_POWER) {
        /*----------------- 查询指令，待验证索引、单位！！！！ -----------------*/
		switch (query_code) {
		case 0x01: {  //查询位置
			Motor_Read_Data(POSITION_ACTUAL_VALUE, 0, &read_data1);	//单位P	
			//read_data是cnt值？如果是进行下述转换,转为rad
			send_data1 = read_data1 * CNT_TO_RAD;	//单位rad
			break;
			}
        case 0x02: {  //查询速度
			Motor_Read_Data(VELOCITY_ACTUAL_VALUE, 0, &read_data1);	//单位P/s
            //如果速度是P/s，进行下列转换，转为rpm
            send_data1 = read_data1 * CNT_S_TO_RPM;	//单位rpm
            break;
        }
        case 0x03: {  //查询电流
			Motor_Read_Data(CURRENT_CURRENT, 0, &read_data1);		//单位mA
			send_data1 = read_data1 * READ_CURRENT_COEFFICIENT;
            break;
        }
        case 0x04:{  //查询功率，暂无实现
//			OD_0x35B5_NooAppParameters_Read_Callback(3, &read_data1);		//读取功率
            break;
        }
        default:
            break;
        }
        /*---------------------------------------------------------------------*/
        uint32_t tx_data = 0;   //将float转为uint32，返回给上位机
        memcpy(&tx_data, &send_data1, sizeof(float));

        tx[2] = ((uint8_t)(tx_data >> 24) & 0xFF);
        tx[3] = (uint8_t)((tx_data >> 16) & 0xFF);
        tx[4] = (uint8_t)((tx_data >> 8) & 0xFF);
        tx[5] = (uint8_t)(tx_data & 0xFF);
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 6);
    }
		// 阻尼系数、反馈补偿系数我司暂不支持,仅支持加速度查询
    else if (query_code >= QUERY_ACCEL && query_code <= QUERY_DAMPING) {
        switch (query_code) {
			case 0x05:
				Motor_Read_Data(PROFILE_ACCELERATION, 0, &read_data2);
				send_data2 =  read_data2 * CNT_S2_TO_RAD_S2;
				break;
			default:
				break;
		}
		tx[2] = send_data2 >> 8;
		tx[3] = send_data2 & 0xFF;
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 4);
    }
}

/*===================================== 电机控制函数 End =====================================*/

/*====================================== 共用函数 Start ======================================*/
static void send_feedback(uint8_t type)
{
    uint8_t data[8] = {0};
	int32_t pos_read = 0, speed_read = 0;          
    int16_t current_read = 0;
	int32_t motor_temp = 0, mos_temp = 0;	
	uint32_t temp = 0;

    uint8_t error = motor_get_error(); // 从硬件获取
    
    data[0] = (type << 5) | (error & 0x1F);

    // 这里读取的参数单位不确定，需要确认！！！！
	Motor_Read_Data(POSITION_ACTUAL_VALUE, 0, &pos_read);		//认为是cnt值
	Motor_Read_Data(VELOCITY_ACTUAL_VALUE, 0, &speed_read);		//认为是cnt/s
	Motor_Read_Data(CURRENT_CURRENT, 0, &current_read);			//读取的电流是额定电流的千分之一

//	OD_0x35B5_NooAppParameters_Read_Callback(1, &motor_temp);	//电机温度,单位0.1°C
	Motor_Read_Data(DRIVE_TEMPERATURE_CODE, 0, &mos_temp);		//MOS温度,单位°C

    switch (type) {
        case MSG_TYPE_CTRL1: {
            uint16_t pos_raw = (pos_read * CNT_TO_RAD + 12.5f) * SENNDBACK_1_POS_COEFFICIENT;
            uint16_t spd_raw = (speed_read * CNT_S_TO_RAD_S + 18.0f) * SENNDBACK_1_SPD_COEFFICIENT;
            uint16_t cur_raw = (current_read * READ_CURRENT_COEFFICIENT +100.0f) * SENNDBACK_1_CUR_COEFFICIENT;
            uint8_t motor_temp_raw = (uint8_t)(motor_temp * 0.2 + 50);											//问题九相关
            uint8_t mos_temp_raw = (uint8_t)(mos_temp * 2 + 50);

            data[1] = pos_raw >> 8;
            data[2] = pos_raw & 0xFF;
            data[3] = spd_raw >> 4;
            data[4] = ((spd_raw & 0x0F) << 4) | ((cur_raw >> 8) & 0x0F);
            data[5] = cur_raw & 0xFF;
			data[6] = motor_temp_raw;
			data[7] = mos_temp_raw;
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            break;
        }
        case MSG_TYPE_CTRL2: {
            float pos_deg = pos_read * CNT_TO_RAD;
			int16_t cur = (current_read * READ_CURRENT_COEFFICIENT) * 100;
			
			memcpy(&temp, &pos_deg, sizeof(float));//通信协议要求大端在前，所以需要进行调换顺序
			data[1] = (temp >> 24) & 0xFF;
			data[2] = (temp >> 16) & 0xFF;
			data[3] = (temp >> 8) & 0xFF;
			data[4] = temp & 0xFF;			
			data[5] = (uint8_t)(cur >> 8);
			data[6] = (uint8_t)(cur & 0xFF);
			data[7] = (uint8_t)(motor_temp * 2 + 50);
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            break;
        }
        case MSG_TYPE_CTRL3: {
            float speed_rpm = speed_read * CNT_S_TO_RPM;
			int16_t cur = (current_read * READ_CURRENT_COEFFICIENT) * 100;
            
			memcpy(&temp, &speed_rpm, sizeof(float));//通信协议要求大端在前，所以需要进行调换顺序
			data[1] = (temp >> 24) & 0xFF;
			data[2] = (temp >> 16) & 0xFF;
			data[3] = (temp >> 8) & 0xFF;
			data[4] = temp & 0xFF;
            data[5] = (uint8_t)(cur >> 8);
			data[6] = (uint8_t)(cur & 0xFF);
            data[7] = (uint8_t)(motor_temp * 2 + 50);
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            break;
        }
        case MSG_TYPE_CONFIG:
        case MSG_TYPE_QUERY:
            // 这两种由专门的处理函数填充数据并直接发送，这里不处理
        default:
            break;
    }
}

/**
 * @brief 获取电机错误，部分错误不确定，待完善！
 * 
 * @return uint8_t 
 */
static uint8_t motor_get_error(void)																	//问题十：错误码的索引码不明确
{
    uint16_t error_code = 0;
	Motor_Read_Data(ERROR_CODE, 0, &error_code);
    switch(error_code)
    {
//		//暂无
//        case 0x4210:    //电机过热，Excess temperature device
//        {
//            return 1;
//        }
        case 0xFF07:    //电机过流，通过判断电机过流时间判断？
        {
            return 2;
        }
		
        case 0x3220:    //电机欠压，DC link under-voltage，这里先用母线电压占位，实际可能是其他！！！！
        {
            return 3;
        }
        case 0x7303:    //编码器错误
        {
            return 4;
        }
        case 0x3210:    //刹车电压过高，DC link over-voltage，不确定！！！
        {
            return 6;
        }
		//暂无
//        case 0x7300:    //手册中该代码值指传感器错误，是否对应DRV驱动，待确定！！！
//        {
//            return 7;
//        }
		default :
			return 8;
    }

    //电机过流，通过判断电机过流时间判断？返回错误码
    //编码器错误：编码器1：0x3006，编码器2：0x358A，返回错误码4
}
/*======================================= 共用函数 End =======================================*/

void CAN1_RX_IRQHandler(void)
{
  can_rxbuf_type can_rxbuf_struct;

  /* rx_buffer had data be received */
  if(can_interrupt_flag_get(CAN1, CAN_RIF_FLAG) != RESET)
  {
    can_flag_clear(CAN1, CAN_RIF_FLAG);
    while(ERROR != can_rxbuf_read(CAN1, &can_rxbuf_struct))
    {
        //执行相应操作
        if(can_rxbuf_struct.id == 0x7FF)
        {
			handle_broadcast_cmd(can_rxbuf_struct.data, can_rxbuf_struct.data_length);
        }
        else if(can_rxbuf_struct.id == g_motor_id)
        {
			handle_unicast_cmd(can_rxbuf_struct.data,can_rxbuf_struct.data_length);
        }
        //其他ID忽略
        // Bsp_Can_Transmit_Classic_Standard(CAN1, can_rxbuf_struct.id, can_rxbuf_struct.data,can_rxbuf_struct.data_length);       //测试样例
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
