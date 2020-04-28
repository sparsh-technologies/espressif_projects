/*
 ****************************************************************************************
 * ble_apis.c
 *
 * Author    : Sathish Chandran
 * Ver       : 1.0
 * Date      : 28-Dec-2019
 *
 * Copyright Infinicomm Solutions (I) Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "esp_log.h"
#include "akbi_serial_task.h"
#include <rom/ets_sys.h>
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"
#define BT_BLE_COEX_TAG             "BLE_APIS"


CCU this_ccu;

int return_msg_len                                = BLE_RETURN_MAX_SIZE;
int searched_ssid_count_index                     = 0;
static char saved_messages[4][21];
static char *p_recvd_msg_full;
static int flag_set_recvd_msg_ptr = 0;
extern char adv_ser_no[5];
// char flag_sending_voice_data = 0;
extern char ep_return_message[20];
// unsigned int voice_msg_length;


int store_and_send_voice_data(char * data, char audio_number,int voice_message_length);
int store_and_send_address_visiting_audio_data(char * data, unsigned char audio_number,int voice_message_length);


/*
 * All extern API's are declared here.
 */
extern int ccu_sent_scan_all_wifi_msg(char * ret_msg);
extern int ccu_send_reg_msg_new(int type, char *received_value_buffer, int data_len);


AKBI_WIFI_SCAN_REPORT  wifi_scan_report;

void set_recvd_msg_pointer(char *received_value_buffer){
    p_recvd_msg_full = received_value_buffer;
    flag_set_recvd_msg_ptr = 1;
}


int save_group_messages(char *received_value_buffer,int type_id)
{
    memcpy(saved_messages[type_id-1],received_value_buffer,BLE_MESSAGE_SIZE);
    return 0;
}

/*
 * Register command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_register(char *i_cmd, char *i_ret_msg)
{
    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble                    = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    switch (data_type)
    {

    case DID_REGISTER_PASSWORD :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_PASSWORD,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_MOB_NO :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_MOB_NUM, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_MOB_NAME :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_MOB_NAME, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_ANDROID_ID_OR_UUID :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_UNIQUE_ID,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET-1],ANDROID_ID_OR_UUID_SIZE);
        break;

    case DID_REGISTER_LAT :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_LAT_STRING, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_LONG :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_LONG_STRING,  &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_MOB_FW_VER :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_MOBILE_FW_VERSION, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    case DID_REGISTER_REG_DATE :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ccu_send_reg_msg_new(TLV_TYPE_REGISTER_TIMESTAMP, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET], data_len_in_ble);
        break;

    default :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
        break;
    }

    return i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

/*
 * login command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_login(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    static char i_pwd[20];
    static char timestamp[15];
    static char latitude[15];
    static char longitude[15];

    switch (data_type)
    {
        case 0x01:
            memset(i_pwd,0x00,20);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            memcpy(i_pwd,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            break;

        case 0x02:
            memset(timestamp,0x00,15);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            memcpy(timestamp,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            // akbi_set_fsm_state(FSM_STATE_LOGIN);
            // ccu_send_login_msg(i_pwd,strlen(i_pwd),timestamp);
            break;

        case 0x03:
            memset(latitude,0x00,15);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            memcpy(latitude,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            // akbi_set_fsm_state(FSM_STATE_LOGIN);
            // ccu_send_login_msg(i_pwd,strlen(i_pwd),timestamp);
            break;

        case 0x04:
            memset(longitude,0x00,15);
            memcpy(longitude,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            akbi_set_fsm_state(FSM_STATE_LOGIN);
            ccu_send_login_msg(i_pwd,strlen(i_pwd),timestamp);
            // ccu_send_login_msg(i_pwd,strlen(i_pwd),timestamp,latitude,longitude);
            break;
    }
    return 0;
}

/*
 * forgot password command execution
 */
int execute_forgot_password()
{
    akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD);
    ccu_sent_user_forgot_passwd_msg();
    return 0;
}

/*
 * change password command execution
 * i_cmd: The command parameters of change password
 * i_ret_msg: pointer to the return message
 */
