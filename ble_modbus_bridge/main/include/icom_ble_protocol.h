/*
 ****************************************************************************************
 * icom_ble_protocol.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2020-21
 *
 ****************************************************************************************
 */

#ifndef _ICOM_BLE_PROTOCOL_H_
#define _ICOM_BLE_PROTOCOL_H_

#define  BLE_CMD_CONNECTION_REQUEST_CMD                         0x01
#define  BLE_CMD_CONNECTION_RESPONSE_CMD                        0x02
#define  BLE_CMD_CONNECTION_TERMINATE_CMD                       0x03
#define  BLE_CMD_LOGIN_USERNAME_CMD                             0x04
#define  BLE_CMD_LOGIN_PASSWD_CMD                               0x05
#define  BLE_CMD_LOGIN_RESPONSE_CMD                             0x06
#define  BLE_CMD_PING_REQUEST_CMD                               0x07
#define  BLE_CMD_LOGOUT_REQUEST_CMD                             0x08


#define  BLE_CMD_WIFI_AP_NAME_CMD                               0x20
#define  BLE_CMD_WIFI_AP_PASSWD_CMD                             0x21
#define  BLE_CMD_WIFI_AP_PROTOCOL_CMD                           0x22
#define  BLE_CMD_CLOUD_PROTOCOL_CMD                             0x23
#define  BLE_CMD_HTTP_SERVER_URL_CMD                            0x24
#define  BLE_CMD_HTTP_POST_URL_CMD                              0x25
#define  BLE_CMD_MQTT_BROKER_URL_CMD                            0x26
#define  BLE_CMD_MQTT_BROKER_PORT_CMD                           0x27
#define  BLE_CMD_MQTT_BROKER_USERNAME_CMD                       0x28
#define  BLE_CMD_MQTT_BROKER_PASSWD_CMD                         0x29
#define  BLE_CMD_MQTT_BROKER_PUBLISH_TOPIC_CMD                  0x2A
#define  BLE_CMD_MQTT_BROKER_SUBSCRIBE_TOPIC_CMD                0x2B


#define  BLE_CMD_MODBUS_RTU_SLAVE_ADDR_CMD                      0x30
#define  BLE_CMD_MODBUS_TCP_SLAVE_ADDR_CMD                      0x31

#define  BLE_CMD_MODBUS_ADD_READ_REGISTER_CMD                   0x40

#define  BLE_CMD_MODBUS_ASSOCIATE_SLAVE_REGISTER_CMD            0x45


typedef struct _icom_ble_pkt_
{
    unsigned short int       session_num;
    unsigned char            cmd_opcode;

} ICOM_BLE_PKT;

#endif

