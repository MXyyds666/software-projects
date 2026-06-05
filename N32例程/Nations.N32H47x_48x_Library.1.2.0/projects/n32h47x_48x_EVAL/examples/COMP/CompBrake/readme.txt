1、功能说明
    1、COMP1的输出刹车ATIM1 的互补信号，COMP OUT变低后恢复ATIM1波形
2、使用环境
    软件开发环境：
        IDE工具:  KEIL MDK-ARM V5.34.0.0
                  IAR EWARM 8.50.1

    硬件环境：      
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：
        基于评估板N32H475UEQ7_STB V1.0开发
3、使用说明
    系统配置；
        1、时钟源： HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：
            240MHz
            N32H475系列：
            240MHz 
        3、端口配置：
            PA1选择为模拟功能COMP INP
            PA4选择为模拟功能COMP INM
            PB1选择为模拟功能COMP OUT
            PC10选择为IO输出
            PC11选择为IO输出  
            PA7选择为ATIM1 CH1N输出
            PB14选择为ATIM1 CH2N输出
        4、TIM：
            ATIM1开启CH1 CH2 CH1N CH2N输出,COMP作为刹车输入
        5、COMP：
            COMP1输出触发ATIM1 刹车，无输出时恢复ATIM1 输出
    使用方法：
        1、编译后打开调试模式，将PC10连接到PA1，PC11连接到PA4，利用示波器或者逻辑分析仪观察ATIM1 输出波形
        2、当软件输出PC10电平大于PC11时，ATIM波形消失，相反时，波形正常输出
4、注意事项
    无


1. Function description
    1. The output of COMP1 brakes the complementary signal of ATIM1. After COMP OUT becomes low, the waveform of ATIM1 is restored.
2. Use environment
        IDE TOOLS:  KEIL MDK-ARM V5.34.0.0
                    IAR EWARM 8.50.1
        
    Hardware development environment:
        N32H473 series:
        Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series:
        Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H475 series:
        Developed based on the evaluation board N32H475UEQ7_STB V1.0
3. Instructions for use
    System Configuration; 
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:
            240MHz
            N32H475 series:
            240MHz
        3. Port configuration:
            PA1 is selected as the analog function COMP INP
            PA4 is selected as the simulation function COMP INM
            PB1 is selected as emulation feature COMP OUT
            PC10 Select IO output
            PC11 select IO output
            PA7选择为ATIM1 CH1N输出
            PB14选择为ATIM1 CH2N输出
        4. TIM:
            ATIM1 turns on CH1 CH2 CH1N CH2N output, COMP is used as brake input
        5. COMP:
            The output of COMP1 triggers the brake of ATIM1, and the output of ATIM1  is restored when there is no output.
    Instructions:
        1. After compiling, open the debug mode, connect PC10 to PA1, connect PC11 to PA4, and use an oscilloscope or logic analyzer to observe the output waveforms of ATIM1
        2. When the software output PC10 level is greater than PC11, the ATIM1 waveform disappears, on the contrary, the waveform is output normally
4. Attention
    none