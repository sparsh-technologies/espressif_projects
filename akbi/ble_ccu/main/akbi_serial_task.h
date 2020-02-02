#ifndef _AKBI_SERIAL_TASK_H_
#define _AKBI_SERIAL_TASK_H_

void uart_app_main(void);
void send_uart_message(const char* i_msg, int length ,char *ret_msg);

#endif
