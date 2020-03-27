/*
 ****************************************************************************************
 * icom_ipc.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_IPC_H_
#define _ICOM_IPC_H_

#define ICOM_IPC_MAX_MSG_SZ                            32
#define ICOM_IPC_MAX_PAYLOAD_SZ                        256

typedef struct _icom_ipc_msg_
{
    unsigned char          opcode;
    unsigned char          data_sz;
    unsigned char          data[14];

} ICOM_IPC_MSG;

/*
 * Various Opcodes are defined here.
 */
#define IPC_OPCODE_PING                                0x01

extern int icom_ipc_init(void);
extern ICOM_IPC_MSG *icom_alloc_ipc_buffer(void);
extern void icom_free_ipc_buffer(ICOM_IPC_MSG *p_msg);
extern int icom_create_task_queue(int task_id);
extern int icom_recv_ipc_buffer(ICOM_IPC_MSG *p_ipc_msg, int task_id);
extern int icom_send_ipc_buffer(int task_id, ICOM_IPC_MSG *p_msg);

#endif

