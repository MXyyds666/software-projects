1、功能说明

    此例程提供了一种DMA用法，用于在外设与RAM之间传输数据。

2、使用环境

    软件开发环境：
        IDE工具：KEIL MDK-ARM 5.34
	    IAR EWARM 8.50.1
    
    硬件开发环境：
        N32H473系列：
        基于评估板N32H473VEL7_STB V1.0开发
        N32H474系列：
        基于评估板N32H474VEL7_STB V1.0开发
        N32H475系列：
        基于评估板N32H475UEQ7_STB V1.0开发
        N32H482系列：
        基于评估板N32H482ZEL7_STB V1.0开发
        N32H487系列：
        基于全功能板N32H487ZEL7_EVB V1.1开发

3、使用说明

    1、时钟源：HSE+PLL
    2、时钟频率：
        N32H473/474系列：240MHz
        N32H475/482/487/488系列：240MHz
    3、DMA通道：DMA1_CH2，DMA1_CH4
    4、I2C1 配置：
        SCL   -->  PB6
        SDA   -->  PB7
        ADDR：0x30(7bit)
        CLOCK：100K
    5、I2C2 配置：
        SCL   -->  PB10
        SDA   -->  PB11
        ADDR：0xA0(7bit)
        CLOCK：100K
    6、LED1：PA3
    7、测试步骤与现象
        a，连接PB6到PB10，连接PB7到PB11
        b，编译下载代码复位运行
        c，DMA传输完成，LED1闪烁

4、注意事项
    在SDA，SCL上需要加上拉电阻。


1. Function description

    This routine provides a DMA usage for transferring data between a peripheral and RAM.

2. Use environment

    Software development environment: KEIL MDK-ARM V5.34
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
        Developed based on the full-featured board N32H487ZEL7_EVB V1.1


3. Instructions for use

    1. Clock source: HSI+PLL
    2. Clock frequency: 
        N32H473/474 series: 240MHz
        N32H475/482/487/488 series: 240MHz
    3. DMA channel: DMA1_CH2, DMA1_CH4
    4. I2C1 Configuration:
        SCL --> PB6
        SDA --> PB7
        ADDR: 0x30(7bit)
        CLOCK: 100K
    5. I2C2 Configuration:
        SCL --> PB10
        SDA --> PB11
        ADDR: 0xA0(7bit)
        CLOCK: 100K
    6. LED1: PA3
    7. Test steps and phenomena
        a, Connect PB6 to PB10, connect PB7 to PB11
        b, Compile and download the code reset run
        c, DMA transmission is completed, LED1 flashes.

4. Attention
    Pull resistors are required on SDA, SCL.
