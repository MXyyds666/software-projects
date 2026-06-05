1、功能说明
    此例程展示了通过SDIO主机接口与TF卡之间数据读写功能。
             
2、使用环境

    软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34
                IAR EWARM 8.50.1
    
    硬件开发环境：
        N32H482系列：基于评估板N32H482ZEL7_EVB V1.1开发
        N32H487系列：基于评估板N32H487ZEL7_EVB V1.1开发
        
3、使用说明
    
    系统配置：
    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H482/487/488系列：240MHz
    3、DMA通道：DMA1_CH1
    4、SDIO 配置：
        端口: PC8  - SDIO_DAT0
              PC9  - SDIO_DAT1
              PC10 - SDIO_DAT2
              PC11 - SDIO_DAT3
              PB2  - SDIO_CLK 
              PD2  - SDIO_CMD 
        时钟源：120MHz（SDIOCLK = HCLK/2）
        分频系数：298（初始化）、4（数据传输），SDIO_CLK = SDIOCLK/(2 + 分频系数)
        SDIO_CLK上升沿有效，禁用硬件流控，总线位宽4bit
    
    5、打印：PA9 - baud rate 115200
    6、GPIO：PA0-KEY

    使用方法：
        1、测试前请先安装好TF卡。
        2、编译后将程序下载到开发板并复位运行。
        3、轻按按键,对TF卡进行一次读写擦测试。
        4、通过串口输出相关提示信息。

4、注意事项
    必须外接TF卡座。

1. Function description
    This example shows the data read and write function between SDIO host interface and TF card.

2. Use environment

    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    
    Hardware development environment:
        N32H482 series: Developed based on the board N32H482ZEL7_EVB V1.1
        N32H487 series: Developed based on the board N32H487ZEL7_EVB V1.1

3. Instructions for use   

    System Configuration:
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H482/487/488 series: 240MHz
        3. DMA channel: DMA1_CH1
        4. SDIO Configuration:
            Ports:  PC8  - SDIO_DAT0
                    PC9  - SDIO_DAT1
                    PC10 - SDIO_DAT2
                    PC11 - SDIO_DAT3
                    PB2  - SDIO_CLK 
                    PD2  - SDIO_CMD 
            clock source：120MHz (SDIOCLK = HCLK/2)
            clock divider factor：298(initialization) and 4(data transfer), SDIO_CLK = SDIOCLK/(2 + factor)
            SDIO_CLK rising edge, hardware flow control disabled, 4bits bus
        5. printf: PA9 - baud rate 115200
        6. GPIO: PA0-KEY

    Instructions:
        1. Please install the TF card before the test.
        2. After compiling, download the program and reset.
        3. Press the button, the read/erase/write test for TF card start.
        4. The serial port will output relevant prompt information.
        
4. Attention
    An extenel TF card slot must be connected.