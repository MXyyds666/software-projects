/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/


/**
*\*\file      spi_flash.c
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
*/
#include    "n32h47x_48x_xspi.h"
#include    "spi_flash.h"
#include    "xspi_cfg.h"
#include    <stdio.h>
#include    <string.h>

/** User application include */

#define FLASH_WAIT_CNT    1000


/**
*\*\name    xSPI_Wait_Flag.
*\*\fun     Get xSPI wait flag .
*\*\param   xSPIx: XSPI
*\*\param   flag: flag
*\*\param   sta: Target status
*\*\param   wtime: time out count
*\*\return  FAILED_STATUS or TestResultStatus
**/
static TestResultStatus xSPI_Wait_Flag(XSPI_Module* xSPIx,uint32_t flag,uint8_t sta,uint64_t wtime)
{
    TestResultStatus result = SUCCESS_STATUS;
    uint8_t flagsta=0;
    while(wtime)
    {
        flagsta=(xSPIx->STS&flag)?1:0; 
        if(flagsta==sta)break;
        wtime--;
    }
    if(wtime)
    {
        result = SUCCESS_STATUS;
    }
    else
    {
        result = FAILED_STATUS;
    }
    return result;
}

/**
*\*\name    xSPI_Wait_TransferComplete.
*\*\fun     Wait xSPI transfer complete .
*\*\param   xSPIx: XSPI
*\*\param   wtime: time out count
*\*\return  FAILED_STATUS or TestResultStatus
**/
static TestResultStatus xSPI_Wait_TransferComplete(XSPI_Module* xSPIx,uint64_t wtime)
{
    TestResultStatus result = SUCCESS_STATUS;
    uint32_t timeout = wtime;

    result |= xSPI_Wait_Flag(xSPIx,XSPI_FLAG_TFE,SET,timeout);//wait for Transmit FIFO is empty:(0x1 EMPTY)
    timeout = wtime;
    result |= xSPI_Wait_Flag(xSPIx,XSPI_FLAG_BUSY,RESET,timeout);//wait for xSPI is idle or disabled:(0x0 idle)
    
    return result;
}

/**
*\*\name    xSPI_Clear_RXFIFO.
*\*\fun     Clear xSPI RX fifo .
*\*\param   xSPIx: XSPI
*\*\return  FAILED_STATUS or TestResultStatus
**/
static TestResultStatus xSPI_Clear_RXFIFO(XSPI_Module* xSPIx)
{
    TestResultStatus result = SUCCESS_STATUS;
    volatile uint32_t data_clearRX = 0;
    uint32_t wtime = FLASH_WAIT_CNT*1000;
    
    //clear rx FIFO
    while((xSPIx->STS&XSPI_FLAG_RFNE) && (wtime--))// RX FIFO Not Empty
    {
        data_clearRX = xSPIx->DAT0;
    }
    if(wtime)
    {
        result = SUCCESS_STATUS;
    }
    else
    {
        result = FAILED_STATUS;
    }
    return result;
}

