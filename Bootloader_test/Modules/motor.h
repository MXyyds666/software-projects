/**
 * @file motor.h
 * @author baishuaijie (baishuaijie17@qq.com)
 * @brief 四足关节电机从机控制头文件
 * @version 0.1
 * @date 2026-03-18
 * 
 * 
 * 
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "at32f45x_board.h"
#include "bsp_can.h"

#define USE_TEST_API            1

#define PI                      3.14159265358f
#define TWO_PI                  6.28318530718f
#define KT                      1000                //力位混控参数
#define ENCODER_ONE_CICLE       4096                //电机转一圈的编码器值，电机端的编码器！！！待指定！！！
#define RATED_CURRENT           3000                //额定电流，用于计算读取的电流大小，待确认是否用该参数读取，单位mA待确定
#define GEAR_RATIO              1                   //减速比，默认为1
// 电机模式
#define MOTOR_MODE_MIXED      0x00   // 方位混控
#define MOTOR_MODE_POSITION   0x01   // 伺服位置
#define MOTOR_MODE_SPEED      0x02   // 伺服速度
#define MOTOR_MODE_CURRENT    0x03   // 电流/力矩/刹车
#define MOTOR_MODE_CONFIG     0x06   // 参数配置
#define MOTOR_MODE_QUERY      0x07   // 参数查询

// 电流/力矩模式中的控制状态
#define CTRL_STATE_CURRENT    0x00   // 电流控制
#define CTRL_STATE_TORQUE     0x01   // 力矩控制
#define CTRL_STATE_DAMPING    0x02   // 变阻尼制动
#define CTRL_STATE_ENERGY     0x03   // 能耗制动
#define CTRL_STATE_REGEN      0x04   // 再生制动

// 配置代码
#define CONFIG_ACCEL          0x01   // 加速度配置
#define CONFIG_COMP_DAMP      0x02   // 补偿系数与阻尼系数配置

// 查询代码
#define QUERY_POSITION        0x01
#define QUERY_SPEED           0x02
#define QUERY_CURRENT         0x03
#define QUERY_POWER           0x04
#define QUERY_ACCEL           0x05
#define QUERY_COMPENSATION    0x06
#define QUERY_DAMPING         0x07

// 错误码
#define ERR_NONE              0x00
#define ERR_OVER_TEMP         0x01
#define ERR_OVER_CURRENT      0x02
#define ERR_UNDER_VOLT        0x03
#define ERR_ENCODER           0x04
#define ERR_BRAKE_OVERVOLT    0x06
#define ERR_DRV               0x07

// 反馈报文类型
#define MSG_TYPE_CTRL1        0x01            // 控制返回报文1
#define MSG_TYPE_CTRL2        0x02            // 控制返回报文2
#define MSG_TYPE_CTRL3        0x03            // 控制返回报文3
#define MSG_TYPE_CONFIG       0x04            // 配置返回报文
#define MSG_TYPE_QUERY        0x05            // 查询返回报文

// 使用到的CIA402索引码
#define REDA_ID_CODE                0x351A          //读取电机ID索引码
#define POSITION_READ_CODE          0X3572          //读取电机端位置码
#define SPEED_READ_CODE             0X3574          //读取电机端速度,这个假设单位是cnt/s
#define MOTOR_TEMPERATURE_CODE      0x3529          //读取电机温度
#define DRIVE_TEMPERATURE_CODE      0x3521          //驱动器温度
#define ERROR_CODE                  0x603F          //错误码
#define CONTROLWORD_CODE            0x6040          //控制码
#define STATUSWORD_CODE             0x6041          //状态码
#define MODE_OPERATION_CODE         0x6060          //电机运行模式码
#define MODE_OPERATION_DISPLAY      0X6061          //电机运行模式显示码
#define TARGET_TORQUE_CODE          0x6071          //扭矩目标值码
#define MAX_CURRENT                 0x6073          //应用电流限制相对值
#define CURRENT_CURRENT             0x6078          //电流当前值码
#define DC_LINK_CIRCUIT_VOLTAGE     0x6079          //直流母线电压，用于计算功率，单位mV，未确定计算功率使用哪个电压和电流！！！
#define TARGET_POSITION             0X607A          //目标位置码
#define PROFILE_ACCELERATION        0x6083          //轮廓加速度码
#define HOMING_METHOD_CODE          0x6098          //回零模式设置码
#define TARGET_VELOCITY             0x60FF          //目标速度码

/* --- 类型转换联合体 --- */
typedef union {
    float f;
    int32_t i32;
    uint32_t u32;
} MotorData_t;

#define Motor_Write_Data(idx, sub, data)             Write_Data(idx, sub, data)
#define Motor_Read_Data(idx, sub, data)              Read_Data(idx, sub, data)

#endif
