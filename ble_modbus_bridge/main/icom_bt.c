/*
 ****************************************************************************************
 * icom_bt.c
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "bt_app_core.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_avrc_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_common_api.h"
#include "esp_int_wdt.h"

#define LOG_TAG                     "INF-LOG"
#define BLE_ADV_NAME                "IVMOD01E"
#define BLE_LOCAL_NAME              "lcln"
#define DEVICE_SERIAL_NUM           "0001"

#define GATTS_SERVICE_UUID_A        0x00FF
#define GATTS_CHAR_UUID_A           0xFF01
#define GATTS_DESCR_UUID_A          0x3333
#define GATTS_NUM_HANDLE_A          4


#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define PREPARE_BUF_MAX_SIZE        1024
#define PROFILE_NUM                 1
#define PROFILE_A_APP_ID            0
#define MAX_RETURN_MSG_LENGTH       20
#define DEBUG_ENABLE
#define ADV_SER_NO_SIZE             4

char egress_buffer[MAX_RETURN_MSG_LENGTH];
static char advertising_name[20];

typedef struct {
    uint8_t  *prepare_buf;
    int      prepare_len;
} prepare_type_env_t;


static prepare_type_env_t a_prepare_write_env;

/*
 * Declare the static function
 */

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, 
                                          esp_gatt_if_t gatts_if, 
                                          esp_ble_gatts_cb_param_t *param);
void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, 
                               esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, 
                               esp_ble_gatts_cb_param_t *param);

/* event for handler "bt_av_hdl_stack_up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

static uint8_t ble_char_value_str[] = {0x11, 0x22, 0x33};
esp_gatt_char_prop_t a_property = 0;
esp_gatt_char_prop_t b_property = 0;

esp_attr_value_t gatts_initial_char_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(ble_char_value_str),
    .attr_value   = ble_char_value_str,
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x060,
    .adv_int_max        = 0x060,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst
{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

/*
 * One gatt-based profile one app_id and one gatts_if, this array will store
 * the gatts_if returned by ESP_GATTS_REG_EVT
 */

static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static void icom_process_mobile_device_data(char *rx_pkt_buffer)
{

    return;
}

static void ble_init_adv_data(const char *name)
{
    int len;
    uint8_t raw_adv_data[30];

    len = strlen(name);

    raw_adv_data[0] = 2;
    raw_adv_data[1] = ESP_BT_EIR_TYPE_FLAGS;
    raw_adv_data[2] = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    raw_adv_data[3] = len + 1;
    raw_adv_data[4] = ESP_BLE_AD_TYPE_NAME_CMPL;

    memcpy(&raw_adv_data[5], name, len);

    esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
    if (raw_adv_ret){
        printf(" INFO : Config raw adv data failed, error code = 0x%x ", raw_adv_ret);
    }
    esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_adv_data, sizeof(raw_adv_data));
    if (raw_scan_ret){
        printf(" INFO : Config raw scan rsp data failed, error code = 0x%x", raw_scan_ret);
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{

    switch (event) {

    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        //esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        #ifdef DEBUG_ENABLE
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            printf(" INFO : Advertising start failed\n");
        }else {
            printf(" INFO : Start adv successfully\n");
        }
        #endif
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            printf(" INFO : Advertising stop failed\n");
        }
        else {
            printf(" INFO : Stop adv successfully\n");
        }
        break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : Update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        #endif
        break;

    default:
        break;
    }
}

