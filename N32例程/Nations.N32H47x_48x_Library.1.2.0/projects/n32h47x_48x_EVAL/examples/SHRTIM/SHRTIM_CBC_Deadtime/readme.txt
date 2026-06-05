1、功能说明
    此示例展示了如何实现带死区时间互补的Cycle by cycle(CBC)电流控制。

2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
                 IAR EWARM 8.50.1
    硬件环境：N32H474VEL7-STB V1.0    

3、使用说明
    系统配置：
        1、 时钟源：
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。

    Demo说明： 
        在TD1和TD2输出上（PB14和PB15）生成两个互补的PWM波形。
        过流条件将关闭两个互补输入，过流条件不满足时则自动恢复两个互补输出。
        过电流数字信号必须连接到PB8和PB9输入的EXEV5和EXEV8外部事件线。
        PWM频率和死区时间持续时间可以分别以kHz和ns为单位输入，使用以下用户常量：
            - TIMER_PWM_FREQ
            - DEADTIME_IN_NS
        更详细的说明，请查阅《CN_UG_N32H47x_48x_SHRTIM_User_Guide》中的"无死区时间插入的逐周期保护"章节
        为方便使用和测试本demo，可使用本Demo专用的测试程序，执行测试前需要开启ENTRY_TEST宏。
    使用方法：
        1. 开启ENTRY_TEST宏，编译后打开调试模式，示波器量测TD1和TD2的波形，可观察到互补，且占空比不断变化的波形
        2. TA1（PA8）产生周期性的过流信号，PA8同时连接到EXEV5和EXEV8（PB9和PB8）。
            2.1 测试电平CBC: PA8高电平时关闭两个互补输出，PA8低电平时恢复两个互补输出。
            2.2 测试边沿CBC：断开PA8与PB9的连接，保留PA8与PB8的连接。PA8边沿处关闭两个互补输出，PA8边沿过后恢复两个互补输出
4、注意事项
    无
    
    
1. Function description
    This example demonstrates how to implement cycle-by-cycle (CBC) current control with complementary dead-time.

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware environment: N32H474VEL7-STB V1.0  

3. Instructions for use
    System Configuration:
        1. clock source
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
    Demo Instructions:
        An over-current condition will turn-off the two complementary outputs, without re-arming the complementary output.
        Two complementary PWM waveforms are generated on the TD1 and TD2 outputs (PB14 and PB15).
        The over-current digital signal must be connected to both EEV5 and EEV8 external event lines on PB9 and PB8 inputs.
        For more detailed instructions, please refer to the "Cycle-by-Cycle Protection without Dead-Time Insertion" section in the "CN_UG_N32H47x_48x_SHRTIM_User_Guide".
        For ease of use and testing of this demo, a dedicated test program is provided. Enable the ENTRY_TEST before test.
    Use method:
        1. Enable the ENTRY_TEST macro and compile to enter debug mode. Use an oscilloscope to measure the waveforms of TD1 and TD2; you will observe complementary waveforms with continuously changing duty cycles.
        2. TA1 (PA8) generates a periodic overcurrent signal, and PA8 is simultaneously connected to EXEV5 and EXEV8 (PB9 and PB8).
            2.1 Level-based CBC test: When PA8 is high, both complementary outputs are turned off; when PA8 is low, both complementary outputs are restored.
            2.2 Edge-based CBC test: Disconnect the connection between PA8 and PB9, keeping the connection between PA8 and PB8. At the edge of PA8, both complementary outputs are turned off; after the edge, both complementary outputs are restored.
4. Attention
     None