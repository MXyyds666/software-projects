1、功能说明

    1、此例程展示了I2C模块作主设备通过FIFO的读写操作。   

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
    
     ：
    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H475/482/487/488系列：
        240MHz
	3、打印：PA9 - baud rate 115200
    4、I2Cx 配置：
		时钟：100KHz
		地址：0x10（7bit）
		引脚：
			N32H475系列：
			SCL--PD11、SDA--PD1
			N32H473/474/482/487/488系列：
			SCL--PC0、SDA--PB5           

    5、测试步骤与现象
        1、连接I2C从设备
        2、编译下载代码复位运行
        3、从串口看打印信息，验证结果

4、注意事项
无

1. Function description

     1. This routine shows the I2C module as the master device's read and write operations through FIFO.

2. Use environment

         Software development environment: KEIL MDK-ARM V5.34

    Hardware development environment:
        N32H473 series:
        Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series:
        Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series:
        Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_STB V1.0



3. Instructions for use
    
     System Configuration:
     1. Clock source: HSI+PLL
     2. System clock frequency:
         N32H473/474 series:
         240MHz
         N32H475/482/487/488 series:
         240MHz
     3. I2Cx configuration:
             SCL --> PB8 	 
             SDA --> PB9 	 
             ADDR: 0x30(7bit)
             CLOCK: 100KHz
            
     4. printf: PA9 - baud rate 115200

      Instructions:
         1. connect the I2C slave device
         2. compile and download the code, reset and run
         3. view the print information from the serial port and verify the result

4. Attention
N32H475： configuration：
            SCL   -->  PD11     
            SDA   -->  PD1 