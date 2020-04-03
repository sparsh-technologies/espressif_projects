/*
 ****************************************************************************************
 * icom_ble_protocol.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 15-November-2019
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2020-21
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
#include "driver/uart.h"
#include "tcpip_adapter.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "peripheral.h"
#include "icom_ble_protocol.h"


int icom_parse_bt_packet(void *p_data)
{
    ICOM_BLE_PKT    *p_ble_pkt = NULL;

    p_ble_pkt = (ICOM_BLE_PKT *)p_data;

    switch(p_ble_pkt->cmd_opcode) {

    case  BLE_CMD_CONNECTION_REQUEST_CMD :

    case  BLE_CMD_CONNECTION_RESPONSE_CMD :

    case  BLE_CMD_CONNECTION_TERMINATE_CMD :

    case  BLE_CMD_LOGIN_USERNAME_CMD :

    case  BLE_CMD_LOGIN_PASSWD_CMD :

    case  BLE_CMD_LOGIN_RESPONSE_CMD :

    case  BLE_CMD_PING_REQUEST_CMD :

    case  BLE_CMD_LOGOUT_REQUEST_CMD :

    case  BLE_CMD_WIFI_AP_NAME_CMD :

    case  BLE_CMD_WIFI_AP_PASSWD_CMD :

    case  BLE_CMD_WIFI_AP_PROTOCOL_CMD :

    case  BLE_CMD_CLOUD_PROTOCOL_CMD :

    case  BLE_CMD_HTTP_SERVER_URL_CMD :

    case  BLE_CMD_HTTP_POST_URL_CMD :

    case  BLE_CMD_MQTT_BROKER_URL_CMD :

    case  BLE_CMD_MQTT_BROKER_PORT_CMD 

    case  BLE_CMD_MQTT_BROKER_USERNAME_CMD :

    case  BLE_CMD_MQTT_BROKER_PASSWD_CMD :

    case  BLE_CMD_MQTT_BROKER_PUBLISH_TOPIC_CMD :

    case  BLE_CMD_MQTT_BROKER_SUBSCRIBE_TOPIC_CMD :

    case  BLE_CMD_MODBUS_RTU_SLAVE_ADDR_CMD :

    case  BLE_CMD_MODBUS_TCP_SLAVE_ADDR_CMD :

    case  BLE_CMD_MODBUS_ADD_READ_REGISTER_CMD :

    case  BLE_CMD_MODBUS_ASSOCIATE_SLAVE_REGISTER_CMD :
}


