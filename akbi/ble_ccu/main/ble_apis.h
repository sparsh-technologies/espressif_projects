#ifndef _BLE_APIS_H_
#define _BLE_APIS_H_

/****************************************************
*function to set return message from incoming message
****************************************************/
int read_ble_message(char *i_msg, char *i_ret_msg);

int execute_akbi_sent_adv_started_msg();

#endif