int execute_change_password(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    char i_pwd[data_len_in_ble];
    char p_password_new[20];
    char p_password_current[20];

    memcpy(i_pwd,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    switch (data_type)
    {

    case DID_CHANGE_PASSWORD_CURRENT:
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        akbi_set_fsm_state(FSM_STATE_INIT);
        save_group_messages(p_recvd_msg_full,DID_CHANGE_PASSWORD_CURRENT);
        break;

    case DID_CHANGE_PASSWORD_NEW:
        save_group_messages(p_recvd_msg_full,DID_CHANGE_PASSWORD_NEW);
        memset(p_password_new , 0x00, 20);
        memset(p_password_current , 0x00, 20);
        memcpy(p_password_current ,&saved_messages[0][BLE_MSG_MULTI_DATA_DATA_OFFSET],saved_messages[0][BLE_MSG_MULTI_DATA_LEN_OFFSET]);
        memcpy(p_password_new ,&saved_messages[1][BLE_MSG_MULTI_DATA_DATA_OFFSET],saved_messages[1][BLE_MSG_MULTI_DATA_LEN_OFFSET]);
        ccu_sent_user_change_passwd_msg(p_password_current, p_password_new );
        break;

    default:
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
        break;

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

// int execute_record_personal_voice_msg(unsigned char voice_msg_index,char * msg)
// {
//     if(msg[BLE_MSG_VOICE_DATA_TLV_LEN_OFFSET] == 0x03 ){//less than 10 kB
//
//         voice_msg_length = msg[5]*100 + msg[6];
//     }
//     else if(msg[BLE_MSG_VOICE_DATA_TLV_LEN_OFFSET] == 0x04 ){//more than 10 kB
//
//         voice_msg_length = (msg[5]*1000) + (msg[6]*10) + msg[7];
//     }
//     ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
//
//     /*
//      * explicit delay for ccu to distinguish between packets
//      */
//     ets_delay_us(200*1000);
//     ccu_sent_record_voice_msg(voice_msg_index,voice_msg_length);
//
//     flag_sending_voice_data = 0x01;
//     return 0;
// }

int execute_record_personal_voice_msg_new(char * msg)
{
    static unsigned int voice_message_length = 0;
    static char         flag_sending_voice_data = 0;
    unsigned char       voice_msg_index;
    unsigned char       voice_msg_tlv_length;

    voice_msg_index  = msg[BLE_MSG_VOICE_RAW_DATA_AUDIO_NUM_OFFSET];

    if (flag_sending_voice_data)
    {
        if (voice_msg_index == 0x08) {
            flag_sending_voice_data = 0;
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            akbi_set_fsm_state(FSM_STATE_VOICE_RECORDING_COMPLETE);
            return 0;
        }
        store_and_send_voice_data(msg, voice_msg_index,voice_message_length);
        return 0;
    }
    else{
        voice_msg_tlv_length = voice_msg_index;
        voice_msg_index = msg[BLE_MSG_VOICE_DATA_AUDIO_NUM_OFFSET];
    }


    switch (voice_msg_tlv_length)
    {
      case 0x03:
          voice_message_length = msg[5]*100 + msg[6];
          flag_sending_voice_data = 1;
          ccu_sent_record_voice_msg(voice_msg_index,voice_message_length);

          break;

      case 0x04:
          voice_message_length = msg[5]*1000 + msg[6]*10 +msg[7];
          flag_sending_voice_data = 1;
          ccu_sent_record_voice_msg(voice_msg_index,voice_message_length);
          break;

      default:
          printf("udefined msg byte %02x\n",voice_msg_index );
    }
    /*
     * explicit delay for ccu to distinguish between packets
     */
    ets_delay_us(200*1000);

    return 0;
}

int execute_store_emergency_number(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];

    akbi_set_fsm_state(FSM_STATE_SET_EMER_NUM_SENDING);

    switch (data_type) {
        case DID_EMERGENCY_FIRST_RESPONDER : {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            ccu_sent_store_emergency_number_msg(p_recvd_msg_full);
            break;
        }
        case DID_EMERGENCY_CLOSE_RELATIVE : {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            ccu_sent_store_emergency_number_msg(p_recvd_msg_full);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_store_personal_number(char *i_cmd, char *i_ret_msg)
{

    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int data_len_in_ble  = MOB_NO_SIZE;
    char i_personal_number[data_len_in_ble];

    akbi_set_fsm_state(FSM_STATE_SET_PERSONAL_NUM_SENDING);

    memcpy(i_personal_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_FIXED_LEN_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    memcpy(this_ccu.conf_personal_nos.mob1_mobile_number,this_ccu.paired_mob1.mobile_number,data_len_in_ble);

    switch (data_type) {
        case DID_PERSONAL_SECOND_NUMBER : {
            memcpy(this_ccu.conf_personal_nos.second_number,i_personal_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            ccu_sent_store_personal_number_msg(p_recvd_msg_full);
            break;
        }
        case DID_PERSONAL_THIRD_NUMBER : {
            memcpy(this_ccu.conf_personal_nos.third_number,i_personal_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            ccu_sent_store_personal_number_msg(p_recvd_msg_full);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_enter_local_help_number(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble = MOB_NO_SIZE;
    char i_local_help_number[data_len_in_ble];
    unsigned char i_visited_locations_count = this_ccu.visited_locations_count;

    memcpy(i_local_help_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_FIXED_LEN_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    switch (data_type) {
        case DID_LOCAL_HELP_FOURTH_NUMBER : {
            memcpy(this_ccu.visited_locations[i_visited_locations_count].fourth_phone_number,i_local_help_number,data_len_in_ble);
            ccu_sent_store_local_help_number_msg(p_recvd_msg_full);
            break;
        }
        case DID_LOCAL_HELP_FIFTH_NUMBER : {
            memcpy(this_ccu.visited_locations[i_visited_locations_count].fifth_phone_number,i_local_help_number,data_len_in_ble);
            ccu_sent_store_local_help_number_msg(p_recvd_msg_full);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_scan_wifis(char *i_cmd ,char *i_ret_msg)
{

    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    if(data_type == BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE){
        akbi_clear_ssids();
        akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_IN_PROGRESS);
        ccu_sent_scan_all_wifi_msg(i_ret_msg);
        memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE,
               BLE_COMMAND_DATA_TYPE_SIZE);
        searched_ssid_count_index = 0;

    }
    else if(data_type == BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE){
        memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE,
              BLE_COMMAND_DATA_TYPE_SIZE);
    }

    /*
     * Send message to the CCU to start scan operation.
     */

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}


int execute_select_a_wifi(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    char i_data_value[data_len_in_ble];
    char p_password[20];
    int  selected_ap_id ;

    memcpy(i_data_value,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    switch (data_type) {
        case DID_SELECT_A_WIFI_SSID : {
            memcpy(this_ccu.conf_wifi.ssid,i_data_value,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            this_ccu.conf_wifi.data_status |= FLAG_DATA_SET_SEL_WIFI_SSID;
            save_group_messages(p_recvd_msg_full,DID_SELECT_A_WIFI_SSID);
            break;
        }
        case DID_SELECT_A_WIFI_NETWORK_KEY : {
            memcpy(this_ccu.conf_wifi.network_key,i_data_value,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            this_ccu.conf_wifi.data_status |= FLAG_DATA_SET_SEL_WIFI_NETWORK_KEY;
            save_group_messages(p_recvd_msg_full,DID_SELECT_A_WIFI_NETWORK_KEY);

            memset(p_password , 0x00, 20);
            selected_ap_id = saved_messages[0][BLE_MSG_MULTI_DATA_DATA_OFFSET];
            memcpy(p_password ,&saved_messages[1][BLE_MSG_MULTI_DATA_DATA_OFFSET],saved_messages[1][BLE_MSG_MULTI_DATA_LEN_OFFSET]);
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_IN_PROGRESS);
            ccu_sent_configure_wifi_credentials(selected_ap_id+1, p_password, 4 );

            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA,
                   BLE_RETURN_RC_SIZE);
            break;
        }
    }
    return 0;
}

int execute_connect_to_wifi(char *i_cmd,char *i_ret_msg)
{
    int  data_len_in_ble = (int)i_cmd[BLE_CMD_SINGLE_DATA_LEN_OFFSET];
    char i_data_value[data_len_in_ble];

    memcpy(i_data_value,&i_cmd[BLE_CMD_SINGLE_DATA_VALUE_OFFSET],data_len_in_ble);

    ccu_sent_connect_to_wifi(i_data_value);
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_disconnect_from_wifi(char *i_ret_msg)
{
    ccu_sent_disconnect_from_wifi();
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_ccu_activate(char *i_cmd,char *i_ret_msg)
{
  char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
  int  data_len_in_ble = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
  char i_data_value[data_len_in_ble];
  char act_latitude[20];
  char act_longitude[20];

  memcpy(i_data_value,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
  memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

  switch (data_type) {
      case DID_ACTIVATE_CCU_LATITUDE : {
          memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
          save_group_messages(p_recvd_msg_full,DID_ACTIVATE_CCU_LATITUDE);
          break;
      }
      case DID_ACTIVATE_CCU_LONGITUDE : {
          memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
          save_group_messages(p_recvd_msg_full,DID_ACTIVATE_CCU_LONGITUDE);
          break;
      }
      case DID_ACTIVATE_CCU_TIMESTAMP : {
          char time_stamp[15];
          memset(act_latitude , 0x00, 20);
          memset(act_longitude , 0x00, 20);
          memcpy(time_stamp,&p_recvd_msg_full[BLE_MSG_SINGLE_DATA_TIMESTAMP_OFFSET],TIMESTAMP_SIZE);
          memcpy(act_latitude ,&saved_messages[0][BLE_MSG_MULTI_DATA_DATA_OFFSET],saved_messages[1][BLE_MSG_MULTI_DATA_LEN_OFFSET]);
          memcpy(act_longitude ,&saved_messages[1][BLE_MSG_MULTI_DATA_DATA_OFFSET],saved_messages[1][BLE_MSG_MULTI_DATA_LEN_OFFSET]);
          // akbi_set_fsm_state(FSM_STATE_ACTIVATE_IN_PROGRESS);
          ccu_sent_activate_system_msg(act_latitude,act_longitude,time_stamp,4);
          break;
      }
      default: {
          memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA,
                 BLE_RETURN_RC_SIZE);
          break;
      }
    }

    return 0;
}

int update_ccu_sw(char *i_cmd,char *i_ret_msg)
{
    int  data_len_in_ble = (int)i_cmd[BLE_CMD_SINGLE_DATA_LEN_OFFSET];
    char i_data_value[data_len_in_ble];

    memcpy(i_data_value,&i_cmd[BLE_CMD_SINGLE_DATA_VALUE_OFFSET],data_len_in_ble);
    ccu_sent_update_sw_msg( i_data_value );
    return 0;
}

int execute_reboot_ccu(char *i_ret_msg)
{
    ccu_sent_reboot_ccu_msg();
    return 0;
}

int set_ccu_wifi_mode(unsigned char wifi_mode)
{
    ccu_sent_wifi_set_mode(wifi_mode);
    return 0;
}

int update_trip_info()
{
    //TODO: Get the info status from the processor
    akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_IN_PROGRESS);
    ccu_sent_upload_trip_info();
    return 0;

}
int execute_akbi_sent_adv_started_msg()
{
    ccu_sent_adv_started_msg();
    return 0;
}

int execute_akbi_sent_mob_connected_msg()
{
    ccu_sent_mob_connected_msg();
    return 0;
}

int execute_akbi_sent_mob_disconnected_msg()
{
    ccu_sent_mob_disconnected_msg();
    return 0;
}

int execute_store_address_visiting_new(char *msg)
{
    static unsigned int voice_message_length = 0;
    static char time_stamp[15];
    unsigned char voice_msg_index;

    voice_msg_index  = msg[BLE_MSG_VOICE_RAW_DATA_AUDIO_NUM_OFFSET];

    switch (voice_msg_index) {
      case 0x00:
          memcpy(time_stamp,&msg[BLE_MSG_SINGLE_DATA_TIMESTAMP_OFFSET-1],TIMESTAMP_SIZE);
          ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;

          /*
           * explicit delay for ccu to distinguish between packets
           */
          ccu_sent_address_visiting(time_stamp,voice_message_length);
          ets_delay_us(200*1000);
          break;

      case 0x01:
          store_and_send_address_visiting_audio_data(msg, voice_msg_index,voice_message_length);
          break;

      case 0x03:
          voice_message_length = msg[5]*100 + msg[6];
          break;

      case 0x04:
          voice_message_length = msg[5]*1000 + msg[6]*10 +msg[7];
          break;

      case 0x08:
          ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
          akbi_set_fsm_state(FSM_STATE_VOICE_RECORDING_COMPLETE);
          break;

      default:
          printf("udefined msg offset %02x\n",voice_msg_index );
    }

    return 0;
}

/*
 * This API will read the packets from the mobile phone and process the packets.
 */

int read_ble_message(char *i_msg, char *i_ret_msg)
{
    int  is_valid_ble_msg;
    char source_app_type_identifier;
    char source_app_identifier;
    char ble_cmd_id;
    char ble_command[BLE_COMMAND_SIZE];
    source_app_type_identifier = i_msg[BLE_APP_TYPE_OFFSET];
    source_app_identifier      = i_msg[BLE_APP_OFFSET];

    memset(ble_command,0x00,BLE_COMMAND_SIZE);

    if(flag_set_recvd_msg_ptr == 0){
      set_recvd_msg_pointer(i_msg);
    }

    if (source_app_type_identifier == MOB1_APP_TYPE_ID) {
        is_valid_ble_msg = 1;
    } else {
        is_valid_ble_msg = 0;
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_SOURCE_APP_TYPE_MISMATCH;
        // return (ERROR_SOURCE_APP_TYPE_MISMATCH);
    }

    ble_cmd_id                           = i_msg[BLE_CMD_OFFSET];
    i_ret_msg[BLE_RET_MSG_CCU_ID_OFFSET] = CCU_TYPE_ID;
    i_ret_msg[BLE_RET_MSG_CMD_ID_OFFSET] = ble_cmd_id;
    memcpy(&i_ret_msg[BLE_RET_MSG_SERIAL_NUM_OFFSET],adv_ser_no,ADV_SER_NO_SIZE);

     /*
     * If this is the first register packet, set the app id in the data structure.
     */
    if ((CID_REGISTER == ble_cmd_id) &&
        ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_ID) == 0x00)) {

        this_ccu.paired_mob1.id = i_msg[BLE_APP_OFFSET];
        // printf("paired_mob1.id stored 1 %c\n",i_msg[BLE_APP_OFFSET] );
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_ID;
        is_valid_ble_msg = 1;
    }
    else {
        if (source_app_identifier == this_ccu.paired_mob1.id) {
            is_valid_ble_msg = 1;
        }
        else {
            is_valid_ble_msg = 1;

        }
    }

    if (is_valid_ble_msg) {
        switch (ble_cmd_id)
        {
        case CID_REGISTER :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_MOB_REGISTERED);
            execute_register(ble_command,i_ret_msg);
            break;

        case CID_LOGIN :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_INIT);
            execute_login(ble_command,i_ret_msg);
            break;

        case CID_FORGOT_PASSWORD :
            execute_forgot_password();
            break;

        case CID_CHANGE_PASSWORD :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_CHANGE_PASSWD);
            execute_change_password(ble_command,i_ret_msg);
            break;

        case CID_RECORD_PERSONAL_VOICE_MSG :
            // if(flag_sending_voice_data){
            //     voice_msg_index  = i_msg[BLE_MSG_VOICE_RAW_DATA_AUDIO_NUM_OFFSET];
            //     store_and_send_voice_data(i_msg , voice_msg_index);
            // }
            // else{
            //     voice_msg_index  = i_msg[BLE_MSG_VOICE_DATA_AUDIO_NUM_OFFSET];
            //     execute_record_personal_voice_msg(voice_msg_index,i_msg);
            // }
            akbi_set_fsm_state(FSM_STATE_VOICE_RECORDING_IN_PROGRESS);
            execute_record_personal_voice_msg_new(i_msg);
            break;

        case CID_STORE_EMERGENCY_NUMBERS :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_store_emergency_number(ble_command,i_ret_msg);
            // akbi_set_fsm_state(FSM_STATE_SET_EMER_NUM);
            break;

        case CID_STORE_PERSONAL_NUMBERS :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_store_personal_number(ble_command,i_ret_msg);
            //akbi_set_fsm_state(FSM_STATE_SET_PERSONAL_NUM );
            break;

        case CID_SCAN_WIFIS :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_scan_wifis(ble_command ,i_ret_msg);
            if((akbi_get_fsm_state()!=FSM_STATE_WIFI_SCAN_IN_PROGRESS)&&
                       (akbi_get_fsm_state()!=FSM_STATE_WIFI_SCAN_COMPLETE)&&
                       (akbi_get_fsm_state()!=FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS)){
                akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_IN_PROGRESS);
            }
            break;

        case CID_SELECT_A_WIFI :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_WIFI_SELECT_IN_PROGRESS );
            execute_select_a_wifi(ble_command,i_ret_msg);
            break;

        case CID_ADDRESS_VISITING :

            // if(flag_sending_voice_data){
            //     voice_msg_index  = i_msg[BLE_MSG_VOICE_RAW_DATA_AUDIO_NUM_OFFSET];
            //     store_and_send_address_visiting_audio_data(i_msg , voice_msg_index);
            // }
            // else{
            //     voice_msg_index  = i_msg[BLE_MSG_VOICE_DATA_AUDIO_NUM_OFFSET];
            //     execute_store_address_visiting(voice_msg_index,i_msg);
            // }
            execute_store_address_visiting_new(i_msg);

            break;

        case CID_ENTER_LOCAL_HELP_NUMBERS :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_CFG_SET_HELP_NUM_SENDING );
            execute_enter_local_help_number(ble_command,i_ret_msg);
            break;

        case CID_CCU_ACTIVATE :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            // akbi_set_fsm_state(FSM_STATE_ACTIVATE_IN_PROGRESS);
            execute_ccu_activate(ble_command,i_ret_msg);
            break;

        case CID_CONNECT_TO_WIFI :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_IN_PROGRESS);
            execute_connect_to_wifi(ble_command,i_ret_msg);
            break;

        case CID_DISCONNECT_FROM_WIFI :
            akbi_set_fsm_state(FSM_STATE_WIFI_DISCONNECT_IN_PROGRESS);
            execute_disconnect_from_wifi(i_ret_msg);
            break;

        case CID_UPDATE_CCU_SW :
            akbi_set_fsm_state(FSM_STATE_FW_UPGRADE_IN_PROGRESS);
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            update_ccu_sw(ble_command,i_ret_msg);
            break;

        case CID_SET_CCU_REBOOT :
            akbi_set_fsm_state(FSM_STATE_FW_REBOOT_MSG_SENDING);
            execute_reboot_ccu(i_ret_msg);
            break;

        case CID_SET_CCU_WIFI_MODE :
            akbi_set_fsm_state(FSM_STATE_WIFI_MODE_SET_IN_PROGRESS);
            set_ccu_wifi_mode(i_msg[BLE_MSG_MULTI_DATA_TYPE_OFFSET]);
            break;

        case CID_UPLOAD_TRIP_INFO:
            akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_IN_PROGRESS);
            update_trip_info(i_ret_msg);
            break;
        default :
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_COMMAND;
            break;

        }
    }
    return i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

char voice_data_buffer[VOICE_DATA_BUFFER_NUM][256];

int store_and_send_address_visiting_audio_data(char * data, unsigned char audio_number,int voice_message_length)
{
    static int chunk_offset = 0x00;
    static int buffer_number = 0x00;
    static unsigned int received_audio_size = 0x00;

    memcpy( &voice_data_buffer[buffer_number][chunk_offset], &data[4] , 16);
    received_audio_size += 16;
    chunk_offset += 16;
    if (received_audio_size >= voice_message_length){
        chunk_offset -= 16 - (voice_message_length %16);
        /*
         * explicit delay for ccu to distinguish between packets
         */
        ets_delay_us(200*1000);

        ccu_sent_address_visiting_raw((char *)&voice_data_buffer[buffer_number], chunk_offset,audio_number);
        buffer_number = 0x00;
        chunk_offset = 0x00;
        received_audio_size = 0x00;
        return 0;
    }

    if(chunk_offset >= 256){//256
      /*
       * explicit delay for ccu to distinguish between packets
       */
      ets_delay_us(200*1000);
        ccu_sent_address_visiting_raw((char *)&voice_data_buffer[buffer_number], chunk_offset,audio_number);
        buffer_number ++;
        buffer_number = buffer_number%VOICE_DATA_BUFFER_NUM;
        chunk_offset = 0x00;
    }


    return 0;
}

int store_and_send_voice_data(char * data, char audio_number,int voice_message_length)
{
    static int chunk_offset = 0x00;
    static int buffer_number = 0x00;
    static unsigned int received_audio_size = 0x00;

    memcpy( &voice_data_buffer[buffer_number][chunk_offset], &data[4] , 16);
    received_audio_size += 16;
    chunk_offset += 16;

    if (received_audio_size >= voice_message_length){
        chunk_offset -= 16 - (voice_message_length %16);
        /*
         * explicit delay for ccu to distinguish between packets
         */
        ets_delay_us(200*1000);

        ccu_sent_record_voice_msg_raw((char *)&voice_data_buffer[buffer_number], chunk_offset,audio_number);
        buffer_number = 0x00;
        chunk_offset = 0x00;
        // flag_sending_voice_data = 0x00;
        received_audio_size = 0x00;
        return 0;
    }

    if(chunk_offset >= 256){//256
      /*
       * explicit delay for ccu to distinguish between packets
       */
      ets_delay_us(200*1000);
        ccu_sent_record_voice_msg_raw((char *)&voice_data_buffer[buffer_number], chunk_offset,audio_number);
        buffer_number ++;
        buffer_number = buffer_number%VOICE_DATA_BUFFER_NUM;
        chunk_offset = 0x00;
    }

    return 0;
}

#if 0
int populate_serial_no_from_eeprom(char *i_ser)
{
    //TODO - access EEPROM
    memset(i_ser,0x00,SER_NO_SIZE);
    memcpy(i_ser,&SER_NO_TEST,SER_NO_SIZE);
    return 0;
}

void print_bytes(char *const_text, char *message, int size_of_msg)
{
    printf("%s",const_text);
    for (int i = 0; i<size_of_msg; i++) {
        printf("_%2x_",message[i]);
    }
    printf("#\n");
}

void print_chars(char *const_text,char *message, int size_of_msg)
{
    printf("%s",const_text);
    for (int i = 0; i<size_of_msg; i++) {
        printf("_%c_",message[i]);
    }
    printf("#\n");
}

void print_ccu()
{
    char buf[20];
    printf("******** CCU DATA START ********\n\n\n");
    print_chars("Serial Number #",this_ccu.serial_number, SER_NO_SIZE);
    print_chars("Password #",this_ccu.password, PASS_SIZE);
    print_bytes("Paired Mob1 ID #",&this_ccu.paired_mob1.id, BLE_APP_ID_SIZE);
    print_chars("Paired Mob1 Number #",this_ccu.paired_mob1.mobile_number, MOB_NO_SIZE);
    print_chars("Paired Mob1 Name #",this_ccu.paired_mob1.mobile_name, MOB_NAME_SIZE);
    print_chars("Paired Mob1 Android ID/UUID #",this_ccu.paired_mob1.android_id_or_uuid, ANDROID_ID_OR_UUID_SIZE);
    printf("Paired Mob1 Authentication Status #%d#\n",this_ccu.paired_mob1.authentication_status);
    printf("PERSONAL VOICE MESSAGES COUNT #%d#\n",PERSONAL_VOICE_MESSAGES_COUNT);
    for (int i = 0; i < PERSONAL_VOICE_MESSAGES_COUNT; i++) {
        sprintf(buf,"%d #",i);
        print_chars(buf,(this_ccu.personal_voice_messages[i]).message_file_name,FILE_NAME_SIZE);
    }
    print_chars("Configured Emergency Default #",this_ccu.conf_emergency_nos.default_emergency_number,DEFAULT_EMERGENCY_NUMBER_SIZE);
    print_chars("Configured Emergency First Responder #",this_ccu.conf_emergency_nos.first_responder,MOB_NO_SIZE);
    print_chars("Configured Emergency Close Relative #",this_ccu.conf_emergency_nos.close_relative,MOB_NO_SIZE);
    print_chars("Configured Personal Default #",this_ccu.conf_personal_nos.mob1_mobile_number,MOB_NO_SIZE);
    print_chars("Configured Personal Second #",this_ccu.conf_personal_nos.second_number,MOB_NO_SIZE);
    print_chars("Configured Personal Third #",this_ccu.conf_personal_nos.third_number,MOB_NO_SIZE);
    printf("\nInterface Wifi Mode #%d#\n",this_ccu.interface_wifi.mode);
    printf("Interface Wifi Status #%d#\n",this_ccu.interface_wifi.status);
    print_chars("Interface WiFi SSID#",this_ccu.interface_wifi.ssid, SSID_SIZE);
    print_chars("Interface WiFi Network Key#",this_ccu.interface_wifi.network_key, NETWORK_KEY_SIZE);
    printf("\nConfigured Wifi Mode #%d#\n",this_ccu.conf_wifi.mode);
    printf("Configured Wifi Status #%d#\n",this_ccu.conf_wifi.status);
    print_chars("Configured WiFi SSID#",this_ccu.conf_wifi.ssid, SSID_SIZE);
    print_chars("Configured WiFi Network Key#",this_ccu.conf_wifi.network_key, NETWORK_KEY_SIZE);
    printf("Scanned WiFis #%d#\n",this_ccu.scanned_wifi_count);

    for (int i = 0; i < this_ccu.scanned_wifi_count; i++) {
        printf("\nScanned Wifi No. %d Mode #%d#\n",i,this_ccu.scanned_wifis[i].mode);
        printf("Scanned Wifi No. %d Status #%d#\n",i,this_ccu.scanned_wifis[i].status);
        print_chars("Scanned WiFi SSID#",this_ccu.scanned_wifis[i].ssid, SSID_SIZE);
        print_chars("Scanned WiFi Network Key#",this_ccu.scanned_wifis[i].network_key, NETWORK_KEY_SIZE);
    }
    printf("Visited locations #%d#\n",this_ccu.visited_locations_count);
    for (int i = 0; i < this_ccu.visited_locations_count + 1; i++) {
        printf("Location #%d# Latitude is #%d#%d#%d#%d#\n",i,this_ccu.visited_locations[i].latitude.degree,this_ccu.visited_locations[i].latitude.minute,this_ccu.visited_locations[i].latitude.second,this_ccu.visited_locations[i].lat_dir);
        printf("Location #%d# Longitude is #%d#%d#%d#%d#\n",i,this_ccu.visited_locations[i].longitude.degree,this_ccu.visited_locations[i].longitude.minute,this_ccu.visited_locations[i].longitude.second,this_ccu.visited_locations[i].long_dir);
        print_chars("Fourth Number is #",this_ccu.visited_locations[i].fourth_phone_number, MOB_NO_SIZE);
        print_chars("Fifth Number is #",this_ccu.visited_locations[i].fifth_phone_number, MOB_NO_SIZE);
    }
    printf("CCU mode is #%d#\n", this_ccu.mode);
    printf("Activations #%d#\n",this_ccu.activations_count);
    for (int i = 0; i < this_ccu.activations_count + 1; i++) {
        printf("Location #%d# Latitude is #%d#%d#%d#%d#\n",i,this_ccu.activations[i].latitude.degree,this_ccu.activations[i].latitude.minute,this_ccu.activations[i].latitude.second,this_ccu.activations[i].lat_dir);
        printf("Location #%d# Longitude is #%d#%d#%d#%d#\n",i,this_ccu.activations[i].longitude.degree,this_ccu.activations[i].longitude.minute,this_ccu.activations[i].longitude.second,this_ccu.activations[i].long_dir);
        printf("Activated time is #%s#\n",this_ccu.activations[i].time);
    }
    time_t time_value = time(NULL);
    struct tm tv = *gmtime(&time_value);
    printf("Time #%ld#%d#%d#%d#%d#%d#%d#\n",time_value,tv.tm_year+1900,tv.tm_mon+1,tv.tm_mday,tv.tm_hour,tv.tm_min,tv.tm_sec);

    printf("\n\n******** CCU DATA END ********\n");
}
#endif
