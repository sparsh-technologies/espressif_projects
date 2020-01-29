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

int ccu_sent_subg_clear_learning_msg(char *p_tx_buffer) 
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_CLEAR_SUB_G_LEARN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;

    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_subg_learning_msg(char *p_tx_buffer)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_START_SUB_G_LEARN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_scan_all_wifi_msg(char *p_tx_buffer)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_START_WIFI_SCAN;
    p_protocol_hdr->trans_id = 44;
    p_protocol_hdr->type     = 0;
    p_protocol_hdr->length   = 0;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    length = sizeof(BT_CP_PROTOCOL_HDR) + p_protocol_hdr->length;
    send_uart_message(p_tx_buffer, length);

    return (0);
}

int ccu_sent_configure_wifi_credentials(char *p_tx_buffer, char *p_ap_name, char *p_passwd, int mode)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    char                *p;
    int                 length;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)p_tx_buffer;

    p_protocol_hdr->opcode   = BT_CP_OPCODE_SET_WIFI_CREDENTIALS;
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
    send_uart_message(p_tx_buffer, length);

    return (0);
}

