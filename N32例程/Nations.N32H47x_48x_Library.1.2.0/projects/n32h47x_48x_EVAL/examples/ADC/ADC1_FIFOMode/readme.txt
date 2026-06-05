1、功能说明
    1、ADC1采样、转换PB1引脚的模拟电压（使用ADC FIFO模式）
    2、其中ADC1转换结果通过DMA1_CH1通道读取到变量ADCConvertedValue[60]
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
            突发长度 = 6, 正常模式, 使能DMA突发， DMA1_CH1通道回环模式搬运6个半字的ADC1转换结果到ADCConvertedValue[60]变量
        5、ADC：
            ADC1独立工作模式、单次转换、单通道模式、软件触发、12位数据右对齐，FIFO长度为6,转换结果就是PB1的模拟电压数据.
    使用方法：
        1、编译后打开调试模式，将变量ADCConvertedValue[60]添加到watch窗口观察
        2、调试模式下，单步模式下，每软件触发6次，ADCConvertedValue[60]中会新增6个PB1引脚上转换的电压值. 
4、注意事项
    1.当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    2.当系统采用HSI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI
    3.不要将调试窗口的ADC外设调出，或者ADCx->DAT寄存器添加到watch窗口，这两种方式都会定期读取ADC的数据，影响ADC_FIFO的功能测试

1. Function description
    1. ADC1 samples ,converts the analog voltage of the PB1 pin (using ADC FIFO mode)
    2. The ADC1 conversion result is read to the variable ADCConvertedValue[60] through the DMA1_CH1 channel
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
    System Configuration; 
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:
            240MHz
            N32H475/482/487/488 series:
            240MHz
        3. Port configuration:
            PB1 selected as analog function 
        4. DMA:
            burstlen = 6, Normal mode, Enable DMA_BURST. DMA1_CH1 channel loopback mode to transfer 6 half-word ADC1 conversion results to the ADCConvertedValue[60] variable
        5. ADC:
            ADC1 independent working mode, one-shot conversion, single-channel mode, software trigger, 12-bit data is right-aligned, FIFO length is 6 .
            conversion result is the analog voltage data of PB1.
    Instructions:
        1. After compiling, open the debug mode and add the variable ADCConvertedValue[60] to the watch window to observe
        2. In debug mode, when the software is triggered 6 times in single step mode, 6 converted voltage values from the PB1 pin will be added to ADCConvertedValue[60]..
4. Attention
    1.When the system uses HSE clock (generally HSI is also turned on), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    2.When the system uses HSI clock (generally HSE is turned off), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI
    3.Do not call out the ADC peripheral of the debug window or add the ADCx->DAT register to the watch window. Both of these methods will regularly read the ADC data, affecting the functional testing of ADC_FIFO.