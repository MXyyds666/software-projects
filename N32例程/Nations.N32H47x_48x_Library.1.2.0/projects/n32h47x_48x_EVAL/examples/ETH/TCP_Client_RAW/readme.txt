1、功能说明
    本工程为裸机环境下（no OS）的TCP客户端示例。开发板作客户端，网络调试助手作服务器。开发板接收来自服务器的数据，并将接收的数据回显给服务器。本示例由RAW API编程。

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
        1、编译后下载程序运行，串口将打印本地IP地址、端口和服务器IP地址、端口等信息，本示例使用静态IP地址
        2、打开网络调试助手，协议类型选择TCP Server，设置IP地址和端口，点击“打开”按钮，等待客户端连接
        3、客户端连接服务器成功后，串口将打印“Connect successful!!”作为提示，此时即可通过网络调试助手发送数据
        4、开发板接收数据，若数据接收无误，开发板随即将接收的数据打包回显发送至服务器
        5、在网络调试助手中查看对比发送和接收的数据，若相同则通信无误，同样也可以通过Wireshark抓取数据包查看结果

4、注意事项
    1、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of TCP client in a bare-metal environment (no OS). The development board acts as the client and the network debugging assistant acts as the server. The board receives data from the server and echos the received data back to the server. This example is programmed by RAW API.

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
        1, Compile and download the program to run, the serial port will print the local IP address, port and server IP address, port and other information, this example uses static IP address.
        2, Open the network debugging assistant, select the protocol type TCP Server, set the IP address and port, click the "Open" button, and wait for the client to connect.
        3, After the client connects to the server successfully, the serial port will print "Connect successful!!!" as a prompt, then you can send data through the network debugging assistant.
        4, The development board receives the data, if the data is received correctly, the development board will then pack the received data and send it back to the server.
        5, In the network debugging assistant to view the comparison of sent and received data, if the same communication is correct, you can also capture the packet through Wireshark to view the results.

4. Attention
    1, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

