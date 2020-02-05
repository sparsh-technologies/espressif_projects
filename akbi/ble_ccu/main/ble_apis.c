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
#include "akbi_serial_task.h"
#include <rom/ets_sys.h>
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"


CCU this_ccu;

const char  SER_NO_TEST[SER_NO_SIZE]                 = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};
const char  MY_SSID_TEST[SSID_SIZE]               = {0x41,0x42,0x43,0x44,0x45,0x46};
const char  MY_NETWORK_KEY_TEST[NETWORK_KEY_SIZE] = {0x41,0x42,0x43,0x44,0x45,0x46};
int return_msg_len                                = BLE_RETURN_MAX_SIZE;
int searched_ssid_count_index                     = 0;
static char saved_messages[4][21];
static char *p_recvd_msg_full;
static char *p_return_msg_full;
static int flag_set_recvd_msg_ptr = 0;
static int flag_set_return_msg_ptr = 0;

/*
 * All extern API's are declared here.
 */
extern int ccu_send_login_msg(void);
extern int ccu_sent_scan_all_wifi_msg(char * ret_msg);

AKBI_WIFI_SCAN_REPORT  wifi_scan_report;

void set_recvd_msg_pointer(char *received_value_buffer){
    p_recvd_msg_full = received_value_buffer;
    flag_set_recvd_msg_ptr = 1;
}
void set_return_msg_pointer(char *ep_return_message){
    p_return_msg_full = ep_return_message;
    flag_set_return_msg_ptr = 1;
}

int save_group_messages(char *received_value_buffer,int type_id){
    memcpy(saved_messages[type_id-1],received_value_buffer,BLE_MESSAGE_SIZE);
    return 0;
}

