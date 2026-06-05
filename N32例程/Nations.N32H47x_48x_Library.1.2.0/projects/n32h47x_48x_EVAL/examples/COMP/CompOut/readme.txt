1、功能说明
    1、COMP1的输出PB1受输入INP PA1和INM PA4的影响
    
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
        4、COMP：
            COMP1输入PA1，PA4，输出PB1
    使用方法：
        1、编译后打开调试模式，将PC10连接到PA1，PC11连接到PA4，利用示波器或者逻辑分析仪观察PB1输出波形
        2、当软件输出PC10电平大于PC11时，PB1输出高电平，相反时，输出低电平
4、注意事项
    无


1. Function description
    1. COMP1 output PB1 is affected by INP PA1 and INM PA4

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
        3. Port Configuration:
            PA1 is selected as the analog function COMP INP
            PA4 is selected as the simulation function COMP INM
            PB1 is selected as emulation feature COMP OUT
            PC10 Select IO output
            PC11 select IO output
        4. COMP：
            COMP1 inputs PA1, PA4, and outputs PB1
    Usage:
        1. After compiling, open the debugging mode, connect PC10 to PA1 and PC11 to PA4, 
          and observe the output waveform of PB1 using oscilloscope or logic analyzer.
        2. When the software outputs PC10 level greater than PC11, PB1 outputs high level; otherwise, PB1 outputs low level 
        
4. Attention
    none