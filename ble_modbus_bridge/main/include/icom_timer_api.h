/*
 ****************************************************************************************
 * icom_timer_api.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_TIMER_API_H_
#define _ICOM_TIMER_API_H_

#include "icom_modbus.h"

extern void icom_create_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info);
extern void icom_start_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info);

#endif

