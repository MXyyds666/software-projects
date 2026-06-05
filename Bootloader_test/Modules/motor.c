/**
 * @file motor.c
 * @author baishuaijie (baishuaijie17@qq.com)
 * @brief 四足关节电机从机控制实现
 * @version 0.1
 * @date 2026-03-18
 * 
 * 
 * 
 */
#include "motor.h"
#include "string.h"

uint16_t g_motor_id = 0x01;         //电机ID
uint16_t g_encoder = 4096;            //编码器转一圈的计数值

#if(USE_TEST_API)
void Write_Data(uint16_t idx,uint16_t sub,uint32_t data)
{
    return;
}

void Read_Data(uint16_t idx,uint16_t sub,uint32_t *data)
{
    return;
}
#endif

/*===================================== 工具接口函数 Start =====================================*/

/* ==========================================
   一、 基础读写包装层 (解决类型兼容性)
   ========================================== */
/**
 * @brief 写入 32 位有符号整数
 * 
 * @param indx : 索引码
 * @param sub  ：子索引
 * @param val  ：写入值
 */
static void Motor_Write_Int32(uint16_t idx, uint8_t sub, int32_t val) {
    Motor_Write_Data(idx, sub, (uint32_t)val);
}

/**
 * @brief 写入 16 位有符号整数 (符号位扩展)
 * 
 * @param idx 
 * @param sub 
 * @param val 
 */
static void Motor_Write_Int16(uint16_t idx, uint8_t sub, int16_t val) {
    // 先转为 int32 保证符号位正确（-1 变成 0xFFFFFFFF），再传给 API
    Motor_Write_Data(idx, sub, (uint32_t)(int32_t)val);
}

/**
 * @brief 写入 16 位无符号整数 (符号位扩展)
 * 
 * @param idx 
 * @param sub 
 * @param val 
 */
static void Motor_Write_Uint16(uint16_t idx, uint8_t sub, uint16_t val) {
    // 先转为 int32 保证符号位正确（-1 变成 0xFFFFFFFF），再传给 API
    Motor_Write_Data(idx, sub, (uint32_t)val);
}

/**
 * @brief 写入 32 位浮点数 (IEEE-754 位拷贝)
 * 
 * @param idx 
 * @param sub 
 * @param val 
 */
static void Motor_Write_Float(uint16_t idx, uint8_t sub, float val) {
    MotorData_t convert;
    convert.f = val;
    Motor_Write_Data(idx, sub, convert.u32);
}

/**
 * @brief 读取并还原为 32 位有符号数
 * 
 * @param id 
 * @param idx 
 * @param sub 
 * @return int32_t 
 */
static int32_t Motor_Read_Int32(uint16_t idx, uint8_t sub) {
    uint32_t raw;
    Motor_Read_Data(idx, sub, &raw);
    return (int32_t)raw;
}

/**
 * @brief 读取并还原为 16 位有符号数 (截断处理)
 * 
 * @param id 
 * @param idx 
 * @param sub 
 * @return int16_t 
 */
static int16_t Motor_Read_Int16(uint16_t idx, uint8_t sub) {
    uint32_t raw;
    Motor_Read_Data(idx, sub, &raw);
    return (int16_t)(uint16_t)raw; // 强转截断
}

/**
 * @brief 读取并还原为 16 位无符号数
 * 
 * @param idx 
 * @param sub 
 * @return uint16_t 
 */
static uint16_t Motor_Read_Uint16(uint16_t idx, uint8_t sub) {
    uint32_t raw = 0;
    Motor_Read_Data(idx, sub, &raw);
    return (uint16_t)raw;
}

/**
 * @brief 读取并还原为 8 位无符号数
 * 
 * @param idx 
 * @param sub 
 * @return uint16_t 
 */
static uint8_t Motor_Read_Uint8(uint16_t idx, uint8_t sub) {
    uint32_t raw = 0;
    Motor_Read_Data(idx, sub, &raw);
    return (uint8_t)raw;
}

/**
 * @brief 读取并还原为浮点数
 * 
 * @param idx 
 * @param sub 
 * @return float 
 */
static float Motor_Read_Float(uint16_t idx, uint8_t sub) {
    uint32_t raw;
    Motor_Read_Data(idx, sub, &raw);
    MotorData_t convert;
    convert.u32 = raw;
    return convert.f;
}

/**
 * @brief 将两个8位无符号整数转为16整数
 * 
 * @param buf 
 * @return uint16_t 
 */
