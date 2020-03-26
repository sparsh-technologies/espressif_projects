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

static void modbus_polling_timer_callback(void* arg);

const esp_timer_create_args_t modbus_timer_args = {
    .callback = &modbus_polling_timer_callback,
    .name = "modbus-one-shot"
};

static void modbus_polling_timer_callback(void* arg)
{
    printf(" INFO : Timer fired \n");

}

static void icom_create_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info)
{
    printf(" INFO : Creating Timer \n");

    ESP_ERROR_CHECK(esp_timer_create(&modbus_timer_args, &p_mbus_reg_info->poll_timer));
}


