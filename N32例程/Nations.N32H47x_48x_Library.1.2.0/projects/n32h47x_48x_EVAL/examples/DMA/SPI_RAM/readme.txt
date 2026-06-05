1、功能说明

    此例程提供了一种DMA用法，用于在外设到RAM之间传输数据。
    
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
    2、主时钟：240MHz
    3、DMA通道：DMA1_CH2，DMA1_CH3，DMA1_CH4，DMA1_CH5
    4、主SPI 配置：
        外设：
            N32H475系列：SPI3
            其他系列：SPI1
        硬件连接：
            N32H475系列：SCK   -->  PD9，MISO  -->  PD11，MOSI  -->  PD6
            其他系列：SCK   -->  PA5，MISO  -->  PA6，MOSI  -->  PA7
        全双工
        主模式
        8bit传输
        极性：起始为低/第二个边沿
        软件片选
        大端在前MSB
    
    5、从SPI 配置：
        外设：
            SPI2
        硬件连接：
            N32H475系列：SCK   -->  PC7，MISO  -->  PC2，MOSI  -->  PC3
            其他系列：SCK   -->  PC7，MISO  -->  PC8，MOSI  -->  PC9
        全双工
        从模式
        8bit传输
        极性：起始为低/第二个边沿
        软件片选
        大端在前MSB

    6、测试步骤与现象
        a，用杜邦线分别将主SPI的SCK/MISO/MOSI连接到从SPI的SCK/MISO/MOSI
        b，编译下载代码复位运行
        c，DMA传输完成，LED PA3 PA8闪烁
        
4、注意事项
    无


1. Function description

    This routine provides a DMA usage for transferring data from peripheral to RAM.  

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
    3. DMA channel: DMA1_CH2, DMA1_CH3, DMA1_CH4, DMA1_CH5
    4. Master SPI Configuration:
        Peripheral:
            N32H475 series: SPI3
            Other series: SPI1
        Hardware Connections:
            N32H475 series: SCK --> PD9, MISO --> PD11, MOSI --> PD6
            Other series: SCK --> PA5, MISO --> PA6, MOSI --> PA7
        Full duplex
        Master Mode
        8bit transmission
        Polarity: start low/second edge
        Software chip select
        Big end in front MSB
    5. Slave SPI Configuration:
        Peripheral:
            SPI2
        Hardware Connections:
            N32H475 series: SCK --> PC7, MISO --> PC2, MOSI --> PC3
            Other series: SCK --> PC7, MISO --> PC8, MOSI --> PC9
        Full duplex
        Slave Mode
        8bit transmission
        Polarity: start low/second edge
        Software chip select
        Big end in front MSB
    6. Test steps and phenomena
        a, Connect the SCK/MISO/MOSI of the master SPI to the SCK/MISO/MOSI of the slave SPI with DuPont cables, respectively
        b, Compile and download the code to reset and run
        c, DMA transmission is completed, LED PA3 PA8 blinking

4. Attention
    None
