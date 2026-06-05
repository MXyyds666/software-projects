1、功能说明
    本示例实现基于UDP的AMD魔术包检测与唤醒STOP功能。用户通过按下按键，ETH进入低功耗模式（掉电模式），MCU进入STOP模式，使系统产生更低的功耗。ETH通过检测AMD魔术包来唤醒网络，在此之前将不再接收和发送其他数据包。当检测到AMD魔术包，ETH产生PMT中断，ETH退出低功耗模式；同时连接到EXTI Line27的中断线产生中断/事件，系统退出STOP模式。AMD魔术包由上位机工具“WakeMeOnLan”生成并发送，该工具免费，用户可自行下载。用户还可以通过“Wireshark”抓取AMD魔术数据包，查看其数据结构。

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

    KEY配置：
        KEY1<----->PC13

    使用方法：
        1、编译后下载程序运行，串口将打印本地IP地址、端口等信息，本示例使用静态IP地址
        2、按下KEY1，ETH模块随即进入低功耗模式，本示例中还配置MCU进入STOP模式，以实现更低能耗
        3、ETH进入低功耗前打印“Enter ETH Low-power mode”，MCU进入STOP模式前打印“Enter STOP mode”
        4、打开“WakeMeOnLan”工具，选中扫描出的开发板对应的IP地址并唤醒该设备，该工具自动向开发板发送魔术包，开发板检测到该魔术包后ETH退出低功耗模式，MCU退出STOP模式并打印“Exit STOP mode”
        5、ETH进入低功耗模式后将不再接收除魔术包外的其他数据包、不再发送任何数据包；ETH退出低功耗模式，且MCU退出STOP模式后，ETH恢复通信（正常接收/发送数据）

4、注意事项
    1、WakeMeOnLan的使用参考网络上的相关教程
    2、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突


1. Function description
    This example implements the UDP-based AMD magic packet detection and wake up STOP function. By pressing the key, ETH enters the low power mode (power down mode) and MCU enters the STOP mode, which makes the system generate lower power consumption. ETH wakes up the network by detecting AMD magic packet, and will not receive and send other packets until then. When AMD magic packet is detected, ETH generates PMT interrupt and ETH exits low power mode; at the same time, interrupt/event is generated from the interrupt line connected to EXTI Line27, and the system exits STOP mode. AMD magic packet is generated and sent by the host computer tool "WakeMeOnLan", which is free of charge and can be downloaded by users. Users can also use "Wireshark" to capture the AMD magic packet and check its data structure.

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

    KEY configuration:
        KEY1<----->PC13

    Usage:
        1, Compile and download the program to run, the serial port will print the local IP address, port and other information, this example uses a static IP address.
        2, By pressing KEY1, the ETH module enters low-power mode, and in this example, the MCU is also configured to enter STOP mode for lower power consumption.
        3, Print "Enter ETH Low-power mode" before ETH enters low-power mode, and print "Enter STOP mode" before MCU enters STOP mode.
        4, Open the "WakeMeOnLan" tool, select the IP address corresponding to the scanned development board and wake up the device, the tool automatically sends a magic packet to the development board, after the development board detects the magic packet, the ETH exits the low-power mode, and the MCU exits the STOP mode and prints "Exit STOP mode".
        5, After ETH enters the low power mode, it will not receive any other packets except the magic packet and will not send any packets; after ETH exits the low power mode and MCU exits the STOP mode, ETH resumes the communication (receiving/sending data normally).

4. Attention
    1, Refer to related tutorials on the web for the use of WakeMeOnLan.
    2, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.

