1、功能说明
    
    此例程展示了FDCAN 节点2如何通过轮询方式接收节点1发送的数据帧。

2、使用环境
    软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34.0.0
                IAR EWARM 8.50.1
                
    硬件开发环境：
        N32H473系列：基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：基于评估板N32H482ZEL7_EVB V1.1开发
        N32H487系列：基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明

    系统配置
        1、时钟源：HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：240MHz
            N32H475/482/487/488系列：240MHz
        3、打印：PA9 - baud rate 115200
        4、节点1:
            FDCAN:
                N32H482/487/488系列：FDCAN1，TX-PG6，RX-PG7
                N32H473/474系列：FDCAN1，TX-PB13，RX-PB14
                N32H475系列：FDCAN3，TX-PB13，RX-PB14
            时钟源：20MHz
            波特率：250Kbps（标称），500Kbps（数据）
            工作模式：普通模式
            标称位时序：预分频=2，TSEG1=31，TSEG2=8，同步跳转=8
            数据位时序：预分频=2，TSEG1=15，TSEG2=4，同步跳转=4
            帧格式：FDCAN，BRS ON，16字节数据
            标准接收过滤器：禁用
            扩展接收过滤器：1个，ID掩码过滤（ID=0x0AAAAAAA,掩码=0x1FFFFFFF），存储在RX FIFO0
            接收FIFO：启用FIFO0，大小为2，FIFO1禁用
            接收缓冲：禁用
            发送缓冲：禁用
            发送FIFO：大小为6
            发送事件FIFO：禁用
            时间戳：内部时间戳，16分频
            其他：拒绝远程帧与非匹配帧
        5、节点2:
            FDCAN:
                N32H482/487/488系列：FDCAN2，TX-PH2，RX-PH3
                N32H473/474/475系列：FDCAN2，TX-PB0，RX-PB1
            时钟源：20MHz
            波特率：250Kbps（标称），500Kbps（数据）
            工作模式：普通模式
            标称位时序：预分频=2，TSEG1=31，TSEG2=8，同步跳转=8
            数据位时序：预分频=2，TSEG1=15，TSEG2=4，同步跳转=4
            帧格式：FDCAN，BRS ON，16字节数据
            标准接收过滤器：禁用
            扩展接收过滤器：1个，ID掩码过滤（ID=0x15555555,掩码=0x1FFFFFFF），存储在RX FIFO1
            接收FIFO：启用FIFO1，大小为6，FIFO1禁用
            接收缓冲：禁用
            发送缓冲：禁用
            发送FIFO：禁用
            发送事件FIFO：禁用
            时间戳：内部时间戳，16分频
            其他：拒绝远程帧与非匹配帧
        6、GPIO：
            N32H473/474系列: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488系列:  PA3-D13,PA8-D15,PA0-KEY
            N32H475系列:     PA3-D1, PA7-D2, PA2-KEY
        7、EXTI:
            EXTI line1，上升沿触发

    使用方法：
        1、编译后将程序下载到开发板并复位运行。
        2、轻按按键，节点1发送一个数据帧：扩展帧，ID=0x15555555，通过TX FIFO发送。
        3、轮询节点2 RX FIFO1，收到数据帧后打印接收数据内容。

4、注意事项
    N32H473/474/475系列必须外接两个CAN PHY芯片。
    
1. Function description

     This example shows how FDCAN node2 receives the data frame sent by node1 through polling.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H473 series: Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series: Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series: Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series: Developed based on the evaluation board N32H482ZEL7_EVB V1.1
        N32H487 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.1

3. Instructions for use

     1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:     240MHz
            N32H475/482/487/488 series: 240MHz
        3. printf: PA9 - baud rate 115200
        4. node1:
            FDCAN:
                N32H482/487/488 Serie: FDCAN1, TX-PG6, RX-PG7
                N32H473/474 Serie: FDCAN1, TX-PB13, RX-PB14
                N32H475 Serie: FDCAN3, TX-PB13, RX-PB14
            Clock source: 20MHz
            Baudrate: 250Kbps(nominal), 500Kbps(data)
            Working mode: normal mode
            Nominal bit timing: prescaler=2, TSEG1=31, TSEG2=8, sync jump=8
            Data bit timing: prescaler=2, TSEG1=15, TSEG2=4, synchronous jump=4
            Frame format: FDCAN, BRS ON, 16 bytes data
            Standard filter: disable
            Extended filter: 1, ID mask filter (ID=0x0AAAAAAA,mask=0x1FFFFFFF), stored in RX FIFO0
            Receive FIFO: FIFO0 enabled, size 2, FIFO1 disabled
            Receive buffer: disable
            Send buffer: disable
            Send FIFO: size 6
            Send event FIFO: disable
            Timestamp: internal timestamp, divided by 16
            Others: Reject remote frames and non-matching frames
        5. node2:
            FDCAN:
                N32H482/487/488 Serie: FDCAN2，TX-PH2，RX-PH3
                N32H473/474/475 Serie: FDCAN2，TX-PB0，RX-PB1
            Clock source: 20MHz
            Baudrate: 250Kbps(nominal), 500Kbps(data)
            Working mode: normal mode
            Nominal bit timing: prescaler=2, TSEG1=31, TSEG2=8, sync jump=8
            Data bit timing: prescaler=2, TSEG1=15, TSEG2=4, synchronous jump=4
            Frame format: FDCAN, BRS ON, 16 bytes data
            Standard filter: disable
            Extended filter: 1, ID mask filter (ID=0x15555555,mask=0x1FFFFFFF), stored in RX FIFO1
            Receive FIFO: FIFO1 enabled, size 6, FIFO0 disabled
            Receive buffer: disable
            Send buffer: disable
            Send FIFO: disable
            Send event FIFO: disable
            Timestamp: internal timestamp, divided by 16
            Others: Reject remote frames and non-matching frames
        6. GPIO:
            N32H473/474 series: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488 series:  PA3-D13,PA8-D15,PA0-KEY
            N32H475 series:     PA3-D1, PA7-D2, PA2-KEY
        7.EXTI:
            EXTI line1,PA2 input, trigged at rising edge
            
     Instructions:
        1. After compiling, download the program and reset, the program start running.
        2. Press the button, node1 sends a data frame through TX FIFO: extended frame, ID=0x15555555.
        3. Poll node2 RX FIFO1, print the data after receiving the frame.

4. Attention
    Two external CAN PHY chips must be connected in N32H473/474/475 series testing.

