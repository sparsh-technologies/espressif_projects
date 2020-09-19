/*
 ****************************************************************************************
 * akbi_esp_update.c
 *
 * Author    : Shikhil Rajith M
 * Ver       : 1.0
 * Date      : 14-Sept-2020
 *
 * Copyright Infinicomm Solutions India Pvt Ltd, 2019-20
 *
 ****************************************************************************************
 */
 #include <string.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "freertos/event_groups.h"
 #include "esp_system.h"
 #include "esp_wifi.h"
 #include "esp_event.h"
 #include "esp_log.h"
 #include "nvs_flash.h"

 #include "esp_https_ota.h"
 #include "esp_ota_ops.h"

 #include "lwip/err.h"
 #include "lwip/sys.h"


#define EXAMPLE_ESP_WIFI_SSID      "Hotspot"
#define EXAMPLE_ESP_WIFI_PASS      "1y2y3y4y5y7y"
#define EXAMPLE_ESP_MAXIMUM_RETRY  10
#define FIRMWARE_UPGRADE_URL       "https://192.168.43.207:8070/akbi_esp32.bin"

extern unsigned char     wifi_ssid[50],wifi_password[50],upgrade_url[200];
extern int               ccu_sent_esp_update_completed_msg();


extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "wifi station";

static int s_retry_num = 0;

esp_err_t do_firmware_upgrade()
{
    const char* p_url = (const char *) upgrade_url;
    ESP_LOGI(TAG," Upgrading firmware from url: %s",p_url);

    esp_http_client_config_t config = {
        //TODO:this url to be changed
        .url = FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        /*
         * increase this timeout if it takes much time to download
         */
        .timeout_ms = 1200000,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG," Upgrade SUCCESS");
        ccu_sent_esp_update_completed_msg();
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

void check_partition_and_upgrade()
{
    const esp_partition_t *running = esp_ota_get_running_partition();

    // Display the running partition
    ESP_LOGI(TAG, "Running partition: %s", running->label);

    do_firmware_upgrade();
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        // esp_wifi_set_ps(WIFI_PS_NONE);
        sleep(1);
        check_partition_and_upgrade();
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    const char* p_wifi_ssid = (const char*)wifi_ssid;
    const char* p_wifi_password = (const char*)wifi_password;

    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS
        },
    };



    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             p_wifi_ssid, p_wifi_password);
}

void update_app_main(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}
