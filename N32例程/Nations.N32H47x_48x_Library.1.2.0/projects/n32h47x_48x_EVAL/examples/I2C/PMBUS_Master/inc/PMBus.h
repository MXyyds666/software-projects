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

#ifndef PMBUS_H
#define PMBUS_H

#include "stdint.h"


#define PEC		1	//PEC 0: Disable Packet Error Checking  1: Enable Packet Error Checking
								
#if PEC
	#define PEC_PASS           	1
	#define PEC_FAIL           	0
	#define CRC8_POLY	   		    0x07
	#define CRC8_INIT_REM      	0x0
#endif

#define READBYTE      0
#define READWORD      2
#define WRITEBYTE     4
#define SENDBYTE      6
#define WRITEWORD     8
#define RWBYTE		    1
#define RWWORD		    3

//GROUP#0
#define CAPABILITY                          1
#define STATUS_BYTE                         2
#define STATUS_VOUT                         3
#define STATUS_IOUT                         4
#define STATUS_INPUT                        5
#define STATUS_TEMPERATURE                  6
#define STATUS_CML                          7
#define STATUS_OTHER                        8
#define STATUS_MFR_SPECIFIC                 9
#define STATUS_FANS_1_2                     10
#define STATUS_FANS_3_4                     11
#define PMBUS_REVISION                      12

// GROUP #2 
#define STATUS_WORD                         13
#define READ_VIN                            14
#define READ_IIN                            15
#define READ_VCAP                           16
#define READ_VOUT                           17
#define READ_IOUT                           18
#define READ_TEMPERATURE_1                  19
#define READ_TEMPERATURE_2                  20
#define READ_TEMPERATURE_3                  21
#define READ_FAN_SPEED_1                    22
#define READ_FAN_SPEED_2                    23
#define READ_FAN_SPEED_3                    24
#define READ_FAN_SPEED_4                    25
#define READ_DUTY_CYCLE                     26
#define READ_FREQUENCY                      27
#define READ_POUT                           28
#define READ_PIN                            29
#define MFR_VIN_MIN                         30
#define MFR_VIN_MAX                         31
#define MFR_IIN_MAX                         32
#define MFR_PIN_MAX                         33
#define MFR_VOUT_MIN                        34
#define MFR_VOUT_MAX                        35
#define MFR_IOUT_MAX                        36
#define MFR_POUT_MAX                        37
#define MFR_TAMBIENT_MAX                    38
#define MFR_TAMBIENT_MIN                    39

// GROUP #4
#define STORE_DEFAULT_CODE                  40
#define RESTORE_DEFAULT_CODE                41
#define STORE_USER_CODE                     42
#define RESTORE_USER_CODE                   43

// GROUP#6
#define CLEAR_FAULTS                        44
#define STORE_DEFAULT_ALL                   45
#define RESTORE_DEFAULT_ALL                 46
#define STORE_USER_ALL                      47
#define RESTORE_USER_ALL                    48

//GROUP#8
#define PAGE                                49
#define OPERATION                           50
#define ON_OFF_CONFIG                       51
#define PHASE                               52
#define WRITE_PROTECT                       53
#define VOUT_MODE                           54
#define FAN_CONFIG_1_2                      55
#define FAN_CONFIG_3_4                      56
#define VOUT_OV_FAULT_RESPONSE              57
#define VOUT_UV_FAULT_RESPONSE              58
#define IOUT_OC_FAULT_RESPONSE              59
#define IOUT_0C_LV_FAULT_RESPONSE           60
#define IOUT_UC_FAULT_RESPONSE              61
#define OT_FAULT_RESPONSE                   62
#define UT_FAULT_RESPONSE                   63
#define VIN_OV_FAULT_RESPONSE               64
#define VIN_UV_FAULT_RESPONSE               65
#define IIN_OC_FAULT_RESPONSE               66
#define TON_MAX_FAULT_RESPONSE              67
#define POUT_OP_FAULT_RESPONSE              68

