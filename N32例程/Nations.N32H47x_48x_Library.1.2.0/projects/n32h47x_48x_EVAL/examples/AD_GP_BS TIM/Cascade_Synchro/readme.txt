1、功能说明
    1、GTIM2周期门控GTIM5，GTIM5周期门控GTIM6      
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,GTIM2_CLK=120M,GTIM5_CLK=120M,GTIM6_CLK=120M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,GTIM2_CLK=120M,GTIM5_CLK=120M,GTIM6_CLK=120M
        2、端口配置：
                    PA6选择为GTIM2的CH1输出
                    PA2选择为GTIM5的CH1输出
                    PD5选择为GTIM6的CH1输出
        3、TIM：
                    GTIM2 的周期门控GTIM5，GTIM5的周期门控GTIM6
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察GTIM5 CH1、GTIM5 CH1、GTIM6 CH1的波形
        2、GTIM5 4倍周期GTIM2，GTIM6 4倍周期GTIM5
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. GTIM2 cycle gate GTIM5, GTIM5 cycle gate GTIM6
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,GTIM2_CLK=120M,GTIM5_CLK=120M,GTIM6_CLK=120M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,GTIM2_CLK=120M,GTIM5_CLK=120M,GTIM6_CLK=120M
        3. Port configuration:
                     PA6 is selected as GTIM2 CH1 output
                     PA2 is selected as GTIM5 CH1 output
                     PD5 is selected as GTIM6 CH1 output
        4. TIM:
                     GTIM2 cycle gating GTIM5, GTIM5 cycle gating GTIM6
    Instructions:
         1. After compiling, turn on the debug mode, and use an oscilloscope or logic analyzer to observe the waveforms of GTIM2 CH1, GTIM5 CH1, and GTIM6 CH1
         2. GTIM5 4 times cycle GTIM2, GTIM6 4 times cycle GTIM5
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

