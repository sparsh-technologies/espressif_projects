#define CURRENT_FIRMWARE_VERSION "1.0(1)"
#define BLE_DEBUG

#define SER_NO_SIZE                   8
#define PASS_SIZE                     15
#define MOB_NO_SIZE                   10
#define MOB_NAME_SIZE                 15
#define IMEI_NO_SIZE                  15
#define BLE_APP_TYPE_ID_SIZE          1
#define BLE_APP_ID_SIZE               2
#define BLE_APP_AUTH_TOKEN_SIZE       2
#define FILE_NAME_SIZE                30
#define PERSONAL_VOICE_MESSAGES_COUNT 6
#define DEFAULT_EMERGENCY_NUMBER      911
#define DEFAULT_EMERGENCY_NUMBER_SIZE 3
#define SSID_SIZE                     15
#define NETWORK_KEY_SIZE              15
#define LOCATION_COUNT                10
#define LAT_LONG_SIZE                 11
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

const int  BLE_MESSAGE_SIZE               = 30;
const int  BLE_COMMAND_SIZE               = 26; //BLE_MESSAGE SIZE - Source App Type (1) - Source App ID (2) - Command Id (1)
const int  BLE_COMMAND_ID_SIZE            = 1;
const int  BLE_RETURN_SIZE                = 8;
const int  BLE_RETURN_RC_SIZE             = 1;

const int  BLE_APP_TYPE_OFFSET            = 0;
const int  BLE_APP_OFFSET                 = 1;
const int  BLE_CMD_OFFSET                 = 3;
const int  BLE_CMD_REG_DATA_TYPE_OFFSET   = 0; //for commands with multiple data types sent as multiple messages
const int  BLE_CMD_REG_DATA_LEN_OFFSET    = 1; //for commands with multiple data types sent as multiple messages
const int  BLE_CMD_REG_DATA_VALUE_OFFSET  = 2; //for commands with multiple data types sent as multiple messages
const int  BLE_CMD_DATA_LEN_OFFSET        = 0; //for commands that are sent in a single message
const int  BLE_CMD_DATA_VALUE_OFFSET      = 1; //for commands that are sent in a single message
const int  BLE_RET_MSG_CMD_ID_OFFSET      = 5;
const int  BLE_RET_MSG_RC_OFFSET          = 6;
const int  BLE_RET_MSG_DATA_TYPE_OFFSET   = 7;
const int  BLE_RET_MSG_AUTH_TKN_OFFSET    = 7;

const char MOB1_APP_TYPE_ID               = 0x4D;
const char CCU_TYPE_ID                    = 0x47;
const int  CCU_ID_SER_NO_SUFFIX_SIZE      = 4;

const int  SUCCESS                        = 0x00;
const int  ERROR_LOGIN_PASSWORD_MISMATCH  = 0x01;
const int  ERROR_SOURCE_APP_TYPE_MISMATCH = 0x20;
const int  ERROR_SOURCE_APP_MISMATCH      = 0x21;
const int  ERROR_UNRECOGNIZED_COMMAND     = 0x22;
const int  ERROR_UNRECOGNIZED_DATA        = 0x23;

#define JSON_REGISTER_CCU_HEAD    "{\"data_type\":\"%s\",\"s_time\":\"%ld\","
#define JSON_REGISTER_CCU_CCU     "\"ccu\":{\"sno\":\"%s\",\"make\":\"AKBI\",\"model\":\"Guardian\",\"name\":\"Smart Security\",\"password\":\"%s\",\"fwversion\":\"%s\"},"
#define JSON_REGISTER_CCU_INSTALL "\"installation\":{\"date\":\"%s\",\"latitude\":\"%s\",\"longitude\":\"%s\"},"
#define JSON_REGISTER_CCU_MOB1    "\"mob1\":{\"version\":\"%s\",\"imei\":\"%s\",\"num\":\"%s\",\"name\":\"%s\"}"
#define JSON_REGISTER_CCU_TAIL    "}"

#define REGISTER_MSG1_TEST        "\x4D\xCC\xAA\x01\x01\x08password"
#define REGISTER_MSG2_TEST        "\x4D\xCC\xAA\x01\x02\x0Aq123456789"
#define REGISTER_MSG3_TEST        "\x4D\xCC\xAA\x01\x03\x0Fgbcdefghijklmno"
#define REGISTER_MSG4_TEST        "\x4D\xCC\xAA\x01\x04\x0Fw12345678901234"
#define SOURCE_APP_ID_TEST        "\xCC\xAA"
#define LOGIN_MSG_TEST            "\x4D\xCC\xAA\x02\x08password"