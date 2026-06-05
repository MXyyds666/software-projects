1、功能说明
    此例程展示了FDCAN 节点1与节点2以经典CAN格式相互收发数据。

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
            波特率：500Kbps
            工作模式：普通模式
            位时序：预分频=1，TSEG1=31，TSEG2=8，同步跳转=8
            帧格式：经典CAN格式
            标准接收过滤器：1个，掩码过滤（ID=0x4AA，掩码=0x7FF），存储在RX FIFO0
            扩展接收过滤器：禁用
            接收FIFO：启用FIFO0，大小为3，FIFO1禁用
            接收缓冲：禁用
            发送缓冲：禁用
            发送FIFO：大小为2
            发送事件FIFO：禁用
            时间戳：内部时间戳，16分频
            中断：
                RX FIFO0新消息中断，中断线1
                TX BUFFER0/1发送完成中断，中断线1
            其他：拒绝远程帧与非匹配帧
        5、节点2:
            FDCAN:
                N32H482/487/488系列：FDCAN2，TX-PH2，RX-PH3
                N32H473/474/475系列：FDCAN2，TX-PB0，RX-PB1
            时钟源：20MHz
            波特率：500Kbps
            工作模式：普通模式
            位时序：预分频=1，TSEG1=31，TSEG2=8，同步跳转=8
            帧格式：经典CAN格式
            标准接收过滤器：1个，掩码过滤（ID=0x555，掩码=0x7FF），存储在RX FIFO0
            扩展接收过滤器：禁用
            接收FIFO：启用FIFO1，大小为3，FIFO0禁用
            接收缓冲：禁用
            发送缓冲：禁用
            发送FIFO：大小为2
            发送事件FIFO：禁用
            时间戳：内部时间戳，16分频
            中断：
                RX FIFO1新消息中断，中断线1
                TX BUFFER0/1发送完成中断，中断线1
            其他：拒绝远程帧与非匹配帧
        6、GPIO：
            N32H473/474系列: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488系列:  PA3-D13,PA8-D15,PA0-KEY
            N32H475系列:     PA3-D1, PA7-D2, PA2-KEY

    使用方法：
        1、编译后将程序下载到开发板并复位运行。
        2、轻按按键，节点1与节点2分别发送一个数据帧:
            - 节点1:标准帧，ID=0x555，通过TX FIFO发送
            - 节点2:标准帧，ID=0x4AA，通过TX FIFO发送
        3、节点1、2检测到FIFO新消息中断后，接收数据帧并打印接收数据。
        4、再次轻按按键，节点1与节点2再次分别发送一个数据帧
        5、节点1、2轮询接收FIFO，接收数据帧并打印接收数据。
        6、开始发送、发送完成、以及接收到数据帧时，分别打印相关信息
        7、继续轻按按键，从第2步开始循环演示
        
4、注意事项
    N32H473/474/475系列必须外接两个CAN PHY芯片。
    
1. Function description

     This example shows FDCAN node1 and node2 sending and receiving data to each other in classic CAN format.

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

     System Configuration:
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
            Baudrate: 500Kbps
            Working mode: normal mode
            Bit timing: prescaler=1, TSEG1=31, TSEG2=8, sync jump=8
            Frame format: classic CAN
            Standard filter: 1, ID mask filtering (ID=0x4AA,mask=0x7FF), stored in RX BUFFER0
            Extended filter: disabled
            Receive FIFO: FIFO0 enabled, size 3, FIFO1 disabled
            Receive buffer: disabled
            Send buffer: disabled
            Send FIFO: size 2
            Send event FIFO: disabled
            Timestamp: internal timestamp, divided by 16
            Interrupt:
                RX FIFO0 new message interrupt, interrupt line 1
                TX BUFFER0/1 transmission completion interrupt, interrupt line 1
            Others: Reject remote frames and non-matching frames
        5. node2:
            FDCAN:
                N32H482/487/488 Serie: FDCAN2，TX-PH2，RX-PH3
                N32H473/474/475 Serie: FDCAN2，TX-PB0，RX-PB1
            Clock source: 20MHz
            Baudrate: 500Kbps
            Working mode: normal mode
            Bit timing: prescaler=1, TSEG1=31, TSEG2=8, sync jump=8
            Frame format: classic CAN
            Standard filter: 1, ID mask filtering (ID=0x555,mask=0x7FF), stored in RX BUFFER1
            Extended filter: disabled
            Receive FIFO: FIFO1 enabled, size 3, FIFO0 disabled
            Receive buffer: disabled
            Send buffer: disabled
            Send FIFO: size 2
            Send event FIFO: disabled
            Timestamp: internal timestamp, divided by 16
            Interrupt:
                RX FIFO1 new message interrupt, interrupt line 1
                TX BUFFER0/1 transmission completion interrupt, interrupt line 1
            Others: Reject remote frames and non-matching frames
        6. GPIO:
            N32H473/474 series: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488 series:  PA3-D13,PA8-D15,PA0-KEY
            N32H475 series:     PA3-D1, PA7-D2, PA2-KEY

     Instructions:
        1. After compiling, download the program and reset, the program start running.
        2. Press the button, node1 and node2 send a data frame respectively:
             - node1: Standard frame, ID=0x555, sent through TX FIFO
             - node2: Standard frame, ID=0x4AA, sent through TX FIFO
        3. After nodes 1 and 2 detect the interrup of the new message in FIFO, 
            they receive the data frame and print the received data.
        4. Press the button again, and node 1 and node 2 send a data frame again respectively.
        4. Nodes 1 and 2 poll the receiving FIFO, receive the data frame and print the received data.
        5. Print the relevant information when starting to send, sending is completed, and receiving the data frame.
        6. Continue to press the button and start the loop demonstration from step 2.

4. Attention
    Two external CAN PHY chips must be connected in N32H473/474/475 series testing.

