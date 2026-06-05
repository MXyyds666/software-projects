1、功能说明

    该测例演示了USART2与智能卡间基础通信，查询智能卡随机数


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34.0.0
                  IAR EWARM 8.50.1
    
    硬件开发环境：    
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_EVB V1.1开发
    智能卡，智能卡座


3、使用说明
    
    系统时钟配置如下：
    - 时钟源 = HSE + PLL
    - 系统时钟
        N32H482/N32H487:
            SystemClock：240MHz
            智能卡工作时钟 = 3MHz
        N32H474/N32H473:
            SystemClock：240MHz
            智能卡工作时钟 = 2.5MHz
    
    USART2配置如下：
    - 波特率 =  (根据设置的智能卡工作时钟配置)
    - 字长 = 8数据位
    - 1.5停止位
    - 校验控制 even
    - 硬件流控制禁用（RTS和CTS信号）
    - 接收器和发送器使能
    
    USART2引脚连接如下：

    N32H475：
    - USART2_Tx.PB3    <-------> SMART_CARD I/O
    - USART2_CLK.PB5   <-------> SMART_CARD CLK
    - SC_PIN_RESET.PB7 <-------> SMART_CARD RST
    - 3V3              <-------> SMART_CARD Vcc
    - GND              <-------> SMART_CARD GND
    N32H473/N32H474/N32H482/N32H487：
    - USART2_Tx.PF11    <-------> SMART_CARD I/O
    - USART2_CLK.PC12   <-------> SMART_CARD CLK
    - SC_PIN_RESET.PB3  <-------> SMART_CARD RST
    - 3V3               <-------> SMART_CARD Vcc
    - GND               <-------> SMART_CARD GND
    
    打印信息USART1配置如下：
    - 波特率 = 115200 baud
    - 字长 = 8数据位
    - 1停止位
    - 校验控制禁用
    - 硬件流控制禁用（RTS和CTS信号）
    - 接收器和发送器使能    
    - TX - PA9
    
    测试步骤与现象：
    - Demo在KEIL环境下编译后，下载至MCU
    - 复位运行，查看串口打印信息


4、注意事项
无

1. Function description

The test example demonstrates the basic communication between USART2 and smart card to query the random number of smart card


2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
                                      IAR EWARM 8.50.1
    Hardware development environment:
        N32H473 series:
        Developed based on the evaluation board N32H473VEL7_STB V1.0
        N32H474 series:
        Developed based on the evaluation board N32H474VEL7_STB V1.0
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_EVB V1.1
    Smart Card, Smart Card Holder


3. Instructions for use

The system clock configuration is as follows:
    System Clock Configuration:
        N32H475/N32H482/N32H487:
            SystemClock：240MHz
            Smartcard clock = 3MHz
        N32H474/N32H473:
            SystemClock：240MHz
            Smartcard clock = 2.5MHz

USART2 configuration is as follows:
- Baud rate = (based on the set smart card working clock)
- Word length = 8 data bits
- 1.5 Stop bit
- Check control even
- Hardware flow control disabled (RTS and CTS signals)
- Enable receiver and transmitter

The USART pins are connected as follows:

    N32H475：
    - USART2_Tx.PB3    <-------> SMART_CARD I/O
    - USART2_CLK.PB5   <-------> SMART_CARD CLK
    - SC_PIN_RESET.PB7 <-------> SMART_CARD RST
    - 3V3              <-------> SMART_CARD Vcc
    - GND              <-------> SMART_CARD GND
    N32H473/N32H474/N32H482/N32H487：
    - USART2_Tx.PF11    <-------> SMART_CARD I/O
    - USART2_CLK.PC12   <-------> SMART_CARD CLK
    - SC_PIN_RESET.PB3  <-------> SMART_CARD RST
    - 3V3               <-------> SMART_CARD Vcc
    - GND               <-------> SMART_CARD GND

USART1 is configured as follows:
- baud rate = 115200 baud
- Word length = 8 data bits
-1 Stop bit
- Check control is disabled
- Hardware flow control disabled (RTS and CTS signals)
- Enable receiver and transmitter
- TX - PA9

Test procedure and phenomenon:
- Demo is compiled in KEIL environment and downloaded to MCU
- Reset and run. View the serial port information


4. Attention
None
