/*
 ****************************************************************************************
 * main.c
 *
 * Author    : Shikhil
 * Ver       : 1.0
 * Date      : 26-Jan-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2020
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
#include "ble_apis.h"
#include "akbi_serial_task.h"
#include "akbi_fsm.h"
#include "esp_int_wdt.h"

#define BT_BLE_COEX_TAG             "BT_BLE_COEX"
#define BLE_ADV_NAME                "Guardian"
#define BLE_LOCAL_NAME              "lcln"

#define GATTS_SERVICE_UUID_A        0x00FF
#define GATTS_CHAR_UUID_A           0xFF01
#define GATTS_DESCR_UUID_A          0x3333
#define GATTS_NUM_HANDLE_A          4


#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define PREPARE_BUF_MAX_SIZE        1024
#define PROFILE_NUM                 1
#define PROFILE_A_APP_ID            0
#define MAX_RETURN_MSG_LENGTH       20
#define ADV_SER_NO_SIZE             4


extern uint8_t return_data[15];
char ep_return_message[MAX_RETURN_MSG_LENGTH];
char adv_ser_no[5];
char adv_full_name[20];
char firmware_version[10];
char ccu_serial_no[20];


typedef struct {
    uint8_t  *prepare_buf;
    int      prepare_len;
} prepare_type_env_t;


static prepare_type_env_t a_prepare_write_env;

//Declare the static function
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
//static void gatts_profile_b_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

extern void create_uart_task(void *param);

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

static void ble_init_adv_data(const char *name)
{
    // const char *local_name = BLE_LOCAL_NAME;
    // int local_name_len = strlen(BLE_LOCAL_NAME);
    int len = strlen(name);
    // uint8_t raw_adv_data[len+5+local_name_len+2+3];
    uint8_t raw_adv_data[len+5];

    //flag
    raw_adv_data[0] = 2;
    raw_adv_data[1] = ESP_BT_EIR_TYPE_FLAGS;
    raw_adv_data[2] = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    //adv name
    raw_adv_data[3] = len + 1;
    raw_adv_data[4] = ESP_BLE_AD_TYPE_NAME_CMPL;
    for (int i = 0;i < len;i++)
    {
        raw_adv_data[i+5] = *(name++);
    }
    //shortened local name
    // raw_adv_data[len+5] = local_name_len + 1;
    // raw_adv_data[len+6] = 0x09;//shortened local name flag
    //
    // for (int i = 0;i < local_name_len;i++)
    // {
    //     raw_adv_data[i+5+len+2] = *(local_name++);
    // }
    //manufacturer specific data
    // raw_adv_data[local_name_len+5+len+2]   = 2;
    // raw_adv_data[local_name_len+5+len+2+1] = 0x47;
    // raw_adv_data[local_name_len+5+len+2+2] = 75;

    //The length of adv data must be less than 31 bytes
    esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
    if (raw_adv_ret){
        ESP_LOGE(BT_BLE_COEX_TAG, "config raw adv data failed, error code = 0x%x ", raw_adv_ret);
    }
    esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_adv_data, sizeof(raw_adv_data));
    if (raw_scan_ret){
        ESP_LOGE(BT_BLE_COEX_TAG, "config raw scan rsp data failed, error code = 0x%x", raw_scan_ret);
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{

    switch (event) {

    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:

        // while(ep_return_message[0] == 0){
        //     akbi_check_fsm_state_and_respond(ep_return_message);
        //     vTaskDelay(1000);
        // }
        esp_ble_gap_start_advertising(&adv_params);
        execute_akbi_sent_adv_started_msg();
        break;

    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        //esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        #ifdef DEBUG_ENABLE
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BT_BLE_COEX_TAG, "Advertising start failed\n");
        }else {
            ESP_LOGI(BT_BLE_COEX_TAG, "Start adv successfully\n");
        }
        #endif
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BT_BLE_COEX_TAG, "Advertising stop failed\n");
        }
        else {
            ESP_LOGI(BT_BLE_COEX_TAG, "Stop adv successfully\n");
        }
        break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        execute_akbi_sent_mob_connected_msg();


        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
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
                    ESP_LOGE(BT_BLE_COEX_TAG, "Gatt_server prep no mem\n");
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
               ESP_LOGE(BT_BLE_COEX_TAG, "Send response error\n");
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

        esp_log_buffer_hex(BT_BLE_COEX_TAG, prepare_write_env->prepare_buf,
                           prepare_write_env->prepare_len);

    }else{
    #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
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
        ESP_LOGI(BT_BLE_COEX_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n",
                 param->reg.status, param->reg.app_id);
        #endif

        esp_ble_gap_config_local_privacy(true);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_A;
        //TODO: get serial number from processor and append with BLE_ADV_NAME
        //init BLE adv data and scan response data

        while(ep_return_message[0] == 0){
            akbi_check_fsm_state_and_respond(ep_return_message);
            vTaskDelay(1000);
        }
        memset(adv_ser_no,0x00,ADV_SER_NO_SIZE+1);
        memcpy(adv_ser_no,ccu_serial_no+5,ADV_SER_NO_SIZE);
        memset(adv_full_name,0x00,strlen(adv_full_name));
        memcpy(adv_full_name,BLE_ADV_NAME,strlen(BLE_ADV_NAME));
        memcpy(adv_full_name+strlen(BLE_ADV_NAME),adv_ser_no,ADV_SER_NO_SIZE);
        ble_init_adv_data(adv_full_name);
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
        ESP_LOGI(BT_BLE_COEX_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n",
                 param->read.conn_id, param->read.trans_id, param->read.handle);
        #endif

        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = MAX_RETURN_MSG_LENGTH;

        /*
         * If the state is not right, we may respond in a different way.
         */
        if (akbi_check_fsm_state_and_respond(ep_return_message) != 0) {
            ep_return_message[AKBI_RC_OFFSET] = RETURN_MSG_NOT_READY ;
        }
        else{
            // ep_return_message[AKBI_RC_OFFSET] = SUCCESS ;
        }

        for(int i = 0 ;i < MAX_RETURN_MSG_LENGTH; i++){
            rsp.attr_value.value[i] = ep_return_message[i];
        }
        #ifdef DEBUG_ENABLE
        esp_log_buffer_hex(BT_BLE_COEX_TAG, &rsp,rsp.attr_value.len );
        // esp_log_buffer_char(BT_BLE_COEX_TAG, &rsp,rsp.attr_value.len );
        #endif


        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);

        break;
    }

    case ESP_GATTS_WRITE_EVT:

        /*
         * When the Mobile phone sends the data, the control comes through this path.
         */
        #ifdef DEBUG_ENABLE
        // ESP_LOGI(BT_BLE_COEX_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d",
        //                param->write.conn_id, param->write.trans_id, param->write.handle);
        #endif

        if (!param->write.is_prep){

            char  rx_pkt_buffer[30];

            memset(rx_pkt_buffer, 0x00, 30);
            memset(ep_return_message, 0x00, MAX_RETURN_MSG_LENGTH);

            #ifdef DEBUG_ENABLE
            ESP_LOGI(BT_BLE_COEX_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            // esp_log_buffer_char(BT_BLE_COEX_TAG, param->write.value, param->write.len);
            esp_log_buffer_hex(BT_BLE_COEX_TAG, param->write.value, param->write.len);
            #endif

            memcpy(rx_pkt_buffer, param->write.value, param->write.len);
            read_ble_message(rx_pkt_buffer, ep_return_message);

        }

        // example_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;

    case ESP_GATTS_EXEC_WRITE_EVT:

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        #endif

        esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                    param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;

    case ESP_GATTS_MTU_EVT:

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        #endif
        break;

    case ESP_GATTS_UNREG_EVT:
        break;

    case ESP_GATTS_CREATE_EVT:

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n",
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
            ESP_LOGE(BT_BLE_COEX_TAG, "add char failed, error code = 0x%x",add_char_ret);
        }
        break;

    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;

    case ESP_GATTS_ADD_CHAR_EVT: {

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
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
            ESP_LOGE(BT_BLE_COEX_TAG, "add char descr failed, error code = 0x%x", add_descr_ret);
        }
        break;
    }

    case ESP_GATTS_ADD_CHAR_DESCR_EVT:

        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                                  param->add_char_descr.status,
                                  param->add_char_descr.attr_handle,
                                  param->add_char_descr.service_handle);
        #endif

        break;

    case ESP_GATTS_DELETE_EVT:
        break;

    case ESP_GATTS_START_EVT:

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
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

        execute_akbi_sent_mob_disconnected_msg();
        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_GATTS_DISCONNECT_EVT");
        #endif
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GATTS_CONF_EVT:

        #ifdef DEBUG_ENABLE
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_GATTS_CONF_EVT, status %d", param->conf.status);
        #endif

        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(BT_BLE_COEX_TAG, param->conf.value, param->conf.len);
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
            ESP_LOGI(BT_BLE_COEX_TAG, "Reg app failed, app_id %04x, status %d\n",
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
        ESP_LOGE(BT_BLE_COEX_TAG, "gatts register error, error code = 0x%x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(BT_BLE_COEX_TAG, "gap register error, error code = 0x%x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(BT_BLE_COEX_TAG, "gatts app register error, error code = 0x%x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(BT_BLE_COEX_TAG, "set local  MTU failed, error code = 0x%x", local_mtu_ret);
    }
}

void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{

    switch (event) {

    case ESP_BT_GAP_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {

            #ifdef DEBUG_ENABLE
            ESP_LOGI(BT_BLE_COEX_TAG, " INFO : Authentication success: %s",
                     param->auth_cmpl.device_name);
            #endif

            esp_log_buffer_hex(BT_BLE_COEX_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(BT_BLE_COEX_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }

    #if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d",
                                   param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;

    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;

    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(BT_BLE_COEX_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
    #endif

    default: {
        ESP_LOGI(BT_BLE_COEX_TAG, "event: %d", event);
        break;
    }

    }
    return;
}



void app_main(void)
{

    printf(" AKBI Security Systems Application Initialization \n");

    /*
     * Initialize NVS — it is used to store PHY calibration data
     */

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);


    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(BT_BLE_COEX_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }


    if ((err = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK) {
        ESP_LOGE(BT_BLE_COEX_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }


    if ((err = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(BT_BLE_COEX_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }


    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(BT_BLE_COEX_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(err));
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


    create_uart_task(NULL);



    //gatt server init
    ble_gatts_init();

}
