/*
 ****************************************************************************************
 * icom_config_storage.c
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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "include/icom_config.h"

#define STORAGE_NAMESPACE "config"

static ICOM_STARTUP_CONFIG  startup_cfg;

//static nvs_handle_t mbus_cfg_file_handle;
static unsigned int mbus_cfg_file_handle;

void populate_dummy_cfg()
{
    ICOM_STARTUP_CONFIG  *p_mbus_cfg;
    ICOM_MBUS_REG_INFO   *p_mbus_reg;
    int                  i;

    p_mbus_cfg = &startup_cfg;
    p_mbus_cfg->slave_address = 100;
    p_mbus_cfg->cfg_reg_count = 1;

    printf(" Slave Address : %d \n", p_mbus_cfg->slave_address);
    printf(" Reg Count     : %d \n", p_mbus_cfg->cfg_reg_count);

    for (i=0; i< p_mbus_cfg->cfg_reg_count; i++) {

        p_mbus_reg = &(p_mbus_cfg->mbus_reg[i]);

        p_mbus_reg->reg_type = 0x03;
        p_mbus_reg->reg_address = i;
        p_mbus_reg->polling_freq_sec = 5;

        printf(" Reg Type      : %d \n", p_mbus_reg->reg_type);
        printf(" Reg Address   : %d \n", p_mbus_reg->reg_address);
        printf(" Polling Time  : %d \n", p_mbus_reg->polling_freq_sec);

    }

    return;
}

esp_err_t icom_write_mbus_reg_config(void)
{
    unsigned int         my_handle;
    esp_err_t            err;
    size_t               required_size = 0;  // value will default to 0, if not set yet in NVS
    uint32_t             *run_time;
    ICOM_STARTUP_CONFIG  *p_mbus_cfg;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);

    if (err != ESP_OK)
        return err;

    /*
     * Read the size of memory space required for blob
     */

    err = nvs_get_blob(my_handle, "mbus_cfg", NULL, &required_size);

    printf(" INFO : Size of BLOB : %d \n", required_size);

    if ((err != ESP_OK) && (err != ESP_ERR_NVS_NOT_FOUND))
        return err;

    // Read previously saved blob if available
