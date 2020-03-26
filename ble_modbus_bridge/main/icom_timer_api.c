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
static void generic_polling_timer_callback(void* arg);

esp_timer_create_args_t modbus_timer_args = {
    .callback = &modbus_polling_timer_callback,
    .name = "modbus-one-shot"
};

esp_timer_create_args_t generic_timer_args = {
    .callback = &generic_polling_timer_callback,
    .name = "modbus-one-shot"
};

static void modbus_polling_timer_callback(void* arg)
{
    ICOM_MBUS_REG_INFO *p_mbus_reg_info;

    p_mbus_reg_info = (ICOM_MBUS_REG_INFO *)arg;

    printf(" INFO : Timer fired :%p \n", arg);

    printf(" INFO : Timer Type (%x) Addr(%x) Freq(%d) \n", 
             p_mbus_reg_info->reg_type,
             p_mbus_reg_info->reg_address,
             p_mbus_reg_info->polling_freq_msec);

    icom_start_modbus_register_poll_timer(p_mbus_reg_info);

}

static void generic_polling_timer_callback(void* arg)
{
    ICOM_TIMER_HNDL *p_timer_hndl;

    p_timer_hndl = (ICOM_TIMER_HNDL *)arg;

    printf(" INFO : Generic-Timer fired :%p \n", arg);

    (*p_timer_hndl->p_callback)(p_timer_hndl);

    if (p_timer_hndl->repeat_flag)
        icom_start_timer(p_timer_hndl, p_timer_hndl->timeout, p_timer_hndl->repeat_flag);

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

ICOM_TIMER_HNDL *icom_create_timer(unsigned short int timer_id, ICOM_TIMER_CALLBACK *p_callback)
{
    ICOM_TIMER_HNDL *p_icom_timer_hndl = NULL;

    p_icom_timer_hndl = malloc(sizeof(ICOM_TIMER_HNDL));
    if (p_icom_timer_hndl == NULL) {
        return (NULL);
    }

    p_icom_timer_hndl->timer_id   = timer_id;
    p_icom_timer_hndl->p_callback = p_callback;

    generic_timer_args.arg = p_icom_timer_hndl;
    ESP_ERROR_CHECK(esp_timer_create(&generic_timer_args, &p_icom_timer_hndl->poll_timer));

    return (0);
}

int icom_start_timer(ICOM_TIMER_HNDL *p_timer, unsigned int timeout, unsigned char repeat_flag)
{
    p_timer->timeout = timeout;
    p_timer->repeat_flag = repeat_flag;
    ESP_ERROR_CHECK(esp_timer_start_once(p_timer->poll_timer, p_timer->timeout));
}


