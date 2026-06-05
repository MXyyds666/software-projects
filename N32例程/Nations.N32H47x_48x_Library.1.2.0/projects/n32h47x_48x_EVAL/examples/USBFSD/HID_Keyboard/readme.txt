1、功能说明
    USB HID 键盘设备

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
		 3. USBClock: 48MHz
         4. GPIO：KEY1(PC13),KEY2(PA15),KEY3(PC8)键盘输入
            键盘灯控制：D14(PA3),D6(PA8)键盘输出

    描述Demo的测试步骤和现象 
         1. 编译后下载程序复位运行；
         2. 通过 USB 线连接 J4 USB 口，电脑识别出键盘设备，按下KEY1,KEY2,KEY3按键，USB 输入 "a","b","c"，
            用另外一个键盘开关Capslock和Numlock，可以看到D14和D6对应键盘灯亮灭

4、注意事项
    无

1. Function description
    USB HID keyboard device

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
        1. SystemClock: 192MHz(N32H473 and N32H474 series)，240MHz(N32H482 and N32H487/488 series)
        2. HSE: 16MHz
		3. USBClock: 48MHz
        4. GPIO: KEY1(PC13),KEY2(PA15),KEY3(PC8)keyboard input 
           Keyboard light control: D14(PA3),D6(PA8)keyboard output
    
    Describe the testing steps and phenomena of the Demo

        1. Download the program after compiling and reset it to run;
        2. Connect the J4 USB port via a USB cable, and the computer recognizes the keyboard device. Press KEY1,KEY2,KEY3, and USB inputs "a", "b", and "c".
           Using another keyboard to toggle Capslock and Numlock, D14 and D6 output corresponding ON and OFF.
 
4. Attention
    None.