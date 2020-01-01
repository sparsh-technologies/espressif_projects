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
#include <akbi_msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {UNAUTHENTICATED = 0, AUTHENTICATED} AUTH_STATUS;

typedef struct _mob1_ {
    unsigned char id[BLE_APP_ID_SIZE];
    unsigned char mobile_number[MOB_NO_SIZE];
    unsigned char mobile_name[MOB_NAME_SIZE];
    unsigned char android_id_or_uuid[ANDROID_ID_OR_UUID_SIZE];
    AUTH_STATUS   authentication_status;
    /*
     * bitmap used to indicate which all data is set as part of registration. lsb (1st bit) will be set for id. That is 0x01.
     * 2nd bit will be set for mobile number (0x02), 3rd bit (0x04) for mobile name and 4th bit (0x08) for android id/ uuid.
     */
    unsigned char data_status;
} MOB1;

typedef struct _personal_voice_message_ {
    char message_file_name[FILE_NAME_SIZE];
} PERSONAL_VOICE_MESSAGE;

typedef struct _emergency_numbers_ {
    char default_emergency_number[DEFAULT_EMERGENCY_NUMBER_SIZE];
    char first_responder[MOB_NO_SIZE];
    char close_relative[MOB_NO_SIZE];
} EMERGENCY_NUMBERS;

typedef struct _personal_numbers_ {
    char mob1_mobile_number[MOB_NO_SIZE];
    char second_number[MOB_NO_SIZE];
    char third_number[MOB_NO_SIZE];
} PERSONAL_NUMBERS;

typedef enum {NOT_CONNECTED = 0, WIFI_CONNECTED, INET_CONNECTED, SERVER_CONNECTED} WIFI_STATUS;

typedef struct _wifi_ {
    char ssid[SSID_SIZE];
    char network_key[NETWORK_KEY_SIZE];
    WIFI_STATUS status;
} WIFI;

typedef struct _site_ {
    char latitude[LAT_LONG_SIZE];
    char longitude[LAT_LONG_SIZE];
    char location_time[TIMESTAMP_SIZE];
    char address_audio_file_name[FILE_NAME_SIZE];
    char audio_time[TIMESTAMP_SIZE];
    char fourth_phone_number[MOB_NO_SIZE];
    char fifth_phone_number[MOB_NO_SIZE];
} SITE;

typedef struct _fw_upgrade_log_ {
    char version[FW_VERSION_STRING_SIZE];
    char date[DATE_SIZE];
    char time[TIMESTAMP_SIZE];
} FW_UPGRADE_LOG;

typedef struct _activation_log_ {
    char date[DATE_SIZE];
    char time[TIMESTAMP_SIZE];
} ACTIVATION_LOG;

typedef struct _rescue_termination_log_ {
    char date[DATE_SIZE];
    char time[TIMESTAMP_SIZE];
} RESCUE_TERMINATION_LOG;

typedef struct _wifi_connections_log_ {
    char date[DATE_SIZE];
    char time[TIMESTAMP_SIZE];
    char ssid[SSID_SIZE];
} WIFI_CONNECTIONS_LOG;

typedef enum {OFFLINE = 0, MONITOR, RESCUE, RESCUE_TERMINATION} CCU_MODE;

typedef struct ccu {
    char                     serial_number[SER_NO_SIZE];
    char                     password[PASS_SIZE];
    MOB1                     paired_mob1;
    PERSONAL_VOICE_MESSAGE   personal_voice_messages[PERSONAL_VOICE_MESSAGES_COUNT];
    EMERGENCY_NUMBERS        conf_emergency_nos;
    PERSONAL_NUMBERS         conf_personal_nos;
    WIFI                     conf_wifi;
    SITE                     visited_locations[LOCATION_COUNT];
    CCU_MODE                 mode;
    FW_UPGRADE_LOG           fw_upgrades[FW_UPGRADE_COUNT];
    ACTIVATION_LOG           activations[ACTIVATIONS_COUNT];
    RESCUE_TERMINATION_LOG   rescue_terminations[ACTIVATIONS_COUNT];
    WIFI_CONNECTIONS_LOG     wifi_connections[WIFI_CONNECTIONS_COUNT];
    unsigned short           data_status;
} CCU;

CCU this_ccu;