//GROUP#10 
#define VOUT_COMMAND                        69
#define VOUT_TRIM                           70
#define VOUT_CAL_OFFSET                     71
#define VOUT_MAX                            72
#define VOUT_MARGIN_HIGH                    73
#define VOUT_MARGIN_LOW                     74
#define VOUT_TRANSITION_RATE                75
#define VOUT_DROOP                          76
#define VOUT_SCALE_LOOP                     77
#define VOUT_SCALE_MONITOR                  78
#define POUT_MAX                            79
#define MAX_DUTY                            80
#define FREQUENCY_SWITCH                    81
#define VIN_ON                              82
#define VIN_OFF                             83
#define INTERLEAVE                          84
#define IOUT_CAL_GAIN                       85
#define IOUT_CAL_OFFSET                     86
#define FAN_COMMAND_1                       87
#define FAN_COMMAND_2                       88
#define FAN_COMMAND_3                       89
#define FAN_COMMAND_4                       90
#define VOUT_OV_FAULT_LIMIT                 91
#define VOUT_OV_WARN_LIMIT                  92
#define VOUT_UV_WARN_LIMIT                  93
#define VOUT_UV_FAULT_LIMIT                 94
#define IOUT_OC_FAULT_LIMIT                 95
#define IOUT_OC_LV_FAULT_LIMIT              96
#define IOUT_OC_WARN_LIMIT                  97
#define IOUT_UC_FAULT_LIMIT                 98
#define OT_FAULT_LIMIT                      99
#define OT_WARN_LIMIT                       100
#define UT_WARN_LIMIT                       101
#define UT_FAULT_LIMIT                      102
#define VIN_OV_FAULT_LIMIT                  103
#define VIN_OV_WARN_LIMIT                   104
#define VIN_UV_WARN_LIMIT                   105
#define VIN_UV_FAULT_LIMIT                  106
#define IIN_OC_FAULT_LIMIT                  107
#define IIN_OC_WARN_LIMIT                   108
#define POWER_GOOD_ON                       109
#define POWER_GOOD_OFF                      110
#define TON_DELAY                           111
#define TON_RISE                            112
#define TON_MAX_FAULT_LIMIT                 113
#define TOFF_DELAY                          114
#define TOFF_FALL                           115
#define TOFF_MAX_WARN_LIMIT                 116
#define POUT_OP_FAULT_LIMIT                 117
#define POUT_OP_WARN_LIMIT                  118
#define PIN_OP_WARN_LIMIT                   119 

