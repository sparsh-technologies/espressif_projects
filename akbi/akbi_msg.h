#define CURRENT_FIRMWARE_VERSION "1.0(1)"
#define BLE_DEBUG

#define SER_NO_SIZE                      8
#define PASS_SIZE                        15
#define DEFAULT_PASSWORD_SIZE            12
#define MOB_NO_SIZE                      10
#define MOB_NAME_SIZE                    15
#define ANDROID_ID_OR_UUID_SIZE          15
#define BLE_APP_TYPE_ID_SIZE             1
#define BLE_APP_ID_SIZE                  1
#define BLE_APP_AUTH_TOKEN_SIZE          2
#define FILE_NAME_SIZE                   30
#define PERSONAL_VOICE_MESSAGES_COUNT    6
#define DEFAULT_EMERGENCY_NUMBER         911
#define DEFAULT_EMERGENCY_NUMBER_SIZE    3
#define SSID_SIZE                        15
#define NETWORK_KEY_SIZE                 15
#define LOCATION_COUNT                   10
#define LAT_LONG_SIZE                    11
#define TIMESTAMP_SIZE                   10
#define FW_UPGRADE_COUNT                 10
#define FW_VERSION_STRING_SIZE           10
#define DATE_SIZE                        10
#define MOB1_VER_SIZE                    10
#define ACTIVATIONS_COUNT                10
#define WIFI_CONNECTIONS_COUNT           10

#define CID_REGISTER                     0x01
#define CID_LOGIN                        0x02
#define CID_FORGOT_PASSWORD              0x03
#define CID_CHANGE_PASSWORD              0x04

#define DID_REGISTER_PASSWORD            0x01
#define DID_REGISTER_MOB_NO              0x02
#define DID_REGISTER_MOB_NAME            0x03
#define DID_REGISTER_ANDROID_ID_OR_UUID  0x04
#define DID_CHANGE_PASSWORD_CURRENT      0x01
#define DID_CHANGE_PASSWORD_NEW          0x02

#define FLAG_DATA_SET_CCU_PASSWORD       0x02
#define FLAG_DATA_SET_CCU_NEW_PASSWORD   0x04
#define FLAG_DATA_SET_CCU_PWD_MATCH      0x08

#define FLAG_DATA_SET_MOB1_ID            0x01
#define FLAG_DATA_SET_MOB1_NUM           0x02
#define FLAG_DATA_SET_MOB1_NAME          0x04
#define FLAG_DATA_SET_ANDROID_ID_OR_UUID 0x08
#define FLAG_DATA_SET_MOB1_ALL           0x0F


const short BLE_MESSAGE_SIZE                  = 20;
const short BLE_COMMAND_SIZE                  = 17; //BLE_MESSAGE SIZE - Source App Type (1) - Source App ID (1) - Command Id (1)
const short BLE_COMMAND_ID_SIZE               = 1;
const short BLE_RETURN_SIZE                   = 8;
const short BLE_RETURN_RC_SIZE                = 1;
const short BLE_COMMAND_DATA_TYPE_SIZE        = 1;

const short BLE_APP_TYPE_OFFSET               = 0;
const short BLE_APP_OFFSET                    = 1;
const short BLE_CMD_OFFSET                    = 2;
const short BLE_CMD_MULTI_DATA_TYPE_OFFSET    = 0; //for commands with multiple data types sent as multiple messages
const short BLE_CMD_MULTI_DATA_LEN_OFFSET     = 1; //for commands with multiple data types sent as multiple messages
const short BLE_CMD_MULTI_DATA_VALUE_OFFSET   = 2; //for commands with multiple data types sent as multiple messages
const short BLE_CMD_SINGLE_DATA_LEN_OFFSET    = 0; //for commands that are sent in a single message
const short BLE_CMD_SINGLE_DATA_VALUE_OFFSET  = 1; //for commands that are sent in a single message
const short BLE_RET_MSG_CMD_ID_OFFSET         = 5;
const short BLE_RET_MSG_RC_OFFSET             = 6;
const short BLE_RET_MSG_DATA_TYPE_OFFSET      = 7;
const short BLE_RET_MSG_AUTH_TKN_OFFSET       = 7;

const char  MOB1_APP_TYPE_ID                  = 0x4D;
const char  CCU_TYPE_ID                       = 0x47;
const short CCU_ID_SER_NO_SUFFIX_SIZE         = 4;
const char  SER_NO_TEST[SER_NO_SIZE]          = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};

const short SUCCESS                           = 0x00;
const short ERROR_LOGIN_PASSWORD_MISMATCH     = 0x01;
const short ERROR_SOURCE_APP_TYPE_MISMATCH    = 0x20;
const short ERROR_SOURCE_APP_MISMATCH         = 0x21;
const short ERROR_UNRECOGNIZED_COMMAND        = 0x22;
const short ERROR_UNRECOGNIZED_DATA           = 0x23;
const short ERROR_AUTHENTICATION              = 0x24;
const short ERROR_MOB1_NO_NOT_CONFIGURED      = 0x01;
const short ERROR_CHANGE_PASSWORD_MISMATCH    = 0x01;

#define JSON_REGISTER_CCU_HEAD    "{\"data_type\":\"%s\",\"s_time\":\"%ld\","
#define JSON_REGISTER_CCU_CCU     "\"ccu\":[{\"sno\":\"%s\",\"make\":\"AKBI\",\"model\":\"Guardian\",\"name\":\"Smart Security\",\"password\":\"%s\",\"fwversion\":\"%s\"}],"
#define JSON_REGISTER_CCU_INSTALL "\"installation\":[{\"date\":\"%s\",\"latitude\":\"%s\",\"longitude\":\"%s\"}],"
#define JSON_REGISTER_CCU_MOB1    "\"mob1\":[{\"version\":\"%s\",\"imei\":\"%s\",\"num\":\"%s\",\"name\":\"%s\"}]"
#define JSON_REGISTER_CCU_TAIL    "}"

#define REGISTER_MSG1_TEST        "\x4D\xCC\x01\x01\x08password"
#define REGISTER_MSG2_TEST        "\x4D\xCC\x01\x02\x0Aq123456789"
#define REGISTER_MSG3_TEST        "\x4D\xCC\x01\x03\x0Fgbcdefghijklmno"
#define REGISTER_MSG4_TEST        "\x4D\xCC\x01\x04\x0Fw12345678901234"
#define SOURCE_APP_ID_TEST        "\xCC"
#define LOGIN_MSG1_TEST           "\x4D\xCC\x02\x08password"
#define LOGIN_MSG2_TEST           "\x4D\xCC\x02\x08passw0rd"
#define FORGOT_PASSWORD_MSG_TEST  "\x4D\xCC\x03"
#define CHANGE_PASSWORD_MSG1_TEST "\x4D\xCC\x04\x01\x08password"
#define CHANGE_PASSWORD_MSG2_TEST "\x4D\xCC\x04\x02\x08passw0rd"
#define CHANGE_PASSWORD_MSG3_TEST "\x4D\xCC\x04\x01\x08passw0rd"
#define CHANGE_PASSWORD_MSG4_TEST "\x4D\xCC\x04\x02\x08password"