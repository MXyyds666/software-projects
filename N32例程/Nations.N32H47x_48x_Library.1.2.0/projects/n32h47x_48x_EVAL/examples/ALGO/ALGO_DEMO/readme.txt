1、功能说明
    1、加密算法库API使用示例
    2、demo展示了SM4、AES128/192/256、DES/2KEY3DES/3KEY3DES 、HASH、SM3、MD5、随机数的算法调用流程

2、使用环境

  软件开发环境：KEIL MDK-ARM V5.36.0.0
    
    硬件环境：
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：
        基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_STB V1.0开发

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
        1、在KEIL下编译后烧录到开发板，通电
        2、通过串口输出运行信息
        3、算法执行成功，打印出相应算法测试success，并返回0，算法执行失败，打印相应算法测试fail，并返回0x5A5A5A5A
               
4、注意事项
    无

1. Function description

     1. Encryption algorithm library API usage example
     2. This demo demonstrates the algorithm calling process for SM4, AES128/192/256 DES/2KEY3DES/3KEY3DES, HASH, SM3, MD5, and random

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
     3. printf: PA9 - baud rate 115200

     Instructions:
         1. Compile under KEIL and burn to the development board, then power on
         2. Output running information through serial port
         3. Algorithm execution successful, print out the corresponding algorithm test success and return 0, algorithm execution failed, print the corresponding algorithm test 
fail and return  0x5A5A5A5A

4. Attention
     None