1、功能说明

    此示例展示IIR DMA计算方式

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
        基于评估板N32H487ZEL7_STB V1.0开发


3、使用说明
    系统配置：
    1、时钟源：PLL
    2、系统时钟频率：
        N32H473/474系列：
        240MHz
        N32H475/482/487/488系列：
        240MHz

    使用方法：
        采样频率为16KHz，采用IIR滤波器阶数为2，截止频率为500Hz。有用信号的幅值为25000，
        频率为500Hz，噪声信号幅值为5000，频率为2500Hz。可以通过比较reference_data和calculated_data之间的差异来验证过滤效果。
        
4、注意事项
    无
    
    
1. Function description
    This example shows how IIR is calculated by DMA
    

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
        Developed based on the evaluation board N32H487ZEL7_STB V1.0


3. Instructions for use
    1. clock source: PLL
    2. System Clock frequency: 
        N32H473/474 series:
        240MHz
        N32H475/482/487/488 series:
        240MHz

    Instructions:
    The sampling frequency is 16KHz,the IIR filter order is 2,cut-off frequency is 500Hz. 
    The amplitude of useful signal is 25000 and the frequency is 500Hz, and the amplitude of noise signal is 5000 and the frequency is 2500Hz.
    The filtering effect can be verified by comparing the difference between the reference_data and the calculated_data.

4. Attention
     None