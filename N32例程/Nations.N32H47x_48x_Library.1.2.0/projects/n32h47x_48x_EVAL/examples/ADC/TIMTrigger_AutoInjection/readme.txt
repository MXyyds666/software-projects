1、功能说明
    1、ADC1规则采样PA1、自动注入采样PB1引脚的模拟电压，在GTIM1 CC4事件下触发采样
    2、其中规则转换结果通过DMA1_CH1通道读取到变量ADC_RegularConvertedValueTab[32]数组
       注入转换结果通过转换结束中断读取到变量ADC_InjectedConvertedValueTab[32]数组
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
        3、中断：
            ADC1注入转换结果完成中断打开，抢断优先级0，子优先级0
            中断函数中将注入结果读取到ADC_InjectedConvertedValueTab[32]数组，并翻转PC6电平
        4、端口配置：
            PA1选择为模拟功能
            PB1选择为模拟功能
            PC6选择为通用IO输出
            PB11选择为TIM1 CH4 PWM输出
        5、DMA：
            DMA1_CH1通道回环模式搬运32个半字的ADC1转换结果到ADC_RegularConvertedValueTab[32]数组
        6、ADC：
            ADC1独立工作模式、GTIM1 CC4触发、12位数据右对齐，规则转换通道PA1，自动注入转换通道PB1的模拟电压数据
        7、GTIM1：
            GTIM1开启CH4输出，CH4用作触发ADC转换        
    使用方法：
        1、编译后打开调试模式，将变量ADC_RegularConvertedValueTab[32],ADC_InjectedConvertedValueTab[32]添加到watch窗口观察
        2、通过改变PA1 PB1引脚的电压，在每个CC4事件发生时转换一次规则和注入通道，变量储存在对应数组中。同时在PB11可以看到GTIM1 CH4的 PWM 波形
4、注意事项
    当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    当系统采用HSI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI


1. Function description
    1. ADC1 regularly samples PA1, automatically injects and samples the analog voltage of the PB1 pin, and triggers sampling under the GTIM1 CC4 event
    2. The regular conversion result is read into the variable ADC_RegularConvertedValueTab[32] array through the DMA1_CH1 channel
       The injected conversion result is read into the variable ADC_InjectedConvertedValueTab[32] array through the conversion end interrupt
       
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
        3. Interrupt:
            ADC1 injects the conversion result to complete the interrupt open, steals priority 0, sub-priority 0
            In the interrupt function, the injection result is read into the ADC_InjectedConvertedValueTab[32] array, and the PC6 level is flipped
        4. Port configuration:
            PA1 is selected as the analog function
            PB1 is selected as the analog function
            PC6 is selected as general IO output
            PB11 is selected as TIM1 CH4 PWM output
        5. DMA:
            DMA1_CH1 channel loopback mode transfers ADC1 conversion result of 32 half words to ADC_RegularConvertedValueTab[32] array
        6. ADC:
            ADC1 independent working mode, GTIM1 CC4 trigger, 12-bit data right-aligned, regular conversion channel PA1, automatic injection of analog voltage data of conversion channel PB1.
        7. GTIM1:
            GTIM1 turns on CH4 output, CH4 is used to trigger ADC conversion
    Instructions:
        1. Open the debug mode after compiling, and add the variables ADC_RegularConvertedValueTab[32], ADC_InjectedConvertedValueTab[32] to the watch window for observation
        2. By changing the voltage of the PA1 and PB1 pins, the rules and injection channels are converted once every CC4 event occurs,
           and the variables are stored in the corresponding arrays. At the same time, you can see the PWM waveforms of GTIM1 CH4 in PB11
4. Attention
    When the system uses HSE clock (generally HSI is also turned on), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    When the system uses HSI clock (generally HSE is turned off), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI