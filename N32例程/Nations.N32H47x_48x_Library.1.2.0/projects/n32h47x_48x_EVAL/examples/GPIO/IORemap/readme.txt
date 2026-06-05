1、功能说明
    
    此例程展示了JTAG接口的Remap功能。

2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
                 IAR EWARM 8.50.1

    硬件开发环境：
        N32H473系列：基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：基于评估板N32H482ZEL7_EVB V1.1开发
        N32H487系列：基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明

    系统配置
        1、时钟源：HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：    240MHz
            N32H475/482/487/488系列：240MHz
        3、GPIO：
            N32H473/474系列: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488系列:  PA3-D13,PA8-D15,PA0-KEY
            N32H475系列:     PA3-D1, PA7-D2, PA2-KEY

        4、打印：PA9 - baud rate 115200
        5、JTAG IO
            PB4   PA15    PA14   PA13
            
    使用方法：
        1、编译后将程序下载到开发板并复位运行。
        2、默认状态为完全JTAG接口，PB4/PA15/PA14/PA13无方波输出。
        3、轻按按键，JTAG状态切换为无NJTRST的完全JTAG状态，PB4被释放，输出方波。
        4、再次轻按按键，JTAG状态切换为SWD状态，PB4/PA15均被释放，输出方波。
        5、再次轻按按键，JTAG接口禁用，PB4/PA15/PA14/PA13全部释放，输出方波。
        6、系统启动或JTAG状态发生变化时，串口都有相关提示信息输出。

4、注意事项
    无


1. Function description

     This example shows the rempa function of JTAG port.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H473 series: Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series: Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series: Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series: Developed based on the evaluation board N32H482ZEL7_EVB V1.1
        N32H487 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.1


3. Instructions for use

     System Configuration:
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:     240MHz
            N32H475/482/487/488 series: 240MHz
        3. GPIO:
            N32H473/474 series: PA3-D1, PA8-D2, PA2-KEY
            N32482/487/488 series:  PA3-D13,PA8-D15,PA0-KEY
            N32H475 series:     PA3-D1, PA7-D2, PA2-KEY

        4. printf: PA9 - baud rate 115200
        5. JTAG port:PB4   PA15    PA14   PA13

     /* Describe the test steps and phenomena of Demo */
     Instructions:
        1.  After compiling, download the program and reset, the program start running.
        2.  Default state is the complete JTAG interface, and PB4/PA15/PA14/PA13 
            have no square wave output.
        3.  Press the button, JTAG state switches to the complete JTAG state without NJTRST, 
            PB4 is released, and a square wave is output.
        4.  Press the button again, JTAG state switches to the SWD, 
            PB4/PA15 are released, and a square wave is output.
        5.  Press the button again, JTAG interface is disabled,  
            PB4/PA15/PA14/PA13 are all released, and a square wave is output.
        6.  When the system starts and the JTAG port changed, the serial port will 
            output relevant prompt information.

4. Attention
    None