/**
*\*\name    XSPI_FlashREG_WR.
*\*\fun     Write read XSPI flash register.
*\*\param   xSPIx: XSPI
*\*\param   cmdbuf: Flash command
*\*\param   rbuf: receive Flash register data
*\*\param   count: data length
*\*\param   type: TYPE_REG_READ or TYPE_REG_WRITE
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus XSPI_FlashREG_WR(uint8_t *cmdbuf,uint8_t *rbuf,uint8_t count,uint8_t type)
{
    uint32_t wtime = FLASH_WAIT_CNT*1000, i;
    uint32_t read_count=0;
    TestResultStatus result = SUCCESS_STATUS;

    if((type == TYPE_REG_READ) && (count == NULL))
    {
        count = 2;
    }
    
    result |= xSPI_Wait_TransferComplete(XSPI,wtime);//wait for xspi transfer complete
    result |= xSPI_Clear_RXFIFO(XSPI);
    
    if(count >= 16)
    {
        return FAILED_STATUS;// send data length over 16, use DMA mode transfer
    }
    else if(count == 0)
    {
        XSPI_SetTXFIFOStartLevel(0);
    }
    else
    {
        XSPI_SetTXFIFOStartLevel(count-1);
    }
    wtime = FLASH_WAIT_CNT*1000;
    if(type == TYPE_REG_READ)/*read register*/
    {
        i=0;
        read_count=0;
        while((i<count)&&(wtime--))
        {
           if(XSPI->STS & XSPI_FLAG_TFNF)//Tx FIFO is not Full:(0x1 NOT_FULL)
           {
               XSPI->DAT0 = cmdbuf[i++];
               wtime = FLASH_WAIT_CNT*1000;
           }
        }
        if(wtime == 0)
        {
            result |= FAILED_STATUS;
        }
        result |= xSPI_Wait_TransferComplete(XSPI, FLASH_WAIT_CNT*1000);//wait for xspi transfer complete
        wtime = FLASH_WAIT_CNT*1000;
        //clear rx FIFO
        while((XSPI->STS&XSPI_FLAG_RFNE)&&(wtime--))// Receive FIFO is not empty:( 0x1 NOT_EMPTY)
        {
            rbuf[read_count++] = XSPI->DAT0;
        }
        if(wtime == 0)
        {
            result |= FAILED_STATUS;
        }
    }
    else if(type == TYPE_REG_WRITE)/*write register*/
    {
        // send data
        wtime = FLASH_WAIT_CNT*1000;
        i=0;
        while((i<count)&&(wtime--))
        {
           if(XSPI->STS&XSPI_FLAG_TFNF)//Tx FIFO is not Full:(0x1 NOT_FULL)
           {
              XSPI->DAT0 = cmdbuf[i++];
              wtime = FLASH_WAIT_CNT*1000;
           }
        }
        if(wtime == 0)
        {
            result |= FAILED_STATUS;
        }
        result |= xSPI_Wait_TransferComplete(XSPI, FLASH_WAIT_CNT*1000);//wait for xspi transfer complete
        result |= xSPI_Clear_RXFIFO(XSPI);    //clear xspi rx fifo
    }
    return result;
}


/**
*\*\name    XSPI_FlashWrite.
*\*\fun     Write data to XSPI flash .
*\*\param   PrgCmd: Program flash command
*\*\param   addr: Program flash address
*\*\param   count: write data length
*\*\param   Wbuf: write data buff
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus XSPI_FlashWrite( uint32_t PrgCmd,uint32_t addr, uint16_t count,uint32_t *Wbuf)
{
    TestResultStatus result = SUCCESS_STATUS;
    uint32_t wtime = FLASH_WAIT_CNT*1000;
    
    XSPI_Cmd(DISABLE);
    /*enable DMA    */
    XSPI_ConfigDMATxLevel(0x08);
    XSPI_ConfigDMARxLevel(0x08);
    XSPI_EnableDMA(XSPI_DMAREQ_TX,ENABLE);
    XSPI_Cmd(ENABLE);
    
    XSPI_SendData(PrgCmd);
    XSPI_SendData(addr);
    DMA_Configuration((uint32_t)&XSPI->DAT0, (uint32_t)Wbuf, (count), DMA_TX, ENABLE);

    /*wait TX end   */
    while(XSPI_GetFlagStatus(XSPI_STS_TXFE) == RESET)/*wait TX FIFO empty*/
    {
        if ((wtime--) == 0)
        {
            result |= FAILED_STATUS;
            break;
        }
    }
    wtime = FLASH_WAIT_CNT*1000;
    while(DMA_GetFlagStatus(DMA_FLAG_TC1,DMA1) == RESET)/*wait DMA transfer complete*/
    {
        if ((wtime--) == 0)
        {
            result |= FAILED_STATUS;
            break;
        }
    }
    /*disable DMA*/
    XSPI_Cmd(DISABLE);
    DMA_Configuration((uint32_t)&XSPI->DAT0, (uint32_t)Wbuf, (count), DMA_TX, DISABLE);
    XSPI_EnableDMA(XSPI_DMAREQ_TX,DISABLE);
  
    return result;
}

