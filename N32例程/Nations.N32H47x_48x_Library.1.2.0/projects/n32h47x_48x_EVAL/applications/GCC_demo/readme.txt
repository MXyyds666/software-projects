1、功能说明
    此例程展示了GPIO的基本配置方法。

2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
                 IAR EWARM 8.50.1

    硬件开发环境：
        N32H473系列：基于评估板N32H487ZEL7_EVB V1.0开发
        N32H474系列：基于评估板N32H487ZEL7_EVB V1.0开发
        N32H475系列：基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：基于评估板N32H487ZEL7_EVB V1.0开发
        N32H487系列：基于评估板N32H487ZEL7_EVB V1.0开发



3、使用说明

    系统配置
        1、时钟源：HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：    200MHz
            N32H475/482/487系列：240MHz
        3、GPIO：
            N32H473/474/482/487系列: PA3-D14,PA8-D6,PB4-D15
            N32H475系列: PA3-D1,PA7-D2,PB4-D3

    使用方法：
        1、编译后将程序下载到开发板并复位运行。
        2、可观察到2个LED同步闪烁4次后，3个LED依次循环点亮。

4、注意事项
    无
    
1. Function description

    This example shows the basic configuration of GPIO.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H473 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.0
        N32H474 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.0
        N32H475 series: Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.0
        N32H487 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.0


3. Instructions for use

    System Configuration:
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:     200MHz
            N32H475/482/487 series: 240MHz
        3.GPIO:
            N32H473/474/482/487 Serie: PA3-D14,PA8-D6,PB4-D15
            N32H475 Serie: PA3-D1,PA7-D2,PB4-D3

     Instructions:
        1. After compiling, download the program and reset, the program start running.
        2. First two LEDs blink 4 times, then the three LEDs turn on one by one and repeat.

4. Attention
    None

