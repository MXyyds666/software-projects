1、功能说明
    1、描述DAC的用户校准流程
    
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
        2、端口配置：
            PA4选择为模拟功能
        3、LOG配置：
            TX  -->  PA9 
            波特率：115200
            数据位：8bit
            停止位：1bit
            无校验
        4、DAC配置：
            DAC选择软件触发，无波形生成模式,输出buffer使能, 对外输出到Pin.

    使用方法：
        1、编译下载代码复位运行
        2、从串口看打印信息，验证结果
4、注意事项
    无


1. Function description
    1. Describe the user calibration procedure for DAC

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
            PA4 is selected as the analog function 
        4. LOG configuration:
             TX --> PA9 
             Baud rate: 115200
             Data bit: 8bit
             Stop bit: 1bit
             No verification
        5. DAC:
            Software-triggered, waveform generation disabled, output buffer enabled, external output to Pin
            
    Usage:
        1. compile and download the code, reset and run
        2. view the print information from the serial port and verify the result
4. Attention
    none
    
