1、功能说明

    1、SPI 读、写、擦除 W25Q128

2、使用环境

    软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34
	       IAR EWARM 8.50.1
    
    硬件开发环境：
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：
        基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于全功能板N32H487ZEL7_EVB V1.1开发

3、使用说明

    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H475/482/487/488系列：
        240MHz
    3、SPI配置：
	N32H475系列：
		SPI6配置：NSS--PA2、SCK--PA1、MISO--PA4、MOSI--PA5
	N32H473/474/482/487/488系列：
           		SPI1配置：NSS--PA4、SCK--PA5、MISO--PA6、MOSI--PA7
    4、USART配置：
          TX--PA9,115200,8bit data,1bit stop

    5、测试步骤与现象
    1、编译后下载程序复位运行；
    2、通过串口工具查看结果。

4、注意事项
更改SPI重新编译时，建议清空之前的编译文件，如已编译过487，改为编译475时，需要先清空之前编译过的内容

1. Function description

    1、SPI Read, Write, Erase W25Q128

2. Use environment

    Software development environment:
        IDE tool: KEIL MDK-ARM 5.34
	       IAR EWARM 8.50.1
    
    Hardware development environment:
        N32H473 series:
        Based on evaluation board N32H473VEL7_STB V1.0 development
        N32H474 series:
        Based on evaluation board N32H474VEL7_STB V1.0 development
        N32H475 Series:
        Based on evaluation board N32H475UEQ7_STB V1.0 development
        N32H482 Series:
        Based on evaluation board N32H482ZEL7_STB V1.0 development
        N32H487 Series:
        Based on the full-featured board N32H487ZEL7_EVB V1.1 development

3. Instructions for use

    1, clock source: HSI + PLL
    2、System clock frequency:
        N32H473/474 series:
        240MHz
        N32H475/482/487/488 series:
        240MHz
    3、SPI configuration:
	N32H475 series:
		SPI6 configuration: NSS--PA2, SCK--PA1, MISO--PA4, MOSI--PA5
	N32H473/474/482/487/488 series:
           		SPI1 configuration: NSS--PA4, SCK--PA5, MISO--PA6, MOSI--PA7
    4、USART configuration:
          TX--PA9,115200,8bit data,1bit stop

    5, test steps and phenomena
    1, compiled and downloaded the program reset run;
    2, through the serial port tool to view the results.

4. Attention
When changing the SPI recompile, it is recommended to clear the previous compiled file, such as compiled 487, to compile 475, you need to first clear the previous compiled content.