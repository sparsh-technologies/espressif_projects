/*
 ****************************************************************************************
 * akbi_ccu_api.h
 *
 * Author    : Shikhil 
 * Ver       : 1.0
 * Date      : 31-Dec-2019
 *
 * Copyright Infinicomm Solutions India Pvt Ltd, 2019-20
 *
 ****************************************************************************************
 */

#ifndef _AKBI_CCU_API_H_
#define _AKBI_CCU_API_H_

#define MAX_AP_NAME_LENGTH                 100

typedef struct _akbi_wifi_scan_report_
{
    int    ap_count;
    char   ap_name[20][MAX_AP_NAME_LENGTH];

} AKBI_WIFI_SCAN_REPORT;


/*****************************************************
*function to send 'sub gigahertz clear' command to ccu
*****************************************************/
int ccu_sent_subg_clear_learning_msg(char *p_tx_buffer) ;


/*****************************************************
*function to send 'sub gigahertz learn' command to ccu
*****************************************************/
int ccu_sent_subg_learning_msg(char *p_tx_buffer);

int ccu_send_reg_msg(char *received_value_buffer ,char *ret_msg);





#endif
