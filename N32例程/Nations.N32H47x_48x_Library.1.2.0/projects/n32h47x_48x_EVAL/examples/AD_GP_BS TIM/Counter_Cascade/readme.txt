1、功能说明
     1、将ATIM1与GTIM5的16位计数器进行级联，实现一个32位计数器功能。
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM5_CLK=120M,GTIM6_CLK=120M
                N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM5_CLK=120M,GTIM6_CLK=120M
        3、中断：
            ATIM1的更新事件中断打开
            GTIM5的更新事件中断打开
        4、端口配置：
            PA5选择为ATIM1的更新事件中断的IO翻转输出
            PA6选择为GTIM5的更新事件中断的IO翻转输出
        5、TIM：
            1、设置ATIM1计数器的更新事件作为GTIM5计数器的输入触发源，在触发源输入给GTIM5时，GTIM5在外部时钟源模式1下仅触发计数1次。
            2、ATIM1的一个周期计数使GTIM5的计数加1，进而实现ATIM1与GTIM5的计数器级联。
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察PA5、PA6的波形
        2、程序运行后，GTIM5 1000倍周期ATIM1
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. Cascade the 16-bit counters of ATIM1 and GTIM5 to achieve a 32-bit counter function.
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM5_CLK=120M,GTIM6_CLK=120M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,PCLK2=120M,ATIM1_CLK=240M,GTIM5_CLK=120M,GTIM6_CLK=120M
        3.interrupt：
            Update event interrupt enabled for ATIM1
            Update event interrupt enabled for GTIM5
        4. Port configuration:
            PA5 selected as ATIM1 update event interrupt IO toggle output
            PA6 selected as GTIM5 update event interrupt IO toggle output
        5. TIM:
            1. Configure the ATIM1 counter's update event as the input trigger source for the GTIM5 counter. When the trigger source inputs to GTIM5, GTIM5 triggers only once in external clock source mode 1.
            2. Each cycle count of ATIM1 increments GTIM5's count by one, thereby achieving counter cascading between ATIM1 and GTIM5.
    Instructions:
        1. After compilation, enable debug mode and observe the waveforms at PA5 and PA6 using an oscilloscope or logic analyzer.
        2. During program execution, GTIM5 operates at 1000 times the period of ATIM1.
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

