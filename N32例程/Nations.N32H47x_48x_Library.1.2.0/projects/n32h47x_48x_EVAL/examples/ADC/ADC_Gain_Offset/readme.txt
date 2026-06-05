1、功能说明
    1、ADC1开启增益补偿与偏移补偿的条件下进行采样、转换PB1引脚的模拟电压
    2、其中ADC1转换结果通过DMA1_CH1通道读取到变量ADCConvertedValue
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
            PB1选择为模拟功能
        4、DMA：
            DMA1_CH1通道回环模式搬运一个半字的ADC1转换结果到ADCConvertedValue变量
        5、ADC：
        ADC1独立工作模式、连续转换、单通道模式、软件触发、12位数据右对齐，转换结果即PB1的模拟电压数据 ,增益补偿系数为2*4096 ,正向偏移 0x400,饱和使能.
    使用方法：
        1、编译后打开调试模式，将变量ADCConvertedValue添加到watch窗口观察
        2、通过改变PB1引脚的电压Voltage，可以看到转换结果变量同步改变，数据寄存器的结果Result = Voltage * 2 * 4095/3.3 + 0x400. 
4、注意事项
    当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    当系统采用HSI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI

1. Function description
    1. ADC1 performs sampling with gain compensation and offset compensation enabled ,converts the analog voltage of the PB1 pin
    2. The ADC1 conversion result is read to the variable ADCConvertedValue through the DMA1_CH1 channel
2. Use environment
    Software development environment: 
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
    System Configuration:
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:
            240MHz
            N32H475/482/487/488 series:
            240MHz
        3. Port configuration:
            PB1 selected as analog function
        4. DMA:
            DMA1_CH1 channel loopback mode transfers a halfword of ADC1 conversion result to ADCConvertedValue variable
        5. ADC:
            ADC1 independent working mode, continuous conversion, single-channel mode, software trigger, 12-bit data is right-aligned,
            conversion result is the analog voltage data of PB1, Gain compensation factor is 2*4096, positive offset 0x400,enable saturation .
    Instructions:
        1. After compiling, open the debug mode and add the variable ADCConvertedValue to the watch window to observe
        2. By changing the voltage (Voltage) of the PB1 pin, you can see that the conversion result variable changes synchronously . the data register result = Voltage * 2 * 4095/3.3 + 0x400.
4. Attention
    When the system uses HSE clock (generally HSI is also turned on), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    When the system uses HSI clock (generally HSE is turned off), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI