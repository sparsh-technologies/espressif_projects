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
    //printf(" INFO : Sent REGISTER Cmd Length(%d) \n", p_protocol_hdr->length);

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

int ccu_sent_connect_to_wifi()
{

    char p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    //printf(" INFO : Sending CONNECT-TO-WIFI Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CONNECT_TO_WIFI;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_disconnect_from_wifi()
{

    char p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    //printf(" INFO : Sending DISCONNECT-FROM-WIFI Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_DISCONNECT_FROM_WIFI;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}
//
// int ccu_sent_user_login_msg(char *p_tx_buffer,char *ep_return_message)
// {
//     BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
//     int                 length;
//
//     printf(" INFO : Sending USER-LOGIN Message \n");
//     p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;
//
//     p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_LOGIN;
//     p_protocol_hdr->trans_id = 44;
//     p_protocol_hdr->type     = 0;
//     p_protocol_hdr->length   = 0;
//
//
//     length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
//     send_uart_message(p_tx_buffer, length );
//
//     return (0);
// }

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

int ccu_sent_user_change_passwd_msg(char *received_value_buffer,char *ep_return_message)
{
    char                p_tx_buffer[20];
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;

    //printf(" INFO : Sending USER-CHANGE_PASSWD Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CHANGE_PASSWORD;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, (received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_record_voice_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    //printf(" INFO : Sending RECORD-VOICE Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_RECORD_PERSONAL_VOICE_MSG;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

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

int ccu_sent_address_visiting(char *received_value_buffer)
{

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    char                p_tx_buffer[25];

    int                 length;

    memset(p_tx_buffer, 0x00, 25);
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_ADDRESS_VISITING;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;//data_len;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    // memcpy(p, received_value_buffer, p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    //printf(" INFO : Sent visiting address \n");

    send_uart_message(p_tx_buffer, length);

    return 0;
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

    length = p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_activate_system_msg()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    //printf(" INFO : Sending ACTIVATE-SYSTEM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CCU_ACTIVATE;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_update_sw_msg()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    //printf(" INFO : Sending UPGRADE-SOFTWARE Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_UPDATE_CCU_SW;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}

int ccu_sent_upload_trip_info()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[20];

    //printf(" INFO : Sending TRIP_INFO Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_UPLOAD_TRIP_INFO;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length );

    return (0);
}
