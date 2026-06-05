1、功能说明
    1、GTIM2 利用更新中断，产生定时翻转IO
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
        基于评估板N32H487ZEL7_STB V1.0开发
3、使用说明
    系统配置:
        1、时钟源：HSI+PLL
        2、时钟频率：
            N32H473/474系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM2_CLK=120M
                N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM2_CLK=120M
        3、中断：
                GTIM2 更新中断打开
        4、端口配置：
                PC6选择为IO输出
        5、TIM：
                GTIM2 使能周期中断
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察PC6的波形
        2、程序运行后，GTIM2 的周期中断来临翻转PC6电平
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. GTIM2 uses the update interrupt to generate timing rollover IO
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
        Developed based on the evaluation board N32H487ZEL7_STB V1.0
3. Instructions for use
    System Configuration;
        1. Clock source: HSI+PLL
        2. Clock frequency: 
            N32H473/474 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM2_CLK=120M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM2_CLK=120M
        3. Interruption:
            GTIM2 update interrupt is turned on
        4. Port configuration:
            PC6 is selected as IO output
        5. TIM:
            GTIM2 enables periodic interrupts
     Instructions:
         1. After compiling, turn on the debug mode and observe the waveform of PC6 with an oscilloscope or logic analyzer
         2. After the program runs, the periodic interrupt of GTIM2 comes to flip the PC6 level
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

