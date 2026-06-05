1、功能说明
    1、PGA1工作在单端模式下，输出电压是输入电压的2倍.
    
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
            PA1选择为模拟功能
            PGA1输出连接ADC1的内部通道1
        4、PGA配置：
            PA1输入，单端工作模式，2倍增益

    使用方法：
        1、编译后打开调试模式，将PA1引脚接入一定的电压，从ADC端口读出的电压大约是输入电压的2倍
4、注意事项
    1. 经过PGA放大后的电压不能超过(VDDA-0.3)V,且不能低于0.3V.


1. Function description
    1. PGA1 operates in single-ended mode, with an output voltage that is twice the input voltage

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
            PA1 is selected as the analog function 
            The output of PGA1 is connected to the internal channel1 of ADC1 
        4. PGA1：
            PA1 is selected as input, single-ended mode, gain x2
    Usage:
        1. After compiling, open the debugging mode,connect the PA1 pin to a certain voltage, and the voltage read from the ADC port is about twice the input voltage 
        
4. Attention
    1. The amplified voltage of the PGA should not exceed (VDDA-0.3)V，and could not  below 0.3V.