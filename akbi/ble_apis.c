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
#include "akbi_bt_msg.h"

CCU this_ccu;

/*
 * Register command execution
 * i_cmd: The command parameters of register
 * i_ret_msg: pointer to the return message
 */
int execute_register(char *i_cmd, char *i_ret_msg) {
    char data_type                          = i_cmd[BLE_CMD_MULTI_DATA_TYPE_OFFSET];
    int  data_len_in_ble                    = (int)i_cmd[BLE_CMD_MULTI_DATA_LEN_OFFSET];
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    switch (data_type) {
        case DID_REGISTER_PASSWORD : {
            printf("Password length is #%d#\n",data_len_in_ble);
            memcpy(this_ccu.password,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_PASSWORD;
            //TODO-Store password in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NO : {
            memcpy(this_ccu.paired_mob1.mobile_number,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NUM;
            //TODO-Store mobile number in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_MOB_NAME : {
            memcpy(this_ccu.paired_mob1.mobile_name,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            this_ccu.paired_mob1.data_status = this_ccu.paired_mob1.data_status | FLAG_DATA_SET_MOB1_NAME;
            //TODO-Store mobile name in EEPROM and populate error code
            break;
        }
        case DID_REGISTER_ANDROID_ID_OR_UUID : {
            memcpy(this_ccu.paired_mob1.android_id_or_uuid,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
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
    int data_len_in_ble  = (int)i_cmd[BLE_CMD_SINGLE_DATA_LEN_OFFSET];
    char i_pwd[data_len_in_ble];

    memcpy(i_pwd,&i_cmd[BLE_CMD_SINGLE_DATA_VALUE_OFFSET],data_len_in_ble);
    #ifdef BLE_DEBUG
    printf("In execute login #%s#%s#%d#\n",i_pwd, this_ccu.password,data_len_in_ble);
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
        if ((this_ccu.paired_mob1.data_status & FLAG_DATA_SET_MOB1_NUM) == 0x00) {
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_MOB1_NO_NOT_CONFIGURED,BLE_RETURN_RC_SIZE);
            return ERROR_MOB1_NO_NOT_CONFIGURED;
        }
        /*
         * TODO - send the password and mobile number over the serial interface to the processor - connection manager.
         * Return 0x00 if text message is sent successfully. 01 if problem sending text, 02 if mobile number not configured.
         */
        printf("Password reset #%s#\n", pass);
    }
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
    memcpy(&i_ret_msg[BLE_RET_MSG_DATA_TYPE_OFFSET],&data_type,BLE_COMMAND_DATA_TYPE_SIZE);

    switch (data_type) {
        case DID_CHANGE_PASSWORD_CURRENT: {
            if (0 == memcmp(this_ccu.password, &i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble)) {
                this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_PWD_MATCH;
                memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            }
            else {
                memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_CHANGE_PASSWORD_MISMATCH,BLE_RETURN_RC_SIZE);
                return ERROR_CHANGE_PASSWORD_MISMATCH;
            }
            break;
        }
        case DID_CHANGE_PASSWORD_NEW: {
            memcpy(this_ccu.new_password_to_be_set,&i_cmd[BLE_CMD_MULTI_DATA_VALUE_OFFSET],data_len_in_ble);
            this_ccu.data_status = this_ccu.data_status | FLAG_DATA_SET_CCU_NEW_PASSWORD;
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
            break;
        }
        default: {
            memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_UNRECOGNIZED_DATA,BLE_RETURN_RC_SIZE);
            break;
        }
    }
    /*
     * Check if current password is matching and we received the new password. If both are done, replace the password.
     */
    if ((this_ccu.data_status & (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) == (FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD)) {
        memcpy(this_ccu.password,this_ccu.new_password_to_be_set, PASS_SIZE);
        memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&SUCCESS,BLE_RETURN_RC_SIZE);
        //Once copied, reset the flags.
        this_ccu.data_status = this_ccu.data_status & (~(FLAG_DATA_SET_CCU_PWD_MATCH | FLAG_DATA_SET_CCU_NEW_PASSWORD));
    }
    return (int)i_ret_msg[BLE_RET_MSG_RC_OFFSET];
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
        memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_SOURCE_APP_TYPE_MISMATCH,BLE_RETURN_RC_SIZE);
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
            case CID_CHANGE_PASSWORD : {
                #ifdef BLE_DEBUG
                printf("Going to call execute change password\n");
                #endif
                if (this_ccu.paired_mob1.authentication_status != AUTHENTICATED) {
                    memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_AUTHENTICATION,BLE_RETURN_RC_SIZE);
                    return ERROR_AUTHENTICATION;
                }
                memcpy(ble_command,&i_msg[BLE_CMD_OFFSET + BLE_COMMAND_ID_SIZE],BLE_COMMAND_SIZE);
                execute_change_password(ble_command,i_ret_msg);
                break;
            }
            default : {
                memcpy(&i_ret_msg[BLE_RET_MSG_RC_OFFSET],&ERROR_UNRECOGNIZED_COMMAND,BLE_RETURN_RC_SIZE);
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
    memcpy(i_ser,&SER_NO_TEST,SER_NO_SIZE);
    return 0;
}

int main(int argc, char** argv) {
    char ble_message[BLE_MESSAGE_SIZE];
    char ble_return_message[BLE_RETURN_SIZE];
    this_ccu.paired_mob1.data_status = 0x00;

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    memset(ble_return_message,0x00,BLE_RETURN_SIZE);
    populate_serial_no_from_eeprom(this_ccu.serial_number);

    memcpy(ble_message, REGISTER_MSG1_TEST, 13);
    memcpy(&ble_return_message[0],&CCU_TYPE_ID,1);
    memcpy(&ble_return_message[1],&this_ccu.serial_number[SER_NO_SIZE - CCU_ID_SER_NO_SUFFIX_SIZE],CCU_ID_SER_NO_SUFFIX_SIZE);
    #ifdef BLE_DEBUG
    printf("BLE Message REGISTER MSG 1#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message REGISTER MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif

    read_ble_message(ble_message, ble_return_message);

    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing REGISTER MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message REGISTER MSG 2#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message REGISTER MSG 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing REGISTER MSG 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG3_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message REGISTER MSG 3#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message REGISTER MSG 3#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing REGISTER MSG 3#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, REGISTER_MSG4_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message REGISTER MSG 4#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message REGISTER MSG 4#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing REGISTER MSG 4#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message LOGIN MSG 1#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message LOGIN MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing LOGIN MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#%d#\n",ble_return_message[6],this_ccu.paired_mob1.authentication_status);

    #endif

/*    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, FORGOT_PASSWORD_MSG_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message FORGOT PASS MSG#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message FORGOT PASS MSG#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing FORGOT PASS MSG#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif
*/
    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message Chg password msg 1#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message Chg password msg 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing Chg password msg 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message Chg password msg 2#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message Chg password msg 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing Chg password msg 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message LOGIN MSG 1#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message LOGIN MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing LOGIN MSG 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG2_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message LOGIN MSG 2#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message LOGIN MSG 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing LOGIN MSG 2#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG3_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message Change Password Msg 3#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message Change Password Msg 3#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing Change Password Msg 3#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, CHANGE_PASSWORD_MSG4_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message Change Password Msg 4#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message Change Password Msg 4#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing Change Password Msg 4#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

    memset(ble_message,0x00,BLE_MESSAGE_SIZE);
    strcpy(ble_message, LOGIN_MSG1_TEST);
    ble_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
    #ifdef BLE_DEBUG
    printf("BLE Message LOGIN Msg 1#");
    for (int i = 0; i<20; i++) {
        printf("%c",ble_message[i]);
    }
    printf("#\n");

    printf("BLE Return Message LOGIN Msg 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#\n");

    #endif
    read_ble_message(ble_message, ble_return_message);
    #ifdef BLE_DEBUG
    printf("BLE Return Message after processing LOGIN Msg 1#");
    for (int i = 0; i<8; i++) {
        printf("%c",ble_return_message[i]);
    }
    printf("#%x#\n",ble_return_message[6]);

    #endif

}
