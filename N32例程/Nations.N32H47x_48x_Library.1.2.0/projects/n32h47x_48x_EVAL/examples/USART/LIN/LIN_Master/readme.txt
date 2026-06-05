1、功能说明

    该演示显示了USART模块LIN模式作为主节点轮询发送主请求帧（0x3C）和从应答帧（0x3D）。

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
    
    N32H475/N32H474/N32H473/N32H482:
    - LIN mode master     LIN mode slave
    - UART5_Tx.PA2  <-->  UART5_Rx.PA3  
    - UART5_Rx.PA3  <-->  UART5_Tx.PA2  

    N32H487:
        master LIN板:
            UART5_Rx:PG7<->连接跳帽J25
            UART5_Tx:PG6<->连接跳帽J27
      
       Slave LIN板:
            UART5_Rx:PH3<->连接跳帽J36
            UART5_Tx:PH2<->连接跳帽J86
       
       Master_LIN.LIN与Slave_LIN.LIN连接，两板公地。

    
    测试步骤与现象：
    a、 跳线连接到主机的从属引脚
    b、 一个开发板编译并下载LIN_Master代码作为主代码
         另一个开发板编译并下载LIN_Slave代码作为从属代码
    c、 重置从设备，然后重置主设备
   d、 主轮询发送请求帧（0x3C）和应答帧（0x3D）
         发送请求帧（0x3C）：主设备向从设备发送一个8字节的0x0F，并打印相关信息
         发送应答帧（0x3D）：从机收到应答帧后，向主机回复8字节0x01消息，并且主控打印相关信息


4、注意事项




1. Function description

    This demo shows that the USART module LIN mode as the master node polling to send the master
     request frame (0x3C) and the slave reply frame (0x3D).


2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
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
            SystemClock:240MHz
        N32H474/N32H473:
            SystemClock:240MHz
    
    The USART configuration is as follows:
    -Baud rate = 9600baud
    -Word length = 8 data bits
    -1 stop bit
    -Parity control disabled
    -Hardware flow control disabled (RTS and CTS signals)
    -Receiver and transmitter enable
    -LIN mode enable

    Print Serial Port Configuration:
        N32H474/N32H473/N32H482/N32H487:
            USART:TX - PA9:RX - PA10, baud rate 115200
        N32H475:
            USART:TX - PA9:RX - PA15, baud rate 115200
    
    N32H475/N32H474/N32H473/N32H482:
        - LIN mode master     LIN mode slave
        - UART5_Tx.PA2  <-->  UART5_Rx.PA3  
        - UART5_Rx.PA3  <-->  UART5_Tx.PA2  

    N32H487.
        master LIN board: UART5_Rx.
            UART5_Rx:PG7 <--> Connect Jumper Cap J25
            UART5_Tx:PG6<-->Connect Jumper Cap J27
      
       Slave LIN board: UART5_Rx:PG7<-->Connect Jumper Cap J25
            UART5_Rx:PH3<->Connect Jumper Cap J36
            UART5_Tx:PH2<->connect jump cap J86
       
       Master_LIN.LIN is connected to Slave_LIN.LIN with common ground for both boards.

    Test steps and phenomena:
    a, the jumper connects to the slave pin of the master
    b, one development board compiles and downloads LIN_Master code as the master and 
        the other development board compiles and downloads LIN_Slave code as the slave
    c, Reset the slave and then the master
    d, master polling sends request frame (0x3C) and reply frame (0x3D)
       Sending request frame (0x3C) : The master sends an 8-byte 0x0F to the slave and prints the related information
       Sending reply frame (0x3D) : After receiving a reply frame, the slave reply an 8-byte 0x01 message to the master, 
        and the master prints the related information

4. Attention
    none