/*
 ****************************************************************************************
 * akbi_msg.h
 *
 * Author    : Sathish Chandran
 * Ver       : 1.0
 * Date      : 31-Dec-2019
 *
 * Copyright Infinicomm Solutions India Pvt Ltd, 2019-20
 *
 ****************************************************************************************
 */

#ifndef _AKBI_MSG_H_
#define _AKBI_MSG_H_

#define CURRENT_FIRMWARE_VERSION "1.0(1)"

#define BLE_DEBUG

#define  NEWLINE                                 '\n'
#define  END_OF_STRING                            0
//const char  *WEBAPP_RETURN_DELIMITER_STRING           = ",";

#define BLE_MESSAGE_SIZE                          20
#define BLE_COMMAND_SIZE                          17 //BLE_MESSAGE SIZE - Source App Type (1) - Source App ID (1) - Command Id (1)
#define BLE_COMMAND_ID_SIZE                       1
#define BLE_RETURN_RC_SIZE                        1
#define BLE_COMMAND_DATA_TYPE_SIZE                1
#define BLE_APP_TYPE_OFFSET                       0
#define BLE_APP_OFFSET                            1
#define BLE_CMD_OFFSET                            2
#define BLE_MSG_MULTI_DATA_TYPE_OFFSET            3
#define BLE_CMD_MULTI_DATA_TYPE_OFFSET            0 // commands with multiple data types sent as multiple messages
#define BLE_CMD_MULTI_DATA_LEN_OFFSET             1 //for commands with multiple data types sent as multiple messages
#define BLE_CMD_MULTI_DATA_VALUE_OFFSET           2 //for commands with multiple data types sent as multiple messages
#define BLE_CMD_MULTI_DATA_VALUE_FIXED_LEN_OFFSET 1 //for commands with multiple data types sent as multiple messages
#define BLE_CMD_SINGLE_DATA_LEN_OFFSET            0 //for commands that are sent in a single message
#define BLE_CMD_SINGLE_DATA_VALUE_OFFSET          1 //for commands that are sent in a single message
#define BLE_RET_MSG_CMD_ID_OFFSET                 5
#define BLE_RET_MSG_RC_OFFSET                     6
#define BLE_RET_MSG_DATA_TYPE_OFFSET              7
#define BLE_RET_MSG_AUTH_TKN_OFFSET               7
#define BLE_RET_MSG_MY_SSID_OFFSET                7
#define BLE_RET_MSG_MY_NETWORK_KEY_OFFSET         13
#define BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET     8
#define BLE_RET_MSG_FIRMWARE_VERSION_OFFSET       8
#define BLE_RET_MSG_SERIAL_NUM_OFFSET             1

#define BLE_MSG_MULTI_DATA_LEN_OFFSET             4
#define BLE_MSG_MULTI_DATA_DATA_OFFSET            5

#define MOB1_APP_TYPE_ID                          0x4D
#define CCU_TYPE_ID                               0x47
#define CCU_ID_SER_NO_SUFFIX_SIZE                 4
#define BLE_RET_MSG_CCU_ID_OFFSET                 0
#define BLE_RET_MSG_SCANNED_WIFI_COUNT_TYPE       0x01
#define BLE_RET_MSG_SCANNED_WIFI_SSID_TYPE        0x02

#define SUCCESS                                   0x00
#define ERROR_LOGIN_PASSWORD_MISMATCH             0x06
#define ERROR_SOURCE_APP_TYPE_MISMATCH            0x20
#define ERROR_SOURCE_APP_MISMATCH                 0x21
#define ERROR_UNRECOGNIZED_COMMAND                0x22
#define ERROR_UNRECOGNIZED_DATA                   0x23
#define ERROR_AUTHENTICATION                      0x24
#define ERROR_MOB1_NO_NOT_CONFIGURED              0x08
#define ERROR_CHANGE_PASSWORD_MISMATCH            0x09
#define ERROR_MY_AP_START                         0x0A
#define ERROR_MY_WIFI_STN_START                   0x0D
#define READY_TO_SEND_REG_DATA_TO_SERIAL          0x0E
#define WIFI_SCANNING_IN_PROGRESS                 0x77//for testing

#define ADV_SER_NO_SIZE                           4
#define BLE_RETURN_MAX_SIZE                       20
#define MOB_NAME_SIZE                             15
#define IMEI_NO_SIZE                              15
#define BLE_APP_TYPE_ID_SIZE                      1

#define BLE_APP_AUTH_TOKEN_SIZE       2
#define FILE_NAME_SIZE                30
#define PERSONAL_VOICE_MESSAGES_COUNT 6


#define DEFAULT_EMERGENCY_NUMBER_SIZE 3
#define SSID_SIZE                     15
#define NETWORK_KEY_SIZE              15
#define LOCATION_COUNT                10

#define TIMESTAMP_SIZE                10
#define FW_UPGRADE_COUNT              10
#define FW_VERSION_STRING_SIZE        10
#define DATE_SIZE                     10
#define MOB1_VER_SIZE                 10
#define ACTIVATIONS_COUNT             10
#define WIFI_CONNECTIONS_COUNT        10

#define CID_REGISTER                  0x01
#define CID_LOGIN                     0x02

#define DID_REGISTER_PASSWORD         0x01
#define DID_REGISTER_MOB_NO           0x02
#define DID_REGISTER_MOB_NAME         0x03
#define DID_REGISTER_IMEI             0x04

#define JSON_REGISTER_CCU_HEAD             "{\"data_type\":\"%s\",\"s_time\":\"%ld\","
#define JSON_REGISTER_CCU_CCU              "\"ccu\":[{\"sno\":\"%s\",\"make\":\"AKBI\",\"model\":\"Guardian\",\"name\":\"Smart Security\",\"password\":\"%s\",\"fwversion\":\"%s\"}],"
#define JSON_REGISTER_CCU_INSTALL          "\"installation\":[{\"date\":\"%s\",\"latitude\":\"%s\",\"longitude\":\"%s\"}],"
#define JSON_REGISTER_CCU_MOB1             "\"mob1\":[{\"version\":\"%s\",\"android_id_or_uuid\":\"%s\",\"num\":\"%s\",\"name\":\"%s\"}]"
#define JSON_REGISTER_CCU_TAIL             "}"
#define JSON_UPLOAD_LOCATION_HEAD          "{\"data_type\":\"upload_info_location\",\"s_time\":\"%ld\",\"sno\":\"%s\",\"location\":["
#define JSON_UPLOAD_LOCATION_LOCATION      "{\"gps_lat\":\"%s\",\"gps_long\":\"%s\",\"gps_time\":\"%ld\",\"audio_msg\":\"%s\",\"audio_time\":\"%ld\",\"phone_1\":\"%s\",\"phone_2\":\"%s\",\"phone_3\":\"%s\",\"phone_4\":\"%s\",\"phone_5\":\"%s\"}"
#define JSON_UPLOAD_LOCATION_TAIL          "]}"
#define JSON_UPLOAD_ACTIVATIONS_HEAD       "{\"data_type\":\"upload_info_activation\",\"s_time\":\"%ld\",\"sno\":\"%s\",\"activation\":["
#define JSON_UPLOAD_ACTIVATIONS_ACTIVATION "{\"date\":\"%s\",\"time\":\"%ld\"}"
#define JSON_UPLOAD_ACTIVATIONS_RT_START   "],\"rescue_termination\": ["
#define JSON_UPLOAD_ACTIVATIONS_RT_CONTENT "{\"date\":\"%s\",\"time\":\"%ld\"}"
#define JSON_UPLOAD_ACTIVATIONS_TAIL       "]}"
#define JSON_UPLOAD_FW_UPGRADE_HEAD        "{\"data_type\":\"upload_info_fw_upgrade\",\"s_time\":\"%ld\",\"sno\":\"%s\",\"fw_upgrade\":["
#define JSON_UPLOAD_FW_UPGRADE_FW_UPGRADE  "{\"date\":\"%s\",\"time\":\"%ld\",\"fwversion\":\"%s\"}"
#define JSON_UPLOAD_FW_UPGRADE_TAIL        "]}"
#define JSON_UPLOAD_WIFI_HEAD              "{\"data_type\":\"upload_info_wifi_connections\",\"s_time\":\"%ld\",\"sno\":\"%s\",\"wifi_connection\":["
#define JSON_UPLOAD_WIFI_WIFI              "{\"date\":\"%s\",\"time\":\"%ld\",\"ssid\":\"%s\"}"
#define JSON_UPLOAD_WIFI_TAIL              "]}"

#define REGISTER_MSG1_TEST                 "\x4D\xCC\x01\x01\x08password"
#define REGISTER_MSG2_TEST                 "\x4D\xCC\x01\x02\x0A""0123456789"
#define REGISTER_MSG3_TEST                 "\x4D\xCC\x01\x03\x0F""abcdefghijklmn"
#define REGISTER_MSG4_TEST                 "\x4D\xCC\x01\x04\x0F""01234567890123"
#define SOURCE_APP_ID_TEST                 "\xCC"
#define LOGIN_MSG1_TEST                    "\x4D\xCC\x02\x08password"
#define LOGIN_MSG2_TEST                    "\x4D\xCC\x02\x08passw0rd"
#define FORGOT_PASSWORD_MSG_TEST           "\x4D\xCC\x03"
#define CHANGE_PASSWORD_MSG1_TEST          "\x4D\xCC\x04\x01\x08password"
#define CHANGE_PASSWORD_MSG2_TEST          "\x4D\xCC\x04\x02\x08passw0rd"
#define CHANGE_PASSWORD_MSG3_TEST          "\x4D\xCC\x04\x01\x08passw0rd"
#define CHANGE_PASSWORD_MSG4_TEST          "\x4D\xCC\x04\x02\x08password"
#define RECORD_PERSONAL_VOICE_MSG_TEST     "\x4D\xCC\x05"
#define STORE_EMERGENCY_NOS_MSG1_TEST      "\x4D\xCC\x06\x01""3333333333"
#define STORE_EMERGENCY_NOS_MSG2_TEST      "\x4D\xCC\x06\x02""5555555555"
#define STORE_PERSONAL_NOS_MSG1_TEST       "\x4D\xCC\x07\x01""7777777777"
#define STORE_PERSONAL_NOS_MSG2_TEST       "\x4D\xCC\x07\x02""6666666666"
#define SCAN_WIFIS_MSG_TEST                "\x4D\xCC\x08"
#define SELECT_A_WIFI_MSG1_TEST            "\x4D\xCC\x09\x01\x0BSecond WiFi"
#define SELECT_A_WIFI_MSG2_TEST            "\x4D\xCC\x09\x02\x08guardian"
#define ADDRESS_VISITING_MSG_TEST          "\x4D\xCC\x0A""0400656N0892338W"
#define ENTER_LOCAL_HELP_NOS_MSG1_TEST     "\x4D\xCC\x0B\x01""4444444444"
#define ENTER_LOCAL_HELP_NOS_MSG2_TEST     "\x4D\xCC\x0B\x02""2222222222"
#define CCU_ACTIVATE_MSG_TEST              "\x4D\xCC\x0C""0400656N0892338W"
#define CONNECT_TO_WIFI_MSG_TEST           "\x4D\xCC\x0D"
#define DISCONNECT_FROM_WIFI_MSG_TEST      "\x4D\xCC\x0E"
#define UPDATE_CCU_SOFTWARE_MSG_TEST       "\x4D\xCC\x0F"
#define UPLOAD_TRIP_INFO_MSG_TEST          "\x4D\xCC\x10"

#endif
