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

int ccu_send_login_msg(char * password, int data_len,char * time_stamp);

int ccu_sent_scan_all_wifi_msg(char *ret_msg);

int ccu_sent_configure_wifi_credentials(unsigned char ap_id, char *p_passwd, int mode);

int ccu_sent_connect_to_wifi(char * timestamp);

int ccu_sent_disconnect_from_wifi();

int ccu_sent_user_login_msg(char *p_tx_buffer,char *ep_return_message);

int ccu_sent_user_forgot_passwd_msg();

int ccu_sent_user_change_passwd_msg(char *received_value_buffer,char *ep_return_message);

int ccu_sent_record_voice_msg(unsigned char voice_msg_index, unsigned int voice_msg_length);

int ccu_sent_store_emergency_number_msg(char *received_value_buffer);

int ccu_sent_store_personal_number_msg(char *received_value_buffer);

int ccu_sent_address_visiting(char *timestamp, unsigned int voice_msg_length);

int ccu_sent_store_local_help_number_msg(char *received_value_buffer);

int ccu_sent_activate_system_msg(char *p_latitude, char *p_longitude,char *timestamp, int mode);

int ccu_sent_update_sw_msg(char * timestamp);

int ccu_sent_reboot_ccu_msg();

int ccu_sent_wifi_set_mode(unsigned char wifi_mode);

int ccu_sent_upload_trip_info();

int ccu_sent_record_voice_msg_raw(char *voice_data,unsigned int chunk_length,unsigned char audio_number);

int ccu_sent_address_visiting_raw(char *voice_data,unsigned int chunk_length,unsigned char audio_number);

int ccu_sent_adv_started_msg();

int ccu_sent_mob_connected_msg();

int ccu_sent_mob_disconnected_msg();


#endif
