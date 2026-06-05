1、功能说明
    1、PGA4的用户模式下的手动校准.
    
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
3、使用说明
    系统配置；
        1、时钟源： HSI+PLL
        2、系统时钟频率：
            N32H473/474系列：
            240MHz
            N32H475系列：
            240MHz 
        3、端口配置：
            PB11选择为模拟功能
            PB1选择为模拟功能
            PB3选择为USART2 TX 复用功能
            PB5选择为USART2 RX 复用功能
        4、Log配置：
            PA9选择为LOG的TX引脚
        5、USART2：
            115200波特率、8位数据位、1位停止位、无奇偶校验位、无硬件流控、发送和接收使能
        6、PGA配置：
            PB11、PB1输入，工作模式为校准模式

    使用方法：
        1、编译后打开调试模式，串口下发“0xAA 0x01”确认并开启PGA校准流程；
        2、将PB11引脚接入0.3v电压,使用USART2 串口下发“0xAA 0x01”确认并进入下一步；
        3、将PB11引脚接入3.0v电压,使用USART2 串口下发“0xAA 0x01”确认并进入下一步；
        4、将PB1引脚接入0.3v电压,使用USART2 串口下发“0xAA 0x01”确认并进入下一步；
        5、将PB1引脚接入3.0v电压,使用USART2 串口下发“0xAA 0x01”确认并完成校准；
        6、再次使用USART2 串口下发“0xAA 0x01”保存校准的数据并通过LOG打印；
4、注意事项
    无


1. Function description
    1. Manual calibration of the PGA4 in user mode.

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
        
3. Instructions for use
    System Configuration; 
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            N32H473/474 series:
            240MHz
            N32H475 series:
            240MHz
        3. Port Configuration:
            PB11 is selected as the analog function 
            PB1 is selected as the analog function
            PB3 is selected as USART2 TX function
            PB5 is selected as USART2 RX function
        4. Log Configuration:
            PA9 is the TX pin of LOG
        5, USART2:
            115200 Baud rate, 8 data bits, 1 Stop bit, no parity bit, no hardware flow control, send and receive enabled
        6. PGA1：
            PB11、PB1 is selected as input, user calibration mode
    Usage:
        1, after compiling, open the debug mode, the serial port sends 0xAA 0x01 to confirm and open the PGA calibration process;
        2, connecting PB11 pin to the 0.3v voltage, using the USART2 serial port to send “0xAA 0x01” to confirm and enter next step;
        3, connecting PB11 pin to the 3.0v voltage, using the USART2 serial port to send the “0xAA 0x01” confirmation and enter next step;
        4, connecting PB1 pin to the 0.3v voltage,  using the USART2 serial port to send the “0xAA 0x01” confirmation and enter next step;
        5, connecting PB1 pin to the 3.0v voltage,  using the USART2 serial port to send the “0xAA 0x01” to confirm and complete the calibration;
        6、using USART2 serial port to send “0xAA 0x01” to save the calibrated data and print through LOG; 
                
4. Attention
    None