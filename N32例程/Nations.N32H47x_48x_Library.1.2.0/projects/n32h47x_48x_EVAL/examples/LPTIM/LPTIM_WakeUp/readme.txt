1、功能说明
    LPTIM1周期性产生EXTI Line 25中断，将芯片从低功耗模式（STOP模式）唤醒

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
        2、时钟频率：
            N32H473/474系列：240MHz
            N32H475/482/487/488系列：240MHz
        3、LPTIM CLK：LSI
        4、打印：PA9 - baud rate 115200

    LED配置：
        LED1：PA3
        LED2：PB4

    LPTIM配置：
        LPTIM1连续计数模式

    使用方法：
        1、编译后下载程序运行。
        2、可观察到LED1、LED2周期性亮灭，指示系统正常进入低功耗模式，且由LPTIM1周期性唤醒并退出低功耗模式。

4、注意事项
    无


1. Function description
    LPTIM1 periodically generates an EXTI Line 25 interrupt to wake up the chip from low-power mode (STOP mode)

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
        Developed based on the full-featured board N32H487ZEL7_EVB V1.1

3. Instructions for use
    
    System configuration:
        1, Clock source: HSI + PLL
        2, Clock frequency: 
            N32H473/474 series: 240MHz
            N32H475/482/487/488 series: 240MHz
        3, LPTIM CLK: LSI
        4, Printing: PA9 - baud rate 115200

    LED Configuration:
        LED1: PA3
        LED2: PB4

    LPTIM Configuration:
        LPTIM1 Continuous Count Mode

    Usage:
        1, Compile and download the program to run.
        2, LED1 and LED2 can be observed to light up and go out periodically, indicating that the system enters the low-power mode normally, and is periodically awakened by LPTIM1 and exits the low-power mode.

4. Attention
    None