//to send all messages altogether
void send_batch_messages(int no_of_messages,int ble_cmd_id){
    for(int i=0 ; i< no_of_messages ; i++){
        switch(ble_cmd_id){
            case CID_REGISTER:
                ccu_send_reg_msg(saved_messages[i],p_return_msg_full);
                break;
            case CID_CHANGE_PASSWORD:
                ccu_sent_user_change_passwd_msg(saved_messages[i],p_return_msg_full);
                break;
            case CID_STORE_EMERGENCY_NUMBERS:
                ccu_sent_store_emergency_number_msg(saved_messages[i]);
                break;
            case CID_STORE_PERSONAL_NUMBERS:
                ccu_sent_store_personal_number_msg(saved_messages[i]);
                break;
            case CID_ENTER_LOCAL_HELP_NUMBERS:
                ccu_sent_store_local_help_number_msg(saved_messages[i]);
                break;
        }
        ets_delay_us(1000000);
    }
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
        memcpy(this_ccu.password,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
        printf(" INFO : Passwd - %s(%d) \n", this_ccu.password, data_len_in_ble);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
		    // To Be checked with Sathish
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_PASSWORD;

        ccu_send_reg_msg_new(DID_REGISTER_PASSWORD, this_ccu.password);

//        save_group_messages(p_recvd_msg_full,DID_REGISTER_PASSWORD);
        break;

    case DID_REGISTER_MOB_NO :
        memcpy(this_ccu.paired_mob1.mobile_number,
               &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
        printf(" INFO : Mob-No - %s(%d) \n", this_ccu.paired_mob1.mobile_number, data_len_in_ble);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NUM;
        //TODO-Store mobile number in EEPROM and populate error code
//        save_group_messages(p_recvd_msg_full,DID_REGISTER_MOB_NO);
        ccu_send_reg_msg_new(DID_REGISTER_MOB_NO, this_ccu.paired_mob1.mobile_number);
        break;

    case DID_REGISTER_MOB_NAME :
        memcpy(this_ccu.paired_mob1.mobile_name,
               &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
        printf(" INFO : Mob-Name - %s(%d) \n", this_ccu.paired_mob1.mobile_name, data_len_in_ble);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NAME;
        //TODO-Store mobile name in EEPROM and populate error code
//        save_group_messages(p_recvd_msg_full,DID_REGISTER_MOB_NAME);
        ccu_send_reg_msg_new(DID_REGISTER_MOB_NAME, this_ccu.paired_mob1.mobile_name);
        break;

    case DID_REGISTER_ANDROID_ID_OR_UUID :
        memcpy(this_ccu.paired_mob1.android_id_or_uuid,
               &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
        printf(" INFO : Mob-UUID - %s(%d) \n", this_ccu.paired_mob1.android_id_or_uuid, 
                 data_len_in_ble);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_ANDROID_ID_OR_UUID;
        //TODO-Store Android ID or UUID in EEPROM and populate error code
//        save_group_messages(p_recvd_msg_full,DID_REGISTER_ANDROID_ID_OR_UUID);
        ccu_send_reg_msg_new(DID_REGISTER_ANDROID_ID_OR_UUID, this_ccu.paired_mob1.android_id_or_uuid);
        break;

    default :
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
        break;
    }

    /*
     * If all information is obtained from the mobile for registration, register the
     * CCU with the Web Application once when the SSID gets configured.
     * mob1.data_status will be set to the bit value [00001111] if all mob1 data is saved.
     * Also the 2nd bit of ccu.data_status will be set if password is set.
     */

    if (this_ccu.paired_mob1.data_status == FLAG_DATA_SET_MOB1_ALL){
       //all messages are saved. now send messages altogether
//       send_batch_messages(DID_REGISTER_ANDROID_ID_OR_UUID,CID_REGISTER);
       //return READY_TO_SEND_REG_DATA_TO_SERIAL;
    }

   /*
    * The below if condition and processing needs to be moved to 'Select A WiFi'
    * and 'Connect to WiFi' sections.
    */

    if ((this_ccu.paired_mob1.data_status == FLAG_DATA_SET_MOB1_ALL) &&
        ((this_ccu.data_status & FLAG_DATA_SET_CCU_PASSWORD) == FLAG_DATA_SET_CCU_PASSWORD)) {
        //TODO - register with the web application to get the user id. Send the data via serial interface
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
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_SINGLE_DATA_LEN_OFFSET];
    char i_pwd[data_len_in_ble];

    memcpy(i_pwd,&i_cmd[BLE_CMD_SINGLE_DATA_VALUE_OFFSET],data_len_in_ble);
    printf("data %s\n",i_pwd);
    #ifdef BLE_DEBUG
    printf("In execute login #%s#%s#%d#\n",i_pwd, this_ccu.password,data_len_in_ble);
    #endif
    //memcmp is used as the strings are not exactly strings ending in \0.
    if (0 == memcmp(i_pwd,this_ccu.password,data_len_in_ble)) {
        this_ccu.paired_mob1.authentication_status = AUTHENTICATED;
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        //TODO: Auth Token to be stored in EEPROM
    }
    else {
        this_ccu.paired_mob1.authentication_status = UNAUTHENTICATED;
        printf("Password MISMATCH\n");
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_LOGIN_PASSWORD_MISMATCH;
        return (1);
    }

    /*
     * If login is successful, then send a message to the CCU that we have logged into the device.
     * This is just to synchronize the state between both the CCU and ESP32 module.
     */
    ccu_send_login_msg();
    return 0;
}

int generate_password(char *i_pass)
{
    int i;
    srand(time(NULL));
    for(i = 0; i < 12; i++) {
        i_pass[i] = 33 + rand() % 94;
    }
    i_pass[i] = '\0';
    return 0;
}

/*
 * forgot password command execution
 * i_ret_msg: pointer to the return message
 */
int execute_forgot_password(char *i_ret_msg)
{
    /*
     * Set a generic password.
     * Send that password to the configured phone number.
     */
    char pass[DEFAULT_PASSWORD_SIZE];
    memset(pass,0x00,DEFAULT_PASSWORD_SIZE);
    if (0 == generate_password(pass)) {
        memcpy(this_ccu.password,pass,DEFAULT_PASSWORD_SIZE);
        if ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_NUM) == 0x00) {
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_MOB1_NO_NOT_CONFIGURED;
            return ERROR_MOB1_NO_NOT_CONFIGURED;
        }
        else{
            akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD);
            ccu_sent_user_forgot_passwd_msg();
        }
        /*
         * TODO - send the password and mobile number over the serial interface to the processor - connection manager.
         * Return 0x00 if text message is sent successfully. 01 if problem sending text
         */
        printf("Password reset #%s#\n", pass);
    }
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

    memcpy(i_pwd,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    switch (data_type)
    {

    case DID_CHANGE_PASSWORD_CURRENT:
        if (0 == memcmp(this_ccu.password, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble)) {
            this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_PWD_MATCH;
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            save_group_messages(p_recvd_msg_full,DID_CHANGE_PASSWORD_CURRENT);
        }
        else {
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_CHANGE_PASSWORD_MISMATCH;
            return ERROR_CHANGE_PASSWORD_MISMATCH;
        }
        break;

    case DID_CHANGE_PASSWORD_NEW:
        memcpy(this_ccu.new_password_to_be_set,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
        this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_NEW_PASSWORD;
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        save_group_messages(p_recvd_msg_full,DID_CHANGE_PASSWORD_NEW);
        send_batch_messages(DID_CHANGE_PASSWORD_NEW,CID_CHANGE_PASSWORD);
        break;

    default:
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
        break;

    }

    /*
     * Check if current password is matching and we received the new password. If both are done,
     * replace the password.
     */

    if ((this_ccu.data_status & (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) == (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) {
        memcpy(this_ccu.password,this_ccu.new_password_to_be_set, PASS_SIZE);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        //Once copied, reset the flags.
        this_ccu.data_status = this_ccu.data_status & (~(FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD));
    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int enable_ccu_access_point()
{
    //TODO: send the AP start request to the processor over serial interface
    this_ccu.interface_wifi.mode = ACCESS_POINT;
    memcpy(this_ccu.interface_wifi.ssid,&MY_SSID_TEST,MY_SSID_SIZE);
    memcpy(this_ccu.interface_wifi.network_key,&MY_NETWORK_KEY_TEST,MY_NETWORK_KEY_SIZE);
    return 0;
}

int enable_ccu_wifi_station()
{
    //TODO: send the AP start request to the processor over serial interface
    this_ccu.interface_wifi.mode = STATION;
    return 0;
}

int execute_record_personal_voice_msg(char *i_ret_msg)
{
    if (this_ccu.interface_wifi.mode != ACCESS_POINT) {
        if (0 != enable_ccu_access_point()) {
    //      memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_MY_AP_START,BLE_RETURN_RC_SIZE);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],ERROR_MY_AP_START,BLE_RETURN_RC_SIZE);
            return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
        }
    }
    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
    memcpy(&i_ret_msg[BLE_RET_MSG_MY_SSID_OFFSET],this_ccu.interface_wifi.ssid,SSID_SIZE);
  //  memcpy(&i_ret_msg[BLE_RET_MSG_MY_NETWORK_KEY_OFFSET],this_ccu.interface_wifi.network_key,NETWORK_KEY_SIZE);

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_store_emergency_number(char *i_cmd, char *i_ret_msg)
{
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int data_len_in_ble  = MOB_NO_SIZE;
    char i_emergency_number[data_len_in_ble];

    memcpy(i_emergency_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_FIXED_LEN_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    memcpy(&this_ccu.conf_emergency_nos.default_emergency_number,DEFAULT_EMERGENCY_NUMBER,DEFAULT_EMERGENCY_NUMBER_SIZE);

    switch (data_type) {
        case DID_EMERGENCY_FIRST_RESPONDER : {
            memcpy(this_ccu.conf_emergency_nos.first_responder,i_emergency_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_EMERGENCY_FIRST_RESPONDER);
            break;
        }
        case DID_EMERGENCY_CLOSE_RELATIVE : {
            memcpy(this_ccu.conf_emergency_nos.close_relative,i_emergency_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_EMERGENCY_CLOSE_RELATIVE);
            send_batch_messages(DID_EMERGENCY_CLOSE_RELATIVE,CID_STORE_EMERGENCY_NUMBERS);
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

    memcpy(i_personal_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_FIXED_LEN_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    memcpy(this_ccu.conf_personal_nos.mob1_mobile_number,this_ccu.paired_mob1.mobile_number,data_len_in_ble);

    switch (data_type) {
        case DID_PERSONAL_SECOND_NUMBER : {
            memcpy(this_ccu.conf_personal_nos.second_number,i_personal_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_PERSONAL_SECOND_NUMBER);
            break;
        }
        case DID_PERSONAL_THIRD_NUMBER : {
            memcpy(this_ccu.conf_personal_nos.third_number,i_personal_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_PERSONAL_THIRD_NUMBER);
            //ccu_sent_store_personal_number_msg();
            send_batch_messages(DID_PERSONAL_THIRD_NUMBER,CID_STORE_PERSONAL_NUMBERS);
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
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_LOCAL_HELP_FOURTH_NUMBER);
            break;
        }
        case DID_LOCAL_HELP_FIFTH_NUMBER : {
            memcpy(this_ccu.visited_locations[i_visited_locations_count].fifth_phone_number,i_local_help_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            save_group_messages(p_recvd_msg_full,DID_LOCAL_HELP_FIFTH_NUMBER);
            send_batch_messages(DID_LOCAL_HELP_FIFTH_NUMBER,CID_ENTER_LOCAL_HELP_NUMBERS);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    //this_ccu.visited_locations_count++;
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}


int execute_scan_wifis(char *i_cmd ,char *i_ret_msg)
{

    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    if (this_ccu.interface_wifi.mode != STATION) {
        if (0 != enable_ccu_wifi_station()) {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_MY_WIFI_STN_START, BLE_RETURN_RC_SIZE);
            return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
        }
    }

    //TODO: Send the command to scan WiFis to the processor and get response.
    if(data_type == BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE){
        akbi_clear_ssids();
        akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_IN_PROGRESS);
        ccu_sent_scan_all_wifi_msg(i_ret_msg);
        memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE,
               BLE_COMMAND_DATA_TYPE_SIZE);
        //memcpy(&i_ret_msg[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],&count,SCANNED_WIFI_COUNT_SIZE);
        searched_ssid_count_index = 0;

    }
    else if(data_type == BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE){
        //RC value set in akbi_process_rx_serial_data()
        memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE,
              BLE_COMMAND_DATA_TYPE_SIZE);
        //ssid name set in akbi_process_rx_serial_data()
        // memcpy(&i_ret_msg[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],
        //       &this_ccu.scanned_wifis[searched_ssid_count_index].ssid,SCANNED_WIFI_NAME_SIZE);
        //searched_ssid_count_index++;
    }

    /*
     * Send message to the CCU to start scan operation.
     */

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int get_wifi_status()
{
    //TODO: Pass on the request to connect wifi to the processor.
    return SERVER_CONNECTED;
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
            ccu_sent_configure_wifi_credentials(selected_ap_id+1, p_password, 4 );

            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA,
                   BLE_RETURN_RC_SIZE);
            break;
        }
    }


    if (FLAG_DATA_SET_SEL_WIFI_ALL == (FLAG_DATA_SET_SEL_WIFI_ALL & this_ccu.conf_wifi.data_status)) {
        //TODO: Get the wifi status from the processor
        this_ccu.conf_wifi.status = get_wifi_status();
        memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&this_ccu.conf_wifi.status,BLE_RETURN_RC_SIZE);
    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_connect_to_wifi(char *i_cmd, char *i_ret_msg)
{
    //TODO: Get the wifi status from the processor
    akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_IN_PROGRESS);
    ccu_sent_connect_to_wifi();
    this_ccu.conf_wifi.status = get_wifi_status();
    //memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&this_ccu.conf_wifi.status,BLE_RETURN_RC_SIZE);
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}
int execute_store_address_visiting(char *i_cmd, char *i_ret_msg)
{
    char degree[LAT_LONG_DEGREE_SIZE];
    char minute[LAT_LONG_MINUTE_SIZE];
    char second[LAT_LONG_SECOND_SIZE];
    char direction;
    unsigned char byte_offset = 0;
    unsigned char i_visited_locations_count = this_ccu.visited_locations_count;

    if (this_ccu.interface_wifi.mode != ACCESS_POINT) {
        if (0 != enable_ccu_access_point()) {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_MY_AP_START, BLE_RETURN_RC_SIZE);
            return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
        }
    }

    memcpy(degree,&i_cmd[byte_offset],LAT_LONG_DEGREE_SIZE);
    byte_offset += LAT_LONG_DEGREE_SIZE;
    memcpy(minute,&i_cmd[byte_offset],LAT_LONG_MINUTE_SIZE);
    byte_offset += LAT_LONG_MINUTE_SIZE;
    memcpy(second,&i_cmd[byte_offset],LAT_LONG_SECOND_SIZE);
    byte_offset += LAT_LONG_SECOND_SIZE;
    direction = i_cmd[byte_offset];
    byte_offset += 1;

    this_ccu.visited_locations[i_visited_locations_count].latitude.degree = atoi(degree);
    this_ccu.visited_locations[i_visited_locations_count].latitude.minute = atoi(minute);
    this_ccu.visited_locations[i_visited_locations_count].latitude.second = atoi(second);

    switch (direction) {
        case 'N' : {
            this_ccu.visited_locations[i_visited_locations_count].lat_dir = NORTH;
            break;
        }
        case 'S' : {
            this_ccu.visited_locations[i_visited_locations_count].lat_dir = SOUTH;
            break;
        }
        default : {
            printf("Direction Parsing Error #%c#\n",direction);
            break;
        }
    }

    memcpy(degree,&i_cmd[byte_offset],LAT_LONG_DEGREE_SIZE);
    byte_offset += LAT_LONG_DEGREE_SIZE;
    memcpy(minute,&i_cmd[byte_offset],LAT_LONG_MINUTE_SIZE);
    byte_offset += LAT_LONG_MINUTE_SIZE;
    memcpy(second,&i_cmd[byte_offset],LAT_LONG_SECOND_SIZE);
    byte_offset += LAT_LONG_SECOND_SIZE;
    direction = i_cmd[byte_offset];
    byte_offset += 1;

    //TODO: get the next vacant location in the visited_locations array
    this_ccu.visited_locations[i_visited_locations_count].longitude.degree = atoi(degree);
    this_ccu.visited_locations[i_visited_locations_count].longitude.minute = atoi(minute);
    this_ccu.visited_locations[i_visited_locations_count].longitude.second = atoi(second);

    switch (direction) {
        case 'E' : {
            this_ccu.visited_locations[i_visited_locations_count].long_dir = EAST;
            break;
        }
        case 'W' : {
            this_ccu.visited_locations[i_visited_locations_count].long_dir = WEST;
            break;
        }
        default : {
            printf("Direction Parsing Error #%c#\n",direction);
            break;
        }
    }

    //this_ccu.visited_locations_count++;
    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
    memcpy(&i_ret_msg[BLE_RET_MSG_MY_SSID_OFFSET],this_ccu.interface_wifi.ssid,MY_SSID_SIZE);
    memcpy(&i_ret_msg[BLE_RET_MSG_MY_NETWORK_KEY_OFFSET],this_ccu.interface_wifi.network_key,MY_NETWORK_KEY_SIZE);

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_ccu_activate(char *i_cmd, char *i_ret_msg)
{
    char degree[LAT_LONG_DEGREE_SIZE];
    char minute[LAT_LONG_MINUTE_SIZE];
    char second[LAT_LONG_SECOND_SIZE];
    char direction;
    unsigned char byte_offset = 0;
    unsigned char i_activations_count = this_ccu.activations_count;

    //TODO: Send message to the processor to switch mode.
    this_ccu.mode = MONITOR;
    memcpy(degree,&i_cmd[byte_offset],LAT_LONG_DEGREE_SIZE);
    byte_offset += LAT_LONG_DEGREE_SIZE;
    memcpy(minute,&i_cmd[byte_offset],LAT_LONG_MINUTE_SIZE);
    byte_offset += LAT_LONG_MINUTE_SIZE;
    memcpy(second,&i_cmd[byte_offset],LAT_LONG_SECOND_SIZE);
    byte_offset += LAT_LONG_SECOND_SIZE;
    direction = i_cmd[byte_offset];
    byte_offset += 1;

    this_ccu.activations[i_activations_count].latitude.degree = atoi(degree);
    this_ccu.activations[i_activations_count].latitude.minute = atoi(minute);
    this_ccu.activations[i_activations_count].latitude.second = atoi(second);

    switch (direction)
    {
    case 'N' :
        this_ccu.activations[i_activations_count].lat_dir = NORTH;
        break;

    case 'S' :
        this_ccu.activations[i_activations_count].lat_dir = SOUTH;
        break;

    default :
        printf("Direction Parsing Error #%c#\n",direction);
        break;

    }

    memcpy(degree,&i_cmd[byte_offset],LAT_LONG_DEGREE_SIZE);
    byte_offset += LAT_LONG_DEGREE_SIZE;
    memcpy(minute,&i_cmd[byte_offset],LAT_LONG_MINUTE_SIZE);
    byte_offset += LAT_LONG_MINUTE_SIZE;
    memcpy(second,&i_cmd[byte_offset],LAT_LONG_SECOND_SIZE);
    byte_offset += LAT_LONG_SECOND_SIZE;
    direction = i_cmd[byte_offset];
    byte_offset += 1;

    //TODO: get the next vacant location in the visited_locations array
    this_ccu.activations[i_activations_count].longitude.degree = atoi(degree);
    this_ccu.activations[i_activations_count].longitude.minute = atoi(minute);
    this_ccu.activations[i_activations_count].longitude.second = atoi(second);

    switch (direction)
    {
    case 'E' :
        this_ccu.activations[i_activations_count].long_dir = EAST;
        break;

    case 'W' :
        this_ccu.activations[i_activations_count].long_dir = WEST;
        break;

    default :
        printf("Direction Parsing Error #%c#\n",direction);
        break;

    }

    //this_ccu.visited_locations_count++;
    sprintf(this_ccu.activations[i_activations_count].time,"%ld",time(NULL));
    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
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

    if(flag_set_recvd_msg_ptr == 0){
      set_recvd_msg_pointer(i_msg);
    }
    if(flag_set_return_msg_ptr == 0){
      set_return_msg_pointer(i_ret_msg);
    }

    if (source_app_type_identifier == MOB1_APP_TYPE_ID) {
        is_valid_ble_msg = 1;
    } else {
        is_valid_ble_msg = 0;
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_SOURCE_APP_TYPE_MISMATCH;
        return (ERROR_SOURCE_APP_TYPE_MISMATCH);
    }

    ble_cmd_id                           = i_msg[BLE_CMD_OFFSET];
    i_ret_msg[BLE_RET_MSG_CCU_ID_OFFSET] = CCU_TYPE_ID;
    i_ret_msg[BLE_RET_MSG_CMD_ID_OFFSET] = ble_cmd_id;

     /*
     * If this is the first register packet, set the app id in the data structure.
     */
    if ((CID_REGISTER == ble_cmd_id) &&
        ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_ID) == 0x00)) {

        this_ccu.paired_mob1.id = i_msg[BLE_APP_OFFSET];
        printf("paired_mob1.id stored %c\n",i_msg[BLE_APP_OFFSET] );
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_ID;
        is_valid_ble_msg = 1;
    }
    else {
        if (source_app_identifier == this_ccu.paired_mob1.id) {
            is_valid_ble_msg = 1;
        }
        else {
            is_valid_ble_msg = 0;
            printf("paired_mob1.id stored %c\n",this_ccu.paired_mob1.id );

            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_SOURCE_APP_MISMATCH;
            return (ERROR_SOURCE_APP_MISMATCH);
        }
    }

    //TODO: Store the Mob1 ID onto EEPROM


    if (is_valid_ble_msg) {

        switch (ble_cmd_id)
        {
        case CID_REGISTER :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_register(ble_command,i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_MOB_REGISTERED);
            break;

        case CID_LOGIN :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_LOGIN);
            execute_login(ble_command,i_ret_msg);
            break;

        case CID_FORGOT_PASSWORD :
            akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD);
            execute_forgot_password(i_ret_msg);
            break;

        case CID_CHANGE_PASSWORD :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_AUTHENTICATION;
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_CHANGE_PASSWD);
            execute_change_password(ble_command,i_ret_msg);
            break;

        case CID_RECORD_PERSONAL_VOICE_MSG :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            execute_record_personal_voice_msg(i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_VOICE_RECORDING_IN_PRORESS);
            break;

        case CID_STORE_EMERGENCY_NUMBERS :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_store_emergency_number(ble_command,i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_SET_EMER_NUM);
            break;

        case CID_STORE_PERSONAL_NUMBERS :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_store_personal_number(ble_command,i_ret_msg);
            //akbi_set_fsm_state(FSM_STATE_SET_PERSONAL_NUM );
            break;

        case CID_SCAN_WIFIS :
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            execute_scan_wifis(ble_command ,i_ret_msg);
            if((akbi_get_fsm_state()!=FSM_STATE_WIFI_SCAN_IN_PROGRESS)&&
                       (akbi_get_fsm_state()!=FSM_STATE_WIFI_SCAN_COMPLETE)&&
                       (akbi_get_fsm_state()!=FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS)){
                akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_IN_PROGRESS);
            }
            break;

        case CID_SELECT_A_WIFI :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_WIFI_SELECT_IN_PROGRESS );
            execute_select_a_wifi(ble_command,i_ret_msg);
            break;

        case CID_ADDRESS_VISITING :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_store_address_visiting(ble_command,i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_CFG_SET_ADDRESS );
            break;

        case CID_ENTER_LOCAL_HELP_NUMBERS :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_enter_local_help_number(ble_command,i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_CFG_SET_HELP_NUM );
            break;

        case CID_CCU_ACTIVATE :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            execute_ccu_activate(ble_command,i_ret_msg);
            akbi_set_fsm_state(FSM_STATE_ACTIVATE_IN_PROGRESS);
            break;

        case CID_CONNECT_TO_WIFI :
            if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                return ERROR_AUTHENTICATION;
            }
            memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_IN_PROGRESS);
            execute_connect_to_wifi(ble_command,i_ret_msg);
            break;

        default :
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_COMMAND;
            printf("Unrecognized Command #%x#\n",ble_cmd_id);
            break;

        }
    }
    return i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

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
