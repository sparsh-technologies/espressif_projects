#ifndef __AKBI_SERIAL_TASK_H__
#define __AKBI_SERIAL_TASK_H__

void uart_app_main(void);
void send_uart_message(const char* i_msg, int length ,char *ret_msg);

#endif