/* ----------- PMBUS command codes definition -------- */
#define PMBC_PAGE                       ((uint8_t)0x00)
#define PMBC_OPERATION                  ((uint8_t)0x01)
#define PMBC_ON_OFF_CONFIG              ((uint8_t)0x02)
#define PMBC_CLEAR_FAULTS               ((uint8_t)0x03)
#define PMBC_PHASE                      ((uint8_t)0x04)
#define PMBC_PAGE_PLUS_WRITE            ((uint8_t)0x05)
#define PMBC_PAGE_PLUS_READ             ((uint8_t)0x06)
#define PMBC_ZONE_CONFIG                ((uint8_t)0x07)
#define PMBC_ZONE_ACTIVE                ((uint8_t)0x08)
#define PMBC_WRITE_PROTECT              ((uint8_t)0x10)
#define PMBC_STORE_DEFAULT_ALL          ((uint8_t)0x11)
#define PMBC_RESTORE_DEFAULT_ALL        ((uint8_t)0x12)
#define PMBC_STORE_DEFAULT_CODE         ((uint8_t)0x13)
#define PMBC_RESTORE_DEFAULT_CODE       ((uint8_t)0x14)
#define PMBC_STORE_USER_ALL             ((uint8_t)0x15)
#define PMBC_RESTORE_USER_ALL           ((uint8_t)0x16)
#define PMBC_STORE_USER_CODE            ((uint8_t)0x17)
#define PMBC_RESTORE_USER_CODE          ((uint8_t)0x18)
#define PMBC_CAPABILITY                 ((uint8_t)0x19)
#define PMBC_QUERY                      ((uint8_t)0x1A)
#define PMBC_SMBALERT_MASK              ((uint8_t)0x1B)
#define PMBC_VOUT_MODE                  ((uint8_t)0x20)
#define PMBC_VOUNT_COMMAND              ((uint8_t)0x21)
#define PMBC_VOUT_TRIM                  ((uint8_t)0x22)
#define PMBC_VOUT_CAL_OFFSET            ((uint8_t)0x23)
#define PMBC_VOUT_MAX                   ((uint8_t)0x24)
#define PMBC_VOUT_MARGIN_HIGH           ((uint8_t)0x25)
#define PMBC_VOUT_MARGIN_LOW            ((uint8_t)0x26)
#define PMBC_VOUT_TRANSITION_RATE       ((uint8_t)0x27)
#define PMBC_VOUT_DROOP                 ((uint8_t)0x28)
#define PMBC_VOUT_SCALE_LOOP            ((uint8_t)0x29)
#define PMBC_VOUT_SCALE_MONITOR         ((uint8_t)0x2A)
#define PMBC_VOUT_MIN                   ((uint8_t)0x2B)
#define PMBC_COEFICIENTS                ((uint8_t)0x30)
#define PMBC_POUT_MAX                   ((uint8_t)0x31)
#define PMBC_MAX_DUTY                   ((uint8_t)0x32)
#define PMBC_FREQUENCY_SWITCH           ((uint8_t)0x33)
#define PMBC_POWER_MODE                 ((uint8_t)0x34)
#define PMBC_VIN_ON                     ((uint8_t)0x35)
#define PMBC_VIN_OFF                    ((uint8_t)0x36)
#define PMBC_INTERLEAVE                 ((uint8_t)0x37)
#define PMBC_IOUT_CAL_GAIN              ((uint8_t)0x38)
#define PMBC_IOUT_CAL_OFFSET            ((uint8_t)0x39)
#define PMBC_FAN_CONFIG_1_2             ((uint8_t)0x3A)
#define PMBC_FAN_COMMAND_1              ((uint8_t)0x3B)
#define PMBC_FAN_COMMAND_2              ((uint8_t)0x3C)
#define PMBC_FAN_CONFIG_3_4             ((uint8_t)0x3D)
#define PMBC_FAN_COMMAND_3              ((uint8_t)0x3E)
#define PMBC_FAN_COMMAND_4              ((uint8_t)0x3F)
#define PMBC_VOUT_OV_FAULT_LIMIT        ((uint8_t)0x40)
#define PMBC_VOUT_OV_FAULT_RESPONSE     ((uint8_t)0x41)
#define PMBC_VOUT_OV_WARN_LIMIT         ((uint8_t)0x42)
#define PMBC_VOUT_UV_WARN_LIMIT         ((uint8_t)0x43)
#define PMBC_VOUT_UV_FAULT_LIMIT        ((uint8_t)0x44)
#define PMBC_VOUT_UV_FAULT_RESPONSE     ((uint8_t)0x45)
#define PMBC_IOUT_OC_FAULT_LIMIT        ((uint8_t)0x46)
#define PMBC_IOUT_OC_FAULT_RESPONSE     ((uint8_t)0x47)
#define PMBC_IOUT_OC_LV_FAULT_LIMIT     ((uint8_t)0x48)
#define PMBC_IOUT_OC_LV_FAULT_RESPONSE  ((uint8_t)0x49)
#define PMBC_IOUT_OC_WARN_LIMIT         ((uint8_t)0x4A)
#define PMBC_IOUT_UC_FAULT_LIMIT        ((uint8_t)0x4B)
#define PMBC_IOUT_UC_FAULT_RESPONSE     ((uint8_t)0x4C)
#define PMBC_OT_FAULT_LIMIT             ((uint8_t)0x4F)
#define PMBC_OT_FAULT_RESPONSE          ((uint8_t)0x50)
#define PMBC_OT_WARN_LIMIT              ((uint8_t)0x51)
#define PMBC_UT_WARN_LIMIT              ((uint8_t)0x52)
#define PMBC_UT_FAULT_LIMIT             ((uint8_t)0x53)
#define PMBC_UT_FAULT_RESPONSE          ((uint8_t)0x54)
#define PMBC_VIN_OV_FAULT_LIMIT         ((uint8_t)0x55)
#define PMBC_VIN_OV_FAULT_RESPONSE      ((uint8_t)0x56)
#define PMBC_VIN_OV_WARN_LIMIT          ((uint8_t)0x57)
#define PMBC_VIN_UV_WARN_LIMIT          ((uint8_t)0x58)
#define PMBC_VIN_UV_FAULT_LIMIT         ((uint8_t)0x59)
#define PMBC_VIN_UV_FAULT_RESPONSE      ((uint8_t)0x5A)
#define PMBC_IIN_OC_FAULT_LIMIT         ((uint8_t)0x5B)
#define PMBC_IIN_OC_FAULT_RESPONSE      ((uint8_t)0x5C)
#define PMBC_IIN_OC_WARN_LIMIT          ((uint8_t)0x5D)
#define PMBC_POWER_GOOD_ON              ((uint8_t)0x5E)
#define PMBC_POWER_GOOD_OFF             ((uint8_t)0x5F)
#define PMBC_TON_DELAY                  ((uint8_t)0x60)
#define PMBC_TON_RISE                   ((uint8_t)0x61)
#define PMBC_TON_MAX_FAULT_LIMIT        ((uint8_t)0x62)
#define PMBC_TON_MAX_FAULT_RESPONSE     ((uint8_t)0x63)
#define PMBC_TOFF_DELAY                 ((uint8_t)0x64)
#define PMBC_TOFF_FALL                  ((uint8_t)0x65)
#define PMBC_TOFF_MAX_WARN_LIMIT        ((uint8_t)0x66)
#define PMBC_TOFF_MAX_FAULT_RESPONSE    ((uint8_t)0x67)  /* Removed in v1.1*/
#define PMBC_POUT_OP_FAULT_LIMIT        ((uint8_t)0x68)
#define PMBC_POUT_OP_FAULT_RESPONSE     ((uint8_t)0x69)
#define PMBC_POUT_OP_WARN_LIMIT         ((uint8_t)0x6A)
#define PMBC_PIN_OP_WARN_LIMIT          ((uint8_t)0x6B)
#define PMBC_STATUS_BYTE                ((uint8_t)0x78)
#define PMBC_STATUS_WORD                ((uint8_t)0x79)
#define PMBC_STATUS_VOUT                ((uint8_t)0x7A)
#define PMBC_STATUS_IOUT                ((uint8_t)0x7B)
#define PMBC_STATUS_INPUT               ((uint8_t)0x7C)
#define PMBC_STATUS_TEMPERATURE         ((uint8_t)0x7D)
#define PMBC_STATUS_CML                 ((uint8_t)0x7E)
#define PMBC_STATUS_OTHER               ((uint8_t)0x7F)
#define PMBC_STATUS_MFR_SPECIFIC        ((uint8_t)0x80)
#define PMBC_STATUS_FANS_1_2            ((uint8_t)0x81)
#define PMBC_STATUS_FANS_3_4            ((uint8_t)0x82)
#define PMBC_READ_KWH_IN                ((uint8_t)0x83)
#define PMBC_READ_KWH_OUT               ((uint8_t)0x84)
#define PMBC_READ_KWH_CONFIG            ((uint8_t)0x85)
#define PMBC_READ_EIN                   ((uint8_t)0x86)
#define PMBC_READ_EOUT                  ((uint8_t)0x87)
#define PMBC_READ_VIN                   ((uint8_t)0x88)
#define PMBC_READ_IIN                   ((uint8_t)0x89)
#define PMBC_READ_VCAP                  ((uint8_t)0x8A)
#define PMBC_READ_VOUT                  ((uint8_t)0x8B)
#define PMBC_READ_IOUT                  ((uint8_t)0x8C)
#define PMBC_READ_TEMPERATURE_1         ((uint8_t)0x8D)
#define PMBC_READ_TEMPERATURE_2         ((uint8_t)0x8E)
#define PMBC_READ_TEMPERATURE_3         ((uint8_t)0x8F)
#define PMBC_READ_FAN_SPEED_1           ((uint8_t)0x90)
#define PMBC_READ_FAN_SPEED_2           ((uint8_t)0x91)
#define PMBC_READ_FAN_SPEED_3           ((uint8_t)0x92)
#define PMBC_READ_FAN_SPEED_4           ((uint8_t)0x93)
#define PMBC_READ_DUTY_CYCLE            ((uint8_t)0x94)
#define PMBC_READ_FREQUENCY             ((uint8_t)0x95)
#define PMBC_READ_POUT                  ((uint8_t)0x96)
#define PMBC_READ_PIN                   ((uint8_t)0x97)
#define PMBC_PMBUS_REVISION             ((uint8_t)0x98)
#define PMBC_MFR_ID                     ((uint8_t)0x99)
#define PMBC_MFR_MODEL                  ((uint8_t)0x9A)
#define PMBC_MFR_REVISION               ((uint8_t)0x9B)
#define PMBC_MFR_LOCATION               ((uint8_t)0x9C)
#define PMBC_MFR_DATE                   ((uint8_t)0x9D)
#define PMBC_MFR_SERIAL                 ((uint8_t)0x9E)
#define PMBC_APP_PROFILE_SUPPORT        ((uint8_t)0x9F)
#define PMBC_MFR_VIN_MIN                ((uint8_t)0xA0)
#define PMBC_MFR_VIN_MAX                ((uint8_t)0xA1)
#define PMBC_MFR_IIN_MAX                ((uint8_t)0xA2)
#define PMBC_MFR_PIN_MAX                ((uint8_t)0xA3)
#define PMBC_MFR_VOUT_MIN               ((uint8_t)0xA4)
#define PMBC_MFR_VOUT_MAX               ((uint8_t)0xA5)
#define PMBC_MFR_IOUT_MAX               ((uint8_t)0xA6)
#define PMBC_MFR_POUT_MAX               ((uint8_t)0xA7)
#define PMBC_MFR_TAMBIENT_MAX           ((uint8_t)0xA8)
#define PMBC_MFR_TAMBIENT_MIN           ((uint8_t)0xA9)
#define PMBC_MFR_EFFICIENCY_LL          ((uint8_t)0xAA)
#define PMBC_MFR_EFFICIENCY_HL          ((uint8_t)0xAB)
#define PMBC_MFR_PIN_ACCURACY           ((uint8_t)0xAC)
#define PMBC_IC_DEVICE_ID               ((uint8_t)0xAD)
#define PMBC_IC_DEVICE_REV              ((uint8_t)0xAE)
#define PMBC_USER_DATA_00               ((uint8_t)0xB0)
#define PMBC_USER_DATA_01               ((uint8_t)0xB1)
#define PMBC_USER_DATA_02               ((uint8_t)0xB2)
#define PMBC_USER_DATA_03               ((uint8_t)0xB3)
#define PMBC_USER_DATA_04               ((uint8_t)0xB4)
#define PMBC_USER_DATA_05               ((uint8_t)0xB5)
#define PMBC_USER_DATA_06               ((uint8_t)0xB6)
#define PMBC_USER_DATA_07               ((uint8_t)0xB7)
#define PMBC_USER_DATA_08               ((uint8_t)0xB8)
#define PMBC_USER_DATA_09               ((uint8_t)0xB9)
#define PMBC_USER_DATA_10               ((uint8_t)0xBA)
#define PMBC_USER_DATA_11               ((uint8_t)0xBB)
#define PMBC_USER_DATA_12               ((uint8_t)0xBC)
#define PMBC_USER_DATA_13               ((uint8_t)0xBD)
#define PMBC_USER_DATA_14               ((uint8_t)0xBE)
#define PMBC_USER_DATA_15               ((uint8_t)0xBF)
#define PMBC_MFR_MAX_TEMP_1             ((uint8_t)0xC0)
#define PMBC_MFR_MAX_TEMP_2             ((uint8_t)0xC1)
#define PMBC_MFR_MAX_TEMP_3             ((uint8_t)0xC2)
/* MFR_SPECIFIC from v1.3.1 on */
#define PMBC_MFR_SPECIFIC_C4            ((uint8_t)0xC4)
#define PMBC_MFR_SPECIFIC_C5            ((uint8_t)0xC5)
#define PMBC_MFR_SPECIFIC_C6            ((uint8_t)0xC6)
#define PMBC_MFR_SPECIFIC_C7            ((uint8_t)0xC7)
#define PMBC_MFR_SPECIFIC_C8            ((uint8_t)0xC8)
#define PMBC_MFR_SPECIFIC_C9            ((uint8_t)0xC9)
#define PMBC_MFR_SPECIFIC_CA            ((uint8_t)0xCA)
#define PMBC_MFR_SPECIFIC_CB            ((uint8_t)0xCB)
#define PMBC_MFR_SPECIFIC_CC            ((uint8_t)0xCC)
#define PMBC_MFR_SPECIFIC_CD            ((uint8_t)0xCD)
#define PMBC_MFR_SPECIFIC_CE            ((uint8_t)0xCE)
#define PMBC_MFR_SPECIFIC_CF            ((uint8_t)0xCF)
#define PMBC_MFR_SPECIFIC_D0            ((uint8_t)0xD0)
#define PMBC_MFR_SPECIFIC_D1            ((uint8_t)0xD1)
#define PMBC_MFR_SPECIFIC_D2            ((uint8_t)0xD2)
#define PMBC_MFR_SPECIFIC_D3            ((uint8_t)0xD3)
#define PMBC_MFR_SPECIFIC_D4            ((uint8_t)0xD4)
#define PMBC_MFR_SPECIFIC_D5            ((uint8_t)0xD5)
#define PMBC_MFR_SPECIFIC_D6            ((uint8_t)0xD6)
#define PMBC_MFR_SPECIFIC_D7            ((uint8_t)0xD7)
#define PMBC_MFR_SPECIFIC_D8            ((uint8_t)0xD8)
#define PMBC_MFR_SPECIFIC_D9            ((uint8_t)0xD9)
#define PMBC_MFR_SPECIFIC_DA            ((uint8_t)0xDA)
#define PMBC_MFR_SPECIFIC_DB            ((uint8_t)0xDB)
#define PMBC_MFR_SPECIFIC_DC            ((uint8_t)0xDC)
#define PMBC_MFR_SPECIFIC_DD            ((uint8_t)0xDD)
#define PMBC_MFR_SPECIFIC_DE            ((uint8_t)0xDE)
#define PMBC_MFR_SPECIFIC_DF            ((uint8_t)0xDF)
#define PMBC_MFR_SPECIFIC_E0            ((uint8_t)0xE0)
#define PMBC_MFR_SPECIFIC_E1            ((uint8_t)0xE1)
#define PMBC_MFR_SPECIFIC_E2            ((uint8_t)0xE2)
#define PMBC_MFR_SPECIFIC_E3            ((uint8_t)0xE3)
#define PMBC_MFR_SPECIFIC_E4            ((uint8_t)0xE4)
#define PMBC_MFR_SPECIFIC_E5            ((uint8_t)0xE5)
#define PMBC_MFR_SPECIFIC_E6            ((uint8_t)0xE6)
#define PMBC_MFR_SPECIFIC_E7            ((uint8_t)0xE7)
#define PMBC_MFR_SPECIFIC_E8            ((uint8_t)0xE8)
#define PMBC_MFR_SPECIFIC_E9            ((uint8_t)0xE9)
#define PMBC_MFR_SPECIFIC_EA            ((uint8_t)0xEA)
#define PMBC_MFR_SPECIFIC_EB            ((uint8_t)0xEB)
#define PMBC_MFR_SPECIFIC_EC            ((uint8_t)0xEC)
#define PMBC_MFR_SPECIFIC_ED            ((uint8_t)0xED)
#define PMBC_MFR_SPECIFIC_EE            ((uint8_t)0xEE)
#define PMBC_MFR_SPECIFIC_EF            ((uint8_t)0xEF)
#define PMBC_MFR_SPECIFIC_F0            ((uint8_t)0xF0)
#define PMBC_MFR_SPECIFIC_F1            ((uint8_t)0xF1)
#define PMBC_MFR_SPECIFIC_F2            ((uint8_t)0xF2)
#define PMBC_MFR_SPECIFIC_F3            ((uint8_t)0xF3)
#define PMBC_MFR_SPECIFIC_F4            ((uint8_t)0xF4)
#define PMBC_MFR_SPECIFIC_F5            ((uint8_t)0xF5)
#define PMBC_MFR_SPECIFIC_F6            ((uint8_t)0xF6)
#define PMBC_MFR_SPECIFIC_F7            ((uint8_t)0xF7)
#define PMBC_MFR_SPECIFIC_F8            ((uint8_t)0xF8)
#define PMBC_MFR_SPECIFIC_F9            ((uint8_t)0xF9)
#define PMBC_MFR_SPECIFIC_FA            ((uint8_t)0xFA)
#define PMBC_MFR_SPECIFIC_FB            ((uint8_t)0xFB)
#define PMBC_MFR_SPECIFIC_FC            ((uint8_t)0xFC)
#define PMBC_MFR_SPECIFIC_FD            ((uint8_t)0xFD)
/* MFR_SPECIFIC up to v1.3 */
#define PMBC_MFR_SPECIFIC_00            ((uint8_t)0xD0)
#define PMBC_MFR_SPECIFIC_01            ((uint8_t)0xD1)
#define PMBC_MFR_SPECIFIC_02            ((uint8_t)0xD2)
#define PMBC_MFR_SPECIFIC_03            ((uint8_t)0xD3)
#define PMBC_MFR_SPECIFIC_04            ((uint8_t)0xD4)
#define PMBC_MFR_SPECIFIC_05            ((uint8_t)0xD5)
#define PMBC_MFR_SPECIFIC_06            ((uint8_t)0xD6)
#define PMBC_MFR_SPECIFIC_07            ((uint8_t)0xD7)
#define PMBC_MFR_SPECIFIC_08            ((uint8_t)0xD8)
#define PMBC_MFR_SPECIFIC_09            ((uint8_t)0xD9)
#define PMBC_MFR_SPECIFIC_10            ((uint8_t)0xDA)
#define PMBC_MFR_SPECIFIC_11            ((uint8_t)0xDB)
#define PMBC_MFR_SPECIFIC_12            ((uint8_t)0xDC)
#define PMBC_MFR_SPECIFIC_13            ((uint8_t)0xDD)
#define PMBC_MFR_SPECIFIC_14            ((uint8_t)0xDE)
#define PMBC_MFR_SPECIFIC_15            ((uint8_t)0xDF)
#define PMBC_MFR_SPECIFIC_16            ((uint8_t)0xE0)
#define PMBC_MFR_SPECIFIC_17            ((uint8_t)0xE1)
#define PMBC_MFR_SPECIFIC_18            ((uint8_t)0xE2)
#define PMBC_MFR_SPECIFIC_19            ((uint8_t)0xE3)
#define PMBC_MFR_SPECIFIC_20            ((uint8_t)0xE4)
#define PMBC_MFR_SPECIFIC_21            ((uint8_t)0xE5)
#define PMBC_MFR_SPECIFIC_22            ((uint8_t)0xE6)
#define PMBC_MFR_SPECIFIC_23            ((uint8_t)0xE7)
#define PMBC_MFR_SPECIFIC_24            ((uint8_t)0xE8)
#define PMBC_MFR_SPECIFIC_25            ((uint8_t)0xE9)
#define PMBC_MFR_SPECIFIC_26            ((uint8_t)0xEA)
#define PMBC_MFR_SPECIFIC_27            ((uint8_t)0xEB)
#define PMBC_MFR_SPECIFIC_28            ((uint8_t)0xEC)
#define PMBC_MFR_SPECIFIC_29            ((uint8_t)0xED)
#define PMBC_MFR_SPECIFIC_30            ((uint8_t)0xEE)
#define PMBC_MFR_SPECIFIC_31            ((uint8_t)0xEF)
#define PMBC_MFR_SPECIFIC_32            ((uint8_t)0xF0)
#define PMBC_MFR_SPECIFIC_33            ((uint8_t)0xF1)
#define PMBC_MFR_SPECIFIC_34            ((uint8_t)0xF2)
#define PMBC_MFR_SPECIFIC_35            ((uint8_t)0xF3)
#define PMBC_MFR_SPECIFIC_36            ((uint8_t)0xF4)
#define PMBC_MFR_SPECIFIC_37            ((uint8_t)0xF5)
#define PMBC_MFR_SPECIFIC_38            ((uint8_t)0xF6)
#define PMBC_MFR_SPECIFIC_39            ((uint8_t)0xF7)
#define PMBC_MFR_SPECIFIC_40            ((uint8_t)0xF8)
#define PMBC_MFR_SPECIFIC_41            ((uint8_t)0xF9)
#define PMBC_MFR_SPECIFIC_42            ((uint8_t)0xFA)
#define PMBC_MFR_SPECIFIC_43            ((uint8_t)0xFB)
#define PMBC_MFR_SPECIFIC_44            ((uint8_t)0xFC)
#define PMBC_MFR_SPECIFIC_45            ((uint8_t)0xFD)
/* End of v1.3 section */
#define PMBC_MFR_SPECIFIC_COMMAND_EXT   ((uint8_t)0xFE)
#define PMBC_PMBUS_COMMAND_EXT          ((uint8_t)0xFF)

//----------------------------------------------------
//	Status registers
//	STATUS_BYTE and STATUS_WORD register bit definitions */
//lower byte
struct STATUS_LOWER_BITS {		    // bits
   uint16_t NONE_OF_THE_ABOVE:1;  // 0
   uint16_t CML:1;        		    // 1
   uint16_t TEMPERATURE:1;        // 2
   uint16_t VIN_UV:1;        		  // 3   
   uint16_t IOUT_OC:1;        	  // 4
   uint16_t VOUT_OV:1;        	  // 5
   uint16_t OFF:1;        		    // 6
   uint16_t BUSY:1;        		    // 7
};
//upper byte
struct STATUS_UPPER_BITS {		    // bits
   uint16_t UNKNOWN:1;  			    // 8
   uint16_t OTHER:1;        		  // 9
   uint16_t FANS:1;        		    // 10
   uint16_t POWER_GOOD:1;         // 11   
   uint16_t MFR:1;        		    // 12
   uint16_t INPUT:1;        		  // 13
   uint16_t IOUT_POUT:1;          // 14
   uint16_t VOUT:1;        		    // 15
   
};

union STATUS_BYTE_REG {
	uint16_t            all;
	struct STATUS_LOWER_BITS bit;
};

union STATUS_WORD_REG {
   uint16_t           all;
   union STATUS_BYTE_REG StatusByte;
   struct STATUS_UPPER_BITS bit;
};

