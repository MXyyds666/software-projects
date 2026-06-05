1、功能说明
    1、ATIM1周期触发DAC1和DAC2输出三角波数据，在PA4 PA5输出三角波形
    
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
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_STB V1.0开发
3、使用说明
    系统配置；
        1、时钟源： HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：
            240MHz
            N32H475/482/487/488系列：
            240MHz 
        3、端口配置：
            PA4选择为模拟功能DAC1 OUT
            PA5选择为模拟功能DAC2 OUT
        4、ATIM：
            ATIM1 更新事件作为触发输出给DAC1和DAC2
        5、DAC配置：
            DAC选择ATIM1 TRGO信号触发，开启DAC1和DAC2，使能DAC1三角波幅度2048，DAC2三角波幅度1024,基值0X100

    使用方法：
        1、编译后打开调试模式，利用示波器观察PA4 PA5输出波形
        2、全速运行时，ATIM1的每个周期触发PA4 PA5的一个数据，形成两个幅值不同的三角波.
        
4、注意事项
    无


1. Function description
    1. ATIM1 periodically triggers DAC1 and DAC2 to output triangular wave data, and outputs triangular waveform in PA4 PA5

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
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_STB V1.0
3. Instructions for use
    System Configuration; 
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:
            240MHz
            N32H475/482/487/488 series:
            240MHz  
        3. Port Configuration:
            PA4 is selected as analog function DAC1 OUT
            PA5 is selected as analog function DAC2 OUT
        4. ATIM1:
            ATIM1 update event as trigger output to DAC1 and DAC2
        5. DAC:
            The DAC selects the ATIM1 TRGO signal to trigger, opens DAC1 and DAC2, enables DAC1 the amplitude of the triangular wave to be 2048, 
            enables DAC2 the amplitude of the triangular wave to be 1024,,and the base value is 0x100

    Usage:
        1. Open the debug mode after compiling, and use the oscilloscope to observe the output waveform of PA4 and PA5
        2. When running at full speed, each cycle of ATIM1 triggers a data of PA4 and PA5 to form two different amplitude triangular wave.
        
4. Attention
    none
    
