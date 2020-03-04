/*
 ****************************************************************************************
 * icom_mobdus.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_MODBUS_H_
#define _ICOM_MODBUS_H_

#define ICOM_MBUS_COIL_STATUS_REG                      0x01
#define ICOM_MBUS_INPUT_STATUS_REG                     0x02
#define ICOM_MBUS_HOLDING_REG                          0x03
#define ICOM_MBUS_INPUT_REG                            0x04

#define MAX_CONFIGURABLE_REGISTERS                     100

typedef struct _icom_mbus_reg_info_
{
    unsigned char         reg_type;            // Specify as Coil, Input status or holding reg
    unsigned short int    reg_address;
    unsigned short int    polling_freq_msec;   // Register polling frequency in msec

} ICOM_MBUS_REG_INFO;

#endif

