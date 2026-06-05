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
*\*\file main.c
*\*\author Nations
*\*\version v1.0.1
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"
#include "I2CMaster.h"
#include "PMBusMaster.h"
#include "PMBus.h"

uint8_t  Temp[1] = {0};
uint8_t  Oprn[1] = {0};
uint8_t  code[1] = {0};
uint8_t  Stat[2] = {0, 0};
uint8_t  VCmd[2] = {0, 0};
uint16_t Vout_Status = 0;
uint32_t Vout_Command = 0;
//uint8_t  pass_fail = 0;
uint8_t  test_status1  = FAILED;
uint8_t  test_status2  = FAILED;
uint8_t  test_status3  = FAILED;
uint8_t  test_status4  = FAILED;
uint8_t  test_status5  = FAILED;
uint8_t  test_status6  = FAILED;
uint8_t  test_status7  = FAILED;

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none
**/
int main(void)
{
    log_init();
    log_info("\nThis is a pmbus master int demo!\r\n");
    /* Initialize the I2C driver  */
    PMBusMaster_Init();

    //reads
    log_info("pmbus read test\r\n");
    log_info("pmbus master read temperature test!\r\n");
    test_status1 = PMBusMaster(STATUS_TEMPERATURE, 1, 0, Temp);	//read byte
    log_info("temperature = %02x\r\n", Temp[0]);

    log_info("pmbus master read Operation test!\r\n");
    test_status2 = PMBusMaster(OPERATION, 1, 0, Oprn);			      //r/w byte (read)
    log_info("Oprn = %02x\r\n", Oprn[0]);

    log_info("pmbus master read Status_Word test!\r\n");
    test_status3 = PMBusMaster(STATUS_WORD, 1, 0, Stat);		      //read word
    Vout_Status  = (Stat[1] << 8) | Stat[0];			//unpacking the two data bytes into one word
    log_info("Oprn = %04x\r\n", Vout_Status);

    log_info("pmbus master r/w word (read) test!\r\n");
    test_status4 = PMBusMaster(VOUT_COMMAND, 1, 0, VCmd);		    //r/w word (read)
    Vout_Command = (VCmd[1] << 8) | VCmd[0];	    //the lowest order byte is received first
    log_info("Oprn = %04x\r\n", Vout_Command);

    //writes
    log_info("pmbus write test\r\n");
    test_status5 = PMBusMaster(STORE_DEFAULT_CODE, 0, 0xAA, 0);	//write byte  default code only read
    test_status6 = PMBusMaster(OPERATION, 0, 0xBB, 0);			      //r/w byte (write)
    test_status7 = PMBusMaster(VOUT_COMMAND, 0, 0xCCDD, 0);		  //r/w word (write)

    //check if write correct
    test_status2 = PMBusMaster(OPERATION, 1, 0, Oprn);			      //r/w byte (read)

    if(Oprn[0] != 0xBB)
    {
        log_info("pmbus write OPERATION cmd fail\r\n");
    }

    test_status4 = PMBusMaster(VOUT_COMMAND, 1, 0, VCmd);		    //r/w word (read)
    Vout_Command = (VCmd[1] << 8) | VCmd[0];	    //the lowest order byte is received first

    if(Vout_Command != 0xCCDD)
    {
        log_info("pmbus write VOUT_COMMAND cmd fail\r\n");
    }


    if(test_status1 == FAILED || test_status2 == FAILED || test_status3 == FAILED
            || test_status4 == FAILED || test_status5 == FAILED || test_status6 == FAILED
            || test_status7 == FAILED)
    {
        log_info("pmbus master int test fail!\r\n");
    }
    else
    {
        log_info("pmbus master int test pass!\r\n");
    }

    while (1)
    {

    }
}