//    required_size = sizeof(ICOM_STARTUP_CONFIG);
//    required_size = 4;

    run_time = malloc(sizeof(ICOM_STARTUP_CONFIG));

    if (required_size > 0) {

        err = nvs_get_blob(my_handle, "mbus_cfg", run_time, &required_size);

        if (err != ESP_OK) {
			printf(" ERROR : nvs_get_blob Failed \n");
            free(run_time);
            return err;
        }
    }

    p_mbus_cfg = (ICOM_STARTUP_CONFIG *)run_time;

    memcpy(p_mbus_cfg, &startup_cfg, sizeof(ICOM_STARTUP_CONFIG));

    required_size = sizeof(ICOM_STARTUP_CONFIG);
    printf(" INFO : Going to write %d bytes \n", required_size);
    err = nvs_set_blob(my_handle, "mbus_cfg", run_time, required_size);
    free(run_time);

    if (err != ESP_OK) {
		printf(" ERROR : nvs_set_blob Failed \n");
        return err;
	}

    /*
     * Commit
     */

    err = nvs_commit(my_handle);

    if (err != ESP_OK)
        return err;

    /*
     * Close
     */

    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t icom_read_mbus_reg_config(void)
{
    unsigned int  my_handle;
    esp_err_t     err;
    size_t required_size = 0;

    /*
     * Open the configuration space here
     */

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    /*
     * Read run time blob
     * obtain required memory space to store blob being read from NVS
     */

    err = nvs_get_blob(my_handle, "mbus_cfg", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        return err;

    printf("Run time:\n");

    if (required_size == 0) {

        printf("Nothing saved yet!\n");

    } else {

        uint32_t             *run_time;
        ICOM_STARTUP_CONFIG  *p_mbus_cfg;
        ICOM_MBUS_REG_INFO   *p_mbus_reg;
        int                  i;

        run_time = malloc(sizeof(ICOM_STARTUP_CONFIG));

        if (run_time == NULL) {
            printf(" ERROR : Not enough memory \n");
            nvs_close(my_handle);
            return (1);
        }

        err = nvs_get_blob(my_handle, "mbus_cfg", run_time, &required_size);

        if (err != ESP_OK) {
            free(run_time);
            return err;
        }

        p_mbus_cfg = (ICOM_STARTUP_CONFIG *)run_time;

        printf(" Slave Address : %d \n", p_mbus_cfg->slave_address);
        printf(" Reg Count     : %d \n", p_mbus_cfg->cfg_reg_count);

        for (i=0; i< p_mbus_cfg->cfg_reg_count; i++) {

            p_mbus_reg = &(p_mbus_cfg->mbus_reg[i]);
            printf(" Reg Type      : %d \n", p_mbus_reg->reg_type);
            printf(" Reg Address   : %d \n", p_mbus_reg->reg_address);
            printf(" Polling Time  : %d \n", p_mbus_reg->polling_freq_sec);

        }

        free(run_time);

    }

    /*
     * Close file handle
     */

    nvs_close(my_handle);
    return ESP_OK;
}

void icom_init_config_subsys()
{
    esp_err_t err;

    err = nvs_flash_init();

    populate_dummy_cfg();

#if 0

    gpio_pad_select_gpio(GPIO_NUM_0);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_DEF_INPUT);

    print_what_saved();

    while (1) {
        if (gpio_get_level(GPIO_NUM_0) == 0) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if(gpio_get_level(GPIO_NUM_0) == 0) {

                err = save_run_time();

                if (err != ESP_OK)
                    printf("Error (%s) saving run time blob to NVS!\n", esp_err_to_name(err));

                printf("Restarting ESP Module...\n");
                fflush(stdout);
                esp_restart();
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    icom_read_mbus_reg_config();

    while (1) {

        if (gpio_get_level(GPIO_NUM_0) == 0) {

            vTaskDelay(1000 / portTICK_PERIOD_MS);

            if(gpio_get_level(GPIO_NUM_0) == 0) {

                populate_dummy_cfg();

                err = icom_write_mbus_reg_config();

                if (err != ESP_OK)
                    printf("Error (%s) saving run time blob to NVS!\n", esp_err_to_name(err));

                printf("Restarting ESP Module...\n");
                fflush(stdout);
                esp_restart();
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
#endif

}

int icom_get_configured_modbus_register_count()
{
    return (startup_cfg.cfg_reg_count);
}

ICOM_MBUS_REG_INFO *icom_get_configured_modbus_register(int index)
{
    /*
     * Do some defensive checks
     */
    if (index > MAX_CONFIGURABLE_REGISTERS)
        return (NULL);

    return (&startup_cfg.mbus_reg[index]);
}

int icom_get_configured_http_server(char *p_http_server)
{
    if (strlen(startup_cfg.http_server) == 0)
        return (1);

    strcpy(p_http_server, startup_cfg.http_server);
    return (0);
}

int icom_get_configured_mqtt_broker(char *p_mqtt_broker)
{
    if (strlen(startup_cfg.mqtt_broker) == 0)
        return (1);

    strcpy(p_mqtt_broker, startup_cfg.mqtt_broker);
    return (0);
}

int icom_get_configured_mqtt_broker_username(char *p_mqtt_broker_username)
{
    if (strlen(startup_cfg.mqtt_broker_username) == 0)
        return (1);

    strcpy(p_mqtt_broker_username, startup_cfg.mqtt_broker_username);
    return (0);
}

int icom_get_configured_mqtt_broker_passwd(char *p_mqtt_broker_passwd)
{
    if (strlen(startup_cfg.mqtt_broker_passwd) == 0)
        return (1);

    strcpy(p_mqtt_broker_passwd, startup_cfg.mqtt_broker_passwd);
    return (0);
}

/*
 * All set API's are here.
 */

int icom_set_configured_http_server(char *p_http_server)
{
    if (strlen(p_http_server) == 0)
        return (1);

    strcpy(startup_cfg.http_server, p_http_server);
    return (0);
}

int icom_set_configured_mqtt_broker(char *p_mqtt_broker)
{
    if (strlen(p_mqtt_broker) == 0)
        return (1);

    strcpy(startup_cfg.mqtt_broker, p_mqtt_broker);
    return (0);
}

int icom_set_configured_mqtt_broker_username(char *p_mqtt_broker_username)
{
    if (strlen(p_mqtt_broker_username) == 0)
        return (1);

    strcpy(startup_cfg.mqtt_broker_username, p_mqtt_broker_username);
    return (0);
}

int icom_set_configured_mqtt_broker_passwd(char *p_mqtt_broker_passwd)
{
    if (strlen(p_mqtt_broker_passwd) == 0)
        return (1);

    strcpy(startup_cfg.mqtt_broker_passwd, p_mqtt_broker_passwd);
    return (0);
}

