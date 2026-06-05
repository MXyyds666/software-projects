1、功能说明
    此示例显示如何配置 HRTIM 来控制半桥 LLC具有同步整流的转换器，使用定时器单元 A 和 B 以及TA1/TA2/TB1/TB2 输出。
2、使用环境
    软件开发环境：KEIL MDK-ARM 5.34
                 IAR EWARM 8.50.1
    硬件环境：N32H474VEL7-STB V1.0    

3、使用说明
    系统配置：
        1、 时钟源：
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 1.6GHz。
            ADC sampling clock = 240MHz/3 =80MHz。
        2、 ADC：
            - ADC1： 间断扫描模式、注入通道、HRTIM触发，序列长度为4、HRTIM_TRG2触发、PA0采样
            - ADC2： 间断扫描模式、注入通道、HRTIM触发，序列长度为4、HRTIM_TRG4触发、PA6采样
            ADC配置为8个转换周期，在以下时间点采样：
                - SR1开启前的250ns， SR1开启后的250ns. SR1关闭前的250ns，SR1关闭后的250ns
                - SR2的采样时间同理
        3、TA1 和 TA2 输出（分别为 PA8、PA9）是互补 PWM 输出，用于控制初级侧半桥开关。
           TB1 和 TB2 输出（分别为 PA10、PA11）是独立的 PWM 信号，用于控制次级侧的同步整流 FET。
           PA12 上启用 FAULT1 输入（高电平激活）以演示 PWM 关闭，适用于所有输出。
           故障触发时（PA12输入高电平） TA1、TA2、TB1、TB2信号被关闭。 按下用户按钮即可重新装备系统。
           电路还通过比较器2在PA1输入上进行延迟空闲保护模式，以防止初级侧过电流。

    使用方法： 
          1. 编译后打开调试模式，示波器观察TA1、TA2、TB1、TB2的波形。可观察到TA1与TA2为互补PWM波，TB1与TB2为独立PWM波；频率均在不断变化
          2. 给PA12输入一个高电平，TA1、TA2、TB1、TB2波形均关闭
          3. 撤掉PA12上的高电平，按下KEY1(PA4),TA1、TA2、TB1、TB2波形恢复
          4. ADC简要测试：给PA0和PA6上面输入一个模拟电压，在ADC1的ADC_JDAT1~ADC+JDAT4可观察到PA0对应的码值,在ADC2的ADC_JDAT1~ADC2_JDAT4可观察到PA6对应的码值
4、注意事项
    开发板PA6接到了按键KEY3，KEY3上有一个下拉电阻，因此ADC2的码值会比输入的电压值低。
    
    
1. Function description
    This example shows how to configure the HRTIM to control a half-bridge LLC 
    converter with synchronous rectification, using timer units A and B and 
    TA1/TA2/TB1/TB2 outputs.

2. Use environment
    Software development environment: KEIL MDK-ARM 5.34
                                      IAR EWARM 8.50.1
    Hardware environment: N32H474VEL7-STB V1.0  

3. Instructions for use
    System Configuration:
        1. clock source
            HSE=8M,PLL=240M,AHB=240M, 
            SHRTIM clock(SHRTPLL) = 250M，High-resolution clock = 1.6GHz。
            ADC sampling clock = 240MHz/3 =80MHz。
        2. ADC
             - ADC1: Discontinuous mode, injected channel, HRTIM trigger, sequence length of 4, triggered by HRTIM_TRG2, sampling PA0
             - ADC2: Discontinuous mode, injected channel, HRTIM trigger, sequence length of 4, triggered by HRTIM_TRG4, sampling PA6
            The ADC is configured to do 8 conversions per switching cycle, at the 
            following time:
                - 250ns before SRT1 turns on, 250ns after SR1 turns on, 250ns before SR1 turns off, 250ns after SR1 turns off
                - The sampling time of SR2 is the same
            - 250ns before SR1 turn-on, 250ns after SR1 turn-on, 250ns before SR1 turn-off
              and 250ns after SR1 turn-off
            - same applies for SR2 also
        3.  The TA1 and TA2 outputs (resp. PA8, PA9)are complementary PWM outputs for
            controlling the primary side half-bridge switches.
            The TB1 and TB2 outputs (resp. PA10, PA11) are independent PWM signals for
            controlling the synchronous rectification FETs on the secondary side.
            The FAULT1 input is enabled on PA12 (high level active) to demonstrate PWM shut down 
            for all outputs.
            When the fault is triggered (PA12 input connected to GND) TA1, TA2, TB1, TB2
            signals are shut-down. The system can be re-armed by pressing the user button.
            The circuit is also protected from over-current on the primary side with the 
            comparator 2, on PA7 input, in delayed idle protection mode.
    Use method
        1. After compiling, open the debug mode and use an oscilloscope to observe the waveforms of TA1, TA2, TB1, and TB2. You will see that TA1 and TA2 are complementary PWM waves, while TB1 and TB2 are independent PWM waves; the frequencies are constantly changing.
        2. When a high level is applied to PA12, the waveforms of TA1, TA2, TB1, and TB2 are all turned off.
        3. Remove the high level from PA12 and press KEY1 (PA4); the waveforms of TA1, TA2, TB1, and TB2 are restored.
        4. ADC brief test: Input an analog voltage to PA0 and PA6. In ADC1's ADC_JDAT1~ADC_JDAT4, you can observe the code value corresponding to PA0; in ADC2's ADC_JDAT1~ADC_JDAT4, you can observe the code value corresponding to PA6.
        
4. Attention
    The development board PA6 is connected to the KEY button, and there is a pull-down resistor on KEY3, so the ADC2 code value will be lower than the input voltage value.
