1、功能说明
    本工程为有操作系统（FreeRTOS）的HTTP客户端示例。开发板作客户端，获取网页数据。本示例中可以使用上位机工具PHPStudy自主搭建服务器，通过USE_PHPSTUDY切换使用（默认为0）。本示例还提供两种API编程方式（socket和netconn），默认使用netconn编程（默认USE_NETCONN=1），用户可以根据需要设置USE_NETCONN和USE_SOCKET切换编程方式。

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
        1、根据需要设置USE_PHPSTUDY、USE_NETCONN
        2、修改HOST_NAME，设置要解析的主机名，本示例中的主机名为："www.example.com"
        3、编译后下载程序运行，串口将打印本地IP地址、端口和服务器IP地址、端口等信息
        4、连接服务器成功后，打印“The server is connected”，开发板向服务器发送GET命令并阻塞式接收服务器发送的数据
        5、若接收数据无误，串口将打印获取的网页数据，用户可以打开网页源代码，检查打印的网页数据是否与其一致

4、注意事项
    1、需要使用DNS功能时，注意设置DNS服务器地址，本例程在lwipopts.h中定义了DNS_SERVER_ADDRESS，并将8.8.8.8（谷歌）设置为DNS服务器地址，用户可根据需要修改该地址；如果同时使用了DHCP功能，该DNS服务器地址也可以设置为由DHCP获取的网关地址（gw）
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of HTTP client with an OS (FreeRTOS). The development board is used as a client to get web data. In this example, you can use the host computer tool PHPStudy to build a server on your own, and switch to use it through USE_PHPSTUDY (default is 0). This example also provides two API programming methods (socket and netconn), the default use of netconn programming (default USE_NETCONN = 1), the user can set up USE_NETCONN and USE_SOCKET to switch the programming method according to the need.

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
        1, Set USE_PHPSTUDY, USE_NETCONN as needed.
        2, Modify HOST_NAME to set the host name to be resolved, in this example, the host name is: "www.example.com".
        3, Compile and download the program to run, the serial port will print the local IP address, port and server IP address, port and other information.
        4, After successfully connecting to the server, "The server is connected" will be printed, and the development board will send GET command to the server and receive the data sent by the server in blocking mode.
        5, If the received data is correct, the serial port will print the acquired web page data, and the user can open the web page source code to check whether the printed web page data is consistent with it.

4. Attention
    1, When you need to use the DNS function, pay attention to set the DNS server address. This routine defines DNS_SERVER_ADDRESS in lwipopts.h and sets 8.8.8.8 (Google) as the DNS server address, which can be modified by the user according to the need; If the DHCP function is also used, this DNS server address can also be set to the gateway address (gw) obtained by DHCP.
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

