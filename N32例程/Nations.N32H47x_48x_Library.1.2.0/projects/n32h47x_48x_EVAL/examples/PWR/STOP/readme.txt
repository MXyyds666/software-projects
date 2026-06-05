1、功能说明
    1、STOP模式的进入和退出。


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
    3、唤醒源：
	N32H473/474/475系列：
	    WAKEUP按键-PA2
	N32H482/487/488系列：
	    WAKEUP按键-PA0
    4、打印：PA9 - baud rate 115200
               
    使用方法：
        在KEIL下编译后烧录到评估板，串接电流表，上电后一段时间进入STOP模式，电流明显变小。按下WAKEUP按键，退出STOP模式，电流变大。
        如果开启打印输出，可以看到串口输出“Entry STOP mode”，唤醒后输出“Exit STOP mode”。


4、注意事项
     在评估功耗的时候，要注意去掉打印


1. Function description
    1. Enter and exit STOP mode.

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
     3. Wake-up source: 
	N32H473/474/475 Series:
	    WAKEUP button-PA2
	N32H482/487/488 Series:
	    WAKEUP button-PA0
     4. printf: PA9 - baud rate 115200
               
     Instructions:
        Compiled under KEIL and burned to the evaluation board, connected an ammeter in series, and entered STOP mode some time after powering up, the current became significantly smaller. Press the WAKEUP button to exit the STOP mode, and the current becomes larger.
        If you turn on the printout, you can see that the serial port outputs "Enter STOP mode" and "Exit STOP mode" after waking up.


4. Attention
    When evaluating power consumption, pay attention to removing the printf

