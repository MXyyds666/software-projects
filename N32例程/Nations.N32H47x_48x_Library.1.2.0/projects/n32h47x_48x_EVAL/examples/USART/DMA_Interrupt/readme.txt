1、功能说明

    该测例演示了USARTy与USARTz间通过DMA（中断）实现的基础通信。
    首先，DMA传输TxBuffer1数据至USARTy发送数据寄存器，传输完成产生
DMA中断，随后数据发送至USARTz。USARTz接收数据，DMA从USARTz接收
数据寄存器搬移数据至RxBuffer2，完成后产生DMA中断。
    同时，DMA传输TxBuffer2数据至USARTz发送数据寄存器，传输完成产生
DMA中断，随后数据发送至LPUART。USARTy接收数据，DMA从USARTy接收
数据寄存器搬移数据至RxBuffer1，完成后产生DMA中断。
    最后，分别比较两组收、发数据，比较结果存入TransferStatus1变量
和TransferStatus2变量。


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

    系统时钟配置：
        N32H475/N32H482/N32H487:
            SystemClock：240MHz
        N32H474/N32H473:
            SystemClock：240MHz
    
    USART配置如下：
    - 波特率 = 115200 baud
    - 字长 = 8数据位
    - 1停止位
    - 校验控制禁用
    - 硬件流控制禁用（RTS和CTS信号）
    - 接收器和发送器使能
    - DMA发送模式使能，DMA接收模式禁用
    
    USART引脚连接如下：
     - USART3_Tx.PA9 <-------> UART5_Rx.PA3
     - USART3_Rx.PA15 <-------> UART5_Tx.PA2

    
    测试步骤与现象：
    - Demo在KEIL环境下编译后，下载至MCU
    - 复位运行，依次查看变量TransferStatus1和TransferStatus2，其中，
      PASSED为测试通过，FAILED为测试异常


4、注意事项

    需先将开发板NS-LINK的MCU_TX和MCU_RX跳线帽断开



1. Function description 

    This test example demonstrates basic communication between USARTy and USARTz via DMA. 

   First, the DMA transfers the TxBuffer1 data to the USARTy transmit data register, and then the 
   data is sent to the USARTz. USARTz uses the interrupt to receive data and store it in RxBuffer2. 

   At the same time, the DMA transfers the TxBuffer2 data to the USARTz transmit data register,
   and then the data is sent to the USARTy. USARTy receives data by querying the check flag and 
   stores it in RxBuffer1. 



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
    
 USART is configured as follows:
     - Baud rate = 115200 baud
     - Word length = 8 data bits
     - 1 stop bit 
     - checksum control disabled
     - Hardware flow control disabled (RTS and CTS signals)
     - Receiver and transmitter enable
     - DMA transmit mode enabled, DMA receive mode disabled 
    
The USART pins are connected as follows:
     - USART3_Tx.PA9 <-------> UART5_Rx.PA3
     - USART3_Rx.PA15 <-------> UART5_Tx.PA2
    
 Test steps and phenomena:
     - Demo is compiled in KEIL environment and downloaded to MCU
     - Reset operation, check the variables TransferStatus1 and TransferStatus2 in turn, where PASSED is the test passed and FAILED is the test abnormal 


4. Attention 
    the MCU_TX and MCU_RX jumper cap of the development board NS-LINK needs to be disconnected first