void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env,
                             esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;

    if (param->write.need_rsp){

        if (param->write.is_prep){

            if (prepare_write_env->prepare_buf == NULL) {

                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE*sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;

                if (prepare_write_env->prepare_buf == NULL) {
                    printf(" INFO : Gatt_server prep no mem\n");
                    status = ESP_GATT_NO_RESOURCES;
                }

            } else {

                if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_OFFSET;
                } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }

            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                                                 param->write.trans_id,
                                                                 status, gatt_rsp);

            if (response_err != ESP_OK){
               printf(" INFO : Send response error\n");
            }

            free(gatt_rsp);
            if (status != ESP_GATT_OK){
                return;
            }
            memcpy(prepare_write_env->prepare_buf + param->write.offset,
                   param->write.value,
                   param->write.len);
            prepare_write_env->prepare_len += param->write.len;

        }else{

          esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                      param->write.trans_id, status,
                                      NULL);

        }
    }

}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{

    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC){

        esp_log_buffer_hex(LOG_TAG, prepare_write_env->prepare_buf,
                           prepare_write_env->prepare_len);

    }else{
    #ifdef DEBUG_ENABLE
        printf(" INFO : ESP_GATT_PREP_WRITE_CANCEL");
    #endif
    }

    if (prepare_write_env->prepare_buf) {

        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;

    }

    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                          esp_ble_gatts_cb_param_t *param)
{

    switch (event) {

    case ESP_GATTS_REG_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : REGISTER_APP_EVT, status %d, app_id %d\n",
                 param->reg.status, param->reg.app_id);
        #endif

        esp_ble_gap_config_local_privacy(true);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_A;

        /*
         * Get the serial number of the device and configure the advertising beacon
         * accordingly.
         */

        memset(advertising_name,0x00,20);
        memcpy(advertising_name,BLE_ADV_NAME,strlen(BLE_ADV_NAME));
        strcat(advertising_name,DEVICE_SERIAL_NUM);

        ble_init_adv_data(advertising_name);
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id,
                                     GATTS_NUM_HANDLE_A);
        break;

    case ESP_GATTS_READ_EVT:

    {
        /*
         * When the Mobile phone try to READ some data, the control comes through this path.
         */
        esp_gatt_rsp_t rsp;

        #ifdef DEBUG_ENABLE
        printf(" INFO : GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n",
                 param->read.conn_id, param->read.trans_id, param->read.handle);
        #endif

        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = MAX_RETURN_MSG_LENGTH;

        for(int i = 0 ;i < MAX_RETURN_MSG_LENGTH; i++){
            rsp.attr_value.value[i] = egress_buffer[i];
        }

        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);

        break;
    }

    case ESP_GATTS_WRITE_EVT:

        /*
         * When the Mobile phone sends the data, the control comes through this path.
         */

        if (!param->write.is_prep){

            char  rx_pkt_buffer[30];

            memset(rx_pkt_buffer, 0x00, 30);
            memcpy(rx_pkt_buffer, param->write.value, param->write.len);
            
            icom_process_mobile_device_data(rx_pkt_buffer);

        }

        // example_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;

    case ESP_GATTS_EXEC_WRITE_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : ESP_GATTS_EXEC_WRITE_EVT");
        #endif

        esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                    param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;

    case ESP_GATTS_MTU_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        #endif
        break;

    case ESP_GATTS_UNREG_EVT:
        break;

    case ESP_GATTS_CREATE_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : CREATE_SERVICE_EVT, status %d,  service_handle %d\n",
                 param->create.status, param->create.service_handle);
        #endif

        gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_A;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE ;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle,
                                                        &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &gatts_initial_char_val, NULL);
        if (add_char_ret){
            printf(" INFO : Add char failed, error code = 0x%x",add_char_ret);
        }
        break;

    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;

    case ESP_GATTS_ADD_CHAR_EVT: {

        #ifdef DEBUG_ENABLE
        printf(" INFO : ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        #endif

        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = GATTS_DESCR_UUID_A;

        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle,
                                                               &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                               NULL, NULL);
        if (add_descr_ret){
            printf(" INFO : Add char descr failed, error code = 0x%x", add_descr_ret);
        }
        break;
    }

    case ESP_GATTS_ADD_CHAR_DESCR_EVT:

        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;

        #ifdef DEBUG_ENABLE
        printf(" INFO : ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                                  param->add_char_descr.status,
                                  param->add_char_descr.attr_handle,
                                  param->add_char_descr.service_handle);
        #endif

        break;

    case ESP_GATTS_DELETE_EVT:
        break;

    case ESP_GATTS_START_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        #endif

        break;

    case ESP_GATTS_STOP_EVT:
        break;

    case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        break;
    }

    case ESP_GATTS_DISCONNECT_EVT:
        #ifdef DEBUG_ENABLE
        printf(" INFO : ESP_GATTS_DISCONNECT_EVT");
        #endif
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GATTS_CONF_EVT:

        #ifdef DEBUG_ENABLE
        printf(" INFO : ESP_GATTS_CONF_EVT, status %d", param->conf.status);
        #endif

        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(LOG_TAG, param->conf.value, param->conf.len);
        }
        break;

    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{

    /*
     * If event is register event, store the gatts_if for each profile
     */
    if (event == ESP_GATTS_REG_EVT) {

        if (param->reg.status == ESP_GATT_OK) {

            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;

        } else {

            #ifdef DEBUG_ENABLE
            printf(" INFO : Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            #endif

            return;

        }
    }

    /*
     * If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback
     */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {

            /*
             * ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every
             * profile cb function
             */

            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == gl_profile_tab[idx].gatts_if) {

                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }

            }

        }

    } while (0);

}

static void ble_gatts_init(void)
{
    esp_err_t ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        printf(" INFO : gatts register error, error code = 0x%x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        printf(" INFO : gap register error, error code = 0x%x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        printf(" INFO : gatts app register error, error code = 0x%x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        printf(" INFO : set local  MTU failed, error code = 0x%x", local_mtu_ret);
    }
}

void icom_bt_init()
{
    esp_bt_controller_config_t bt_cfg;

    printf(" BT-INIT : Initializing BT subsystem \n");

    bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        printf( " BT-INIT : %s initialize controller failed: %s\n", 
               __func__, esp_err_to_name(err));
        return;
    }

    if ((err = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK) {
        printf( " BT-INIT : %s enable controller failed: %s\n", 
               __func__, esp_err_to_name(err));
        return;
    }

    if ((err = esp_bluedroid_init()) != ESP_OK) {
        printf( " BT-INIT : %s initialize bluedroid failed: %s\n", 
                __func__, esp_err_to_name(err));
        return;
    }
    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        printf( " BT-INIT :  %s enable bluedroid failed: %s\n", 
               __func__, esp_err_to_name(err));
        return;
    }

    /* create application task */
    bt_app_task_start_up();



    #if (CONFIG_BT_SSP_ENABLED == true)

    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
    #endif

    /*
     * Set default parameters for Legacy Pairing
     * Use fixed pin code
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;
    pin_code[0] = '1';
    pin_code[1] = '2';
    pin_code[2] = '3';
    pin_code[3] = '4';


    esp_bt_gap_set_pin(pin_type, 4, pin_code);

    ble_gatts_init();

}