/**
*\*\name    XSPI_FlashRead.
*\*\fun     Read data from  XSPI flash .
*\*\param   PrgCmd: Program flash command
*\*\param   addr: Program flash address
*\*\param   len: Read data length
*\*\param   DstBuf: Read data buff
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus XSPI_FlashRead(uint32_t PrgCmd, uint32_t  StrAddr, uint32_t Len, uint32_t *DstBuf)
{
    TestResultStatus result = SUCCESS_STATUS;
    uint32_t wtime = FLASH_WAIT_CNT*1000;
   
    XSPI_Cmd(DISABLE);
	/*enable DMA    */
    XSPI_ConfigDMATxLevel(0x08);
    XSPI_ConfigDMARxLevel(0x08);
    XSPI_EnableDMA(XSPI_DMAREQ_RX,ENABLE);
    XSPI_Cmd(ENABLE);
    
    XSPI_SendData(PrgCmd);
    XSPI_SendData(StrAddr);
    DMA_Configuration((uint32_t)&XSPI->DAT0, (uint32_t)DstBuf, (Len), DMA_RX, ENABLE);

    /*wait RX end  */
    while(DMA_GetFlagStatus(DMA_FLAG_TC2,DMA1) == RESET)/*wait DMA transfer complete*/
    {
        if ((wtime--) == 0)
        {
            result |= FAILED_STATUS;
            break;
        }
    }
    
    /*disable DMA*/
    XSPI_Cmd(DISABLE);
    DMA_Configuration((uint32_t)&XSPI->DAT0, (uint32_t)DstBuf, (Len), DMA_RX, DISABLE);
    XSPI_EnableDMA(XSPI_DMAREQ_TX,DISABLE);
    
    return result;
}

/**
*\*\name    sFLASH_WriteEnable.
*\*\fun     Enables the write access to the FLASH.
*\*\param   void
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_WriteEnable(void)
{
    TestResultStatus flag = SUCCESS_STATUS;
    uint32_t timeout = FLASH_WAIT_CNT*500;
    unsigned char bufw[4] = {0xff,0xff};
    unsigned char bufr[4] = {0xff,0xff};
    
    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CMDCLKDIV);
    
    bufw[0] = SPIFLASH_Write_Enable;
    XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_WRITE);
    do/*write enable*/
    {
        bufw[0] = SPIFLASH_Read_Reg1;
        XSPI_FlashREG_WR(bufw,bufr,NULL,TYPE_REG_READ);
    }while(((bufr[1]&0x03) != 0x02)  && (timeout--));
    
    if(((bufr[1]&0x03) != 0x02) && (timeout == 0))
    {
        flag = FAILED_STATUS; 
    }
    return flag;
}

/**
*\*\name    sFLASH_WaitForWriteEnd.
*\*\fun     Polls the status of the Write In Progress (WIP) flag in the FLASH's status register and loop until write opertaion has completed.
*\*\param   void
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_WaitForWriteEnd(void)
{
    TestResultStatus flag = SUCCESS_STATUS;
    uint32_t timeout = FLASH_WAIT_CNT*500;
    unsigned char bufw[4] = {0xff,0xff};
    unsigned char bufr[4] = {0xff,0xff};

    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CMDCLKDIV);
    
    do/*Waiting for flash idle*/
    {
        bufw[0] = SPIFLASH_Read_Reg1;
        XSPI_FlashREG_WR(bufw,bufr,NULL,TYPE_REG_READ);
    }while(((bufr[1]&0x03) != 0x00) && (timeout--));
    
    if(((bufr[1]&0x03) != 0x00) && (timeout == 0))
    {
        flag = FAILED_STATUS; 
    }
    return flag;
}

