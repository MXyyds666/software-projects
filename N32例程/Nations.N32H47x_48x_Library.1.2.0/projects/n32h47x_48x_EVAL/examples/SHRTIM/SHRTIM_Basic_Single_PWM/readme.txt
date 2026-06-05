1、功能说明
    此示例描述了通过SHRTIM输出PWM波形

2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
                 IAR EWARM 8.50.1
    硬件环境：N32H474VEL7-STB V1.0    

3、使用说明
    系统配置：
        1、 时钟源：
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
        2、 端口配置：
            PB14选择为SHRTIM的TD1的输出
        3、SHRTIM：
            TD1输出，连续计数模式
    使用方法： 
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察SHRTIM的TD1的输出波形，频率200KHz，占空比50%

4、注意事项
    无
    
    
1. Function description
    This example describes outputting a PWM waveform via SHRTIM

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware environment: N32H474VEL7-STB V1.0  

3. Instructions for use
    System Configuration:
        1. clock source
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
        2. Port configuration:
            PB14 is selected as SHRTIM1 TD1 output 
        3. SHRTIM： 
            TD1 output, continuous mode.
    Instructions:
        1. Open the debug mode after compiling, and observe the output waveform of SHRTIM's TD1 with an oscilloscope or logic analyzer. Frequency is 200KHz, duty cycle is 50%

4. Attention
     None