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

CCU this_ccu;

const char  SER_NO_TEST[SER_NO_SIZE]                 = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};
const char  MY_SSID_TEST[SSID_SIZE]               = {0x41,0x42,0x43,0x44,0x45,0x46};
const char  MY_NETWORK_KEY_TEST[NETWORK_KEY_SIZE] = {0x41,0x42,0x43,0x44,0x45,0x46};
int return_msg_len                                = BLE_RETURN_MAX_SIZE;
int searched_ssid_count_index                     = 0;
/*
 * Register command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_register(char *i_cmd, char *i_ret_msg) {
    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble                    = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;


    switch (data_type) {
        case DID_REGISTER_PASSWORD : {
            printf("Password length is #%d#\n",data_len_in_ble);
            memcpy(this_ccu.password,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_PASSWORD;
            //TODO-Store password in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NO : {
            memcpy(this_ccu.paired_mob1.mobile_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NUM;
            //TODO-Store mobile number in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NAME : {
            memcpy(this_ccu.paired_mob1.mobile_name,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NAME;
            //TODO-Store mobile name in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_ANDROID_ID_OR_UUID : {
            memcpy(this_ccu.paired_mob1.android_id_or_uuid,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_ANDROID_ID_OR_UUID;
            //TODO-Store Android ID or UUID in EEPROM and populate error code
            break;
        }
        default : {
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
            break;
        }
    }
    /*
     * If all information is obtained from the mobile for registration, register the CCU with the Web Application once when the SSID gets configured.
     * mob1.data_status will be set to the bit value [00001111] if all mob1 data is saved. Also the 2nd bit of ccu.data_status will be set if password is set.
     */
    #ifdef BLE_DEBUG
    printf("Data Status #%x#%x#\n", this_ccu.paired_mob1.data_status, this_ccu.data_status);
    printf("flag data set all #%x\n",FLAG_DATA_SET_MOB1_ALL );
    #endif

   if (this_ccu.paired_mob1.data_status == FLAG_DATA_SET_MOB1_ALL){
       return READY_TO_SEND_REG_DATA_TO_SERIAL;
   }

    //The below if condition and processing needs to be moved to 'Select A WiFi' and 'Connect to WiFi' sections.
    if ((this_ccu.paired_mob1.data_status == FLAG_DATA_SET_MOB1_ALL) && ((this_ccu.data_status & FLAG_DATA_SET_CCU_PASSWORD) == FLAG_DATA_SET_CCU_PASSWORD)) {
        //TODO - register with the web application to get the user id. Send the data via serial interface
    }
    return i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

