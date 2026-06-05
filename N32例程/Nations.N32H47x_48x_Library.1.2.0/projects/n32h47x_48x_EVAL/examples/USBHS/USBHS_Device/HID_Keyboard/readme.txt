1、功能说明
    USB Mouse设备

2、使用环境

    软件开发环境：KEIL MDK-ARM 5.34
                  IAR EWARM 8.50.1

    硬件开发环境：
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于评估板N32H487ZEL7_EVB V1.1开发


3、使用说明
    描述相关模块配置方法；例如:时钟，I/O等 
         1. SystemClock：240MHz
         2. USBClock: HSE 16MHz
         3. GPIO：KEY1(PC13),KEY2(PA15),KEY3(PC8)键盘输入
            键盘灯控制：D14(PA3),D6(PA8)键盘输出
            
    描述Demo的测试步骤和现象 
         1. 编译后下载程序复位运行；
         2. 通过 USB 线连接 J62 USB 口，电脑识别出键盘设备
         3. 按下KEY1,KEY2,KEY3按键，USB 输入 "a","b","c"
         4. 用另外一个键盘开关Capslock和Numlock，可以看到D14和D6对应键盘灯亮灭

4、注意事项
    可通过修改宏定义 USE_USB_HS_IN_FS 或 USE_USB_HS_IN_HS 切换设备为全速模式或者高速模式；
    另外需要使用16MHz、19.2MHz、20MHz、24MHz、26MHz或32MHz外部晶体。

1. Function description
    USB Mouse device

2. Use environment
    Software development environment: KEIL MDK-ARM V5.34
                                      IAR EWARM 8.50.1

    Hardware development environment:
        N32H482 series:
        Developed based on the evaluation board N32H482ZEL7_STB V1.0
        N32H487 series:
        Developed based on the evaluation board N32H487ZEL7_EVB V1.1

3. Instructions for use
    Describe the configuration method of related modules; for example: clock, I/O, etc.
        1. SystemClock: 240MHz
        2. USBClock: HSE 16MHz
        3. GPIO: KEY1(PC13),KEY2(PA15),KEY3(PC8)keyboard input
           Keyboard light control: D14(PA3),D6(PA8)keyboard output
    
    Describe the testing steps and phenomena of the Demo

        1. Download the program after compiling and reset it to run;
        2. Connect the J62 USB port via a USB cable, and the computer recognizes the mouse device; 
        3. Press KEY1,KEY2,KEY3, and USB inputs "a", "b", and "c".
        4. Using another keyboard to toggle Capslock and Numlock, D14 and D6 output corresponding ON and OFF.
        
4. Attention
    The device can be switched to Full-Speed mode or High-Speed mode by modify the macro definition USE_USB_HS_IN_FS or USE_USB_HS_IN_HS;
    In addition, external crystal of 16MHz, 19.2MHz, 20MHz, 24MHz, 26MHz or 32MHz is required.