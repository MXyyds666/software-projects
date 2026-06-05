1、功能说明
    以EXTI0为例演示GPIO外部中断功能，分别将PA4/PA5/PA6做为EXTI0输入端口。

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
            N32H473/474系列: PA3-D1, PA8-D2, PB4-D3, PA4-KEY1, PA5-KEY2, PA6-KEY3
            N32482/487/488系列:  PA3-D13,PB4-D14,PA8-D15,PC13-KEY1,PA15-KEY2,PC8-KEY3
            N32H475系列:     PA3-D1, PA7-D2, PB4-D3, PA4-KEY1, PA5-KEY2, PA6-KEY3

        4、打印：PA9 - baud rate 115200

    使用方法：
        1、编译后将程序下载到开发板并复位运行,仅1个LED闪烁。
        2、默认状态下仅KEY1触发EXTI0外部中断，在中断处理中修改EXTI0输入端口为KEY2，之后仅KEY2可触发EXTI0中断。
        3、每次进入EXTI中断，EXTI0输入端口修改一次，按照顺序循环演示：KEY1->KEY2->KEY3->KEY1->...
        4、EXTI0输入端口不同时，闪烁的LED也不同。
        5、系统启动和EXTI0输入端口发生变化时，串口都有相关提示信息输出。
        

4、注意事项
    无
    
1. Function description

     Uses EXTI0 as an example to shows the GPIO external interrupt function,
     and PA4/PA5/PA6 are all partly used as the EXTI0 input port.

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
        3.GPIO:
            N32H473/474 Serie: PA3-D1, PA8-D2, PB4-D3, PA4-KEY1, PA5-KEY2, PA6-KEY3
            N32H482/487/488 Serie: PA3-D13,PB4-D14,PA8-D15,PC13-KEY1,PA15-KEY2,PC8-KEY3
            N32H475 Serie:     PA3-D1, PA7-D2, PB4-D3, PA4-KEY1, PA5-KEY2, PA6-KEY3
            
        4. printf: PA9 - baud rate 115200

     Instructions:
        1.  After compiling, download the program and reset, the program start running.
            And a led blink.
        2.  Only KEY1 triggers the EXTI0 external interrupt by default. 
            During interrupt processing, the EXTI0 input port changed to KEY2. 
            After that, only KEY2 can trigger the EXTI0 interrupt.
        3.  Each time the EXTI interrupt is entered, the EXTI0 input port is changed once, 
            and the demonstration is performed in sequence: KEY1->KEY2->KEY3->KEY1->...
        4.  When the EXTI0 input port is changed, the blinked LED will also changed.
        5.  When the system starts and the EXTI0 input port changes, the serial port will 
            output relevant prompt information.

4. Attention
    None

