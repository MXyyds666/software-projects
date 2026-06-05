1、功能说明
    USB CDC 设备回环测试

2、使用环境

    软件开发环境：KEIL MDK-ARM 5.34
                  IAR EWARM 8.50.1

    硬件开发环境：
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_STB V1.1开发


3、使用说明
    描述相关模块配置方法；例如:时钟，I/O等 
         1. SystemClock：240MHz
		 2. USBClock: HSE 16MHz

    描述Demo的测试步骤和现象 
         1. 编译后下载程序复位运行；
         2. 通过 USB 线连接 J62 USB 口，电脑识别出虚拟串口设备
         3. 在PC上打开 USB 虚拟串口，USB 虚拟串口发送数据，串口工具会马上收到刚刚发送的数据。


4、注意事项
    可通过修改宏定义 USE_USB_HS_IN_FS 或 USE_USB_HS_IN_HS 切换设备为全速模式或者高速模式；
    另外需要使用16MHz、19.2MHz、20MHz、24MHz、26MHz或32MHz外部晶体。

1. Function description
    USB CDC device loopback test

2. Use environment
    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_STB V1.1

3. Instructions for use
    Describe the configuration method of related modules; for example: clock, I/O, etc. 
        1. SystemClock: 240MHz
		2. USBClock: HSE 16MHz
        3. Serial port: USART1 TX -- PA9 RX -- PA10 Baud rate: 115200

    Describe the testing steps and phenomena of the Demo
    1. After compiling, the downloaded program is reset and run;
    2. Connect a USB cable to the J62 USB port. The computer identifies the virtual serial port device
    3. Open the USB virtual serial port on the PC, and send data through the USB virtual serial port, the serial port tool will immediately receive the data just sent.
      
        
4. Attention
    The device can be switched to Full-Speed mode or High-Speed mode by modify the macro definition USE_USB_HS_IN_FS or USE_USB_HS_IN_HS;
    In addition, external crystal of 16MHz, 19.2MHz, 20MHz, 24MHz, 26MHz or 32MHz is required.