//----------------------------------------------------
//	STATUS_VOUT register bit definitions */
struct STATUS_VOUT_BITS {		// bits
   uint16_t POWER_ON_TRACKING_ERROR:1;	// 0
   uint16_t TOFF_MAX_WARNING:1;        	// 1
   uint16_t TON_MAX_WARNING:1;        	// 2
   uint16_t VOUT_MAX_WARNING:1;        	// 3   
   uint16_t VOUT_UV_FAULT:1;        		// 4
   uint16_t VOUT_UV_WARNING:1;        	// 5
   uint16_t VOUT_OV_WARNING:1;        	// 6
   uint16_t VOUT_OV_FAULT:1;        		// 7
};

union STATUS_VOUT_REG {
   uint16_t            all;
   struct STATUS_VOUT_BITS bit;
};

//----------------------------------------------------
//	STATUS_IOUT register bit definitions */
struct STATUS_IOUT_BITS {		// bits
   uint16_t POUT_OP_WARNING:1;			     // 0
   uint16_t POUT_OP_FAULT:1;        	   // 1
   uint16_t IN_POWER_LIMITING_MODE:1;    // 2
   uint16_t CURRENT_SHARE_FAULT:1;       // 3   
   uint16_t IOUT_UC_FAULT:1;        		 // 4
   uint16_t IOUT_OC_WARNING:1;        	 // 5
   uint16_t IOUT_OC_FAULT_LV_SHUTDOWN:1; // 6
   uint16_t IOUT_OC_FAULT:1;        		 // 7
};

