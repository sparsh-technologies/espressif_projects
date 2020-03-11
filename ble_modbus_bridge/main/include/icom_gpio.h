/*
 ****************************************************************************************
 * icom_gpio.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_GPIO_H_
#define _ICOM_GPIO_H_

#define GPIO_PORT_OUTPUT_IO_23                     23
#define GPIO_PORT_OUTPUT_PIN_SEL                   ((1ULL << GPIO_PORT_OUTPUT_IO_23))


/*
 * Function prototypes are declared here.
 */
extern void icom_configure_gpio_port(void);
extern void icom_set_port(int port_num, unsigned char state);

#endif

