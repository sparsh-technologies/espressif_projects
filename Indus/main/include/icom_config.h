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
    ICOM_MBUS_CONFIG_INFO    mbus_cfg;
    unsigned char            cloud_protocol_flag;
    char                     mqtt_broker[MQTT_BROKER_NAME_LENGTH_MAX];
    char                     mqtt_broker_username[MQTT_BROKER_USER_NAME_LENGTH_MAX];
    char                     mqtt_broker_passwd[MQTT_BROKER_USER_PASSWD_LENGTH_MAX];
    char                     http_server[HTTP_BROKER_NAME_LENGTH_MAX];

} ICOM_STARTUP_CONFIG;

extern int icom_get_configured_modbus_register_count(void);
extern ICOM_MBUS_REG_INFO *icom_get_configured_modbus_register(int index);
extern int icom_get_configured_http_server(char *p_http_server);
extern int icom_get_configured_mqtt_broker(char *p_mqtt_broker);
extern int icom_get_configured_mqtt_broker_username(char *p_mqtt_broker_username);
extern int icom_get_configured_mqtt_broker_passwd(char *p_mqtt_broker_passwd);
extern int icom_set_configured_http_server(char *p_http_server);
extern int icom_set_configured_mqtt_broker(char *p_mqtt_broker);
extern int icom_set_configured_mqtt_broker_username(char *p_mqtt_broker_username);
extern int icom_set_configured_mqtt_broker_passwd(char *p_mqtt_broker_passwd);

#endif

