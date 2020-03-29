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

#include "esp_timer.h"
#include "icom_timer_api.h"

#define ICOM_MBUS_COIL_STATUS_REG                      0x01
#define ICOM_MBUS_INPUT_STATUS_REG                     0x02
#define ICOM_MBUS_HOLDING_REG                          0x03
#define ICOM_MBUS_INPUT_REG                            0x04

#define MAX_CONFIGURABLE_REGISTERS                     100

typedef struct _icom_mbus_reg_info_
{
    unsigned char         reg_type;            // Specify as Coil, Input status or holding reg
    unsigned short int    reg_address;
    unsigned short int    polling_freq_sec;    // Register polling frequency in sec
    esp_timer_handle_t    poll_timer;

} ICOM_MBUS_REG_INFO;

typedef struct _icom_mbus_rt_reg_info_
{
    unsigned short int    reg_address;
    unsigned char         reg_bits;
    unsigned short int    reg_value;
    ICOM_TIMER_HNDL       *timer_handle;  
    ICOM_MBUS_REG_INFO    *p_mbus_conf_reg;

} ICOM_MBUS_RT_REG_INFO;

#endif

