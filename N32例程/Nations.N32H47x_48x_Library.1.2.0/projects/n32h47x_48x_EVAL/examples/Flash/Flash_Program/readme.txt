1、功能说明

	/* 简单描述工程功能 */
        这个例程配置并演示直接对FLASH进行读写操作


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
        2.查看串口打印信息，当写入FLASH的数据与读取数据对比均相同时，打印信息为测试结束；


4、注意事项


1. Function description

	/* Briefly describe the project function */
         This routine configures and demonstrates direct read and write operations to FLASH


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
         2. Check the printing information of the serial port. When the data written to FLASH is the same as the data read, the printing information is the end of the test;


4. Attention
