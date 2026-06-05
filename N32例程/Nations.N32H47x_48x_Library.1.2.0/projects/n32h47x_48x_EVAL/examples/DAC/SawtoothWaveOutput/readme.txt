1、功能说明
   高精度定时器或者ATIM触发DAC1生成锯齿波,复位值0x100,步进0x40.
    
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
        3、端口配置：
            PA4选择为模拟功能
            PB14选择为SHRTIM的TD1的输出
        4、DAC配置： 
            N32H474系列：
                DAC1选择步进触发源"SHRTIM1_STEP_TRG4"，DAC1复位触发源为"SHRTIM1_RST_TRG4"
                递增锯齿波步进值0x40 复位值0x100.
            N32H473/475/482/487/488系列：
                DAC1选择步进触发源"DAC_Trigger_ATIM1_TRGO"，DAC1复位触发源为"DAC_Trigger2_Software"
                递增锯齿波步进值0x40 复位值0x100.
        5、SHRTIME 配置：
            TD1输出，连续计数模式
    使用方法：
        1、编译后打开调试模式，利用示波器观察PA4输出波形
        2、全速运行时，可看到PA4输出递增的锯齿波.
4、注意事项
    无


1. Function description
    The SHR timer or ATIM triggers DAC1 to generate a sawtooth wave, with a reset value of 0x100 and a step value of 0x40.

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
            PB14 is selected as the output of SHRTIM's TD
        4. DAC:
            N32H474 series：
                DAC1 selects the step trigger source "DAC_Trigger2_SHRTIM1_STEP_TRG4", and the reset trigger source for DAC1 is "DAC_Trigger_SHRTIM1_RST_TRG4".
                The increment sawtooth step value is 0x40 and Reset value is 0x100.
            N32H473/475/482/487/488 series:
                DAC1 selects the step trigger source "DAC_Trigger_ATIM1_TRGO", and the reset trigger source for DAC1 is "DAC_Trigger2_Software".
                The increment sawtooth step value is 0x40 and Reset value is 0x100.
        5. SHRTIME configuration:
            TD1 output, continuous counting mode
    Usage:
        1. After compiling, open debug mode and observe the output waveform of PA4 using an oscilloscope
        2. When running at full speed, you can see the increasing sawtooth wave output from PA4.
4. Attention
    none
    
