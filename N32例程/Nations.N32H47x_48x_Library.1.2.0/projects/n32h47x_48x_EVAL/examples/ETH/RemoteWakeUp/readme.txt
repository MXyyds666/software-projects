1、功能说明
    本示例实现基于UDP的远程唤醒包检测与唤醒STOP功能。用户通过按下按键，ETH进入低功耗模式（掉电模式），MCU进入STOP模式，使系统产生更低的功耗。ETH通过检测远程唤醒包来唤醒网络，在此之前将不再接收和发送其他数据包。当检测到远程唤醒包，ETH产生PMT中断，ETH退出低功耗模式，同时连接到EXTI Line27的中断线产生中断/事件，系统退出STOP模式。远程唤醒包由上位机工具“网络调试助手”发送，远程唤醒包可以由任意数据组成，但必须满足远程唤醒过滤器寄存器的相关设置和符合有效远程唤醒数据包（具体见用户手册PMT章节相关描述）。
    本示例中远程唤醒数据包的数据域为（HEX）：00 01 02 03 04 05 06 07 00 01 02 03 04 05 06 07 00 01 02 03 04 05 06 07

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
        4、打开网络调试助手，协议类型选择UDP，设置IP地址和端口，再将远程唤醒数据包的数据域部分（见上文）复制粘贴至发送数据窗口并发送该数据包（选择HEX格式发送），开发板检测到该远程唤醒数据包后ETH退出低功耗模式，MCU退出STOP模式并打印“Exit STOP mode”
        5、ETH进入低功耗模式后将不再接收除远程唤醒数据包外的其他数据包、不再发送任何数据包；ETH退出低功耗模式，且MCU退出STOP模式后，ETH恢复通信（正常接收/发送数据）

4、注意事项
    1、开发板默认静态IP地址为：192.168.0.32，使用静态IP地址时要确保开发板与电脑处于同一网段，且无IP地址冲突
    2、字节掩码字段用于计算CRC-16，例如：本例中配置为0xFF，指示唤醒数据包的数据域的第1~第8个字节将作为CRC-16的待校验数据
    3、偏移字段用于计算CRC-16，该字段主要是设置待校验数据的首字节在数据包中的具体位置，该字段是基于远程唤醒包的第1个字节的偏移量，偏移量为0即指数据包的第1个字节。例如：本例中待校验数据位于以太网头+IP头+UDP头之后，故该字段配置为0x2A
    4、计算CRC-16时，需要设置初始值（INIT）为0xFFFF、结果异或值（XOROUT）为0、输入数据反转（REFIN）、多项式（POLY）为0x8005
    5、当设置USE_GLOBAL_UNICAST为1时，将启动全局单播检测与唤醒功能，在这种情况下，不需要配置远程唤醒过滤器寄存器，因为此时任何目的MAC地址与MAC地址0寄存器匹配的单播数据包都将被检测为远程唤醒数据包


1. Function description
    This example implements the UDP-based remote wake-up packet detection and wake-up STOP function. By pressing the key, ETH enters the low power mode (power down mode) and MCU enters the STOP mode, which makes the system generate lower power consumption. ETH wakes up the network by detecting remote wake-up packet, and will not receive and send other packets until then. When remote wake-up packet is detected, ETH generates PMT interrupt and ETH exits low power mode; at the same time, interrupt/event is generated from the interrupt line connected to EXTI Line27, and the system exits STOP mode. The remote wake-up packet is sent by the host computer tool "Network Debug Assistant", the remote wake-up packet can be composed of any data, but it must meet the relevant settings of the remote wake-up filter register and meet the valid remote wake-up packet (see the relevant description in the PMT chapter of the user manual).
    The data field of the remote wake-up packet in this example is (HEX): 00 01 02 03 04 05 06 07 00 01 02 03 04 05 06 07 00 01 02 03 04 05 06 07

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
        4, Open the network debugging assistant, select UDP for the protocol type, set the IP address and port, then copy and paste the data field part of the remote wake-up packet (see above) into the send data window and send the packet (Select HEX format to send), the development board detects the remote wake-up packet and ETH exits the low-power mode, the MCU exits the STOP mode and prints "Exit STOP mode".
        5, After ETH enters the low power mode, it will not receive any other packets except the remote wake-up packet and will not send any packets; after ETH exits the low power mode and MCU exits the STOP mode, ETH resumes the communication (receiving/sending data normally).

4. Attention
    1, The default static IP address of the development board is 192.168.0.32. When using a static IP address, make sure that the board is on the same network segment as the computer and that there are no IP address conflicts.
    2, Byte mask field is used to calculate CRC-16, e.g., 0xFF is configured in this example to indicate that bytes 1 to 8 of the data field of the wake-up packet will be used as the CRC-16 to-be-checked data.
    3, Offset field is used to calculate CRC-16, this field mainly sets the specific position of the first byte of the data to be verified in the data packet, this field is based on the offset of the 1st byte of the remote wake-up packet, an offset of 0 means the 1st byte of the data packet. For example, in this example, the data to be verified is located after the Ethernet header + IP header + UDP header, so this field is configured as 0x2A.
    4, When calculating CRC-16, you need to set the initial value (INIT) to 0xFFFF, the resultant xor value (XOROUT) to 0, the input data reversed (REFIN), and the polynomial (POLY) to 0x8005.
    5, When USE_GLOBAL_UNICAST is set to 1, the global unicast detect and wake-up function will be activated, in which case there is no need to configure the Remote Wake-up Filter register, because at this point any unicast packet whose destination MAC address matches the MAC address 0 register will be detected as a Remote Wake-up packet.

