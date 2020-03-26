/*
 ****************************************************************************************
 * icom_timer_api.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 25-March-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2020
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/param.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "icom_modbus.h"

#define TIMER_FREQUENCY    (10000000)

void icom_start_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info);

static void modbus_polling_timer_callback(void* arg);

esp_timer_create_args_t modbus_timer_args = {
    .callback = &modbus_polling_timer_callback,
    .name = "modbus-one-shot"
};

static void modbus_polling_timer_callback(void* arg)
{
    ICOM_MBUS_REG_INFO *p_mbus_reg_info;

    p_mbus_reg_info = (ICOM_MBUS_REG_INFO *)arg;

    printf(" INFO : Timer fired :%p \n", arg);
#if 1
    printf(" INFO : Timer Type (%x) Addr(%x) Freq(%d) \n", 
             p_mbus_reg_info->reg_type,
             p_mbus_reg_info->reg_address,
             p_mbus_reg_info->polling_freq_msec);

    icom_start_modbus_register_poll_timer(p_mbus_reg_info);
#endif
}

void icom_create_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info)
{
    printf(" INFO : Creating Timer : %p \n", p_mbus_reg_info);

    modbus_timer_args.arg = p_mbus_reg_info;
    ESP_ERROR_CHECK(esp_timer_create(&modbus_timer_args, &p_mbus_reg_info->poll_timer));
}

void icom_start_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info)
{
    printf(" INFO : Starting Timer \n");

    ESP_ERROR_CHECK(esp_timer_start_once(p_mbus_reg_info->poll_timer, TIMER_FREQUENCY));
}