union STATUS_IOUT_REG {
   uint16_t           all;
   struct STATUS_IOUT_BITS bit;
};

//----------------------------------------------------
//	STATUS_TEMPERATURE register bit definitions */
struct STATUS_TEMPERATURE_BITS {		     // bits
   uint16_t rsvd:4;			                 // 0:3   
   uint16_t UT_FAULT:1;		               // 4
   uint16_t UT_WARNING:1;		             // 5
   uint16_t OT_WARNING:1;                // 6
   uint16_t OT_FAULT:1;                  // 7
};

union STATUS_TEMPERATURE_REG {
   uint16_t            all;
   struct STATUS_TEMPERATURE_BITS bit;
};

//----------------------------------------------------
//	STATUS_CML register bit definitions */
struct STATUS_CML_BITS {		                // bits
   uint16_t OTHER_MEMORY_OR_LOGIC_FAULT:1;	// 0
   uint16_t OTHER_COMMUNICATION_FAULT:1;    // 1
   uint16_t rsvd:1;        		              // 2
   uint16_t PROCESSOR_FAULT_DETECTED:1;		  // 3   
   uint16_t MEMORY_FAULT_DETECTED:1;			  // 4
   uint16_t PACKET_ERROR_CHECK_FAILED:1;		// 5
   uint16_t INVALID_DATA:1;        			    // 6
   uint16_t INVALID_COMMAND:1;        		  // 7
};

