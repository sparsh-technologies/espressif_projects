/*
 ****************************************************************************************
 * icom_wifi.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

//#define EXAMPLE_ESP_WIFI_SSID      "Airplane"
//#define EXAMPLE_ESP_WIFI_PASS      "sruman23#"
#define EXAMPLE_ESP_WIFI_SSID      "technomics1"
#define EXAMPLE_ESP_WIFI_PASS      "technomics1"
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

wifi_config_t wifi_config = {
    .sta = {
        .ssid = EXAMPLE_ESP_WIFI_SSID,
        .password = EXAMPLE_ESP_WIFI_PASS
    },
};

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static int s_retry_num = 0;

extern void mqtt_app_start(void);

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {

    case SYSTEM_EVENT_STA_START :
        printf( " INFO : Received SYSTEM_EVENT_STA_START Event \n");
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_GOT_IP :
        printf( " INFO : Received SYSTEM_EVENT_STA_GOT_IP  Event \n");
        printf( " INFO : Obtained IP:%s \n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        /*
         * Now send a message to MQTT Task to initiate a connection
         */
        mqtt_app_start();
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED :
        printf( " INFO : Received SYSTEM_EVENT_STA_DISCONNECTED  Event \n");

        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            printf(" INFO : Retry to connect to the AP \n");
        }
        
        break;

    default:
        break;

    }

    return ESP_OK;
}

void wifi_init_station()
{
    esp_err_t    err_code;

    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    err_code =  esp_event_loop_init(wifi_event_handler, NULL);
    if (err_code != ESP_OK) {
        printf(" ERROR : Cannot create event loop \n");
        abort();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err_code = esp_wifi_init(&cfg);
    if (err_code != ESP_OK) {
        printf(" ERROR : Cannot initialize wifi \n");
        abort();
    }

    err_code = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err_code != ESP_OK) {
        printf(" ERROR : Cannot register event handle for ESP_EVENT_ANY_ID \n");
        abort();
    }

    err_code = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (err_code != ESP_OK) {
        printf(" ERROR : Cannot register event handle for ESP_EVENT_ANY_ID \n");
        abort();
    }

    err_code = esp_wifi_start();
    if (err_code != ESP_OK) {
        printf(" ERROR : Cannot register event handle for ESP_EVENT_ANY_ID \n");
        abort();
    }

    printf(" INFO : Trying to connect to AP SSID:%s password:%s \n",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void icom_init_station_cfg()
{
    wifi_init_station();
}
