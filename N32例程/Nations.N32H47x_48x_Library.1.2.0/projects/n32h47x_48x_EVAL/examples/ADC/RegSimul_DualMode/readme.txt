1、功能说明
    1、ADC1 ADC2规则通道同步采样，ADC1采样PB0、PB1引脚的模拟电压，ADC2采样PB11、PB15引脚的模拟电压
    2、同步转换的结果通过DMA_CH1传输到ADC_DualConvertedValueTab[16]数组，只传输16次
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
            PB0选择为模拟功能
            PB1选择为模拟功能
            PB11选择为模拟功能
            PB15选择为模拟功能
        4、DMA：
            DMA1_CH1通道正常模式搬运16个字的ADC1、ADC2同步规则通道转换结果到ADC_DualConvertedValueTab[16]数组
        5、ADC：
            ADC1规则通道同步规则通道模式、连续扫描模式、软件触发、12位数据右对齐，转换通道PB0和PB1的模拟电压数据
            ADC2规则通道同步规则通道模式、连续扫描模式、软件触发、12位数据右对齐，转换通道PB11和PB15的模拟电压数据
    使用方法：
        1、编译后打开调试模式，将变量ADC_DualConvertedValueTab[16]添加到watch窗口观察
        2、全速运行前，在PB0、PB1、PB11、PB15给确定的电压信号，全速运行后，即可转换16次结果到ADC_DualConvertedValueTab[16]数组
4、注意事项
    ADC转换虽然是连续的，但是DMA只存储前16次数据到数组
    当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    当系统采用HSI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI


1. Function description
    1. ADC1 ADC2 regular channel synchronous sampling, ADC1 samples the analog voltage of PB0 and PB1 pins, ADC2 samples the analog voltage of PB11 and PB15 pins
    2. The result of the synchronous conversion is transferred to the ADC_DualConvertedValueTab[16] array through DMA_CH1, only 16 times
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
            PB0 is selected as the analog function
            PB1 is selected as the analog function 
            PB11 is selected as the analog function 
            PB15 is selected as the analog function
        4. DMA:
            DMA1_CH1 channel normal mode transfers 16 words of ADC1, ADC2 synchronous rule channel conversion results to ADC_DualConvertedValueTab[16] array
        5. ADC:
            ADC1 regular channel synchronization regular channel mode, continuous scan mode, software trigger, 12-bit data right-justified, analog voltage data of conversion channels PB0 and PB1
            ADC2 regular channel synchronization regular channel mode, continuous scan mode, software trigger, 12-bit data right justification, conversion of analog voltage data of channels PB11 and PB15
    Instructions:
        1. Open the debug mode after compiling, and add the variable ADC_DualConvertedValueTab[16] to the watch window to observe
        2. Before running at full speed, PB0、PB1、PB11、PB15 gives the determined voltage signal, after running at full speed, the result can be converted 16 times to the ADC_DualConvertedValueTab[16] array
4. Attention
    Although the ADC conversion is continuous, the DMA only stores the first 16 data to the array
    When the system uses HSE clock (generally HSI is also turned on), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    When the system uses HSI clock (generally HSE is turned off), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI