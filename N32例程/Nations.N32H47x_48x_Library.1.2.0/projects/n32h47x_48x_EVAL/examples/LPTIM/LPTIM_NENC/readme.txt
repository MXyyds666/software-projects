1、功能说明
    LPTIM1 计数IN1 IN2非正交编码的下降沿个数

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

    端口配置：
        PC0选择为LPTIM1 IN1输入
        PC2选择为LPTIM1 IN2输入
        PB0选择为IO输出
        PB1选择为IO输出

    LPTIM配置：
        LPTIM1非正交编码器模式，利用内部LSI时钟连续计数IN1 IN2下降沿个数

    使用方法：
        1、连接PB0和PC0，PB1和PC2
        2、编译后下载程序运行，PB0 PB1输出20个脉冲周期，若NencCNT等于20，则系统运行正常，并打印“LPTIM_NENC demo success!!!”，否则，打印“LPTIM_NENC demo failed!!!”

4、注意事项
    无


1. Function description
    LPTIM1 Counts the number of falling edges of IN1 IN2 Non-encoder

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

    Port Configuration:
        PC0 selected for LPTIM1 IN1 input
        PC2 selected for LPTIM1 IN2 input
        PB0 selected for IO output
        PB1 selected for IO output

    LPTIM Configuration:
        LPTIM1 Non-encoder mode, utilizing the internal LSI clock to continuously count the number of IN1 IN2 falling edges.

    Usage:
        1, Connect PB0 and PC0, PB1 and PC2.
        2, Compile and download the program to run, PB0 PB1 output 20 pulse cycles, if NencCNT is equal to 20, then the system is running normally, and print "LPTIM_NENC demo success!!!". Otherwise, print "LPTIM_NENC demo failed!!!".

4. Attention
    None

