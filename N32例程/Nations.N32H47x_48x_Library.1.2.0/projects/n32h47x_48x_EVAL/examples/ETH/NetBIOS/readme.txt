1、功能说明
    本工程为NetBIOS协议下的名称服务（NetBIOS Name Service）示例，简称NBNS。用于实现将设备的NetBIOS名与IP地址相映射，使得在同一局域网中的其他设备可以通过该NetBIOS名访问该设备。

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
        1、根据需要修改NETBIOS_HOST_NAME
        2、编译后下载程序运行，串口将打印本地IP地址、子网掩码、网关等信息
        3、完成NetBIOS名与本地IP地址的映射后，串口将打印该NetBIOS名
        4、在电脑端ping打印信息中的NetBIOS名，可以看到回复信息中的IP地址信息与开发板本地IP地址一致

4、注意事项
    1、NetBIOS名必须少于15个字符
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of NetBIOS Name Service (NBNS for short) under the NetBIOS protocol. which is used to implement the mapping of a device's NetBIOS name to an IP address so that other devices on the same LAN can access the device through that NetBIOS name.

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
        1, Modify NETBIOS_HOST_NAME as needed.
        2, Compile and download the program to run, the serial port will print the local IP address, subnet mask, gateway and other information.
        3, After completing the mapping between NetBIOS name and local IP address, the serial port will print the NetBIOS name.
        4, Ping the NetBIOS name in the printed message on the computer, you can see that the IP address information in the reply message is the same as the local IP address of the development board.

4. Attention
    1, NetBIOS name must be less than 15 characters
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

