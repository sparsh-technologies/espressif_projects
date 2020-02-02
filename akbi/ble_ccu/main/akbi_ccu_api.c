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

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;

    send_uart_message(p_tx_buffer, length , ep_return_message);

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

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_send_reg_msg(char *received_value_buffer,char *ep_return_message)
{

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    char                p_tx_buffer[25];

    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = received_value_buffer[BLE_CMD_OFFSET];
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = received_value_buffer[BLE_MSG_MULTI_DATA_TYPE_OFFSET];
    p_protocol_hdr->length   = received_value_buffer[BLE_MSG_MULTI_DATA_LEN_OFFSET];

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p,(received_value_buffer+BLE_MSG_MULTI_DATA_LEN_OFFSET+1), p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;

    send_uart_message(p_tx_buffer, length , ep_return_message);

    return 0;
}

int ccu_send_login_msg()
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                p_tx_buffer[25];
    char                p_rx_buffer[25];

    printf(" INFO : Sending LOGIN Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_LOGIN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , p_rx_buffer);

    return 0;
}

int ccu_sent_scan_all_wifi_msg(char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    //int                 count;
    char                p_tx_buffer[25];

    printf(" INFO : Sending SCAN-ALL-WIFI Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_SCAN_WIFIS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_configure_wifi_credentials(char *p_tx_buffer,char *ep_return_message, char *p_ap_name, char *p_passwd, int mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;
    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_SELECT_A_WIFI;
    p_protocol_hdr->trans_id = 44;

    /*
     * Now set all the TLV data here.
     */
    p_protocol_hdr->type     = TLV_TYPE_WIFI_AP_NAME;
    length = strlen(p_ap_name) + strlen(p_passwd) + 1;
    p_protocol_hdr->length   = length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);

    /*
     * Append Wifi AP name here
     */

    *p = TLV_TYPE_WIFI_AP_NAME;
    p++;
    memcpy(p, p_ap_name, strlen(p_ap_name));
    p += strlen(p_ap_name);

    /*
     * Append Wifi Passwd here
     */
    *p = TLV_TYPE_WIFI_PASSWD_NAME;
    p++;
    memcpy(p, p_passwd, strlen(p_passwd));
    p += strlen(p_passwd);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_user_login_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending USER-LOGIN Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_LOGIN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_user_forgot_passwd_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending USER-FORGOT_PASSWD Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_FORGOT_PASSWORD;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_user_change_passwd_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending USER-FORGOT_PASSWD Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_FORGOT_PASSWORD;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_record_voice_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending RECORD-VOICE Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_RECORD_PERSONAL_VOICE_MSG;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

/*
 * API to send Emerggency number to CCU
 */
int ccu_sent_store_emergency_number_msg(char *p_tx_buffer,int emergency_no_id,
                                        char *emergency_num, int num_length)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;

    printf(" INFO : Sending EMERGENCY-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_STORE_EMERGENCY_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = emergency_no_id;
    p_protocol_hdr->length   = num_length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, emergency_num, num_length);

    length = p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

/*
 * API to send Personal number to CCU
 */
int ccu_sent_store_personal_number_msg(char *p_tx_buffer, int pers_no_id,
                                       char *personal_num, int num_length)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;

    printf(" INFO : Sending PERSONAL-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_STORE_PERSONAL_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = pers_no_id;
    p_protocol_hdr->length   = num_length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, personal_num, num_length);

    length = p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

/*
 * API to send Local number to CCU
 */
int ccu_sent_store_local_help_number_msg(char *p_tx_buffer, int local_num_id, 
                                         char *local_num, int num_length)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;
    char                *p;

    printf(" INFO : Sending LOCAL-HELP-NUM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_ENTER_LOCAL_HELP_NUMBERS;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = local_num_id;
    p_protocol_hdr->length   = num_length;

    p = p_tx_buffer + sizeof(BT_CP_PROTOCOL_HDR);
    memcpy(p, local_num, num_length);

    length = p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}

int ccu_sent_activate_system_msg(char *p_tx_buffer,char *ep_return_message)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    int                 length;

    printf(" INFO : Sending ACTIVATE-SYSTEM Message \n");
    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CID_CCU_ACTIVATE;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;


    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length , ep_return_message);

    return (0);
}
