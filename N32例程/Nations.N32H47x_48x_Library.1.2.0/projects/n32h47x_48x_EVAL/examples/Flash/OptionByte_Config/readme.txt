1、功能说明

	/* 简单描述工程功能 */
        这个例程配置并演示开启FLASH写保护


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

	/* 描述Demo的测试步骤和现象 */
        1.编译后下载程序复位运行；
        2.对选项字节进行编程，编程OK，打印信息为测试通过；


4、注意事项


/***   For English user   ***/
1. Function description

	/* Briefly describe the project function */
         This routine configures and demonstrates enabling FLASH write protection


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

	/* Describe the test steps and phenomena of the Demo */
         1. After compiling, download the program to reset and run;
         2. Programming the option byte, programming OK, the print information is the test passed;


4. Attention

