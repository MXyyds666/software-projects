1、功能说明
    1、GTIM1 CH2上升沿触发CH1输出一个单脉冲
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
        3、端口配置：
                            PA5选择为GTIM1的CH1输出
                            PA1选择为GTIM1的CH2输入
                            PA3选择为IO输出
        4、TIM：
                            GTIM1 配置CH2上升沿触发CH1输出一个单脉冲
    使用方法：
        1、编译后打开调试模式，PA3连接PA1，用示波器或者逻辑分析仪观察GTIM1 的CH1 的波形
        2、通过调试窗口配置gSendTrigEn=1，程序发送PA3的上升沿，GTIM1 CH1输出一个单脉冲
4、注意事项
    GTIM1-7/BTIM1-2最大工作时钟为180M。HCLK大于180M时，如果要使用GTIM1-7/BTIM1-2，PCLK1的分频不能是1或者2分频。
    

1. Function description
     1. The rising edge of GTIM1 CH2 triggers CH1 to output a single pulse
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
        3. Port configuration:
                             PA0 is selected as the CH1 output of GTIM1
                             PA1 is selected as the CH2 input of GTIM1
                             PA3 is selected as IO output
        4. TIM:
                             GTIM1 configures the rising edge of CH2 to trigger CH1 to output a single pulse
    Instructions:
         1. After compiling, turn on the debug mode, connect PA3 to PA1, and use an oscilloscope or logic analyzer to observe the waveform of CH1 of GTIM1
         2. Configure gSendTrigEn=1 in the debug window,the program sends the rising edge of PA3, and GTIM1 CH1 outputs a single pulse
4. Attention
    The maximum operating clock of GTIM1-7/BTIM1-2 is 180 M. When HCLK is greater than 180 M, if GTIM1-7/BTIM1-2 is to be used, the division frequency of PCLK1 cannot be 1 or 2 divisions.

