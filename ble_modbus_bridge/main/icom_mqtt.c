/*
 ****************************************************************************************
 * icom_mqtt.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"

/* FreeRTOS event group to signal when we are connected*/
extern EventGroupHandle_t s_wifi_event_group;

//const int WIFI_CONNECTED_BIT = BIT0;
#define WIFI_CONNECTED_BIT     BIT0

/*
 * Forward decleration of the API
 */
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

esp_mqtt_client_config_t mqtt_cfg = {
    .uri = "mqtt://broker.hivemq.com",
    .event_handle = mqtt_event_handler,
};

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    // your_context_t *context = event->context;
    printf(" INFO : MQTT_EVENT_HANDLER Invoked with Event (%d) \n", event->event_id);

    switch (event->event_id) {

    case MQTT_EVENT_CONNECTED:
        printf(" INFO : MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/icom/qos1", "data_3", 0, 1, 0);
        printf(" INFO : Sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/icom/qos0", 0);
        printf(" INFO : Sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/icom/qos1", 1);
        printf(" INFO : Sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/icom/qos1");
        printf(" INFO : Sent unsubscribe successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        printf(" INFO : MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        printf(" INFO : MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/icom/qos0", "data", 0, 0, 0);
        printf(" INFO : Sent publish successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        printf(" INFO : MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        printf(" INFO : MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        printf(" INFO : MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    case MQTT_EVENT_ERROR:
        printf(" INFO : MQTT_EVENT_ERROR");
        break;

    default:
        printf(" INFO : Other event id:%d", event->event_id);
        break;
    }

    return ESP_OK;
}

void mqtt_app_start(void)
{
	printf(" INFO : About to start MQTT Client Application \n");
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void mqtt_wait_event()
{
//    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    mqtt_app_start();
}