/**
*\*\name    sFLASH_QuadEnable.
*\*\fun     Enable the Quad mode of Flash
*\*\param   void
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_QuadEnable(void)
{
    TestResultStatus flag = SUCCESS_STATUS;
    uint32_t timeout = FLASH_WAIT_CNT;
    unsigned char bufw[4] = {0xff,0xff};
    unsigned char bufr[4] = {0xff,0xff};

    GPIO_HD_WP_Configuration();
    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CMDCLKDIV);
    flag |= sFLASH_WriteEnable();
    
    do/*set QE*/
    {
#if EXTERNAL_FLASH == FLASH_P25Q40
        bufw[0] = SPIFLASH_Write_Reg1;
        bufw[1] = 0x00;
        bufw[2] = SPIFLASH_REG2CMD_SETQE;
        XSPI_FlashREG_WR(bufw,bufr,3,TYPE_REG_WRITE);
#else 
        bufw[0] = SPIFLASH_Write_Reg2;
        bufw[1] = SPIFLASH_REG2CMD_SETQE;
        XSPI_FlashREG_WR(bufw,bufr,2,TYPE_REG_WRITE);
#endif
        bufw[0] = SPIFLASH_Read_Reg2;
        XSPI_FlashREG_WR(bufw,bufr,NULL,TYPE_REG_READ);
    }while(!(bufr[1]&SPIFLASH_REG2CMD_SETQE) && timeout--);
    
    if(!(bufr[1]&SPIFLASH_REG2CMD_SETQE) && (timeout == 0))
    {
        flag |= FAILED_STATUS;
    }
    else
    {
        flag |= sFLASH_WaitForWriteEnd();
    }
    GPIO_Configuration();
    
    return flag;

}

/**
*\*\name    sFLASH_EraseSector.
*\*\fun     Erases the specified FLASH sector(erase 4k).
*\*\param   SectorAddr:(4k byte alignment) address of the sector to erase.
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_EraseSector(uint32_t SectorAddr)
{
    TestResultStatus flag = SUCCESS_STATUS;
    unsigned char bufw[4] = {0xff,0xff};
    unsigned char bufr[4] = {0xff,0xff};

    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CMDCLKDIV);
    flag |= sFLASH_WriteEnable();

    /*Erase flash*/
    bufw[0] = SPIFLASH_Block_Erase4KB;
    bufw[1] = (SectorAddr & 0xff0000) >> 16;
    bufw[2] = (SectorAddr & 0xff00) >> 8;
    bufw[3] = SectorAddr & 0xff;
    flag |= XSPI_FlashREG_WR(bufw,bufr,4,TYPE_REG_WRITE);
    
    flag |= sFLASH_WaitForWriteEnd();
    return flag;
}

/**
*\*\name    sFLASH_EraseChip.
*\*\fun     Erases the specified FLASH chip
*\*\param   void
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_EraseChip(void)
{
    TestResultStatus flag = SUCCESS_STATUS;
    unsigned char bufw[4] = {0xff,0xff};
    unsigned char bufr[4] = {0xff,0xff};

    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CMDCLKDIV);
    flag |= sFLASH_WriteEnable();

    /*Erase flash*/
    bufw[0] = SPIFLASH_Block_EraseChip;
    flag |= XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_WRITE);

    flag |= sFLASH_WaitForWriteEnd();
    return flag;
}

/**
*\*\name    sFLASH_ReadBuffer.
*\*\fun     Reads a block of data from the FLASH.
*\*\param   pBuffer pointer to the buffer that receives the data read from the FLASH.
*\*\param   ReadAddr(4 byte alignment) FLASH's internal address to read from.
*\*\param   NumWordToRead(1 word alignment) number of words to read from the FLASH.
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_ReadBuffer(uint32_t* pBuffer, uint32_t ReadAddr, uint32_t NumWordToRead)
{
    TestResultStatus flag = SUCCESS_STATUS;
    XSPI_Configuration(QUAD_SPI_FORMAT,RX_ONLY,DFS_32_BIT,(NumWordToRead-1),TEST_WR_CLKDIV);
    
    flag |= XSPI_FlashRead(SPIFLASH_Read_QUAD,ReadAddr,NumWordToRead,pBuffer);

    return flag;
}

/**
*\*\name    sFLASH_WritePage.
*\*\fun     Writes more than one byte to the FLASH with a single WRITE cycle (Page WRITE sequence).
*\*\param   pBuffer pointer to the buffer that write the data to from the FLASH.
*\*\param   WriteAddr(4 byte alignment) FLASH's internal address to write to.
*\*\param   NumWordToWrite(1 word alignment) number of bytes to write to the FLASH, must be equal
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_WritePage(uint32_t* pBuffer, uint32_t WriteAddr, uint32_t NumWordToWrite)
{
    TestResultStatus flag = SUCCESS_STATUS;
    if(NumWordToWrite != 0)
    {
        flag |= sFLASH_WriteEnable();

        XSPI_Configuration(QUAD_SPI_FORMAT,TX_ONLY,DFS_32_BIT,(NumWordToWrite-1),TEST_WR_CLKDIV);
        
        flag |= XSPI_FlashWrite(SPIFLASH_QuadPage_Pro,WriteAddr,NumWordToWrite,pBuffer);

        /* Wait the end of Flash writing */
        flag |= sFLASH_WaitForWriteEnd();
    }
    return flag;
}

