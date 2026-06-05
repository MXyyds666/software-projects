1、功能说明
    此示例展示了如何配置SHRTIM来控制一个多相降压转换器。它在A、B、C和D定时器单元上处理5个相位，并输出TA2、TB1、TC2、TD1、TD2。

2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
                 IAR EWARM 8.50.1
    硬件环境：N32H474VEL7-STB V1.0    

3、使用说明
    系统配置：
        1、 时钟源：
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
            ADC sampling clock = 240MHz/3 =80MHz。
        2、SHRTIM&ADC
            TA2（PA9）、TB1(PA10)、TC2（PB13）、TD1（PB14）、TD2(PB15)为五个相的输出。
            PA12为FAULT1，高电平关波
            ADC配置为在每个5相位的转换器开启时间的中间触发转换，PA0为ADC的通道。
    Demo说明：
        更详细的说明，请查阅《CN_UG_N32H47x_48x_SHRTIM_User_Guide》中的"多相降压转换器"章节
    使用方法：
        1. 编译后打开调试模式，示波器量测PA9、PA10、PB13、PB14和PB15，可观察到五相交错的PWM波形
        2. 按下KEY1（PA4），每按下KEY1，相数会变化。5相 -> 4相 -> 3相 -> 2相 -> 1相 -> 突发模式（2个周期一个波）
        3. 给PA12输入一个高电平，PA9、PA10、PB13、PB14和PB15的波形被关闭
        4. 撤掉PA12上的高电平，按下KEY1，PA9、PA10、PB13、PB14和PB15的波形被恢复
        5. ADC的测试，若要测试完备，需要采用《CN_UG_N32H47x_48x_SHRTIM_User_Guide》中的"多相降压转换器"的"调试ADC操作"章节。若简要测试，将aADCxConvertedValues添加到watch窗口，给PA0输入一个模拟电压，可在aADCxConvertedValues上观察到码值变化。
4、注意事项
    无
    
    
1. Function description
    This example shows how to configure the SHRTIM to control a multiphase buck converter. It handles here 5-phases on timer unit A, B C and D and outputs TA2, TB1, TC2, TD1, TD2.

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware environment: N32H474VEL7-STB V1.0  

3. Instructions for use
    System Configuration:
        1. clock source
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 8GHz。
            ADC sampling clock = 240MHz/3 =80MHz。
        2. SHRTIM & ADC
            TA2 (PA9), TB1 (PA10), TC2 (PB13), TD1 (PB14), and TD2 (PB15) are the outputs for the five phases.
            PA12 is FAULT1; a high level shuts off the waveform.
            The ADC is configured to trigger conversion in the middle of the on-time of the converter for each of the 5 phases, and PA0 is the ADC channel.
    Demo instructions:
        For more detailed instructions, please refer to the "Multi-phase Buck Converter" section in the "CN_UG_N32H47x_48x_SHRTIM_User_Guide"
    Usage Instructions:
        1. After compiling, enable debug mode and use an oscilloscope to measure PA9, PA10, PB13, PB14, and PB15. You can observe five-phase interleaved PWM waveforms.
        2. Press KEY1 (PA4). Each time you press KEY1, the number of phases will change. 5-phase -> 4-phase -> 3-phase -> 2-phase -> 1-phase -> burst mode (one waveform every 2 cycles)
        3. Input a high level to PA12, and the waveforms on PA9, PA10, PB13, PB14, and PB15 will be disabled.
        4. Remove the high level on PA12 and press KEY1; the waveforms on PA9, PA10, PB13, PB14, and PB15 will be restored.
        5. For ADC testing, a comprehensive test requires following the "Debugging ADC Operation" section in the "Multi-phase Buck Converter" chapter of the "CN_UG_N32H47x_48x_SHRTIM_User_Guide". For a brief test, add aADCxConvertedValues to the watch window, input an analog voltage to PA0, and you can observe code value changes in aADCxConvertedValues.
        
4. Attention
     None