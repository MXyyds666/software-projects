1、功能说明

    1、I2S 通过中断收发数据

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
	N32H475/N32H473/474/482/487/488 系列：  
		 I2S2:            I2S3:
                WS  -- PC6 <--> WS  -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD  -- PC9 <--> SD  -- PC1

    4、中断：I2S2 中断入口函数SPI2_I2S2_IRQHandler，I2S3 中断入口函数SPI3_I2S3_IRQHandler

     5、测试步骤与现象
    	1、编译后下载程序复位运行；
    	2、I2S 初始化成 16 位数据格式，48K 采样频率，I2S3 进入 SPI3_I2S3_IRQHandler函数发送数据，I2S2 进入 SPI2_I2S2_IRQHandler 函数接收数据，运行完成后，查看 TransferStatus1 状态为 PASSED；
    	3、I2S 初始化成 24 位数据格式，16K 采样频率，I2S3 进入 SPI3_I2S3_IRQHandler函数发送数据，I2S2 进入 SPI2_I2S2_IRQHandler 函数接收数据，运行完成后，查看 TransferStatus2 状态为 PASSED；

4、注意事项
无


1. Function description

    1、I2S Send and receive data through interrupt

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
	N32H475/N32H473/474/482/487/488 series: :  
		 I2S2: I2S3.
                WS -- PC6 <--> WS -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD -- PC9 <--> SD -- PC1
				
    4, interrupt: I2S2 interrupt entry function SPI2_I2S2_IRQHandler, I2S3 interrupt entry function SPI3_I2S3_IRQHandler

     5、Test steps and phenomena
    	1. Compile and download the program to reset and run;
    	2, I2S initialized to 16-bit data format, 48K sampling frequency, I2S3 into the SPI3_I2S3_IRQHandler function to send data, I2S2 into the SPI2_I2S2_IRQHandler function to receive data, after the completion of the operation, check TransferStatus1 status is PASSED;
    	3、I2S initialized to 24-bit data format, 16K sampling frequency, I2S3 into the SPI3_I2S3_IRQHandler function to send data, I2S2 into the SPI2_I2S2_IRQHandler function to receive the data, after the completion of the operation, view TransferStatus2 status is PASSED;

4. Attention
No