/**
*\*\name    sFLASH_WriteBuffer.
*\*\fun     Writes block of data to the FLASH. In this function, the number of WRITE cycles are reduced, using Page WRITE sequence.
*\*\param   pBuffer pointer to the buffer  containing the data to be written to the FLASH.
*\*\param   WriteAddr(4 byte alignment) FLASH's internal address to write to.
*\*\param   NumWordToWrite(1 word alignment) number of bytes to write to the FLASH.
*\*\return  FAILED_STATUS or TestResultStatus
**/
TestResultStatus sFLASH_WriteBuffer(uint32_t* pBuffer, uint32_t WriteAddr, uint32_t NumWordToWrite)
{
    TestResultStatus flag = SUCCESS_STATUS;
    uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0, byte_size = 0;
    
    byte_size = (sFLASH_SPI_PAGESIZE*4);

    Addr        = WriteAddr % byte_size;
    count       = (byte_size - Addr)/4;
    NumOfPage   = NumWordToWrite / sFLASH_SPI_PAGESIZE;
    NumOfSingle = NumWordToWrite % sFLASH_SPI_PAGESIZE;

    if (Addr == 0) /* WriteAddr is sFLASH_PAGESIZE aligned  */
    {
        if (NumOfPage == 0) /* NumWordToWrite < sFLASH_PAGESIZE */
        {
            flag |= sFLASH_WritePage(pBuffer, WriteAddr, NumWordToWrite);
        }
        else /* NumWordToWrite > sFLASH_PAGESIZE */
        {
            while (NumOfPage--)
            {
                flag |= sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
                WriteAddr += byte_size;
                pBuffer += sFLASH_SPI_PAGESIZE;
            }
            flag |= sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else /* WriteAddr is not sFLASH_PAGESIZE aligned  */
    {
        if (NumOfPage == 0) /* NumWordToWrite < sFLASH_PAGESIZE */
        {
            if (NumOfSingle > count) /* (NumWordToWrite + WriteAddr) > sFLASH_PAGESIZE */
            {
                temp = NumOfSingle - count;

                flag |= sFLASH_WritePage(pBuffer, WriteAddr, count);
                WriteAddr += (count*4);
                pBuffer += count;

                flag |= sFLASH_WritePage(pBuffer, WriteAddr, temp);
            }
            else
            {
                flag |= sFLASH_WritePage(pBuffer, WriteAddr, NumWordToWrite);
            }
        }
        else /* NumWordToWrite > sFLASH_PAGESIZE */
        {
            NumWordToWrite -= count;
            NumOfPage   = NumWordToWrite / sFLASH_SPI_PAGESIZE;
            NumOfSingle = NumWordToWrite % sFLASH_SPI_PAGESIZE;

            flag |= sFLASH_WritePage(pBuffer, WriteAddr, count);
            WriteAddr += (count*4);
            pBuffer += count;

            while (NumOfPage--)
            {
                flag |= sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
                WriteAddr += byte_size;
                pBuffer += sFLASH_SPI_PAGESIZE;
            }

            flag |= sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
            
        }
    }
    return flag;
}
