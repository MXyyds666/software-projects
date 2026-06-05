1、功能说明

    /* 简单描述工程功能 */
        这个例程配置并演示bitband位带操作


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34.0.0
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

    /* 描述相关模块配置方法；例如:时钟，I/O等 */
    N32H475/N32H482/N32H487:
        SystemClock：240MHz
        USART：TX - PA9，波特率115200
    N32H474/N32H473:
        SystemClock：240MHz
        USART：TX - PA9，波特率115200

    /* 描述Demo的测试步骤和现象 */
        1.编译后下载程序复位运行；
        2.对32位数据的不同位进行置1置0操作，再读取写入值与读取值，查看打印信息；


4、注意事项


1. Function description
    /* A brief description of the engineering function */
    This routine configures and demonstrates the bitBand bitband operation
2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
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
        Developed based on the evaluation board N32H487ZEL7_EVB V1.1

3. Instructions for use
    
    /* Describe the related module configuration method; For example: clock, I/O, etc. */
    N32H475/N32H482/N32H487:
        SystemClock: 240MHZ
        USART: TX-PA9, baud rate 115200
    N32H474/N32H473:
        SystemClock: 240MHZ
        USART: TX-PA9, baud rate 115200
    /* Describes the test steps and symptoms of Demo */
    1. Reset and run the downloaded program after compilation;
    2. Set 1 to 0 for different bits of 32-bit data, read the write value and read value, and view the printed information.
4. Attention
    None