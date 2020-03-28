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
#include "icom_timer_api.h"

#define TIMER_FREQUENCY    (10000000)

void icom_start_modbus_register_poll_timer(ICOM_MBUS_REG_INFO *p_mbus_reg_info);

static void modbus_polling_timer_callback(void* arg);
static void generic_polling_timer_callback(void* arg);

esp_timer_create_args_t generic_timer_args = {
    .callback = &generic_polling_timer_callback,
    .name = "icom-one-shot"
};

static void generic_polling_timer_callback(void* arg)
{
    ICOM_TIMER_HNDL *p_timer_hndl;

    p_timer_hndl = (ICOM_TIMER_HNDL *)arg;
    if (p_timer_hndl == NULL)
        return;

    (p_timer_hndl->p_callback)(p_icom_timer_hndl->p_callback_context);

    if (p_timer_hndl->repeat_flag)
        icom_start_timer(p_timer_hndl, p_timer_hndl->timeout, p_timer_hndl->repeat_flag);

}

ICOM_TIMER_HNDL *icom_create_timer(unsigned short int timer_id, ICOM_TIMER_CALLBACK *p_callback,
                                   void *p_callback_ctx)
{
    ICOM_TIMER_HNDL *p_icom_timer_hndl = NULL;

    p_icom_timer_hndl = malloc(sizeof(ICOM_TIMER_HNDL));
    if (p_icom_timer_hndl == NULL) {
        return (NULL);
    }

    p_icom_timer_hndl->timer_id           = timer_id;
    p_icom_timer_hndl->p_callback         = p_callback;
    p_icom_timer_hndl->p_callback_context = p_callback_ctx;
    generic_timer_args.arg                = p_icom_timer_hndl;
    ESP_ERROR_CHECK(esp_timer_create(&generic_timer_args, &p_icom_timer_hndl->poll_timer));

    return (p_icom_timer_hndl);
}

int icom_start_timer(ICOM_TIMER_HNDL *p_timer, unsigned int timeout, unsigned char repeat_flag)
{
    int    local_timeout = timeout * 1000000; 
    p_timer->timeout = timeout;
    p_timer->repeat_flag = repeat_flag;
    ESP_ERROR_CHECK(esp_timer_start_once(p_timer->poll_timer, local_timeout));
    return (0);
}


