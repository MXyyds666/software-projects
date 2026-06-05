1、功能说明
    本工程为有操作系统（FreeRTOS）的DNS示例。通过DNS解析域名。

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
        1、电脑和开发板均用网线连接路由
        2、修改HOST_NAME，设置要解析的主机名，修改USE_BLOCKING，设置调用阻塞式API或非阻塞式API
        3、编译后下载程序运行，串口将打印解析的IP地址等信息
        4、用户可以在电脑端ping主机名，检查回复信息中IP地址是否与打印的IP地址相同

4、注意事项
    1、需要使用DNS功能时，注意设置DNS服务器地址，本例程在lwipopts.h中定义了DNS_SERVER_ADDRESS，并将8.8.8.8（谷歌）设置为DNS服务器地址，用户可根据需要修改该地址；如果同时使用了DHCP功能，该DNS服务器地址也可以设置为由DHCP获取的网关地址（gw）
    2、如果不使用DHCP功能，则必须确保开发板的静态IP地址与路由器处于同一网段（开发板静态IP默认为：192.168.0.32），并与该局域网内的其他设备无IP地址冲突
    3、建议在使用DNS功能时，同时也开启DHCP功能


1. Function description
    This project is an example of DNS with an OS (FreeRTOS). Resolve domain names via DNS.

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
        1, Computer and development board are connected to the route with a network cable.
        2, Modify HOST_NAME to set the hostname to be resolved, and modify USE_BLOCKING to set whether to call the blocking API or the non-blocking API.
        3, Compile and download the program to run, the serial port will print the resolved IP address and other information.
        4, Users can ping the hostname on the computer side to check if the IP address in the reply message is the same as the printed IP address.

4. Attention
    1, When you need to use the DNS function, pay attention to set the DNS server address. This routine defines DNS_SERVER_ADDRESS in lwipopts.h and sets 8.8.8.8 (Google) as the DNS server address, which can be modified by the user according to the need; If the DHCP function is also used, this DNS server address can also be set to the gateway address (gw) obtained by DHCP.
    2, If you do not use the DHCP function, you must ensure that the static IP address of the board is in the same network segment as the router (the default static IP address of the board is 192.168.0.32) and that there is no IP address conflict with other devices on the LAN.
    3, It is recommended that you use the DNS function while also enabling the DHCP function.
