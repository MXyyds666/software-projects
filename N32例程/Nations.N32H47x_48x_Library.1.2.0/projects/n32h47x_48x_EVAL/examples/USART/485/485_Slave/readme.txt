1、功能说明

    该测例演示了两块开发板USARTz间通过中断实现的基础通信。
    Master发送TxBuffer2数据至Slave，USARTz接收数据存至RxBuffer2。
    比较接收数据与发送数据，比较结果存入TransferStatus变量。


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34.0.0
                  IAR EWARM 8.50.1
    
    硬件开发环境：    
        N32H487系列：
        基于评估板N32H487ZEL7_STB V1.0开发
        

3、使用说明
	
	/* 描述相关模块配置方法；例如:时钟，I/O等 */
    系统时钟配置：
        N32H487:
            SystemClock：240MHz
    
    USART配置如下：
    - 波特率 = 115200 baud
    - 字长 = 8数据位
    - 1停止位
    - 校验控制禁用
    - 硬件流控制禁用（RTS和CTS信号）
    - 接收器和发送器使能
    
    USART引脚连接如下：   
    Master:
     - UART5_Rx.PG7(J33)
     - UART5_Tx.PG6(J35)
     - UART5_DE.PG15(J34) 
     - 485模块
    Slave:
     - UART5_Rx.PG7(J33)
     - UART5_Tx.PG6(J35) 
     - UART5_DE.PG15(J34) 
     - 485模块
     
     485连接：
     A<--->A
     B<--->B

    
    测试步骤与现象：
    - Demo在KEIL环境下编译后，分别下载Master与Slavedemo至Master与Slave开发板
    - 复位运行，查看串口打印信息，PASSED为测试通过，FAILED为测试异常


4、注意事项

    需先将开发板NS-LINK的MCU_TX和MCU_RX跳线帽断开
    Slave开发板需先上电，Master再上电




1. Function description

    This test case demonstrates basic communication between two development board USARTz via interrupts.
    The Master sends TxBuffer2 data to the Slave and the USARTz receives the data and stores it in RxBuffer2.
    The received data is compared with the sent data and the comparison is stored in the TransferStatus variable.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
                                      IAR EWARM 8.50.1
    Hardware development environment:
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_STB V1.0
        

3. Instructions for use

	/* Describe related module configuration methods; for example: clock, I/O, etc. */
    System Clock Configuration:
        N32H487:
            SystemClock：240MHz

    USART is configured as follows:
    - Baud rate = 115200 baud
    - Word length = 8 data bits
    - 1 stop bit
    - checksum control disabled
    - Hardware flow control disabled (RTS and CTS signals)
    - Receiver and transmitter enable
    - DMA transmit mode enabled, DMA receive mode disabled

    Master.
     - UART5_Rx.PG7
     - UART5_Tx.PG6 
     - UART5_DE.PG15 
     - 485 Module
    Slave: UART5_Rx.
     - UART5_Rx.PG7
     - UART5_Tx.PG6 
     - UART5_DE.PG15
     - 485 Module
     
     485 connection:
     A<--->A
     B<--->B

    Test steps and phenomena:
    - After the demo is compiled in KEIL environment, download Master and Slavedemo to Master and Slave development boards respectively.
    - Reset running, Viewing Serial Port Printing Information, PASSED for the test passed, FAILED for the test exception

4. Attention
    the MCU_TX and MCU_RX jumper cap of the development board NS-LINK needs to be disconnected first
    The Master board needs to be powered up first, then the Slave.