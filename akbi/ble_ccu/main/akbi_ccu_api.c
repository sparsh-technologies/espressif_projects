/*
 ****************************************************************************************
 * akbi_ccu_api.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 28-Jan-2020
 *
 * Copyright Infinicomm Solutions (I) Pvt Ltd, 2020
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
#include "esp_log.h"

#define BT_BLE_COEX_TAG             "CCU_API"

extern char saved_timestamp[15];

int get_timestamp(char *time_stamp)
{
    memcpy(time_stamp,saved_timestamp,TIMESTAMP_SIZE);
    return 0;
}

int ccu_sent_subg_clear_learning_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CLEAR_SUB_G_LEARN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    //printf(" INFO : Sent CLEAR_SUB_G_LEARN Cmd \n");

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;

    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_subg_learning_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_START_SUB_G_LEARN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    //printf(" INFO : Sent START_SUB_G_LEARN Cmd \n");

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_send_reg_msg(char *received_value_buffer,char *ep_return_message)
{

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    char                p_tx_buffer[25];

    int                 length;

    memset(p_tx_buffer, 0x00, 25);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_REGISTER;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p,(received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    //printf(" INFO : Sent REGISTER Cmd Length(%d) \n", p_protocol_hdr->length);

    send_uart_message(p_tx_buffer, length + 1);

    return 0;
}

int ccu_send_reg_msg_new(int type, char *received_value_buffer, int data_len)
{

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    char                p_tx_buffer[25];

    int                 length;

    memset(p_tx_buffer, 0x00, 25);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_REGISTER;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = type;
    p_protocol_hdr->length   = data_len;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, received_value_buffer, p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    printf(" INFO : Sending REGISTER (%s) \n", received_value_buffer);

    send_uart_message(p_tx_buffer, length);

    return 0;
}

int ccu_send_login_msg(char * password, int data_len)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[25];
    char                *p;

    //printf(" INFO : Sending LOGIN Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_LOGIN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = data_len;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, password, p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return 0;
}

int ccu_sent_scan_all_wifi_msg(char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    //int                 count;
    char                p_tx_buffer[25];
    //printf(" INFO : Sending SCAN-ALL-WIFI Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_SCAN_WIFIS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

/*
 * This API will be used to send the selected Wifi information to the CCU.
 */