union STATUS_CML_REG {
   uint16_t          all;
   struct STATUS_CML_BITS bit;
};

//----------------------------------------------------
//	STATUS_INPUT register bit definitions */
struct STATUS_INPUT_BITS {		                 // bits
   uint16_t PIN_OP_WARNING:1;	                 // 0
   uint16_t IIN_OC_WARNING:1;        	         // 1
   uint16_t IIN_OC_FAULT:1;        	           // 2
   uint16_t UNIT_OFF_FOR_LOW_INPUT_VOLTAGE:1;  // 3   
   uint16_t VIN_UV_FAULT:1;        		         // 4
   uint16_t VIN_UV_WARNING:1;        	         // 5
   uint16_t VIN_OV_WARNING:1;        	         // 6
   uint16_t VIN_OV_FAULT:1;        		         // 7
};

union STATUS_INPUT_REG {
   uint16_t         all;
   struct STATUS_INPUT_BITS bit;
};

//----------------------------------------------------
//	STATUS_FANS_1_2 register bit definitions */
struct STATUS_FANS_1_2_BITS {		               // bits
   uint16_t AIR_FLOW_WARNING:1;		             // 0
   uint16_t AIR_FLOW_FAULT:1;        	         // 1
   uint16_t FAN2_SPEED_OVERRIDE:1;	           // 2
   uint16_t FAN1_SPEED_OVERRIDE:1;             // 3   
   uint16_t FAN2_WARNING:1;        	           // 4
   uint16_t FAN1_WARNING:1;        	           // 5
   uint16_t FAN2_FAULT:1;        		           // 6
   uint16_t FAN1_FAULT:1;        		           // 7
};

