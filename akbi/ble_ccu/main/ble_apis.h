#ifndef _BLE_APIS_H_
#define _BLE_APIS_H_

/****************************************************
*function to set return message from incoming message
****************************************************/
int read_ble_message(char *i_msg, char *i_ret_msg);

/****************************************************
*function to populate message from mobile to linux
****************************************************/
int populate_bt_msg_to_serial(char *received_value_buffer,char *msg_to_ccu, int *msg_length);



#endif
