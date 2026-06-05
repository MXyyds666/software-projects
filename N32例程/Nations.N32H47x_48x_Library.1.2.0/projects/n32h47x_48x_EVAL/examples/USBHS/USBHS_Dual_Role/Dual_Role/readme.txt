1、功能说明
    USB 双角色 MSC

2、使用环境

    /* 软件开发环境：当前工程使用的软件工具名称及版本号 */
    IDE工具：KEIL MDK-ARM 5.34
      
    /* 硬件环境：工程对应的开发硬件平台 */
    N32H482系列：
    基于评估板N32H482ZEL7_STB V1.0开发
    N32H487系列：
    基于评估板N32H487ZEL7_STB V1.1开发


3、使用说明
    描述相关模块配置方法；例如:时钟，I/O等 
    1. SystemClock：240MHz
    2. USBClock: HSE 16MHz
    3. GPIO: 上-KEY1（PC13）、下-KEY2（PA15）、确定-WAKEUP（PA0）、KEY3（PC8）、VBUS驱动引脚-PE1。
    4. SPI1配置：NSS--PA4、SCK--PA5、MISO--PA6、MOSI--PA7 (连接W25Q128)
    5. 串口配置：
            - 串口为USART1（TX：PA9  RX：PA10）:
            - 数据位：8
            - 停止位：1
            - 奇偶校验：无
            - 波特率： 115200 

    描述Demo的测试步骤和现象 
    1. 编译后下载程序复位运行；
    2. 打开串口调试助手，设置波特率为115200bps
    3. 根据打印使用按键KEY1,KEY2,WAKEUP来选择当前USB角色（主机模式或设备模式），如下所示，箭头指向当前角色选择
       -->  	1 - Host mode
            2 - Device mode
    4. 如果是选择设备，通过 USB 线连接 J62 USB 口，USB 挂载完成后，识别成 U 盘设备
    5. 如果选择主机，通过J62连接OTG转接线，再接上U盘，等待枚举完成，按下KEY3按键，根据打印选择读写U盘

4、注意事项
    可通过修改宏定义 USE_USB_HS_IN_FS 或 USE_USB_HS_IN_HS 切换设备为全速模式或者高速模式；
    需要使用16MHz、19.2MHz、20MHz、24MHz、26MHz或32MHz外部晶体。

1. Function description
    USB Dual role MSC

2. Use environment
     /* Software development environment: the name and version number of the software tool used in the current project */
     IDE tool: KEIL MDK-ARM 5.34
      
     /* Hardware environment: development hardware platform corresponding to the project */
    N32H482 series:
    Developed based on the evaluation board N32H482ZEL7_STB V1.0
    N32H487 series:
    Developed based on the evaluation board N32H487ZEL7_STB V1.1

3. Instructions for use
    Describe the configuration method of related modules; for example: clock, I/O, etc. 
    1. SystemClock: 240MHz
    2. USBClock: HSE 16MHz
    3. GPIO: UP-KEY1(PC13), DOWN-KEY2(PA15), SEL-WAKEUP(PA0), KEY3(PC8), VBUS driver pin(PB3)。
    4. SPI1：NSS--PA4、SCK--PA5、MISO--PA6、MOSI--PA7 (connect W25Q128)
    5. Serial port configuration:
            - Serial port: USART1 (TX: PA9 RX: PA10) :
            - Data bit: 8
            - Stop bit: 1
            - Parity check: None
            - Baud rate: 115200
    
    Describe the testing steps and phenomena of the Demo
    1. Compile the program and download it for reset and execution.
    2. Open the serial port debugging Assistant and set the baud rate to 115200bps.
    3. Use KEY1,KEY2,WAKEUP to select the current USB role (host mode or device mode) according to the print, as shown below, the arrow points to the current role selection.
       -->      1 - Host mode
            2 - Device mode
    4. If device mode is selected, connect it to the J62 USB port using a USB cable. After the USB is mounted, the device is identified as a USB flash drive.
    5. If host mode is selected, connect the OTG conversion cable through J62, and then connect the USB flash drive. 
       Wait for the enumeration to complete, press KEY3, then select read or write the USB flash drive according to the print
    
4. Attention
    The device can be switched to Full-Speed mode or High-Speed mode by modify the macro definition USE_USB_HS_IN_FS or USE_USB_HS_IN_HS;
    External crystal of 16MHz, 19.2MHz, 20MHz, 24MHz, 26MHz or 32MHz is required.