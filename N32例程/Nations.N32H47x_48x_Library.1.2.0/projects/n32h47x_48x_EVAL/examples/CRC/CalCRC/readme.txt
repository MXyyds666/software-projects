1、功能说明

    1、此例程展示了硬件CRC的基本功能和算法。
    
    其中有16bit校验与32bit校验。
    
2、使用环境

      软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34
	       IAR EWARM 8.50.1
    
    硬件开发环境：
        N32H473系列：基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：基于评估板N32H482ZEL7_EVB V1.1开发
        N32H487系列：基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明
    
    1、时钟源：HSI+PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H475/482/487/488系列：
        240MHz
   3、打印：PA9 - baud rate 115200

   4、测试步骤与现象：
        1、在KEIL下编译后烧录到开发板，通电
        2、通过串口输出运行信息

4、注意事项
    无
    
    
1. Function description
   1. This example demonstrates the basic functions and algorithms of hardware CRC.
    Among them are 16bit parity and 32bit parity.
    
2. Use environment

    Software development environment:
        IDE tool: KEIL MDK-ARM 5.34
	       IAR EWARM 8.50.1
    
    Hardware development environment:
        N32H473 series: Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series: Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series: Developed based on the evaluation board N32H475UEQ7_STB V1.0
        N32H482 series: Developed based on the evaluation board N32H482ZEL7_EVB V1.1
        N32H487 series: Developed based on the evaluation board N32H487ZEL7_EVB V1.1



3. Instructions for use

     1. Clock source: HSI+PLL
     2. System clock frequency:
         N32H473/474 series:
         240MHz
         N32H475/482/487/488 series:
         240MHz
     3. printf: PA9 - baud rate 115200

     4. test steps and phenomena
         1. Compile under KEIL and burn to the development board, then power on
         2. Output running information through serial port

4. Attention
     None