/*
 * Register command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_register(char *i_cmd, char *i_ret_msg) {
    char data_type                          = i_cmd[BLE_CMD_REG_DATA_TYPE_OFFSET];
    int  data_len_in_ble                    = (int)i_cmd[BLE_CMD_REG_DATA_LEN_OFFSET];
    i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET] = data_type;

    switch (data_type) {
        case DID_REGISTER_PASSWORD : {
            printf("Password length is #%d#\n",data_len_in_ble);
            memcpy(this_ccu.password,&i_cmd[BLE_CMD_REG_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_PASSWORD;
            //TODO-Store password in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NO : {
            memcpy(this_ccu.paired_mob1.mobile_number,&i_cmd[BLE_CMD_REG_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NUM;
            //TODO-Store mobile number in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NAME : {
            memcpy(this_ccu.paired_mob1.mobile_name,&i_cmd[BLE_CMD_REG_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NAME;
            //TODO-Store mobile name in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_ANDROID_ID_OR_UUID : {
            memcpy(this_ccu.paired_mob1.android_id_or_uuid,&i_cmd[BLE_CMD_REG_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_ANDROID_ID_OR_UUID;
            //TODO-Store Android ID or UUID in EEPROM and populate error code
            break;
        }
        default : {
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_UNRECOGNIZED_DATA,BLE_RETURN_RC_SIZE);
            break;
        }
    }
    /*
     * If all information is obtained from the mobile for registration, register the CCU with the Web Application once when the SSID gets configured.
     * mob1.data_status will be set to the bit value [00001111] if all mob1 data is saved. Also the 2nd bit of ccu.data_status will be set if password is set.
     */
    #ifdef BLE_DEBUG
    printf("Data Status #%x#%x#\n", this_ccu.paired_mob1.data_status, this_ccu.data_status);
    #endif
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
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_DATA_LEN_OFFSET];
    char i_pwd[data_len_in_ble];

    memcpy(i_pwd,&i_cmd[BLE_CMD_DATA_VALUE_OFFSET],data_len_in_ble);
    #ifdef BLE_DEBUG
    printf("In execute login #%s#%s#\n",i_pwd, this_ccu.password);
    #endif
    //memcmp is used as the strings are not exactly strings ending in \0.
    if (0 == memcmp(i_pwd,this_ccu.password,data_len_in_ble)) {
        this_ccu.paired_mob1.authentication_status = AUTHENTICATED;
        memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
        //TODO: Auth Token to be stored in EEPROM
    }
    else {
        this_ccu.paired_mob1.authentication_status = UNAUTHENTICATED;
        printf("Password MISMATCH\n");
        memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_LOGIN_PASSWORD_MISMATCH,BLE_RETURN_RC_SIZE);
    }
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
    unsigned char pass[DEFAULT_PASSWORD_SIZE];
    memset(pass,0x00,DEFAULT_PASSWORD_SIZE);
    if (0 == generate_password(pass)) {
        memcpy(this_ccu.password,pass,DEFAULT_PASSWORD_SIZE);
        //send the password and mobile number over the serial interface to the processor - connection manager.
        printf("Password reset #%s#\n", pass);
    }
}

int read_ble_message(char *i_msg, char *i_ret_msg) {
    unsigned int  is_valid_ble_msg;
    unsigned char source_app_type_identifier;
    unsigned char source_app_identifier[BLE_APP_ID_SIZE];
    unsigned char ble_cmd_id;
    unsigned char ble_command[BLE_COMMAND_SIZE];

    memcpy(&source_app_type_identifier,&i_msg[BLE_APP_TYPE_OFFSET],BLE_APP_TYPE_ID_SIZE);
    memcpy(source_app_identifier,&i_msg[BLE_APP_OFFSET],BLE_APP_ID_SIZE);

    if (source_app_type_identifier == MOB1_APP_TYPE_ID) {
        is_valid_ble_msg = 1;
    } else {
        is_valid_ble_msg = 0;
        i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_SOURCE_APP_TYPE_MISMATCH;
        return (ERROR_SOURCE_APP_TYPE_MISMATCH);
    }
    memcpy(&ble_cmd_id,&i_msg[BLE_CMD_OFFSET],BLE_COMMAND_ID_SIZE);
     /*
     * If this is the first register packet, set the app id in the data structure.
     */
    if ((CID_REGISTER == ble_cmd_id) && ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_ID) == 0x00)) {
        memcpy(this_ccu.paired_mob1.id,&i_msg[BLE_APP_OFFSET],BLE_APP_ID_SIZE);
        this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_ID;
        is_valid_ble_msg = 1;
    }
    else {
        if (0 == memcmp(source_app_identifier,this_ccu.paired_mob1.id,BLE_APP_ID_SIZE)) {
            is_valid_ble_msg = 1;
        }
        else {
            is_valid_ble_msg = 0;
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_SOURCE_APP_MISMATCH,BLE_RETURN_RC_SIZE);
            return (ERROR_SOURCE_APP_MISMATCH);
        }
    }

    //TODO: Store the Mob1 ID onto EEPROM


    memcpy(&i_ret_msg[BLE_RET_MSG_CMD_ID_OFFSET],&ble_cmd_id,BLE_COMMAND_ID_SIZE);

    if (is_valid_ble_msg) {
        switch (ble_cmd_id) {
            case CID_REGISTER : {
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_register(ble_command,i_ret_msg);
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
            default : {
                i_ret_msg[BLE_RET_MSG_RC_OFFSET] = ERROR_UNRECOGNIZED_COMMAND;
                #ifdef BLE_DEBUG
                printf("Unrecognized Command #%x#\n",ble_cmd_id);
                #endif
                break;
            }
        }
    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
}

int populate_serial_no_from_eeprom(char *i_ser) {
    //TODO - access EEPROM
    memset(i_ser,0x00,SER_NO_SIZE);
    strcpy(i_ser,"ABCDEFGH");
    return 0;
}

int main(int argc, char** argv) {
    char ble_message[BLE_MESSAGE_SIZE];
    char ble_return_message[BLE_RETURN_SIZE];
    this_ccu.paired_mob1.data_status = 0x00;

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_SIZE);
    populate_serial_no_from_eeprom(this_ccu.serial_number);

    memcpy(ble_message, REGISTER_MSG1_TEST, 14);
    memcpy(&ble_return_message[0],&CCU_TYPE_ID,1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#\n", ble_return_message, ble_return_message[6]);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#\n", ble_return_message, ble_return_message[6]);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG3_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#\n", ble_return_message, ble_return_message[6]);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG4_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#\n", ble_return_message, ble_return_message[6]);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#%x%x#\n", ble_return_message, ble_return_message[6],ble_return_message[7],ble_return_message[8]);
    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, FORGOT_PASSWORD_MSG_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message #%s#\n",ble_message);
    printf("BLE Return Message #%s#\n", ble_return_message);
    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing #%s#%x#%x%x#\n", ble_return_message, ble_return_message[6],ble_return_message[7],ble_return_message[8]);
    #endif

}
