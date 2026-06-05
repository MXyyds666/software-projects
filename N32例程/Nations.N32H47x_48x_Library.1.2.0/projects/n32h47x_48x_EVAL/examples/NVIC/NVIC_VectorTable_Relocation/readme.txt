1、功能说明

    /* 简单描述工程功能 */
        这个例程配置并演示NVIC中断向量表重定位


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34.0.0
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
	
	/* 描述相关模块配置方法；例如:时钟，I/O等 */
    系统时钟配置：
        N32H475/N32H482/N32H487:
            SystemClock：240MHz
        N32H474/N32H473:
            SystemClock：240MHz
            
    打印串口配置：
        N32H474/N32H473/N32H482/N32H487:
            USART：TX - PA9，RX - PA10，波特率115200
        N32H475：
            USART：TX - PA9，RX - PA15，波特率115200
            
    EXIT：PA4为浮空输入模式，外部中断线 - EXIT_LINE4，开启外部中断，优先级为0

    /* 描述Demo的测试步骤和现象 */
        1.编译后下载程序复位运行；
        2.一开始向量表位于FLASH，当按键按下后向量表重定位至SRAM，并打印相关信息，程序运行正常；


4、注意事项


1. Function description
    /* A brief description of the engineering function */
    This routine configures and demonstrates NVIC interrupt directional table relocation

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
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

	/* Describe related module configuration methods; for example: clock, I/O, etc. */
    System Clock Configuration:
        N32H475/N32H482/N32H487:
            SystemClock：240MHz
        N32H474/N32H473:
            SystemClock：240MHz
            
    Print Serial Port Configuration:
        N32H474/N32H473/N32H482/N32H487:
            USART：TX - PA9，RX - PA10, baud rate 115200
        N32H475：
            USART：TX - PA9，RX - PA15, baud rate 115200
    EXIT: PA4 is floating input mode, external interrupt line -exit_line4, external interrupt is enabled, and the priority is 0
    /* Describes the test steps and symptoms of Demo */
    1. Reset and run the downloaded program after compilation;
    2. At the beginning, the directional table is located in FLASH. When the button is pressed, the backward table is repositioned to SRAM and relevant information is printed, and the program runs normally;

4. Attention
    None