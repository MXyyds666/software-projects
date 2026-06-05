1、功能说明

    1、XSPI Quad SPI 模式擦除、读、写外部Spi_Flash设备

2、使用环境

    软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34
                        IAR EWARM 8.50.1
    
    硬件开发环境：
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于全功能板N32H487ZEL7_EVB V1.1开发

3、使用说明
    
    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H482/487/488系列：
        240MHz
    3、打印：PA9 - baud rate 115200
    4、XSPI配置：
          时钟：系统时钟6分频
          引脚：NSS0--PD3(CS)、CLK--PA5(SCLK)、IO0--PA6(SI)、IO1--PA7(SO)、IO2--PC4(WP)、IO3--PC5(HOLD)
          外部Spi_Flash设备：W25Q128或P25Q40


    5、测试步骤与现象
          a.编译后下载程序复位运行；
          b.XSPI 初始化完成后，先往Spi_Flash设备写256字节，然后读取确认数据是否为写入的内容，有错误则打印报错，
             再全片擦除Spi_Flash设备，然后读取确认数据是否为0xFF，有错误则打印报错。

4、注意事项
    需连接全功能板的跳线帽J1\J31



1. Function description

     1. XSPI Quad SPI mode erase, read, write external Spi_Flash device

2. Use environment

     Software development environment:
         IDE tool: KEIL MDK-ARM 5.34
	           IAR EWARM 8.50.1
    
     Hardware development environment:
        N32H473 series:
        Based on evaluation board N32H473VEL7_STB V1.0 development
        N32H474 series:
        Based on evaluation board N32H474VEL7_STB V1.0 development
        N32H482 Series:
        Based on evaluation board N32H482ZEL7_STB V1.0 development
        N32H487 Series:
        Based on full function board N32H487ZEL7_EVB V1.1 development

3. Instructions for use
    
    1、clock source: HSI + PLL
    2、System clock frequency:
        N32H473/474 series:
        240MHz
        N32H482/487/488 series:
        240MHz
    3、Printing: PA9 - baud rate 115200
    4、XSPI Configuration:
          Clock: System clock 6 divisions;
          Pin: NSS0--PD3(CS), CLK--PA5(SCLK), IO0--PA6(SI), IO1--PA7(SO), IO2--PC4(WP), IO3--PC5(HOLD)
          External Spi_Flash device: W25Q128 or P25Q40


    5、Test steps and phenomena
          a. Compile and download the program to reset and run;
          b. After the initialization of XSPI is completed, first write 256 bytes to the Spi_Flash device, then read to confirm whether the data is what is written, and if there is an error, print an error report.
             Then erase the Spi_Flash device, and then read to confirm whether the data is 0xFF, and if there is an error, print the error report.

4. Attention
     Jumper caps J1\J31 required to connect full function boards
	 