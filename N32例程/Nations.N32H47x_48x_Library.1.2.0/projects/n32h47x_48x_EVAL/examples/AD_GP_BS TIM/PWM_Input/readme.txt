1、功能说明
    1、GTIM1 CH2捕获引脚通过CH1下降沿和CH2上升沿计算占空比和频率
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM1_CLK=120M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM1_CLK=120M
        3、中断：
                            GTIM1 CC2比较中断打开
        4、端口配置：
                            PA1选择为GTIM1 CH2输入
                            PA3选择为IO 输出
        5、TIM：
                            GTIM1 CH1下降沿捕获CH2信号，CH2上升沿捕获CH2信号
    使用方法：
        1、编译后打开调试模式，连接PA3与PA1，将Frequency、DutyCycle添加到watch窗口
        2、程序运行后，PA3发送的脉冲数据可以被捕获到占空比和频率到变量
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    本例程捕获的最小频率为GTIM1_CLK/65535.

1. Function description
     1. GTIM1 CH2 capture pin calculates the duty cycle and frequency through the falling edge of CH1 and the rising edge of CH2
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
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM1_CLK=120M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK1=60M,GTIM1_CLK=120M
         3. Interruption:
                            GTIM1 CC2 compare interrupt is turned on
         4. Port configuration:
                            PA1 is selected as GTIM1 CH2 input
                            PA3 is selected as IO output
         5. TIM:
                            GTIM1 CH1 falling edge captures CH2 signal, CH2 rising edge captures CH2 signal
     Instructions:
         1. After compiling, open the debug mode, connect PA3 and PA1, and add Frequency and DutyCycle to the watch window
         2. After the program runs, the pulse data sent by PA3 can be captured to the duty cycle and frequency to the variable
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.
    The minimum frequency captured by this example is GTIM1_CLK/65535.