int ccu_sent_configure_wifi_credentials( unsigned char ap_id, char *p_passwd, int mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;
    char                p_tx_buffer[100];
    BT_CP_TLV_HDR       *p_tlv_hdr;

    //printf(" INFO : Sending CONFIGURE-WIFI Message \n");
    //printf(" INFO : PASSWD : %s(%d) \n", p_passwd, strlen(p_passwd));
    //printf(" INFO : AP-ID  : %d \n", ap_id);

    memset(p_tx_buffer, 0x00, 100);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;
    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_SELECT_A_WIFI;
    p_protocol_hdr->trans_id = 44;

    /*
     * Now set all the TLV data here.
     */
    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    p_protocol_hdr->type     = TLV_TYPE_WIFI_AP_ID;
    p_protocol_hdr->length   = 1;
    *p = ap_id;
    p++;

    p_tlv_hdr  = (BT_CP_TLV_HDR*)p;

    p_tlv_hdr->type   = TLV_TYPE_WIFI_PASSWD_NAME;
    p_tlv_hdr->length = strlen(p_passwd);
    //p_tlv_hdr->length = pw_len;

     memcpy(p_tlv_hdr->data, p_passwd, strlen(p_passwd));
    //memcpy(p_tlv_hdr->data, p_passwd, pw_len);


    length = sizeof(BT_CP_PROTOCOL_HDR) + 1 + sizeof(BT_CP_TLV_HDR) + strlen(p_passwd);
    //length = sizeof(BT_CP_PROTOCOL_HDR) + 1 + sizeof(BT_CP_TLV_HDR) + pw_len;

    p_tx_buffer[length] = 0x00;

    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_connect_to_wifi(char * timestamp)
{

    char                p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p_payload;

    printf(" INFO : Sending CONNECT-TO-WIFI Message %s \n",timestamp);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CONNECT_TO_WIFI;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_TIMESTAMP;
    p_protocol_hdr->length   = strlen(timestamp);

    p_payload = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    memcpy(p_payload,timestamp,TIMESTAMP_SIZE);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_disconnect_from_wifi()
{

    char p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending DISCONNECT-FROM-WIFI Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_DISCONNECT_FROM_WIFI;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_user_forgot_passwd_msg()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    printf(" INFO : Sending USER-FORGOT_PASSWD Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_FORGOT_PASSWORD;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );
    printf("ptx in FPpp=%s\n", p_tx_buffer);

    return (0);
}

int ccu_sent_user_change_passwd_msg(char *p_password_current,char *p_password_new)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;
    char                p_tx_buffer[100];

    memset(p_tx_buffer,0x00,100);
    printf(" INFO : Sending USER-CHANGE_PASSWD Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CHANGE_PASSWORD;
    p_protocol_hdr->trans_id = 44;

    /*
     * Now set all the TLV data here.
     */
    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    p_protocol_hdr->type     = TLV_TYPE_CURRENT_PASSWD;
    p_protocol_hdr->length   = strlen(p_password_current);
    memcpy(p,p_password_current,strlen(p_password_current));
    p+=strlen(p_password_current);

    p[0]=TLV_TYPE_NEW_PASSWD;
    p++;
    p[0]=strlen(p_password_new);
    p++;
    memcpy(p,p_password_new,strlen(p_password_new));

    length = sizeof(BT_CP_PROTOCOL_HDR) + strlen(p_password_current) + sizeof(BT_CP_TLV_HDR) + strlen(p_password_new);

    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_record_voice_msg(unsigned char voice_msg_index, unsigned int voice_msg_length)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[25];
    char                *p;
    VOICE_DATA_DETAILS  *voice_details;

    memset(p_tx_buffer, 0x00, 25);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_RECORD_PERSONAL_VOICE_MSG;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_PERSONAL_VOICE;
    p_protocol_hdr->length   = 0;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    voice_details = (VOICE_DATA_DETAILS *)p;

    voice_details->msg_number  = voice_msg_index;
    voice_details->length = voice_msg_length;

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length + sizeof(VOICE_DATA_DETAILS);

    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_record_voice_msg_raw(char *voice_data,unsigned int chunk_length,unsigned char audio_number)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[270];
    char                *p;
    VOICE_DATA_DETAILS  *p_audio_data;

    // printf(" INFO : Sending RECORD-VOICE Message RAW  index(%d)\n",audio_number);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_RECORD_PERSONAL_VOICE_MSG;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_VOICE_MSG_RAW_DATA;
    p_protocol_hdr->length   = 0;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    p_audio_data = (VOICE_DATA_DETAILS *)p;

    p_audio_data->msg_number = audio_number;
    p_audio_data->length     = chunk_length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR) + sizeof(VOICE_DATA_DETAILS);
    memcpy(p , voice_data , chunk_length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + chunk_length + sizeof(VOICE_DATA_DETAILS) ;
    send_uart_message(p_tx_buffer, length);

    return (0);
}
/*
 * API to send Emerggency number to CCU
 */
int ccu_sent_store_emergency_number_msg(char *received_value_buffer)
{
    char                p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;
    int  type = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];

    //printf(" INFO : Sending emergency-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_STORE_EMERGENCY_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    switch(type){
      case 1:
          p_protocol_hdr->type = TLV_TYPE_EMERGENCY_NUMBER_2;
          break;
      case 2:
          p_protocol_hdr->type = TLV_TYPE_EMERGENCY_NUMBER_3;
          break;
    }
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, (received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}
/*
 * API to send Personal number to CCU
 */
int ccu_sent_store_personal_number_msg(char *received_value_buffer)
{
    char p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;
    int  type = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];

    //printf(" INFO : Sending PERSONAL-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_STORE_PERSONAL_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    switch(type){
      case 0:
          p_protocol_hdr->type = TLV_TYPE_PERSONAL_NUMBER_1;
          break;
      case 1:
          p_protocol_hdr->type = TLV_TYPE_PERSONAL_NUMBER_2;
          break;
      case 2:
          p_protocol_hdr->type = TLV_TYPE_PERSONAL_NUMBER_3;
          break;
    }    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, (received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_address_visiting(unsigned char voice_msg_index, unsigned int voice_msg_length)
{

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[25],timestamp[15];
    char                *p;
    VOICE_DATA_DETAILS  *voice_details;
    BT_CP_TLV_HDR       *p_tlv_hdr;


    memset(p_tx_buffer, 0x00, 25);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_ADDRESS_VISITING;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_ADDRESS_VISITING_VOICE;
    p_protocol_hdr->length   = 0;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    voice_details = (VOICE_DATA_DETAILS *)p;

    voice_details->msg_number  = voice_msg_index;
    voice_details->length = voice_msg_length;

    p = p + sizeof(VOICE_DATA_DETAILS);

    p_tlv_hdr = (BT_CP_TLV_HDR *)p;

    p_tlv_hdr->type   = TLV_TYPE_TIMESTAMP;
    p_tlv_hdr->length = TIMESTAMP_SIZE;

    get_timestamp(timestamp);
    memcpy(p_tlv_hdr->data,timestamp,TIMESTAMP_SIZE);

    printf(" INFO : Sending Address visiting Message %s\n",timestamp);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length + sizeof(VOICE_DATA_DETAILS)+sizeof(BT_CP_TLV_HDR)+TIMESTAMP_SIZE;

    send_uart_message(p_tx_buffer, length);

    return 0;
}

int ccu_sent_address_visiting_raw(char *voice_data,unsigned int chunk_length,unsigned char audio_number)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[270];
    char                *p;
    VOICE_DATA_DETAILS  *p_audio_data;

    // printf(" INFO : Sending Enter address visiting Message RAW\n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_ADDRESS_VISITING;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_VOICE_MSG_RAW_DATA;
    p_protocol_hdr->length   = 0;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    p_audio_data = (VOICE_DATA_DETAILS *)p;

    p_audio_data->msg_number = audio_number;
    p_audio_data->length     = chunk_length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR) + sizeof(VOICE_DATA_DETAILS);
    memcpy(p , voice_data , chunk_length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + chunk_length + sizeof(VOICE_DATA_DETAILS) ;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

/*
 * API to send Local number to CCU
 */
int ccu_sent_store_local_help_number_msg(char *received_value_buffer)
{
    char p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;

    //printf(" INFO : Sending local help-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_ENTER_LOCAL_HELP_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];
    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, (received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) +p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_activate_system_msg(char *p_latitude, char *p_longitude,char *timestamp, int mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;
    char                p_tx_buffer[100];
    BT_CP_TLV_HDR       *p_tlv_hdr;

    //printf(" INFO : Sending CONFIGURE-WIFI Message \n");
    //printf(" INFO : PASSWD : %s(%d) \n", p_passwd, strlen(p_passwd));
    //printf(" INFO : AP-ID  : %d \n", ap_id);

    memset(p_tx_buffer, 0x00, 100);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;
    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CCU_ACTIVATE;
    p_protocol_hdr->trans_id = 44;

    /*
     * Now set all the TLV data here.
     */
    p_protocol_hdr->type     = TLV_TYPE_ACTIVATE_LAT_STRING;
    p_protocol_hdr->length   = strlen(p_latitude);

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, p_latitude, strlen(p_latitude));

    p = p + strlen(p_latitude);

    p_tlv_hdr  = (BT_CP_TLV_HDR*)p;

    p_tlv_hdr->type   = TLV_TYPE_ACTIVATE_LONG_STRING;
    p_tlv_hdr->length = strlen(p_longitude);

    memcpy(p_tlv_hdr->data, p_longitude, strlen(p_longitude));

    p = p + strlen(p_longitude)+sizeof(BT_CP_TLV_HDR);

    p_tlv_hdr  = (BT_CP_TLV_HDR*)p;

    p_tlv_hdr->type   = TLV_TYPE_ACTIVATE_TIMESTAMP_STRING;
    p_tlv_hdr->length = TIMESTAMP_SIZE;

    // get_timestamp(timestamp);
    memcpy(p_tlv_hdr->data,timestamp,TIMESTAMP_SIZE);

    length = sizeof(BT_CP_PROTOCOL_HDR) + strlen(p_latitude) + sizeof(BT_CP_TLV_HDR) + strlen(p_longitude) + sizeof(BT_CP_TLV_HDR) + TIMESTAMP_SIZE;

    p_tx_buffer[length] = 0x00;

    printf("ptx buf =\n" );
    for (size_t i = 0; i < length; i++) {
        printf("%02x ",p_tx_buffer[i] );
    }
    printf("\n" );


    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_update_sw_msg(char * timestamp)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];
    char                *p_timestamp;

    printf(" INFO : Sending UPGRADE-SOFTWARE Message %s\n",timestamp);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_UPDATE_CCU_SW;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = TLV_TYPE_TIMESTAMP;
    p_protocol_hdr->length   = TIMESTAMP_SIZE;

    p_timestamp = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    memcpy(p_timestamp,timestamp,TIMESTAMP_SIZE);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_reboot_ccu_msg()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    printf(" INFO : Sending reboot ccu Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_REQUEST_REBOOT;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_wifi_set_mode(unsigned char wifi_mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];
    char                *p;

    printf(" INFO : Sending WIFI-SET-MODE Message = %02x  \n",wifi_mode);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_WIFI_SET_MODE;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0x01;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    p[0] = wifi_mode;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_upload_trip_info(unsigned char wifi_mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    printf(" INFO : Sending TRIP_INFO Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_UPLOAD_TRIP_INFO;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}
