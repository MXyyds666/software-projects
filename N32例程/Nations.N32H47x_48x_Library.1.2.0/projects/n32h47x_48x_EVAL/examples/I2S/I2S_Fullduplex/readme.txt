1、功能说明

   1、I2S全双工模式下收发数据演示


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
	N32H475 系列：
	   I2S2:            I2S3:
                WS  -- PC6 <--> WS  -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD1 -- PC9 <--> SD1 -- PC1
				SD2 -- PB14 <--> SD2 -- PB4
	N32H473/474/482/487/488系列：
	  I2S2:       I2S3:
                WS  -- PC6 <--> WS  -- PA4
                SCK -- PC7 <--> SCK -- PC3
                SD1 -- PC9 <--> SD1 -- PC1
                SD2 -- PC2 <--> SD2 -- PB4
	 
    4、测试步骤与现象
    	1、编译后下载程序复位运行；
    	1、I2S主机和I2S从机在全双工模式下相互收发数据 主机发送，从机接收正常，TransferStatus1 状态为 PASSED；从机发送，主机接收正常，TransferStatus2 状态为 PASSED；

4、注意事项
无

1. Function description

   1、Demonstration of sending and receiving data in I2S full duplex mode


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
	N32H475 Series:
	   I2S2: I2S3.
                WS  -- PC6 <--> WS  -- PC2
                SCK -- PC7 <--> SCK -- PC3
                SD1 -- PC9 <--> SD1 -- PC1
				SD2 -- PB14 <--> SD2 -- PB4

	N32H473/474/482/487/488 series:
	  I2S2: I2S3.
                WS  -- PC6 <--> WS  -- PA4
                SCK -- PC7 <--> SCK -- PC3
                SD1 -- PC9 <--> SD1 -- PC1
                SD2 -- PC2 <--> SD2 -- PB4
	 
    4、Test steps and phenomena
    	1. Compile and download the program to reset and run;
    	1, I2S host and I2S slave send and receive data to and from each other in full-duplex mode The host sends and the slave receives normally, and the status of TransferStatus1 is PASSED; the slave sends and the host receives normally, and the status of TransferStatus2 is PASSED;

4. Attention
No