static inline uint16_t unpack_uint16(const uint8_t* buf)
{
    return (buf[0] << 8) | buf[1];
}

/**
 * @brief 将四个8位无符号整数转为float整数
 * 
 * @param buf 
 * @return float 
 */
static inline float unpack_float(const uint8_t* buf)
{
    uint32_t u = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return *(float*)&u;
}

/**
 * @brief 将16位无符号整数按大端序打包到两个字节
 * @param buf 目标缓冲区（至少2字节）
 * @param val 要打包的值
 */
static inline void pack_uint16(uint8_t* buf, uint16_t val)
{
    buf[0] = (val >> 8) & 0xFF;  // 高8位
    buf[1] = val & 0xFF;          // 低8位
}

/**
 * @brief 将32位浮点数按大端序打包到四个字节
 * @param buf 目标缓冲区（至少4字节）
 * @param val 要打包的浮点数
 */
static inline void pack_float(uint8_t* buf, float val)
{
    uint32_t u;
    // 将浮点数的内存表示视为32位无符号整数
    u = *(uint32_t*)&val;
    buf[0] = (u >> 24) & 0xFF;   // 最高8位
    buf[1] = (u >> 16) & 0xFF;
    buf[2] = (u >> 8) & 0xFF;
    buf[3] = u & 0xFF;            // 最低8位
}

/* ==========================================
   二、 物理单位换算层 (度/RPM/脉冲)
   ========================================== */
/**
 * @brief 角度（deg，单位：rad）-> 脉冲（cnt）
 * 
 * @param deg ：角度值
 * @return int32_t 
 */
static int32_t Deg_To_Cnt(float deg) {
    return (int32_t)(deg * (ENCODER_ONE_CICLE * GEAR_RATIO / TWO_PI));
}

/**
 * @brief 脉冲（cnt）->角度（deg，单位rad）
 * 
 * @param cnt 
 * @return float
 */
static float Cnt_To_Deg(int32_t cnt) {
    return (float)cnt * (TWO_PI / (ENCODER_ONE_CICLE * GEAR_RATIO));
}

/**
 * @brief 速度(RPM) -> 脉冲每秒(cnt/s)
 * 
 * @param rpm 
 * @return int32_t 
 */
static int32_t Rpm_To_CntS(float rpm) {
    return (int32_t)(rpm * (ENCODER_ONE_CICLE * GEAR_RATIO / 60.0f));
}

/**
 * @brief 脉冲每秒(cnt/s) -> 速度(RPM)
 * 
 * @param cnt_s 
 * @return float 
 */
static float CntS_To_Rpm(int32_t cnt_s) {
    return (float)cnt_s * (60.0f / (ENCODER_ONE_CICLE * GEAR_RATIO));
}

/*===================================== 工具接口函数 End =====================================*/


/*===================================== 电机配置函数 Start =====================================*/
/**
 * @brief 零点设置函数
 * 
 * @param data 
 * @param len 
 * 
 * @note 已验证
 */
