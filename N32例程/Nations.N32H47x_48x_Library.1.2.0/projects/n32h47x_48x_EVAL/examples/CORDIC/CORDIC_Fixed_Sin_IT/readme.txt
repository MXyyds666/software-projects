1、功能说明
    Cordic在中断模式下计算sin函数的示例。
    本示例中输入数据为从 0 到 2*pi 有规律地递增的Q1.31 格式的定点数据，存储在数组aInData[]中，长度64*4字节；Cordic计算结果存储于数组aCalResult[]中。Cordic计算结果与参考结果数组aRefResult[]的值进行比较，若低于最大误差值，则计算结果符合预期。最大误差值与配置的Precision有关。


2、使用环境

    软件开发环境：KEIL MDK-ARM V5.34
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
    
    系统配置：
        1、时钟源：HSI+PLL
        2、时钟频率：
            N32H473/474系列：240MHz
            N32H475/482/487/488系列：240MHz
        3、打印：PA9 - baud rate 115200

    Cordic配置：
        1、Function：sin
        2、Precision：6CYCLES（建议最大配置为6cycles）
        3、Scale：0
        4、NbWrite：1
        5、NbRead：1
        6、InSize：32-bits
        7、OutSize：32-bits
        8、InSelect：定点
        9、OutSelect： 定点
        10、CodinLimit：禁用
        11、PhaseLimit：禁用

    使用方法：
        在KEIL下编译后烧录到评估板。
        开启打印输出，可以看到串口输出“CORDIC_Fixed_Sin_IT demo go...”；若计算结果与参考值的差大于最大误差值，则串口输出“Error”；若计算结果与参考值的差小于等于最大误差值，则串口输出“CORDIC demo success!!!”，指示计算结果符合预期。


4、注意事项
     无


1. Function description
    Example of the Cordic compute sin function in interrupt mode.
    In this example, the input data is the fixed-point data in Q1.31 format regularly increasing from 0 to 2*pi, stored in the array aInData[], length 64*4 bytes; the result of the Cordic calculation is stored in the array aCalResult[]. The result of the Cordic calculation is compared with the value of the reference result array aRefResult[]. The result of the Cordic calculation is compared with the value of the reference result array aRefResult[], and if it is lower than the maximum error value, the result of the calculation is as expected. The maximum error value is related to the configured Precision.

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
    
     System Configuration:
        1. Clock source: HSI+PLL
        2. Clock frequency: 
            N32H473/474 series: 240MHz
            N32H475/482/487/488 series: 240MHz
        3. printf: PA9 - baud rate 115200

    Cordic configuration:
        1、Function：sin
        2、Precision：6CYCLES（Maximum recommended configuration is 6cycles）.
        3、Scale：0
        4、NbWrite：1
        5、NbRead：1
        6、InSize：32-bits
        7、OutSize：32-bits
        8、InSelect：Fixed Point
        9、OutSelect： Fixed point
        10、CodinLimit：Disabled
        11、PhaseLimit：Disabled

     Instructions:
        Compile it under KEIL and burn it to the evaluation board.
        Turn on the printout, you can see the serial output "CORDIC_Fixed_Sin_IT demo go..."; If the difference between the calculated result and the reference value is greater than the maximum error value, the serial port outputs "Error"; if the difference between the calculated result and the reference value is less than or equal to the maximum error value, the serial port outputs "CORDIC demo success!!!", indicating that the calculated result meets the expectation.

4. Attention
    None

