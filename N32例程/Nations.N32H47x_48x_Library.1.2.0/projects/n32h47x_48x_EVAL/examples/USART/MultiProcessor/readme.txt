1、功能说明

    该测例演示了如何使用USART多处理器模式?
    首先，分别设置USARTy和USARTz的地址为0x1和0x2。USARTy连续给USARTz
发送字符0x33。USARTz收到0x33，便翻转LED1的引脚。
    一旦KEY1_INT_EXTI_LINE线检测到上升沿，则产生EXTI0中断，在
EXTI0_IRQHandler中断处理函数中(the ControlFlag = 0)，USARTz进入静默
模式，在静默模式中，LED引脚停止翻转，直到KEY1_INT_EXTI_LINE线检测到
上升沿(the ControlFlag = 1)。在EXTI0_IRQHandler中断处理函数中，USARTy
发送地址0x102唤醒USARTz。LED引脚重新启动翻转。


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
    
    USARTy配置如下：
    - 波特率 = 115200 baud
    - 字长 = 9数据位
    - 1停止位
    - 校验控制禁用
    - 硬件流控制禁用（RTS和CTS信号）
    - 接收器和发送器使能  
    
    
    USART引脚连接如下：    
     - USART3_Tx.PA9 <-------> UART5_Rx.PA3
     - USART3_Rx.PA15 <-------> UART5_Tx.PA2
    
    N32H487系列：
        EXIT：PC13为浮空输入模式，外部中断线 - EXIT_LINE13，开启外部中断
    N32H482/N32H475/N32H474/N32H473系列:
        EXIT：PA4为浮空输入模式，外部中断线 - EXIT_LINE4，开启外部中断
        
    LED <-------> PA8

    
    测试步骤与现象：
    - Demo在KEIL环境下编译后，下载至MCU
    - 复位运行，观察LED1是否处于闪烁状态
    - 按按键KEY，观察LED1是否停止闪烁
    - 再次按按键KEY，观察LED1是否恢复闪烁


4、注意事项
   需先将开发板NS-LINK的MCU_TX和MCU_RX跳线帽断开


1. Function description
    This test example demonstrates how to use the USART multiprocessor mode.
    First, set the addresses of USARTy and USARTz to 0x1 and 0x2, respectively. USARTy continuously gives USARTz send the character
    0x33. USARTz receives 0x33 and flips the pin of LED1.
    Once a rising edge is detected on the KEY1_INT_EXTI_LINE line, an EXTI0 interrupt will be generated.
    In the EXTI0_IRQHandler interrupt handler (the ControlFlag = 0), USARTz goes silent mode, in silent mode, the LED pin stops toggling.
    toggling until the KEY1_INT_EXTI_LINE line detects rising edge (the ControlFlag = 1). In the EXTI0_IRQHandler interrupt handler, 
    USARTysend address 0x102 to wake up USARTz. The LED pin restarts toggling.


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
    
    USARTy is configured as follows:
    - Baud rate = 115200 baud
    - Word length = 9 data bits
    - 1 stop bit
    - checksum control disabled
    - Hardware flow control disabled (RTS and CTS signals)
    - Receiver and transmitter enable
    
    
    The USART pins are connected as follows:
     - USART3_Tx.PA9 <-------> UART5_Rx.PA3
     - USART3_Rx.PA15 <-------> UART5_Tx.PA2
    
    N32H487：               
        EXIT: PC13 is in floating input mode, and external interrupt line -exit_line13 is used to enable external interrupt
    N32H482/N32H475/N32H474/N32H473：               
        EXIT: PA4 is in floating input mode, and external interrupt line -exit_line4 is used to enable external interrupt
        
    
    LED <-------> PA8

    
    Test steps and phenomena:
    - Demo is compiled in KEIL environment and downloaded to MCU
    - Reset operation and observe whether LED1 are blinking
    - Press the button KEY and observe whether LED1 stop flashing
    - Press the button KEY again and observe whether LED1 resume to flash


4. Attention
    the MCU_TX and MCU_RX jumper cap of the development board NS-LINK needs to be disconnected first