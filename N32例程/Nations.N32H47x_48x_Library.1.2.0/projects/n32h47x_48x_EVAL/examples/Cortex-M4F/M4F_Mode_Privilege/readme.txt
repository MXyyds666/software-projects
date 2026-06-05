1、功能说明

    /* 简单描述工程功能 */
        该例程配置并演示了如何修改线程模式特权访问和堆栈。


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
        基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明

    /* 描述相关模块配置方法；例如:时钟，I/O等 */
    N32H475/N32H482/N32H487:
        SystemClock：240MHz
        USART：TX - PA9，波特率115200
    N32H474/N32H473:
        SystemClock：240MHz
        USART：TX - PA9，波特率115200

    /* 描述Demo的测试步骤和现象 */
        1. 重新设置并在编译后运行下载的程序；
        2. 使用编译器的 Cortex 寄存器窗口查看线程模式下的堆栈和代码访问级别（特权/非特权）。


4、注意事项


1. Function description
    /* A brief description of the engineering function */
    This routine configudes and demonstrates how to modify thread mode privileged access and stack

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
    2. Use the Compiler's Cortex Register window to view the stack and code access level (privileged/non-privileged) in thread mode.

4. Attention
    None