1、功能说明
    本工程为有操作系统（FreeRTOS）的DHCP示例。通过DHCP为开发板动态分配IP地址。

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
        2、编译后下载程序运行，串口将打印IP地址、子网掩码、网关等信息
        3、在电脑端使用ping命令以及打印信息中的IP地址，测试网络是否畅通

4、注意事项
    1、使用DHCP功能动态分配IP地址时，必须确保LWIP_DHCP已定义为1（在lwipopts.h文件修改定义）
    2、开发板、PC机均需要连接到路由器或具有DHCP功能的交换机，才能完成本示例的测试


1. Function description
    This project is an example of DHCP with an OS (FreeRTOS). Dynamically assign an IP address to the development board via DHCP.

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
        2, Compile and download the program to run, the serial port will print the IP address, Netmask, Gateway and other information.
        3, Use the ping command on the computer side and the IP address in the printout to test if the network is open.

4. Attention
    1, When using the DHCP function to dynamically assign IP addresses, you must ensure that LWIP_DHCP has been defined as 1 (change the definition in the lwipopts.h file)
    2, Both the development board and the PC need to be connected to a router or a switch with DHCP functionality to complete the tests in this example

