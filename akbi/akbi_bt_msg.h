/*
 ****************************************************************************************
 * akbi_bt_msg.h
 *
 * Author    : Sathish Chandran
 * Ver       : 1.0
 * Date      : 31-Dec-2019
 *
 * Copyright Infinicomm Solutions India Pvt Ltd, 2019-20
 *
 ****************************************************************************************
 */

#ifndef _AKBI_BT_MSG_H_
#define _AKBI_BT_MSG_H_

#define CURRENT_FIRMWARE_VERSION "1.0(1)"
#define BLE_DEBUG

#define SER_NO_SIZE                        8
#define PASS_SIZE                          15
#define DEFAULT_PASSWORD_SIZE              12
#define MOB_NO_SIZE                        10
#define MOB_NAME_SIZE                      15
#define ANDROID_ID_OR_UUID_SIZE            15
#define BLE_APP_TYPE_ID_SIZE               1
#define BLE_APP_ID_SIZE                    1
#define BLE_APP_AUTH_TOKEN_SIZE            2
#define FILE_NAME_SIZE                     30
#define PERSONAL_VOICE_MESSAGES_COUNT      6
#define DEFAULT_EMERGENCY_NUMBER           "911"
#define DEFAULT_EMERGENCY_NUMBER_SIZE      3
#define SSID_SIZE                          15
#define NETWORK_KEY_SIZE                   15
#define MY_SSID_SIZE                       6
#define MY_NETWORK_KEY_SIZE                6
#define SCANNED_WIFI_COUNT_SIZE            1
#define MAX_WIFI_SCAN_COUNT                10
#define LOCATION_COUNT                     10
#define LAT_LONG_SIZE                      8
#define LAT_LONG_DEGREE_SIZE               3
#define LAT_LONG_MINUTE_SIZE               2
#define LAT_LONG_SECOND_SIZE               2
#define LAT_LONG_DIRECTION_SIZE            1
#define TIMESTAMP_SIZE                     10
#define FW_UPGRADE_COUNT                   10
#define FW_VERSION_STRING_SIZE             10
#define DATE_SIZE                          10
#define MOB1_VER_SIZE                      10
#define ACTIVATIONS_COUNT                  10
#define WIFI_CONNECTIONS_COUNT             10

#define CID_REGISTER                       0x01
#define CID_LOGIN                          0x02
#define CID_FORGOT_PASSWORD                0x03
#define CID_CHANGE_PASSWORD                0x04
#define CID_RECORD_PERSONAL_VOICE_MSG      0x05
#define CID_STORE_EMERGENCY_NUMBERS        0x06
#define CID_STORE_PERSONAL_NUMBERS         0x07
#define CID_SCAN_WIFIS                     0x08
#define CID_SELECT_A_WIFI                  0x09
#define CID_ADDRESS_VISITING               0x0A
#define CID_ENTER_LOCAL_HELP_NUMBERS       0x0B
#define CID_CCU_ACTIVATE                   0x0C
#define CID_CONNECT_TO_WIFI                0x0D

#define DID_REGISTER_PASSWORD              0x01
#define DID_REGISTER_MOB_NO                0x02
#define DID_REGISTER_MOB_NAME              0x03
#define DID_REGISTER_ANDROID_ID_OR_UUID    0x04
#define DID_CHANGE_PASSWORD_CURRENT        0x01
#define DID_CHANGE_PASSWORD_NEW            0x02
#define DID_EMERGENCY_FIRST_RESPONDER      0x01
#define DID_EMERGENCY_CLOSE_RELATIVE       0x02
#define DID_PERSONAL_SECOND_NUMBER         0x01
#define DID_PERSONAL_THIRD_NUMBER          0x02
#define DID_SELECT_A_WIFI_SSID             0x01
#define DID_SELECT_A_WIFI_NETWORK_KEY      0x02
#define DID_LOCAL_HELP_FOURTH_NUMBER       0x01
#define DID_LOCAL_HELP_FIFTH_NUMBER        0x02

#define FLAG_DATA_SET_CCU_PASSWORD         0x02
#define FLAG_DATA_SET_CCU_NEW_PASSWORD     0x04
#define FLAG_DATA_SET_CCU_PWD_MATCH        0x08

#define FLAG_DATA_SET_MOB1_ID              0x01
#define FLAG_DATA_SET_MOB1_NUM             0x02
#define FLAG_DATA_SET_MOB1_NAME            0x04
#define FLAG_DATA_SET_ANDROID_ID_OR_UUID   0x08
#define FLAG_DATA_SET_MOB1_ALL             0x0F

#define FLAG_DATA_SET_SEL_WIFI_SSID        0x01
#define FLAG_DATA_SET_SEL_WIFI_NETWORK_KEY 0x02
#define FLAG_DATA_SET_SEL_WIFI_ALL         0x03

//const char  SER_NO_TEST[SER_NO_SIZE]          = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};