union STATUS_FANS_1_2_REG {
   uint16_t             all;
   struct STATUS_FANS_1_2_BITS bit;
};

//----------------------------------------------------
//	STATUS_FANS_3_4 register bit definitions */
struct STATUS_FANS_3_4_BITS {		              // bits
   uint16_t rsvd:2;					                  // 0:1
   uint16_t FAN4_SPEED_OVERRIDE:1;	          // 2
   uint16_t FAN3_SPEED_OVERRIDE:1;            // 3   
   uint16_t FAN4_WARNING:1;        	          // 4
   uint16_t FAN3_WARNING:1;        	          // 5
   uint16_t FAN4_FAULT:1;        		          // 6
   uint16_t FAN3_FAULT:1;        		          // 7
};

union STATUS_FANS_3_4_REG {
   uint16_t              all;
   struct STATUS_FANS_3_4_BITS bit;
};

//----------------------------------------------------
//	STATUS_OTHER register bit definitions */
struct STATUS_OTHER_BITS {		               // bits
   uint16_t rsvd1:1;						             // 0
   uint16_t OUTPUT_OR_DEVICE_FAULT:1;        // 1
   uint16_t INPUTB_OR_DEVICE_FAULT:1;		     // 2
   uint16_t INPUTA_OR_DEVICE_FAULT:1;  	     // 3   
   uint16_t INPUTB_FUSE_FAULT:1;        	   // 4
   uint16_t INPUTA_FUSE_FAULT:1;        	   // 5
   uint16_t rsvd2:2;        				         // 6:7
};

union STATUS_OTHER_REG {
   uint16_t           all;
   struct STATUS_OTHER_BITS bit;
};

//----------------------------------------------------
//	STATUS registers */
struct STATUS_REGS {
   union  STATUS_WORD_REG			    StatusWord; 
   union  STATUS_VOUT_REG			    StatusVout; 
   union  STATUS_IOUT_REG			    StatusIout;
   union  STATUS_TEMPERATURE_REG	StatusTemperature;
   union  STATUS_CML_REG     		  StatusCml;
   union  STATUS_INPUT_REG     		StatusInput;
   union  STATUS_FANS_1_2_REG     StatusFans12;
   union  STATUS_FANS_3_4_REG     StatusFans34;
   union  STATUS_OTHER_REG			  StatusOther;
};
#endif