/*
 * login command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_login(char *i_cmd, char *i_ret_msg) {
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
    }
    return 0;
}

int generate_password(char *i_pass) {
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
int execute_forgot_password(char *i_ret_msg) {
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
        /*
         * TODO - send the password and mobile number over the serial interface to the processor - connection manager.
         * Return 0x00 if text message is sent successfully. 01 if problem sending text, 02 if mobile number not configured.
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
int execute_change_password(char *i_cmd, char *i_ret_msg) {
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    char i_pwd[data_len_in_ble];

    memcpy(i_pwd,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    switch (data_type) {
        case DID_CHANGE_PASSWORD_CURRENT: {
            if (0 == memcmp(this_ccu.password, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble)) {
                this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_PWD_MATCH;
                i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            }
            else {
                i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_CHANGE_PASSWORD_MISMATCH;
                return ERROR_CHANGE_PASSWORD_MISMATCH;
            }
            break;
        }
        case DID_CHANGE_PASSWORD_NEW: {
            memcpy(this_ccu.new_password_to_be_set,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_NEW_PASSWORD;
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            break;
        }
        default: {
            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_DATA;
            break;
        }
    }
    /*
     * Check if current password is matching and we received the new password. If both are done, replace the password.
     */
    if ((this_ccu.data_status & (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) == (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) {
        memcpy(this_ccu.password,this_ccu.new_password_to_be_set, PASS_SIZE);
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        //Once copied, reset the flags.
        this_ccu.data_status = this_ccu.data_status & (~(FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD));
    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int enable_ccu_access_point() {
    //TODO: send the AP start request to the processor over serial interface
    this_ccu.interface_wifi.mode = ACCESS_POINT;
    memcpy(this_ccu.interface_wifi.ssid,&MY_SSID_TEST,MY_SSID_SIZE);
    memcpy(this_ccu.interface_wifi.network_key,&MY_NETWORK_KEY_TEST,MY_NETWORK_KEY_SIZE);
    return 0;
}

int enable_ccu_wifi_station() {
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

int execute_store_emergency_number(char *i_cmd, char *i_ret_msg) {
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
            break;
        }
        case DID_EMERGENCY_CLOSE_RELATIVE : {
            memcpy(this_ccu.conf_emergency_nos.close_relative,i_emergency_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_store_personal_number(char *i_cmd, char *i_ret_msg) {
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
            break;
        }
        case DID_PERSONAL_THIRD_NUMBER : {
            memcpy(this_ccu.conf_personal_nos.third_number,i_personal_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            break;
        }
        default: {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_UNRECOGNIZED_DATA, BLE_RETURN_RC_SIZE);
            break;
        }

    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_enter_local_help_number(char *i_cmd, char *i_ret_msg) {
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
            break;
        }
        case DID_LOCAL_HELP_FIFTH_NUMBER : {
            memcpy(this_ccu.visited_locations[i_visited_locations_count].fifth_phone_number,i_local_help_number,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
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

int get_scanned_wifis() {
    //get the SSIDs from the serial response from the processor and populate the CCU data strucure.
    char ssids[MAX_WIFI_SCAN_COUNT][SSID_SIZE] = {
        {"First WiFi"},
        {"Second WiFi"},
        {"Third WiFi"}
    };
    int i;
    for (i = 0; i < 3; i++) {
        strcpy(this_ccu.scanned_wifis[i].ssid, ssids[i]);
    }
    return i;
}

int execute_scan_wifis(char *i_cmd ,char *i_ret_msg) {

    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    if (this_ccu.interface_wifi.mode != STATION) {
        if (0 != enable_ccu_wifi_station()) {
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_MY_WIFI_STN_START, BLE_RETURN_RC_SIZE);
            return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
        }
    }
    //TODO: Send the command to scan WiFis to the processor and get response.
    printf("data_type %c\ncount type %c\n",data_type,BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE);
    if(data_type == BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE){
        int count = get_scanned_wifis();
        this_ccu.scanned_wifi_count = count;
        printf("\ncount %d\n",count);
        memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
        memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE,
               BLE_COMMAND_DATA_TYPE_SIZE);
        memcpy(&i_ret_msg[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],&count,SCANNED_WIFI_COUNT_SIZE);
        searched_ssid_count_index = 0;
    }
    else if(data_type == BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE){
       memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
       memset(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET], BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE,
              BLE_COMMAND_DATA_TYPE_SIZE);
       memcpy(&i_ret_msg[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],&this_ccu.scanned_wifis[searched_ssid_count_index].ssid,SCANNED_WIFI_NAME_SIZE);
       printf("\nssid %s\n",this_ccu.scanned_wifis[searched_ssid_count_index].ssid);
       searched_ssid_count_index++;
    }

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int get_wifi_status() {
    //TODO: Pass on the request to connect wifi to the processor.
    return SERVER_CONNECTED;
}

int execute_select_a_wifi(char *i_cmd, char *i_ret_msg) {
    char data_type       = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    char i_data_value[data_len_in_ble];

    memcpy(i_data_value,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    switch (data_type) {
        case DID_SELECT_A_WIFI_SSID : {
            memcpy(this_ccu.conf_wifi.ssid,i_data_value,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            this_ccu.conf_wifi.data_status |= FLAG_DATA_SET_SEL_WIFI_SSID;
            break;
        }
        case DID_SELECT_A_WIFI_NETWORK_KEY : {
            memcpy(this_ccu.conf_wifi.network_key,i_data_value,data_len_in_ble);
            memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);
            this_ccu.conf_wifi.data_status |= FLAG_DATA_SET_SEL_WIFI_NETWORK_KEY;
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

int execute_store_address_visiting(char *i_cmd, char *i_ret_msg) {
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

int execute_ccu_activate(char *i_cmd, char *i_ret_msg) {
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

    switch (direction) {
        case 'N' : {
            this_ccu.activations[i_activations_count].lat_dir = NORTH;
            break;
        }
        case 'S' : {
            this_ccu.activations[i_activations_count].lat_dir = SOUTH;
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
    this_ccu.activations[i_activations_count].longitude.degree = atoi(degree);
    this_ccu.activations[i_activations_count].longitude.minute = atoi(minute);
    this_ccu.activations[i_activations_count].longitude.second = atoi(second);

    switch (direction) {
        case 'E' : {
            this_ccu.activations[i_activations_count].long_dir = EAST;
            break;
        }
        case 'W' : {
            this_ccu.activations[i_activations_count].long_dir = WEST;
            break;
        }
        default : {
            printf("Direction Parsing Error #%c#\n",direction);
            break;
        }
    }

    //this_ccu.visited_locations_count++;
    sprintf(this_ccu.activations[i_activations_count].time,"%ld",time(NULL));
    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], SUCCESS, BLE_RETURN_RC_SIZE);

    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int execute_connect_to_wifi(char *i_cmd, char *i_ret_msg) {
    //TODO: Get the wifi status from the processor
    this_ccu.conf_wifi.status = get_wifi_status();
    memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&this_ccu.conf_wifi.status,BLE_RETURN_RC_SIZE);
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int read_ble_message(char *i_msg, char *i_ret_msg) {
    int  is_valid_ble_msg;
    char source_app_type_identifier;
    char source_app_identifier;
    char ble_cmd_id;
    char ble_command[BLE_COMMAND_SIZE];
    source_app_type_identifier = i_msg[BLE_APP_TYPE_OFFSET];
    source_app_identifier      = i_msg[BLE_APP_OFFSET];

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
    if ((CID_REGISTER == ble_cmd_id) && ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_ID) == 0x00)) {
        memcpy(this_ccu.paired_mob1.id,&i_msg[BLE_APP_OFFSET],BLE_APP_ID_SIZE);
        printf("paired_mob1.id stored %c\n",i_msg[BLE_APP_OFFSET] );
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_ID;
        is_valid_ble_msg = 1;//----------------------------------------------------------------------------------------??
    }
    else {
        if (source_app_identifier == this_ccu.paired_mob1.id[0]) {
            is_valid_ble_msg = 1;
        }
        else {
            is_valid_ble_msg = 0;
            printf("paired_mob1.id stored %c\n",this_ccu.paired_mob1.id[0] );

            i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_SOURCE_APP_MISMATCH;
            return (ERROR_SOURCE_APP_MISMATCH);
        }
    }

    //TODO: Store the Mob1 ID onto EEPROM


    if (is_valid_ble_msg) {
        switch (ble_cmd_id) {
            case CID_REGISTER : {
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                return execute_register(ble_command,i_ret_msg);
                break;
            }
            case CID_LOGIN : {
                #ifdef BLE_DEBUG
                printf("Going to call execute login\n");
                #endif
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_login(ble_command,i_ret_msg);
                break;
            }
            case CID_FORGOT_PASSWORD : {
                #ifdef BLE_DEBUG
                printf("Going to call execute forgot password\n");
                #endif
                execute_forgot_password(i_ret_msg);
                break;
            }
            case CID_CHANGE_PASSWORD : {
                #ifdef BLE_DEBUG
                printf("Going to call execute change password\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_AUTHENTICATION;
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_change_password(ble_command,i_ret_msg);
                break;
            }
            case CID_RECORD_PERSONAL_VOICE_MSG : {
                #ifdef BLE_DEBUG
                printf("Going to call execute record pers voice message\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                execute_record_personal_voice_msg(i_ret_msg);
                break;
            }
            case CID_STORE_EMERGENCY_NUMBERS : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_store_emergency_number\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_store_emergency_number(ble_command,i_ret_msg);
                break;
            }
            case CID_STORE_PERSONAL_NUMBERS : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_store_personal_number\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_store_personal_number(ble_command,i_ret_msg);
                break;
            }
            case CID_SCAN_WIFIS : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_scan_wifis\n");
                #endif
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                execute_scan_wifis(ble_command ,i_ret_msg);
                break;
            }
            case CID_SELECT_A_WIFI : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_select_a_wifi\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_select_a_wifi(ble_command,i_ret_msg);
                break;
            }
            case CID_ADDRESS_VISITING : {
                #ifdef BLE_DEBUG
                printf("Going to call execute store address visiting\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_store_address_visiting(ble_command,i_ret_msg);
                break;
            }
            case CID_ENTER_LOCAL_HELP_NUMBERS : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_enter_local_help_number\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_enter_local_help_number(ble_command,i_ret_msg);
                break;
            }
            case CID_CCU_ACTIVATE : {
                #ifdef BLE_DEBUG
                printf("Going to call execute ccu activate\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_ccu_activate(ble_command,i_ret_msg);
                break;
            }
            case CID_CONNECT_TO_WIFI : {
                #ifdef BLE_DEBUG
                printf("Going to call execute_connect_to_wifi\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memset(&i_ret_msg[BLE_RET_MSG_RC_OFFSET], ERROR_AUTHENTICATION, BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_connect_to_wifi(ble_command,i_ret_msg);
                break;
            }
            default : {
                i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_COMMAND;
                #ifdef BLE_DEBUG
                printf("Unrecognized Command #%x#\n",ble_cmd_id);
                #endif
                break;
            }
        }
    }
    return i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int populate_serial_no_from_eeprom(char *i_ser) {
    //TODO - access EEPROM
    memset(i_ser,0x00,SER_NO_SIZE);
    memcpy(i_ser,&SER_NO_TEST,SER_NO_SIZE);
    return 0;
}

void print_bytes(char *const_text, char *message, int size_of_msg) {
    printf("%s",const_text);
    for (int i = 0; i<size_of_msg; i++) {
        printf("_%2x_",message[i]);
    }
    printf("#\n");
}

void print_chars(char *const_text,char *message, int size_of_msg) {
    printf("%s",const_text);
    for (int i = 0; i<size_of_msg; i++) {
        printf("_%c_",message[i]);
    }
    printf("#\n");
}

void print_ccu() {
    char buf[20];
    printf("******** CCU DATA START ********\n\n\n");
    print_chars("Serial Number #",this_ccu.serial_number, SER_NO_SIZE);
    print_chars("Password #",this_ccu.password, PASS_SIZE);
    print_bytes("Paired Mob1 ID #",this_ccu.paired_mob1.id, BLE_APP_ID_SIZE);
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

int populate_bt_msg_to_serial(char *received_value_buffer,char *msg_to_ccu, int *msg_length){

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)msg_to_ccu;

    p_protocol_hdr->opcode   = received_value_buffer[BLE_CMD_OFFSET];
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    p = msg_to_ccu + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p,(received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    *msg_length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;

    return 0;
}

int main(int argc, char** argv) {
    char ble_message[BLE_MESSAGE_SIZE];
    char ble_return_message[BLE_RETURN_MAX_SIZE];
    char ble_return_message_with_scanned_wifis[MAX_WIFI_SCAN_COUNT][BLE_RETURN_MAX_SIZE];
    this_ccu.paired_mob1.data_status = 0x00;

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    populate_serial_no_from_eeprom(this_ccu.serial_number);

    memcpy(ble_message, REGISTER_MSG1_TEST, 13);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);

    #ifdef BLE_DEBUG
    print_bytes("BLE Message REGISTER MSG 1#", ble_message, BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message REGISTER MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing REGISTER MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message REGISTER MSG 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message REGISTER MSG 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing REGISTER MSG 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG3_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message REGISTER MSG 3#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message REGISTER MSG 3#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing REGISTER MSG 3#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG4_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message REGISTER MSG 4#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message REGISTER MSG 4#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing REGISTER MSG 4#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    print_bytes("BLE Message LOGIN MSG 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message LOGIN MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing LOGIN MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    // memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    // strcpy(ble_message, FORGOT_PASSWORD_MSG_TEST);
    // ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    //
    // #ifdef BLE_DEBUG
    // print_bytes("BLE Message FORGOT PASS MSG#",ble_message,BLE_MESSAGE_SIZE);
    // print_bytes("BLE Return Message FORGOT PASS MSG#",ble_return_message,BLE_RETURN_MAX_SIZE);
    // #endif
    //
    // read_ble_message(ble_message, ble_return_message);
    // #ifdef BLE_DEBUG
    // print_bytes("BLE Return Message after processing FORGOT PASS MSG#",ble_return_message,BLE_RETURN_MAX_SIZE);
    // #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message Chg password msg 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message Chg password msg 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing Chg password msg 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message Chg password msg 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message Chg password msg 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing Chg password msg 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message LOGIN MSG 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message LOGIN MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing LOGIN MSG 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message LOGIN MSG 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message LOGIN MSG 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing LOGIN MSG 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG3_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message Change Password Msg 3#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message Change Password Msg 3#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing Change Password Msg 3#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG4_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message Change Password Msg 4#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message Change Password Msg 4#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing Change Password Msg 4#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message LOGIN Msg 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message LOGIN Msg 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing LOGIN Msg 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, RECORD_PERSONAL_VOICE_MSG_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message RECORD PERSONAL VOICE MESSAGE#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message RECORD PERSONAL VOICE MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing RECORD PERSONAL VOICE MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, STORE_EMERGENCY_NOS_MSG1_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message STORE EMERGENCY NOS MESSAGE 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message STORE EMERGENCY NOS MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing STORE EMERGENCY NOS MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, STORE_EMERGENCY_NOS_MSG2_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message STORE EMERGENCY NOS MESSAGE 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message STORE EMERGENCY NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing STORE EMERGENCY NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, STORE_PERSONAL_NOS_MSG1_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message STORE PERSONAL NOS MESSAGE 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message STORE PERSONAL NOS MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing STORE PERSONAL NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, STORE_PERSONAL_NOS_MSG2_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message STORE PERSONAL NOS MESSAGE 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message STORE PERSONAL NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing STORE PERSONAL NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, SCAN_WIFIS_MSG_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message SCAN WIFIS MESSAGE#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message SCAN WIFIS MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing SCAN WIFIS MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    for (int i = 0; i <this_ccu.scanned_wifi_count; i++) {
        memset(&ble_return_message_with_scanned_wifis[i][0], CCU_TYPE_ID, 1);
        memcpy(&ble_return_message_with_scanned_wifis[i][1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
        memcpy(&ble_return_message_with_scanned_wifis[i][BLE_RET_MSG_RC_OFFSET],&ble_return_message[BLE_RET_MSG_RC_OFFSET],BLE_RETURN_RC_SIZE);
        ble_return_message_with_scanned_wifis[i][BLE_RET_MSG_DATA_TYPE_OFFSET] = BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE;
        memcpy(&ble_return_message_with_scanned_wifis[i][BLE_RET_MSG_DATA_TYPE_OFFSET+BLE_CMD_SINGLE_DATA_VALUE_OFFSET],&this_ccu.scanned_wifis[i].ssid,SSID_SIZE);
    }
    #ifdef BLE_DEBUG
        printf("BLE Return Message after populating SCAN WIFIS\n");
    for (int i = 0; i <this_ccu.scanned_wifi_count; i++) {
        char buf[BLE_RETURN_MAX_SIZE];
        memcpy(&buf,&ble_return_message_with_scanned_wifis[i][0],BLE_RETURN_MAX_SIZE);
        print_chars("SSID No. #",buf,BLE_RETURN_MAX_SIZE);
    }
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, SELECT_A_WIFI_MSG1_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message SELECT A WIFI MESSAGE 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message SELECT A WIFI MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing SELECT A WIFI MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, SELECT_A_WIFI_MSG2_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message SELECT A WIFI MESSAGE 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message SELECT A WIFI MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing SELECT A WIFI MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, ADDRESS_VISITING_MSG_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message ADDRESS VISITING MESSAGE#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message ADDRESS VISITING MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing ADDRESS VISITING MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, ENTER_LOCAL_HELP_NOS_MSG1_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message ENTER LOCAL HELP NOS MESSAGE 1#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message ENTER LOCAL HELP NOS MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing ENTER LOCAL HELP NOS MESSAGE 1#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, ENTER_LOCAL_HELP_NOS_MSG2_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message ENTER LOCAL HELP NOS MESSAGE 2#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message ENTER LOCAL HELP NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing ENTER LOCAL HELP NOS MESSAGE 2#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, CCU_ACTIVATE_MSG_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message CCU ACTIVATE#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message CCU ACTIVATE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing CCU ACTIVATE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_MAX_SIZE);
    strcpy(ble_message, CONNECT_TO_WIFI_MSG_TEST);
    memset(&ble_return_message[0], CCU_TYPE_ID, 1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;

    #ifdef BLE_DEBUG
    print_bytes("BLE Message CONNECT TO WIFI MESSAGE#",ble_message,BLE_MESSAGE_SIZE);
    print_bytes("BLE Return Message CONNECT TO WIFI MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    print_bytes("BLE Return Message after processing CONNECT TO WIFI MESSAGE#",ble_return_message,BLE_RETURN_MAX_SIZE);
    #endif

    print_ccu();
}
