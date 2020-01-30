#ifndef __AKBI_CCU_API_H__
#define __AKBI_CCU_API_H__



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
