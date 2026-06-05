1、功能说明
    本工程为有操作系统（FreeRTOS）的HTTP服务器示例。开发板作服务器，电脑浏览器作客服端访问开发板并显示一个简单网页。本示例提供两种API编程方式（socket和netconn），默认使用netconn编程（默认USE_NETCONN=1），用户可以根据需要设置USE_NETCONN和USE_SOCKET切换编程方式。

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

    使用方法：
        1、根据需要设置LWIP_DHCP、USE_NETCONN
        2、编译后下载程序运行，串口将打印本地IP地址、端口等信息
        3、在电脑的浏览器中输入开发板IP地址，连接成功后会打印客户端的IP地址、端口信息，并显示一个简单的网页

4、注意事项
    1、如果需要使用DHCP功能，必须确保LWIP_DHCP已定义为1（在lwipopts.h文件修改定义），开发板、电脑均需要连接到路由器或具有DHCP功能的交换机
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of HTTP server with an OS (FreeRTOS). The board acts as a server and the computer browser acts as a client to access the board and display a simple web page. This example provides two API programming methods (socket and netconn), the default use of netconn programming (default USE_NETCONN = 1), the user can set up USE_NETCONN and USE_SOCKET to switch the programming method according to the need.

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

    Usage:
        1, Set LWIP_DHCP, USE_NETCONN as needed.
        2, Compile and download the program to run, the serial port will print the local IP address, port and other information.
        3, Enter the IP address of the development board in the computer's browser, and after a successful connection, it will print the client's IP address, port information, and display a simple web page.

4. Attention
    1, If you need to use the DHCP function, you must make sure that LWIP_DHCP has been defined as 1 (change the definition in the lwipopts.h file), and that the development board and the computer need to be connected to a router or a switch with DHCP function.
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

