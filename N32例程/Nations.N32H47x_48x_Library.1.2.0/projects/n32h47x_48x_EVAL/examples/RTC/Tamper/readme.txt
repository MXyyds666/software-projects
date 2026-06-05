1、功能说明
    1、通过检测PC13 IO 高电平触发入侵中断。

2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
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
        1、时钟源：LSE
        2、入侵检测IO：PC13
        3、串口配置：
                    - 串口为USART1（TX：PA9  RX：PA10）:
                    - 数据位：8
                    - 停止位：1
                    - 奇偶校验：无
                    - 波特率： 115200 
    使用方法：
        1、编译后烧录到评估板，上电后，PC13添加外部下拉，给PC13灌入高电平，串口输出RTC Tamper Interrupt。说明入侵检测发生了入侵中断
               
4、注意事项
    如果配置成上升沿或下降沿触发入侵，需要在入侵引脚上外接上拉或下拉


1. Function description

    1. Tamper interrupt is triggered by detecting PC13 IO high level.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
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

    System configuration:

        1. RTC Clock source: LSE
        2. Tamper detection IO: PC13
        3. Serial port configuration:
                            - Serial port: USART1 (TX: PA9 RX: PA10) :
                            - Data bit: 8
                            - Stop bit: 1
                            - Parity check: None
                            - Baud rate: 115200

    Instructions:
         1. After compiling, it is burned to the evaluation board. After powering on, add an external pull-down on PC13, then high level is artificially injected into PC13 and "RTC Tamper interrupt" is output through the serial port. 
            It indicates that the tamper interrupt is detected
        
4. Attention
    If a rising or falling edge is configured to trigger a tamper, an external pull up or pull down is required on the tamper pin