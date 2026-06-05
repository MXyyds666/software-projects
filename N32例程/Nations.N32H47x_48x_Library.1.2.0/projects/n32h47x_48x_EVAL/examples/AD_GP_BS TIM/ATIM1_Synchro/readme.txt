1、功能说明
     1、GTIM3 GTIM4在ATIM1周期下计数
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM3_CLK=120M,GTIM4_CLK=120M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM3_CLK=120M,GTIM4_CLK=120M
        3、端口配置：
            PA11选择为GTIM3的CH1输出
            PA0选择为GTIM4的CH1输出
            PA8选择为ATIM1的CH1输出
        4、TIM：
            ATIM1 CH1 周期触发GTIM3 GTIM4的门控
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察ATIM1 CH1、GTIM3 CH1、GTIM4 CH1的波形
        2、程序运行后，GTIM3 15倍周期ATIM1，GTIM4 10倍周期ATIM1
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. GTIM3 GTIM4 counts under the ATIM1 cycle
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM3_CLK=120M,GTIM4_CLK=120M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM3_CLK=120M,GTIM4_CLK=120M
        3. Port configuration:
                PA11 is selected as the CH1 output of GTIM3
                PA0 is selected as the CH1 output of GTIM4
                PA8 is selected as the CH1 output of ATIM1
        4. TIM:
                ATIM1 CH1 period triggers the gating of GTIM3 GTIM4
    Instructions:
         1. After compiling, turn on the debug mode, use an oscilloscope or logic analyzer to observe the waveforms of ATIM1 CH1, GTIM3 CH1, and GTIM4 CH1
         2. After the program runs, GTIM3 15 times cycle ATIM1, GTIM4 10 times cycle ATIM1
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

