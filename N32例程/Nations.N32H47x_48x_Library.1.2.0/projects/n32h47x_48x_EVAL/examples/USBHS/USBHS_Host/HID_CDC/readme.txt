1、功能说明
    USB 主机 HID mouse & HID keyboard & CDC

2、使用环境

    软件开发环境：KEIL MDK-ARM 5.34
                  IAR EWARM 8.50.1

    硬件开发环境：
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明
    描述相关模块配置方法；例如:时钟，I/O等 
    1. SystemClock：240MHz
	2. USBClock: HSE 16MHz
    3. GPIO: 上-KEY1（PC13）、下-KEY2（PA15）、左（PC6）、右（PC7）、确定-WAKEUP（PA0）、KEY3（PC8）、VBUS驱动引脚-PE1。
    4. 串口配置：
            - 串口为USART1（TX：PA9  RX：PA10）:
            - 数据位：8
            - 停止位：1
            - 奇偶校验：无
            - 波特率： 115200 

    描述Demo的测试步骤和现象 
    1. 编译后下载程序复位运行；
    2. 打开串口调试助手，设置波特率为115200bps
    3. 连接OTG转接线，再接上鼠标或键盘，等待枚举完成
    4. 枚举完成后，按下KEY3按键，开始鼠标或键盘或CDC设备数据通信 
    5. 串口调试助手界面打印相应鼠标坐标或键盘键值信息或
       根据打印信息按KEY1, KEY2, PC6, PC7选择发送或接收串口数据或串口配置，WAKEUP按键确认选项。。

4、注意事项
    可通过修改宏定义 USE_USB_HS_IN_FS 或 USE_USB_HS_IN_HS 切换设备为全速模式或者高速模式；
    需要使用16MHz、19.2MHz、20MHz、24MHz、26MHz或32MHz外部晶体。

1. Function description
    USB host HID mouse & HID keyboard & CDC

2. Use environment
    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_EVB V1.1

3. Instructions for use
    Describe the configuration method of related modules; for example: clock, I/O, etc. 
    1. SystemClock: 240MHz
	2. USBClock: HSE 16MHz
    3. GPIO: UP-KEY1(PC13), DOWN-KEY2(PA15), LEFT(PC6), RIGHT(PC7), ENTER-WAKEUP(PA0)、KEY3(PC8)、VBUS driver pin(PE1).
    4. Serial port configuration:
            - Serial port: USART1 (TX: PA9 RX: PA10) :
            - Data bit: 8
            - Stop bit: 1
            - Parity check: None
            - Baud rate: 115200
    
    Describe the testing steps and phenomena of the Demo
    1. Compile the program and download it for reset and execution.
    2. Open the serial port debugging Assistant and set the baud rate to 115200bps.
    3. Connect the OTG adapter, followed by the mouse or keyboard, and wait for enumeration to complete.
    4. After enumeration is complete, press the KEY3 button to start mouse or keyboard or CDC device data communication.
    5. The serial port Debugging Assistant interface will display the corresponding mouse coordinates or keyboard key value information or
       based on the printed information, pressing KEY1, KEY2, PC6, PC7 can select send or receive data or configurate USART, and pressing the WAKEUP KEY confirms the selection.
    
4. Attention
    The device can be switched to Full-Speed mode or High-Speed mode by modify the macro definition USE_USB_HS_IN_FS or USE_USB_HS_IN_HS;
    External crystal of 16MHz, 19.2MHz, 20MHz, 24MHz, 26MHz or 32MHz is required.