typedef enum {UNAUTHENTICATED = 0, AUTHENTICATED} AUTH_STATUS;

typedef struct _mob1_ {

    char id[BLE_APP_ID_SIZE];
    char mobile_number[MOB_NO_SIZE];
    char mobile_name[MOB_NAME_SIZE];
    char android_id_or_uuid[ANDROID_ID_OR_UUID_SIZE];
    AUTH_STATUS   authentication_status;

    /*
     * bitmap used to indicate which all data is set as part of
     * registration. lsb (1st bit) will be set for id. That is 0x01.
     * 2nd bit will be set for mobile number (0x02), 3rd bit (0x04)
     * for mobile name and 4th bit (0x08) for android id/ uuid.
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

typedef enum {STATION = 0, ACCESS_POINT} WIFI_MODE;

typedef enum {NOT_CONNECTED = 3, WIFI_CONNECTED = 2, INET_CONNECTED = 1, SERVER_CONNECTED = 0} WIFI_STATUS;

typedef struct _wifi_ {

    char ssid[SSID_SIZE];
    char network_key[NETWORK_KEY_SIZE];
    WIFI_STATUS status;
    /*
     * bitmap to indicate whether both ssid and network key are configured
     */
    unsigned char data_status;

} WIFI;

typedef struct _gpscoordinate_ {

    unsigned char degree;
    unsigned char minute;
    unsigned char second;

} GPSCOORDINATE;

typedef enum {SOUTH = -1, NORTH = 1} LATITUDE_DIRECTION;

typedef enum {WEST = -1, EAST = 1} LONGITUDE_DIRECTION;

typedef struct _site_ {

    GPSCOORDINATE latitude;
    LATITUDE_DIRECTION lat_dir;
    GPSCOORDINATE longitude;
    LONGITUDE_DIRECTION long_dir;
    unsigned int location_time;
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

    char time[TIMESTAMP_SIZE];
    GPSCOORDINATE latitude;
    LATITUDE_DIRECTION lat_dir;
    GPSCOORDINATE longitude;
    LONGITUDE_DIRECTION long_dir;

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

    unsigned char            serial_number[SER_NO_SIZE];
    unsigned char            password[PASS_SIZE];
    unsigned char            new_password_to_be_set[PASS_SIZE];
    MOB1                     paired_mob1;
    PERSONAL_VOICE_MESSAGE   personal_voice_messages[PERSONAL_VOICE_MESSAGES_COUNT];
    EMERGENCY_NUMBERS        conf_emergency_nos;
    PERSONAL_NUMBERS         conf_personal_nos;
    WIFI                     interface_wifi; //this is CCU's wifi interface that can be configured as STN or AP
    WIFI                     conf_wifi; //this is the wifi SSID to which CCU gets connected to.
    WIFI                     scanned_wifis[MAX_WIFI_SCAN_COUNT]; //this is the list of WiFis scanned by the CCU.
    unsigned char            scanned_wifi_count;
    SITE                     visited_locations[LOCATION_COUNT];
    unsigned char            visited_locations_count;
    CCU_MODE                 mode;
    ACTIVATION_LOG           activations[ACTIVATIONS_COUNT];
    unsigned char            activations_count;
    FW_UPGRADE_LOG           fw_upgrades[FW_UPGRADE_COUNT];
    RESCUE_TERMINATION_LOG   rescue_terminations[ACTIVATIONS_COUNT];
    WIFI_CONNECTIONS_LOG     wifi_connections[WIFI_CONNECTIONS_COUNT];
    unsigned short           data_status;
    WIFI                     interface_wifi;

} CCU;

typedef struct _mob_cloud_register_info_ {

    unsigned char    mobile_number[MOB_NO_SIZE];
    unsigned char    mobile_name[MOB_NAME_SIZE];
    unsigned char    unique_id[ANDROID_ID_OR_UUID_SIZE];
    char             serial_number[SER_NO_SIZE];
    char             passwd[PASS_SIZE];

} MOB_CLOUD_REGISTER_INFO;

/*
 * The following structure is used to exchange the data between BT and CP
 */

#define PROTO_TYPE_SEND                                           0x01
#define PROTO_TYPE_SEND_ACK                                       0x02

typedef struct _bt_cp_protocol_hdr_ {

    unsigned char       opcode;
    unsigned short int  trans_id;
    unsigned char       type;

} BT_CP_PROTOCOL_HDR;

/*
 * Various Opcodes are defined here.
 */
#define BT_CP_OPCODE_MOB1_REGISTER                                0x01


/*
 * All TVL Types are defined here.
 */

/*
 * TLV Types for BT_CP_OPCODE_MOB1_REGISTER Opcode
 */

#define TLV_TYPE_REGISTER_MOB_NUM                                 0x01
#define TLV_TYPE_REGISTER_MOB_NAME                                0x02
#define TLV_TYPE_REGISTER_UNIQUE_ID                               0x03
#define TLV_TYPE_REGISTER_PASSWORD                                0x04

#endif
