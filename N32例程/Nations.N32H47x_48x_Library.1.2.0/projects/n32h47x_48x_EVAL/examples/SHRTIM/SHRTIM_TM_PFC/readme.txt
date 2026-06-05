1、功能说明
    此示例展示了如何配置SHRTIM来控制 transition mode PFC。

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
        更详细的说明，请查阅《CN_UG_N32H47x_48x_SHRTIM_User_Guide》中的"过渡模式功率因数控制器"章节
    使用方法： 
        完备的测试:
            请查阅《CN_UG_N32H47x_48x_SHRTIM_User_Guide》中的"过渡模式功率因数控制器"章节。
        简要测试：
            1、编译后打开调试模式，示波器量测TD1的波形，可观察到占空比固定的波形
            2、给PA12输入一个高电平，TD1波形关闭
            3. 撤掉PA12上的高电平，按下KEY1(PA4),TD1波形恢复

4、注意事项
    无
    
    
1. Function description
     This example shows how to configure the SHRTIM to control a transition mode PFC.

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware environment: N32H474VEL7-STB V1.0  

3. Instructions for use
    System Configuration:
        1. clock source
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
    Demo instructions:
        For more detailed information, please refer to the "Transition Mode Power Factor Controller" section in the "CN_UG_N32H47x_48x_SHRTIM_User_Guide".
    Usage:
        Comprehensive testing:
            Please refer to the "Transition Mode Power Factor Controller" section in the "CN_UG_N32H47x_48x_SHRTIM_User_Guide".
        Brief Test:
            1. After compiling, open the debug mode. Use an oscilloscope to measure the waveform of TD1; you should observe a waveform with a fixed duty cycle.
            2. Input a high level to PA12, and the TD1 waveform will turn off.
            3. Remove the high level from PA12 and press KEY1 (PA4); the TD1 waveform will be restored.
4. Attention
     None