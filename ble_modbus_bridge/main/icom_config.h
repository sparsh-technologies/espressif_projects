/*
 ****************************************************************************************
 * icom_config.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_CONFIG_H_
#define _ICOM_CONFIG_H_

#include "icom_modbus.h"

#define MQTT_BROKER_NAME_LENGTH_MAX            100
#define HTTP_BROKER_NAME_LENGTH_MAX            100
#define MQTT_BROKER_USER_NAME_LENGTH_MAX       32
#define MQTT_BROKER_USER_PASSWD_LENGTH_MAX     32


/*
 * Possible flag values for 'cloud_protocol_flag'
 */
#define CLOUD_PROTOCOL_HTTP_ENABLE             0x01
#define CLOUD_PROTOCOL_MQTT_ENABLE             0x02


typedef struct _icom_startup_config_
{
    int                   slave_address;
    int                   cfg_reg_count;
    ICOM_MBUS_REG_INFO    mbus_reg[MAX_CONFIGURABLE_REGISTERS];
    unsigned char         cloud_protocol_flag;
    char                  mqtt_broker[MQTT_BROKER_NAME_LENGTH_MAX];
    char                  mqtt_broker_username[MQTT_BROKER_USER_NAME_LENGTH_MAX];
    char                  mqtt_broker_passwd[MQTT_BROKER_USER_PASSWD_LENGTH_MAX];
    char                  http_broker[HTTP_BROKER_NAME_LENGTH_MAX];

} ICOM_STARTUP_CONFIG;

#endif

