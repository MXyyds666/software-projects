1、功能说明
    本工程为有操作系统（FreeRTOS）的HTTP服务器控制两个LED的示例。开发板作服务器，电脑浏览器访问开发板，并在网页中控制LED1和LED2的开/关。本示例中包含了检测GET、POST命令及发送相应的响应数据。

2、使用环境

    软件开发环境：
        KEIL MDK-ARM V5.34
        IAR EWARM 8.50.1

    硬件开发环境：
        基于全功能板N32H487ZEL7_EVB V1.1开发

3、使用说明
    
    系统配置：
        1、时钟源：HSI+PLL
        2、时钟频率：240MHz
        3、打印：PA9 - baud rate 115200

    以太网配置：
        1、PHY接口：RMII
        2、引脚分配：
            ETH_MDC<----->PC1
            ETH_MDIO<----->PA2
            ETH_RMII_REF_CLK<----->PA1
            ETH_RMII_CRS_DV<----->PD8
            ETH_RMII_RX_D0<----->PD9
            ETH_RMII_RX_D1<----->PD10
            ETH_RMII_TX_EN<----->PG11
            ETH_RMII_TX_D0<----->PG13
            ETH_RMII_TX_D1<----->PG14
            ETH_PPS_OUT<----->PB5

    LED配置：
        LED1<----->PA3
        LED2<----->PB4

    使用方法：
        1、根据需要设置LWIP_DHCP
        2、编译后下载程序运行，串口将打印本地IP地址、端口等信息
        3、在电脑的浏览器中输入开发板IP地址，连接成功后会打印客户端的IP地址、端口信息，并显示一个简单的LED控制页面
        4、该网页中的LED1和LED2的状态与开发板上的LED1和LED2的状态一致，用户点击相应的按钮将打开/关闭对应的LED，实现对开发板上LED的控制

4、注意事项
    1、如果需要使用DHCP功能，必须确保LWIP_DHCP已定义为1（在lwipopts.h文件修改定义），开发板、电脑均需要连接到路由器或具有DHCP功能的交换机
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of HTTP server with an OS (FreeRTOS) to control two LEDs. The development board acts as a server and a computer browser accesses the board and controls LED1 and LED2 on/off in a web page. This example includes detecting GET and POST commands and sending the corresponding response data.

2. Use environment

    Software development environment:
        KEIL MDK-ARM V5.34
        IAR EWARM 8.50.1

    Hardware development environment:
        Developed based on the full-featured board N32H487ZEL7_EVB V1.1

3. Instructions for use
    
    System configuration:
        1, clock source: HSI + PLL
        2, clock frequency: 240MHz
        3, print: PA9 - baud rate 115200

    Ethernet configuration:
        1, PHY interface: RMII
        2, Pin assignment:
            ETH_MDC<----->PC1
            ETH_MDIO<----->PA2
            ETH_RMII_REF_CLK<----->PA1
            ETH_RMII_CRS_DV<----->PD8
            ETH_RMII_RX_D0<----->PD9
            ETH_RMII_RX_D1<----->PD10
            ETH_RMII_TX_EN<----->PG11
            ETH_RMII_TX_D0<----->PG13
            ETH_RMII_TX_D1<----->PG14
            ETH_PPS_OUT<----->PB5

    LED configuration:
        LED1<----->PA3
        LED2<----->PB4

    Usage:
        1, Set LWIP_DHCP as needed.
        2, Compile and download the program to run, the serial port will print the local IP address, port and other information.
        3, Enter the IP address of the development board into the browser of the computer, and after successful connection, the IP address and port information of the client will be printed, and a simple LED control page will be displayed.
        4, The status of LED1 and LED2 in the web page is the same as that of LED1 and LED2 on the development board, and the user clicks the corresponding buttons to turn on/off the corresponding LEDs, realizing the control of LEDs on the development board.

4. Attention
    1, If you need to use the DHCP function, you must make sure that LWIP_DHCP has been defined as 1 (change the definition in the lwipopts.h file), and that the development board and the computer need to be connected to a router or a switch with DHCP function.
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

