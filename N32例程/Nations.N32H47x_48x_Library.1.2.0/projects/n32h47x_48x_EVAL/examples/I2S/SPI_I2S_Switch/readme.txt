1、功能说明

    1、I2S 收发数据完成以后切换成 SPI 收发数据，再切换成 I2S 收发数据

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
    3、GPIO： 
	N32H475/N32H473/474/482/487/488 系列 ：   
	       I2S2:            I2S3:
                WS  -- PC6 <--> WS  -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD  -- PC9 <--> SD  -- PC1

    4、测试步骤与现象
   	1、编译后下载程序复位运行；
   	2、 I2S2 和 I2S3 初始化，I2S3 发送数据，I2S2 接收数据 ，运行完成后，查看 TransferStatus1 状态为 PASSED；
        	SPI2 和 SPI3 初始化，SPI3 发送数据，SPI2 接收数据 ，运行完成后，查看 TransferStatus2 状态为 PASSED；
        	I2S2 和 I2S3 初始化，I2S2 发送数据，I2S3 接收数据 ，运行完成后，查看 TransferStatus3 状态为 PASSED；

4、注意事项
无


1. Function description

    1、Switch to SPI after I2S transmitting/receiving data, then switch to I2S transmitting/receiving data.

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
    3.GPIO: 
	N32H475/N32H473/474/482/487/488 Series :   
	       I2S2:		 I2S3.
                WS -- PC6 <--> WS -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD -- PC9 <--> SD -- PC1

    4、Test steps and phenomena
   	1. Compile and download the program to reset and run;
   	2, I2S2 and I2S3 initialize, I2S3 sends data, I2S2 receives data, after running, check the status of TransferStatus1 is PASSED;
        	SPI2 and SPI3 are initialized, SPI3 sends data, SPI2 receives data, after running, check the status of TransferStatus2 is PASSED;
        	I2S2 and I2S3 initialize, I2S2 sends data, I2S3 receives data, after running, check the status of TransferStatus3 is PASSED;

4. Attention
No
