1、功能说明
    1、ATIM1输出4对互补波形
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
        3、端口配置：
            PC0选择为ATIM1 CH1输出
            PA9选择为ATIM1 CH2输出
            PC2选择为ATIM1 CH3输出
            PC3选择为ATIM1 CH4输出
            PA7选择为ATIM1 CH1N输出
            PB14选择为ATIM1 CH2N输出
            PD9选择为ATIM1 CH3N输出
            PD5选择为ATIM1 CH4N输出    
            PA6选择为刹车输入
        4、TIM：
            ATIM1 4对互补输出带死区，PA6刹车
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察ATIM1的波形
        2、PA6拉低可观察到4对互补PWM，PA6拉高PWM消失
4、注意事项
    无
    

1. Function description
     1. ATIM1 outputs 4 complementary waveforms
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
        3. Port configuration:
                PC0 is selected as ATIM1 CH1 output
                PA9 is selected as ATIM1 CH2 output
                PC2 is selected as ATIM1 CH3 output
                PC3 is selected as ATIM1 CH4 output
                PA7 is selected as ATIM1 CH1N output
                PB14 is selected as ATIM1 CH2N output
                PD9 is selected as ATIM1 CH3N output
                PD5 is selected as ATIM1 CH4N output
                PA6 is selected as break input
        4. TIM:
                ATIM1 4-pair complementary with dead zone, PA6 is brake input
    Instructions:
         1. After compiling, turn on the debug mode, and use an oscilloscope or logic analyzer to observe the waveform of ATIM1
         2. When PA6 is low, 4 complementary PWM can be observed, and when PA6 is high, PWM disappears
4. Attention
    without
