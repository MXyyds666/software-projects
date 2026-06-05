1、功能说明
    本工程为有操作系统（FreeRTOS）的MQTT客户端示例。开发板接入百度智能云，并订阅主题为"$iot/n001/user/temp_hum"的消息。开发板周期性发布DHT11温湿度数据，发布时的主题同为"$iot/n001/user/temp_hum"。由于开发板订阅了相同的主题，在发布消息成功后，开发板也会接收到对应的含温湿度数据的消息。

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

    DHT11配置：
        1、引脚分配：
            DHT11_DATA<----->PF11

    使用方法：
        1、开发板通过网线连接路由器，编译程序并下载运行
        2、等待串口调试助手打印本地IP地址、连接MQTT服务端成功等信息
        3、在打印信息中查看“温湿度数据”和“解析的消息中的温湿度值”

4、注意事项
    1、需要使用DNS功能时，注意设置DNS服务器地址，本例程在lwipopts.h中定义了DNS_SERVER_ADDRESS，并将8.8.8.8（谷歌）设置为DNS服务器地址，用户可根据需要修改该地址；如果同时使用了DHCP功能，该DNS服务器地址也可以设置为由DHCP获取的网关地址（gw）
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This project is an example of MQTT client with an OS (FreeRTOS). The development board is connected to Baidu Cloud and subscribes to messages with the topic "$iot/n001/user/temp_hum". The development board periodically publishes DHT11 temperature and humidity data with the same topic "$iot/n001/user/temp_hum". Since the development board is subscribed to the same topic, the development board will also receive the corresponding message containing temperature and humidity data after the successful publish of the message.

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

    DHT11 configuration:
        1, Pin assignment:
            DHT11_DATA<----->PF11

    Usage:
        1, The development board through the network cable connected to the router, compile the program and download and run.
        2, Wait for the serial debugging assistant to print the local IP address, connect to the MQTT server success and other information.
        3, View "Temperature and humidity data" and "Temperature and humidity values in parsed messages" in the print message.

4. Attention
    1, When you need to use the DNS function, pay attention to set the DNS server address. This routine defines DNS_SERVER_ADDRESS in lwipopts.h and sets 8.8.8.8 (Google) as the DNS server address, which can be modified by the user according to the need; If the DHCP function is also used, this DNS server address can also be set to the gateway address (gw) obtained by DHCP.
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

