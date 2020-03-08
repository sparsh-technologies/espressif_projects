/*
 ****************************************************************************************
 * icom_gpio.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 15-November-2019
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
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
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "icom_gpio.h"

void icom_configure_gpio_port(void)
{
    gpio_config_t io_conf;

    /*
     * Configure the GPIO line here
     */

    io_conf.intr_type    = GPIO_PIN_INTR_DISABLE;
    io_conf.mode         = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_PORT_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en   = 0;

    gpio_config(&io_conf);

}

void icom_set_port(int port_num, unsigned char state)
{
    if (state == 0)
        gpio_set_level(port_num, 0);
    else if (state == 0)
        gpio_set_level(port_num, 1);
}


