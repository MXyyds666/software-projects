1、功能说明
     1、GTIM7的CH1、CH2、CH3、CH4分别触发ATIM1_CH1、ATIM2_CH1、GTIM8_CH1、GTIM9_CH1输出，
          ATIM1_CH1与ATIM2_CH1、GTIM8_CH1、GTIM9_CH1相位差分别为90，180，270
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,ATIM2_CLK=240M,GTIM7_CLK=120M,GTIM8_CLK=240M,GTIM9_CLK=240M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,ATIM2_CLK=240M,GTIM7_CLK=120M,GTIM8_CLK=240M,GTIM9_CLK=240M
        3、端口配置：
                            PC0选择为ATIM1的CH1输出
                            PD14选择为ATIM2的CH1输出
                            PB14选择为GTIM8的CH1输出
                            PA6选择为GTIM9的CH1输出
        4、TIM：
                            GTIM7的CH1、CH2、CH3、CH4分别触发ATIM1_CH1、ATIM2_CH1、GTIM8_CH1、GTIM9_CH1输出
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察ATIM1_CH1、ATIM2_CH1、GTIM8_CH1、GTIM9_CH1的波形
        2、程序运行后，ATIM1_CH1与ATIM2_CH1相位差为90，ATIM1_CH1与GTIM8_CH1相位差为180，ATIM1_CH1与GTIM9_CH1相位差为270。
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. CH1, CH2, CH3 and CH4 of GTIM7 trigger the output of ATIM1_CH1, ATIM2_CH1, GTIM8_CH1 and GTIM9_CH1, respectively.
          The phase difference between ATIM1_CH1 and ATIM2_CH1, GTIM8_CH1, GTIM9_CH1 is 90, 180, 270 respectively
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,ATIM2_CLK=240M,GTIM7_CLK=120M,GTIM8_CLK=240M,GTIM9_CLK=240M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,ATIM2_CLK=240M,GTIM7_CLK=120M,GTIM8_CLK=240M,GTIM9_CLK=240M
        3. Port configuration:
                            PC0 is selected as the CH1 output of ATIM1
                            PD14 is selected as the CH1 output of ATIM2
                            PB14 is selected as the CH1 output of GTIM8
                            PA6 is selected as the CH1 output of GTIM9
        4. TIM:
                            CH1, CH2, CH3 and CH4 of GTIM7 trigger the output of ATIM1_CH1, ATIM2_CH1, GTIM8_CH1 and GTIM9_CH1, respectively
    Instructions:
        1. After compiling, open the debug mode and observe the waveforms of ATIM1_CH1, ATIM2_CH1, GTIM8_CH1 and GTIM9_CH1 with an oscilloscope or logic analyzer.
        2. After the program is run, the phase difference between ATIM1_CH1 and ATIM2_CH1 is 90, the phase difference between ATIM1_CH1 and GTIM8_CH1 is 180, 
            and the phase difference between ATIM1_CH1 and GTIM9_CH1 is 270.
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

