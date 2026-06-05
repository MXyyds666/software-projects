1、功能说明

    /* 简单描述工程功能 */
    这个例程配置并演示设置不同的系统时钟，并用MCO从PA7输出出来


2、使用环境

    /* 软件开发环境：当前工程使用的软件工具名称及版本号 */
    IDE工具：KEIL MDK-ARM 5.34
                        IAR EWARM 8.50.1
      
    /* 硬件环境：工程对应的开发硬件平台 */
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
    USART：TX - PA9，波特率115200
    GPIO：PA7 - 复用为MCO时钟输出

    /* 描述Demo的测试步骤和现象 */
    1.编译后下载程序复位运行；
    2.分别配置系统时钟为HSI、HSE、PLL、SHRTPLL、USB240M，分别使用串口打印出当前SYSCLK、HCLK、PCLK1、PCLK2等信息，并且可以使用PA7复用引脚输出时钟，用示波器查看；


4、注意事项
    

1. Function description

     /* Briefly describe the engineering function */
     This example configures and demonstrates the setting of different system clocks, and uses MCO to output from PA7


2. Use environment

     /* Software development environment: the name and version number of the software tool used in the current project */
     IDE tool: KEIL MDK-ARM 5.34
               IAR EWARM 8.50.1
      
     /* Hardware environment: development hardware platform corresponding to the project */
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

     /* Describe the configuration method of related modules; for example: clock, I/O, etc. */
     USART: TX-PA9, baud rate 115200
     GPIO: PA7 -multiplexed as MCO clock output

     /* Describe the test steps and phenomena of Demo */
     1. After compiling, download the program to reset and run;
     2. Configure the system clock as HSI, HSE, PLL, SHRTPLL and USB240M respectively, and use the serial port to print out the current SYSCLK, HCLK, PCLK1, PCLK2 and other information, and use the PA7 multiplex pin to output the clock and view it with an oscilloscope;


4. Attention
    None