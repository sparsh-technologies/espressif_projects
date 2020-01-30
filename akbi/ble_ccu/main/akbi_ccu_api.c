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

int ccu_sent_subg_clear_learning_msg(char *p_tx_buffer)
{
    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;
    //char                *p;
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
    //char                *p;
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



int ccu_send_reg_msg(char *received_value_buffer)
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

    send_uart_message(p_tx_buffer, length);

    return 0;
}
