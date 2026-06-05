1、功能说明
    1、systick 100ms触发ATIM1输出6步换相波形
2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
                  IAR EWARM 8.50.1
    硬件开发环境：
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：
        基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_EVB V1.1开发
3、使用说明
    系统配置:
        1、时钟源：HSI+PLL
        2、时钟频率：
            N32H473/474系列：
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
        3、中断：
                    ATIM1 COM事件中断打开，抢断优先级0，子优先级1
                    Systick 10ms中断，优先级0
        4、端口配置：
                    PC0选择为ATIM1 CH1输出
                    PA9选择为ATIM1 CH2输出
                    PC2选择为ATIM1 CH3输出
                    PB13选择为ATIM1 CH1N输出
                    PB14选择为ATIM1 CH2N输出
                    PB1选择为ATIM1 CH3N输出
        5、TIM：
                    ATIM1 6路互补冻结输出模式，无刹车，开COM中断
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察ATIM1的输出波形
        2、每隔100ms systick触发COM中断，在ATIM的COM中断里面输出AB AC BC BA CA CB的6步换相波形
4、注意事项
     默认情况下，开发板的PA9和PA10跳线帽连接到NSLINK的虚拟串行端口。如果PA9和PA10未用作项目中的串行端口，而是用于其他用途，则必须拔掉串行端口跳线帽。
    
    
    
1. Function description
    1. systick triggers ATIM1 for 100ms to output 6-step commutation waveform

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware development environment:
        N32H473 series:
        Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series:
        Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series:
        Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_EVB V1.1
3. Instructions for use
    System Configuration;
        1. Clock source: HSI+PLL
        2. Clock frequency: 
            N32H473/474 series:
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
        3. Interrupt:
                    ATIM1 COM event interrupt on, steal priority level 0, sub priority level 1
                    Systick 10ms interrupt, priority 0
        4. Port configuration:
                    PC0 is selected as ATIM1 CH1 output
                    PA9 is selected as ATIM1 CH2 output
                    PC2 is selected as ATIM1 CH3 output
                    PB13 is selected as ATIM1 CH1N output
                    PB14 is selected as ATIM1 CH2N output
                    PB1 is selected as ATIM1 CH3N output
        5. TIM:
                   ATIM1 6-channel complementary freeze output mode, no brake, open COM interrupt
    Instructions:
        1. Open the debug mode after compiling, and observe the output waveform of ATIM1 with an oscilloscope or logic analyzer
        2. The systick triggers the COM interrupt every 100ms, and outputs the 6-step commutation waveform of AB AC BC BA CA CB in the COM interrupt of the ATIM
4. Attention
       By default, the PA9 and PA10 jumper caps of the development board are connected to the virtual serial port of NSLINK. If PA9 and PA10 are not used as serial ports in the project, and are used for other purposes, the serial port jumper caps must be unplugged.
    