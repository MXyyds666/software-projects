1、功能说明
    以FDCAN node为例演示FDCAN 外部回环模式基本功能。

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
        4、node:
            FDCAN:
                N32H482/487/488系列：FDCAN1，TX-PG6
                N32H473/474系列：FDCAN1，TX-PB13
                N32H475系列：FDCAN3，TX-PB13
            时钟源：20MHz
            波特率：500Kbps（标称），1Mbps（数据）
            工作模式：外部回环模式
            标称位时序：预分频=1，TSEG1=31，TSEG2=8，同步跳转=8
            数据位时序：预分频=1，TSEG1=15，TSEG2=4，同步跳转=4
            帧格式：FDCAN，BRS ON，20字节数据
            标准接收过滤器：1个，双ID过滤（0x111，0x555），存储在RX BUFFER0
            扩展接收过滤器：1个，范围过滤（0x1111111~0x2222222），存储在RX FIFO1
            接收FIFO：启用FIFO1，大小为3，FIFO0禁用
            接收缓冲：1个
            发送缓冲：1个
            发送FIFO：大小为2
            发送事件FIFO：大小为9
            时间戳：内部时间戳，16分频
            其他：拒绝远程帧与非匹配帧
        5、GPIO：
            N32H473/474系列: PA3-D1, PA8-D2, PB4-D3, PA2-KEY
            N32482/487/488系列:  PA3-D13,PB4-D14,PA8-D15,PA0-KEY
            N32H475系列:     PA3-D1, PA7-D2, PB4-D3, PA2-KEY

    使用方法：
        1、编译后将程序下载到开发板并复位运行。
        2、每次轻按按键，FDCAN发送一个数据帧。3个数据帧依次轮流发送:
            - 标准帧（ID=0x111，通过TX BUFFER0发送）
            - 扩展帧1（ID=0x1111111，通过TX FIFO发送）
            - 扩展帧2（ID=0x2222222，通过TX FIFO发送）
        3、轮询RX BUFFER0，收到数据帧后打印接收数据内容。
        4、轮询RX FIFO1，收到数据帧后打印接收数据内容。
        5、轮询TX EVENT FIFO，打印发送事件信息。

4、注意事项
    无
    
1. Function description

     Use FDCAN node as an example to shows the basic functions of FDCAN external loopback mode.

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
                N32H482/487/488 Serie: FDCAN1, TX-PA1, RX-PG6
                N32H473/474 Serie: FDCAN1, TX-PB13
                N32H475 Serie: FDCAN3, TX-PB13
            Clock source: 20MHz
            Baudrate: 500Kbps(nominal), 1Mbps(data)
            Working mode: external loopback mode
            Nominal bit timing: prescaler=1, TSEG1=31, TSEG2=8, sync jump=8
            Data bit timing: prescaler=1, TSEG1=15, TSEG2=4, synchronous jump=4
            Frame format: FDCAN, BRS ON, 20 bytes data
            Standard filter: 1, double ID filtering (0x111, 0x555), stored in RX BUFFER0
            Extended filter: 1, range filter (0x1111111~0x2222222), stored in RX FIFO1
            Receive FIFO: FIFO1 enabled, size 3, FIFO0 disabled
            Receive buffer: 1
            Send buffer: 1
            Send FIFO: size 2
            Send event FIFO: size 9
            Timestamp: internal timestamp, divided by 16
            Others: Reject remote frames and non-matching frames
        5. GPIO:
            N32H473/474 series: PA3-D1, PA8-D2, PB4-D3, PA2-KEY
            N32482/487/488 series:  PA3-D13,PB4-D14,PA8-D15,PA0-KEY
            N32H475 series:     PA3-D1, PA7-D2, PB4-D3, PA2-KEY
             
     Instructions:
        1. After compiling, download the program and reset, the program start running.
        2. Each time the button is pressed, FDCAN sends a data frame.
           and 3 data frames are sent in turn:
                - Standard frame (ID=0x111, sent through TX BUFFER0)
                - Extended frame 1 (ID=0x1111111, sent through TX FIFO)
                - Extended frame 2 (ID=0x2222222, sent through TX FIFO)
        3. Poll RX BUFFER0, print the data after frame received.
        4. Poll RX FIFO1, print the data after frame received.
        5. Polling TX EVENT FIFO，print the information of transmit event.


4. Attention
    None

