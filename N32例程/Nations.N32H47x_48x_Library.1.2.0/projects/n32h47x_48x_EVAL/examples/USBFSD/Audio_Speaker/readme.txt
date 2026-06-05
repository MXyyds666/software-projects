1、功能说明
    USB 音频扬声器设备

2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
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

3、使用说明
    描述相关模块配置方法；例如:时钟，I/O等 
         1. SystemClock：192MHz(N32H473和N32H474系列)，240MHz(N32H482和N32H487/488系列)
         2. HSE: 16MHz
         3. I2SClock:    SHRTPLL 241.664MHz(I2S 音频频率16KHz)
         4. WM8978(音频芯片): I2C SCL -- PB6
                                  SDL -- PB7
                              I2S CK  -- PC7
                              I2S WS  -- PC6
                              I2S SD  -- PC2
                              I2S MCK -- PA8

    描述Demo的测试步骤和现象 
         1. 编译后下载程序复位运行；
         2. 通过 USB 线连接J4 USB 口，电脑识别出音频设备，用耳机接入J21口
         3. 电脑播放音乐，通过音频芯片，耳机可以听到声音

4、注意事项
    无

1. Function description
    USB Audio speaker device

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
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

3. Instructions for use
    Describe the configuration method of related modules; for example: clock, I/O, etc.
        1. SystemClock：192MHz(N32H473 and N32H474 series)，240MHz(N32H482 and N32H487/488 series)  HSE 16MHz
        2. HSE: 16MHz
		3. I2SClock: SHRTPLL 241.664(for I2S audio clock 16KHz)
        4. WM8978(Audio chip): I2C SCL -- PB6
                                   SDL -- PB7
                               I2S CK  -- PC7
                               I2S WS  -- PC6
                               I2S SD  -- PC2
                               I2S MCK -- PA8
                  
    Describe the test steps and phenomena of Demo 
        1. After compiling, download the program to reset and run;
        2. Connect the USB port through the J4 USB cable, and the computer recognizes the Audio device, and connects to the J21 port with the headphone; 
        3. The computer plays music, and the headphones can hear the sound via audio chip.
 
4. Attention
    None