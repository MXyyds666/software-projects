1、功能说明
    1、ADC1采样转换内部温度传感器的模拟电压，并转换为温度值。
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
        3、DMA：
            DMA1_CH1通道循环模式搬运一个半字的ADC1转换结果到ADCConvertedValue变量
        4、ADC：
            ADC1独立工作模式、连续转换、软件触发、12位数据右对齐，转换通道16即内部温度传感器的模拟电压数据
        5、Log配置：
            PA9选择为LOG的TX引脚
        6、USART：
            115200波特率、8位数据位、1位停止位、无奇偶校验位、无硬件流控、发送和接收使能
        7、功能函数：
            TempValue = TempCal(ADCConvertedValue)函数将温度ADC原始格式数据转为度的单位的格式
    使用方法：
        1、编译后打开调试模式，将变量ADCConvertedValue,TempValue添加到watch窗口观察
        2、将串口工具连接到PA9引脚，并打开串口接收工具
        3、全速运行，可以看到温度变量的数值在常温下接近25度左右，同时串口工具显示实时芯片内的温度值
4、注意事项
    当系统采用HSE时钟时（一般HSI也是打开的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8)可以配置为HSE或者HSI
    当系统采用SI时钟时（一般HSE是关闭的），RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8)只能配置为HSI


1. Function description
    1. ADC1 samples and converts the analog voltage of the internal temperature sensor to the temperature value.
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
        3, DMA:
            DMA1_CH1 channel is configuered as circular mode, carries a half-word of ADC1 conversion result to the ADC1ConvertedValue variable
        4, ADC:
            ADC1 configuration: independent mode, continuous conversion, software trigger, 12 bit data right aligned, conversion channel 16is the analog voltage data of the internal temperature sensor
        5. Log Configuration:
            PA9 is the TX pin of LOG
        6, USART:
            115200 Baud rate, 8 data bits, 1 Stop bit, no parity bit, no hardware flow control, send and receive enabled
        7. Functions:
            The TempValue = TempCal(ADCConvertedValue) function converts temperature ADC raw format data into degrees
    Instructions:
        1, compiled to open the debug mode, variable ADCConvertedValue, TempValue added to the watch window to observe
        2. Connect the serial port tool to the PA9 pin and open the serial port receiver tool
        3. Running at full speed, it can be seen that the value of the temperature variable is close to 25 degrees at room temperature, and the serial port tool displays the real-time temperature value in the chip
4. Attention
    When the system uses the HSE clock (HSI is generally enabled), ), RCC_ConfigAdc1mClk (RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8) can be configured as HSE or HSI
    When the system uses the HSI clock(HSE is generally disabled), RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8) can only be configured as HSI