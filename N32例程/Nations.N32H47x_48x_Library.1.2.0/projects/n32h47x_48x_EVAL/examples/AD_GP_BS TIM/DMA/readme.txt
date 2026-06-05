1、功能说明
    1、ATIM1 CH3 CH3N互补信号每6个周期改变一次占空比
2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
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
    系统配置:
        1、时钟源：HSI+PLL
        2、时钟频率：
            N32H473/474系列：
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
               N32H475/482/487/488系列：
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
        3、端口配置：
                PC2选择为ATIM1 CH3输出
                PD9选择为ATIM1 CH3N输出
        4、TIM：
                ATIM1 CH3 CH3N互补输出，每6个周期触发一次DMA传输
        5、DMA：
                DMA1_CH5通道循环环模式搬运3个字SRC_Buffer[3]变量到ATIM1 CCDAT3寄存器
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察ATIM1 CH3 CH3N的波形
        2、ATIM1的6个周期改变一次CH3 CH3N的占空比，循环改变
4、注意事项
    无
    

1. Function description
    1. ATIM1 CH3 CH3N complementary signal changes duty cycle every 6 cycles
2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
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
    System Configuration;
        1. Clock source: HSI+PLL
        2. Clock frequency: 
            N32H473/474 series:
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
            N32H475/482/487/488 series:
                HSI=8M,SYS_CLK=240M,PCLK2=120M,ATIM1_CLK=240M
        3. Port configuration:
                PC2 selected as ATIM1 CH3 Output
                PD9 selected as ATIM1 CH3N Output
        4. TIM:
                ATIM1 CH3 CH3N complementary output triggers DMA transmission every 6 cycles
        5. DMA:
                DMA1_ CH5 Channel circular mode handling 3 word SRC_ Buffer[3] variable to ATIM1 CCDAT3 register
    Instructions:
         1. After compiling, turn on the debug mode, use an oscilloscope or logic analyzer to observe the waveform of ATIM1 CH3 CH3N
         2. Change the duty cycle of CH3 and CH3N once in 6 cycles of ATIM1, and change cyclically
4. Attention
    without

