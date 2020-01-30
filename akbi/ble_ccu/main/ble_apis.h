#ifndef _BLE_APIS_H_
#define _BLE_APIS_H_

/****************************************************
*function to set return message from incoming message
****************************************************/
int read_ble_message(char *i_msg, char *i_ret_msg);

/****************************************************
*function to prepare and send message to ccu
****************************************************/
int prep_and_send_msg_to_ccu(char *received_value_buffer ,int read_ble_message_result);



#endif
