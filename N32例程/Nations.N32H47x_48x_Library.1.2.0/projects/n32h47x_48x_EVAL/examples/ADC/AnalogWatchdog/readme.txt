1、功能说明
    1、ADC1采样转换PB1引脚的模拟电压，如果超过模拟看门狗1/2定义的阈值范围，则跳入中断程序
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
        3、ADC：
            ADC1独立工作模式、连续转换、软件触发、12位数据右对齐，转换PA2引脚的模拟电压数据
        4、端口配置：
            PB1选择为模拟功能
        5、中断：
            ADC1模拟看门狗1和模拟看门狗2中断打开，优先级分组为2，抢断优先级0，子优先级0
    使用方法：
            1、编译后打开调试模式，将变量gCntAwdg添加到watch窗口观察
            2、改变PB1引脚电压值，当电压值超出模拟看门狗1或者看门狗2定义的阈值范围外，则进入一次中断，变量做累加操作
4、注意事项
    当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    当系统采用HSI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI

1. Function description
    1. ADC1 samples and converts the analog voltage of the PB1 pin. If it exceeds the threshold range defined by the analog watchdog1/2, it will jump into the interrupt program
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
        3. ADC:
            ADC1 independent working mode, continuous conversion, software trigger, 12-bit data right-justified, convert the analog voltage data of PA2 pin
        4. Port configuration:
            PB1 selected as analog function
        5. Interrupt:
            ADC1 analog watchdog interrupt on, priority group is 2, preemptive priority 0, sub priority 0
    Instructions:
        1. After compiling, open the debug mode and add the variable gCntAwdg to the watch window to observe
        2. Change the voltage value of the PB1 pin. When the voltage value exceeds the threshold range defined by the analog watchdog1 or analog watchdog2 ,
           it will enter an interrupt and the variable will be accumulated.
4. Attention
    When the system uses HSE clock (generally HSI is also turned on), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    When the system uses HSI clock (generally HSE is turned off), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI