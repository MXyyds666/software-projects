1、功能说明
    1、PVD配置电压产生对应的中断。


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
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
    
    系统配置：
     1、时钟源：HSI+PLL
     2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H475/482/487/488系列：
        240MHz
	 3、打印：PA9 - baud rate 115200
        
    使用方法：
        在KEIL下编译后烧录到评估板，把电压调到PVD设置电压附近，此时程序会进PVD_IRQHandler()中断，并打印相关信息。


4、注意事项
    


1. Function description
    1. The PVD configuration voltage generates a corresponding interrupt.


2. Use environment

     Software development environment: KEIL MDK-ARM V5.34
			              IAR EWARM 8.50.1

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
        Developed based on the full function board N32H487ZEL7_EVB V1.1
        

3. Instructions for use
    
     System Configuration:
     1. Clock source: HSI+PLL
     2. System Clock frequency: 
        N32H473/474 series:
        240MHz
        N32H475/482/487/488 series:
        240MHz
	 3. printf: PA9 - baud rate 115200

     Instructions:
         After compiling under KEIL and burning to the evaluation board, adjust the voltage to near the PVD setting voltage, at this time the program will go into the PVD_IRQHandler() interrupt and print the relevant information..

4. Attention
    

