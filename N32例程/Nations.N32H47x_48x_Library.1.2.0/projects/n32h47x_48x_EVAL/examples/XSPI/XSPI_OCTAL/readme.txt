1、功能说明

    1、XSPI Octal SPI 模式擦除、读、写外部Spi_Flash设备

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
        基于评估板N32H487ZEL7_STB V1.0开发

3、使用说明
    
    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H482/487/488系列：
        240MHz
    3、打印：PA9 - baud rate 115200
    4、XSPI配置：
          时钟：系统时钟6分频；
          引脚：NSS0--PD3、CLK--PF1、IO0--PF2、IO1--PF3、IO2--PF4、IO3--PF5、IO4--PD4、IO5--PD5、IO6--PD6、IO7--PD7、RXDS--PD13
          外部Spi_Flash设备：IS25WX256


    5、测试步骤与现象
          a.编译后下载程序复位运行；
          b.XSPI 初始化完成后，先擦除Spi_Flash设备32KB，然后读取确认数据是否为0xFF，有错误则打印报错，
             再往Spi_Flash设备写256字节，然后读取确认数据是否为写入的内容，有错误则打印报错。

4、注意事项
    无



1. Function description

     1. XSPI Octal SPI mode erase, read, write external Spi_Flash device

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
        Based on evaluation board N32H487ZEL7_STB V1.0 development

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
          Pin: NSS0 - PD3, CLK - PF1, IO0 - PF2, IO1 - PF3, IO2 - PF4, IO3 - PF5, IO4--PD4, IO5--PD5, IO6--PD6, IO7--PD7, RXDS--PD13
          External Spi_Flash device: IS25WX256


    5、Test steps and phenomena
          a. Compile and download the program to reset and run;
          b. After the initialization of XSPI is completed, first erase the Spi_Flash device 32KB, and then read to confirm whether the data is 0xFF, and if there is an error, print the error report.
             Then write 256 bytes to the Spi_Flash device, then read to confirm whether the data is what is written, and if there is an error, print an error report.

4. Attention
     none
	 