static void handle_zero_set(uint8_t *data, uint8_t len)
{
    uint8_t result = 0;         // 默认失败
    uint8_t actual_mode = 0;    //设置完成标志位
    uint16_t status = 0;         //状态标志位
    int time_out = 0;           //设置超时时间

    /*----------------- 设置零点指令，待验证索引的正确性！！！！ -----------------*/
    Motor_Write_Data(MODE_OPERATION_CODE, 0, 6);     //设置回零模式
    time_out = 100;
    while(time_out--)                   //等待模式切换确认
    {
        actual_mode = Motor_Read_Uint8(MODE_OPERATION_DISPLAY, 0); 
        if(actual_mode == 6)
            break;
        delay_ms(1);
    }
    if(actual_mode == 6)
    {
        Motor_Write_Data(HOMING_METHOD_CODE, 0, 35);    //将当前位置直接定义为零点
        Motor_Write_Data(CONTROLWORD_CODE, 0, 0x0F);  //使能电机
        time_out = 200;
        while (time_out--)                  //等待使能成功
        {    
            status = Motor_Read_Uint16(STATUSWORD_CODE, 0);
            if ((status & 0x000F) == 0x0007) 
                break;
            delay_ms(1);
        }
        if ((status & 0x000F) == 0x0007)
        {
            Motor_Write_Data(CONTROLWORD_CODE, 0, 0x1F);  //触发回零
            time_out = 1000;
            while(time_out--)                   //检查设置
            {
                status = Motor_Read_Uint16(STATUSWORD_CODE, 0);
                if((status >> 12) & 0x01)
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    if(result)
    {
        Motor_Write_Data(CONTROLWORD_CODE, 0, 0x0F); // 清除触发位
        Motor_Write_Data(MODE_OPERATION_CODE, 0, 1);    // 切换回正常位置模式
    }
    /*-----------------------------------------------------------------------*/

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
static void handle_id_set(uint8_t *data, uint8_t len)
{
    if (len < 6) return;
    uint16_t new_id = (data[4] << 8) | data[5];
    g_motor_id = new_id;
    uint8_t status = 0;

    /*----------------- 设置ID指令，待验证索引的正确性！！！！ -----------------*/
    Motor_Write_Data(REDA_ID_CODE, 0, new_id);
    //写入ID后什么时候生效？？？
    //是否需要其他什么措施？？？
    g_motor_id = Motor_Read_Uint16(REDA_ID_CODE, 0);    //读取电机ID，检查是否设置成功
    if(g_motor_id == new_id)                    //设置成功
    {
        status =  1;
        Bsp_Can_Filter_config(CAN1,new_id,0x0,CAN_FILTER_NUM_1);//设置ID后，更新FIFO1，防止报文被过滤
    }
    /*-----------------------------------------------------------------------*/

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
static void handle_id_reset(uint8_t *data, uint8_t len)
{
    // 检查数据是否符合（可选）
    g_motor_id = 0x01;

    /*----------------- 设置ID指令，待验证索引的正确性！！！！ -----------------*/
    Motor_Write_Data(REDA_ID_CODE, 0, 0x01);
    //重置ID后什么时候生效？？？
    //是否需要其他什么措施？？？
    g_motor_id = Motor_Read_Uint16(REDA_ID_CODE, 0);    //读取电机ID，检查是否设置成功
    if(g_motor_id == 0x01)                      //重置成功
    {
        Bsp_Can_Filter_config(CAN1,0x01,0x0,CAN_FILTER_NUM_1);//设置ID后，更新FIFO1，防止报文被过滤
    }
    else
        return ;
    /*-----------------------------------------------------------------------*/

    uint8_t tx[6] = {0x7F, 0x7F, 0x01, 0x05, 0x7F, 0x7F};
    Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 6);
}

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
        /*----------------- 查询ID响应，待确认索引的正确性！！！！ -----------------*/
        g_motor_id = Motor_Read_Uint16(REDA_ID_CODE, 0);
        /*-----------------------------------------------------------------------*/
        uint8_t tx[5] = {0xFF, 0xFF, 0x01, (g_motor_id >> 8) & 0xFF, g_motor_id & 0xFF};
        Bsp_Can_Transmit_Classic_Standard(CAN1, 0x7FF, tx, 5);
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
            handle_zero_set(data, len);
            break;
        case 0x04: // ID设置
            handle_id_set(data, len);
            break;
        case 0x05: // ID重置
            handle_id_reset(data, len);
            break;
        default:
            break;
    }
}
/*===================================== 电机配置函数 End =====================================*/

/*===================================== 电机控制函数 Start =====================================*/
/**
 * @brief 获取电机错误，部分错误不确定，待完善！
 * 
 * @return uint8_t 
 */
static uint8_t motor_get_error(void)
{
    uint16_t error_code = 0;
    error_code = Motor_Read_Uint16(ERROR_CODE, 0);
    switch(error_code)
    {
        case 0x4210:    //电机过热，Excess temperature device
        {
            return 1;
            break;
        }
        // case 0x4210:    //电机过流，通过判断电机过流时间判断？
        // {
        //     return 2;
        //     break;
        // }
        case 0x3220:    //电机欠压，DC link under-voltage，这里先用母线电压占位，实际可能是其他！！！！
        {
            return 3;
            break;
        }
        // case 0x4210:    //编码器错误
        // {
        //     return 4;
        //     break;
        // }
        case 0x3210:    //刹车电压过高，DC link over-voltage，不确定！！！
        {
            return 6;
            break;
        }
        case 0x7300:    //手册中该代码值指传感器错误，是否对应DRV驱动，待确定！！！
        {
            return 7;
            break;
        }
    }

    //电机过流，通过判断电机过流时间判断？返回错误码
    //编码器错误：编码器1：0x3006，编码器2：0x358A，返回错误码4
}

static void send_feedback(uint8_t type)
{
    uint8_t data[8];
    // 读取电机错误代码，待接入！！！！！
    uint8_t error = motor_get_error(); // 从硬件获取
    
    data[0] = (type << 5) | (error & 0x1F);

    // 这里读取的参数单位不确定，需要确认！！！！
    int32_t pos_read = Motor_Read_Int32(POSITION_READ_CODE, 0);            //认为是cnt值
    int32_t speed_read = Motor_Read_Int32(SPEED_READ_CODE, 0);             //认为是cnt/s
    int16_t current_read= Motor_Read_Int32(CURRENT_CURRENT, 0);            //读取的电流是额定电流的千分之多少吗，待确定
    //温度需要处理！！！！
    int32_t motor_temp = Motor_Read_Int32(MOTOR_TEMPERATURE_CODE, 0);
    int32_t mos_temp = Motor_Read_Int32(DRIVE_TEMPERATURE_CODE, 0);

    switch (type) {
        case MSG_TYPE_CTRL1: {
            uint16_t pos_raw = Cnt_To_Deg(pos_read);
            uint16_t spd_raw = CntS_To_Rpm(speed_read);
            uint16_t cur_raw = current_read * (float)RATED_CURRENT;
            uint8_t motor_temp_raw = (uint8_t)(motor_temp * 2 + 50);
            uint8_t mos_temp_raw = (uint8_t)(mos_temp * 2 + 50);

            data[1] = pos_raw >> 8;
            data[2] = pos_raw & 0xFF;
            data[3] = spd_raw >> 4;
            data[4] = ((spd_raw & 0x0F) << 4) | ((cur_raw >> 8) & 0x0F);
            data[5] = cur_raw & 0xFF;
            data[6] = motor_temp_raw;
            data[7] = mos_temp_raw;
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            //测验样例返回值 0x25 8A 3C 8E 38 3C 3A 3C
            break;
        }
        case MSG_TYPE_CTRL2: {
            float pos_deg = Cnt_To_Deg(pos_read);
            pack_float(&data[1], pos_deg);
            float cur = (int16_t)(current_read / 100.0f);
            pack_float(&data[5], cur);
            data[7] = (uint8_t)(motor_temp * 2 + 50);
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            //测试样例返回值：0x45 0x42 0x65 0x2E 0xE1 0x01 0x2C 0x3A
            break;
        }
        case MSG_TYPE_CTRL3: {
            float speed_rpm = CntS_To_Rpm(speed_read);
            pack_float(&data[1], speed_rpm);
            float cur = (int16_t)(current_read / 100.0f);
            pack_float(&data[5], (uint16_t)cur);
            data[7] = (uint8_t)(motor_temp * 2 + 50);
            Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, data, 8);
            //测试样例返回值：0x65 0x41 0x98 0xC9 0xEF 0x01 0x2C 0x3A
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
    uint8_t status = 0;                     // 默认失败

    // 控制预留位必须为0
    if(!res_control_status)
    {
        if (config_code == 0x01 && len >= 4) { // 加速度配置
            uint16_t accel_raw = unpack_uint16(&data[2]);

            /*----------------- 加速度配置，待验证索引、单位！！！！ -----------------*/
            Motor_Write_Uint16(PROFILE_ACCELERATION, 0, accel_raw);
            /*---------------------------------------------------------------------*/

            status = 1; // 成功
        }
        // 我司暂不支持接口
        // else if (config_code == 0x02 && len >= 6) { // 补偿系数与阻尼
        //     uint16_t comp_raw = unpack_uint16(&data[2]);
        //     uint16_t damp_raw = unpack_uint16(&data[4]);

        //     //调用接口，写入电机反馈系数、阻尼系数
        //     // motor_set_param(CONFIG_COMP, comp_raw);
        //     // motor_set_param(CONFIG_DAMP, damp_raw);
        //     status = 1;
        // }
    }
    
    if (ret_type == 1) { // 只有返回类型1才发送配置反馈
        uint8_t tx[3];
        // 调用接口，需要获取电机的错误信息发送再发送
        // tx[0] = (MSG_TYPE_CONFIG << 5) | (motor_get_error() & 0x1F);
        tx[0] = MSG_TYPE_CONFIG << 5;
        tx[1] = config_code;
        tx[2] = status;
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 3);
        /*
            发送C1 00 00 00 返回：80 00 00
            发送C1 01 00 00 返回：80 01 01
            发送C1 02 00 00 返回：80 02 00
            发送C1 02 00 00 00 00 返回：80 02 01
        */ 

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
    uint8_t query_code = data[1];
    uint8_t tx[8];
    int32_t read_data = 0;       //存储接收的数据
    float send_data1;
    // 调用接口，需要获取电机的错误信息发送再发送!!!!，未知指令
    // tx[0] = (MSG_TYPE_QUERY << 5) | (motor_get_error() & 0x1F);
    tx[0] = MSG_TYPE_QUERY;
    tx[1] = query_code;

    if (query_code >= QUERY_POSITION && query_code <= QUERY_POWER) {
        /*----------------- 查询指令，待验证索引、单位！！！！ -----------------*/
        switch (query_code)
        {
        case 0x01:  //查询位置
        {
            read_data = Motor_Read_Int32(POSITION_READ_CODE, 0);
            //read_data是cnt值？如果是进行下述转换,转为rad
            send_data1 = Cnt_To_Deg(read_data);
            break;
        }
        case 0x02:  //查询速度
        {
            read_data = Motor_Read_Int32(SPEED_READ_CODE, 0);
            //如果速度是cnt/s，进行下列转换，转为rpm
            send_data1 = CntS_To_Rpm(read_data);
            break;
        }
        case 0x03:  //查询电流
        {
            read_data = Motor_Read_Int32(CURRENT_CURRENT, 0);
            //计算电流是否正确，单位A，待确认！！！！！！！！！！！！！！！
            send_data1 = read_data * (float)RATED_CURRENT;
            break;
        }
        case 0x04:  //查询功率，需要确认！！！！！！！！！！！！！！！！
        {
            int16_t i_A = 0;
            int16_t u_V = 0;
            i_A = Motor_Read_Int16(CURRENT_CURRENT, 0);
            u_V = Motor_Read_Int16(DC_LINK_CIRCUIT_VOLTAGE, 0);
            read_data = i_A * u_V;
            send_data1 = read_data * (float)RATED_CURRENT * 1000.0f;
            break;
        }
        default:
            break;
        }
        /*---------------------------------------------------------------------*/
        uint32_t tx_data;   //将float转为uint32，返回给上位机
        memcpy(&tx_data, &send_data1, sizeof(float));

        tx[2] = ((uint8_t)(tx_data >> 24) & 0xFF);
        tx[3] = (uint8_t)((tx_data >> 16) & 0xFF);
        tx[4] = (uint8_t)((tx_data >> 8) & 0xFF);
        tx[5] = (uint8_t)(tx_data & 0xFF);
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 6);
    }
    // 阻尼系数、反馈补偿系数我司暂不支持
    else if (query_code >= QUERY_ACCEL && query_code <= QUERY_DAMPING) {
        // 调用底层接口，获取参数，待实现！！！
        // uint16_t val = motor_get_param(query_code);
        // pack_uint16(&tx[2], val);
        tx[2] = 0x89;
        tx[3] = 0x10;
        Bsp_Can_Transmit_Classic_Standard(CAN1, g_motor_id, tx, 4);
    }
}

static void handle_unicast_cmd(uint8_t *data, uint8_t len)
{
    uint8_t mode = data[0] >> 5;
    uint8_t ret_type = data[0] & 0x03; // 返回类型，速度控制、电流力矩控制、加速度配置、补偿系数的返回类型未bite[0]的低两位决定，其他需要重新获取
    // uint8_t ctrl_state = (data[0] >> 2) & 0x07; // 预留控制位

    switch (mode) {
        case MOTOR_MODE_MIXED: // 0x00：力位混控模式
            if (len >= 8) {
                // 解析参数
                uint16_t kp_raw = ((data[0] & 0x1F) << 7) | (data[1] >> 1);
                uint16_t kd_raw = ((data[1] & 0x01) << 8) | data[2];
                uint16_t pos_raw = (data[3] << 8) | data[4];
                uint16_t spd_raw = (data[5] << 4) | (data[6] >> 4);
                uint16_t tor_raw = ((data[6] & 0x0F) << 8) | data[7];

                // 转换为物理量
                float kp = kp_raw * 500.0f / 4095.0f;
                float kd = kd_raw * 5.0f / 511.0f;

                //待转换，本司电机使用的编码器计数值
                float pos_desire = pos_raw * 25.0f / 65535.0f - 12.5f;
                float spd_desire = spd_raw * 36.0f / 4095.0f - 18.0f;
                float tor = tor_raw * 360.0f / 4095.0f - 180.0f;

                /*----------------- 力位混控指令，待验证索引、单位和计算的正确性！！！！ -----------------*/
                int32_t motor_pos_raw = 0;
                int32_t motor_spd_raw = 0;
                motor_pos_raw = Motor_Read_Int32(POSITION_READ_CODE, 0);
                motor_spd_raw = Motor_Read_Int32(SPEED_READ_CODE, 0);
                float pos_current = Cnt_To_Deg(motor_pos_raw);
                float spd_current = Cnt_To_Deg(motor_spd_raw);
                // CIA402中没有写入电流的索引值，只有扭矩的索引值，这里使用扭矩的目标值，代替电流值，由于使用了扭矩，故不需要除以KT
                // uint32_t motor_current = (kp * (pos_desire - pos_current) + 
                //                             kd * (spd_desire - spd_current) + tor) / KT;
                int16_t motor_current = (kp * (pos_desire - pos_current) + 
                                            kd * (spd_desire - spd_current) + tor);
                //写入电流参数，待接入！！！！
                Motor_Write_Int16(TARGET_TORQUE_CODE, 0, motor_current);
                /*-----------------------------------------------------------------------*/

                // 返回类型1报文
                send_feedback(MSG_TYPE_CTRL1);
            }
            break;
        case MOTOR_MODE_POSITION: // 0x01：位置控制模式
            if (len >= 8) {
                //数据拼接
                uint32_t pos_bits = ((uint32_t)(data[0] & 0x1F) << 27)    | 
                                    ((uint32_t)data[1] << 19)             |
                                    ((uint32_t)data[2] << 11)             |
                                    ((uint32_t)data[3] << 3)              |
                                    ((uint32_t)(data[4] >> 5) & 0x07);
                uint16_t speed_raw = ((uint16_t)(data[4] & 0x1F) << 10)               |
                                     ((uint16_t)data[5] << 2)                         | 
                                     ((uint16_t)(data[6] >> 6) & 0x03);
                uint16_t current_raw = ((uint16_t)(data[6] & 0x3F) << 6)              | 
                                       ((uint16_t)data[7] >> 2);
                uint8_t ret_type_pos = data[7] & 0x03; // 返回类型在Byte7低2位

                /*----------------- 位置控制模式，待验证索引、单位和转换的正确性！！！！ -----------------*/
                //进行转换
                float pos_deg;
                memcpy(&pos_deg, &pos_bits, sizeof(float));     //期望位置，单位°
                float speed_rpm = (float)speed_raw / 10.0f;            //期望速度，单位rpm
                float current_A = (float)current_raw / 10.0f;          //电流阈值，单位A
                int32_t pos_cnt = Deg_To_Cnt(pos_deg / 360.0f);        //单位cnt
                int32_t speed_cnt_s = Rpm_To_CntS(speed_rpm);          //单位cnt/s
                //这里一定需要确认再确认！！！！！！！！！！
                Motor_Write_Data(TARGET_POSITION, 0, pos_cnt);
                Motor_Write_Data(TARGET_VELOCITY, 0, speed_cnt_s);
                Motor_Write_Float(MAX_CURRENT, 0, current_A * 1000);  //写入电流阈值（额定电流），单位mA
                /*----------------------------------------------------------------------------------*/

                if (ret_type_pos != 0) {
                    if (ret_type_pos == 1) 
                        send_feedback(MSG_TYPE_CTRL1);
                    else if (ret_type_pos == 2) 
                        send_feedback(MSG_TYPE_CTRL2);
                    else if (ret_type_pos == 3) 
                        send_feedback(MSG_TYPE_CTRL3);
                }
            }
            break;

        case MOTOR_MODE_SPEED: // 0x02：速度控制模式
            if (len >= 7) {
                //获取返回类型，开始已获取
                // ret_type = data[0] & 0x03;
                float speed = unpack_float(&data[1]); // rpm
                uint16_t current_raw = unpack_uint16(&data[5]); // 16位
                float current_A = current_raw * 6553.5f / 65535.0f;

                /*----------------- 位置控制模式，待验证索引、单位和转换的正确性！！！！ -----------------*/
                int32_t speed_cnt_s = Rpm_To_CntS(speed);       //单位cnt/s

                //这里一定需要确认再确认！！！！！！！！！！
                Motor_Write_Data(TARGET_VELOCITY, 0, speed_cnt_s);
                Motor_Write_Float(MAX_CURRENT, 0, current_A * 1000);  //写入电流阈值（额定电流），单位mA
                /*----------------------------------------------------------------------------------*/

                // 发送返回消息
                if (ret_type != 0) {
                    if (ret_type == 1) send_feedback(MSG_TYPE_CTRL1);
                    else if (ret_type == 2) send_feedback(MSG_TYPE_CTRL2);
                    else if (ret_type == 3) send_feedback(MSG_TYPE_CTRL3);
                }
            }
            break;

        case MOTOR_MODE_CURRENT: // 0x03：电流、力矩控制模式和刹车指令
            if (len >= 3) {
                // 开始已获取
                // ret_type = data[0] & 0x03;
                uint8_t control_mode = (data[0] >> 5) & 0x07;
                // 预留控制位
                // uint8_t ctrl_state = (data[0] >> 2) & 0x07;
                // 期望电流、力矩
                int16_t current_raw = (data[1] << 8) | data[2];
                
                // 写入值
                float current_A = current_raw / 100.0f;

                //根据控制模式，控制电机，写入控制参数
                switch(control_mode)
                {
                    case CTRL_STATE_CURRENT:// 电流控制模式
                    {
                        /* ==========================================需要检查！！！！！！！！！！！！！！========================================== */
                        /*----------------- 电流控制模式，待验证索引、手册中暂未明确单位，需核实！！！！ -----------------*/
                        Motor_Write_Float(MAX_CURRENT, 0, current_A * 1000);  //写入电流阈值（额定电流），单位mA
                        /*------------------------------------------------------------------------------------------*/
                        break;
                    }
                    case CTRL_STATE_TORQUE://力矩控制模式
                    {
                        /*----------------- 力矩控制模式，待验证索引、已经传入参数单位！！！！ -----------------*/
                        Motor_Write_Float(TARGET_TORQUE_CODE, 0, current_A * KT);  //写入电流阈值（额定电流），单位mA
                        /*------------------------------------------------------------------------------------------*/
                        /* =================================================================================================================== */
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
            break;

        case MOTOR_MODE_CONFIG: // 0x06：电机控制参数配置
            // 配置指令处理
            handle_config_cmd(data, len);
            break;

        case MOTOR_MODE_QUERY: // 0x07：电机控制参数查询
            // 查询指令处理
            handle_query_cmd(data, len);
            break;

        default:
            break;
    }
}
/*===================================== 电机控制函数 End =====================================*/

//void CAN1_RX_IRQHandler(void)
//{
//  can_rxbuf_type can_rxbuf_struct;

//  /* rx_buffer had data be received */
//  if(can_interrupt_flag_get(CAN1, CAN_RIF_FLAG) != RESET)
//  {
//    can_flag_clear(CAN1, CAN_RIF_FLAG);
//    while(ERROR != can_rxbuf_read(CAN1, &can_rxbuf_struct))
//    {
//        //执行相应操作
//        if(can_rxbuf_struct.id == 0x7FF)
//        {
//            handle_broadcast_cmd(can_rxbuf_struct.data, can_rxbuf_struct.data_length);
//        }
//        else if(can_rxbuf_struct.id == g_motor_id)
//        {
//            handle_unicast_cmd(can_rxbuf_struct.data,can_rxbuf_struct.data_length);
//        }
//        //其他ID忽略

//        // Bsp_Can_Transmit_Classic_Standard(CAN1, can_rxbuf_struct.id, can_rxbuf_struct.data,can_rxbuf_struct.data_length);       //测试样例
//    }
//  }

//  /* rx_buffer almost full */
//  if(can_interrupt_flag_get(CAN1, CAN_RAFIF_FLAG) != RESET)
//  {
//    can_flag_clear(CAN1, CAN_RAFIF_FLAG);
//  }

//  /* rx_buffer full */
//  if(can_interrupt_flag_get(CAN1, CAN_RFIF_FLAG) != RESET)
//  {
//    can_flag_clear(CAN1, CAN_RFIF_FLAG);
//  }

//  /* rx_buffer overflow */
//  if(can_interrupt_flag_get(CAN1, CAN_ROIF_FLAG) != RESET)
//  {
//    can_flag_clear(CAN1, CAN_ROIF_FLAG);
//  }
//}
