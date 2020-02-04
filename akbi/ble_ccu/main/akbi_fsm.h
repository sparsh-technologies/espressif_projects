/*
 ****************************************************************************************
 * akbi_fsm.h
 *
 * Author    : Shikhil
 * Ver       : 1.0
 * Date      : 31-Dec-2019
 *
 * Copyright Infinicomm Solutions India Pvt Ltd, 2019-20
 *
 ****************************************************************************************
 */

#ifndef _AKBI_FSM_H_
#define _AKBI_FSM_H_

typedef enum {
    FSM_STATE_INIT,
    FSM_STATE_MOB_REGISTERED,
    FSM_STATE_LOGIN,
    FSM_STATE_FORGOT_PASSWD,
    FSM_STATE_CHANGE_PASSWD,
    FSM_STATE_VOICE_RECORDING_IN_PRORESS,
    FSM_STATE_SET_EMER_NUM,
    FSM_STATE_SET_PERSONAL_NUM,
    FSM_STATE_WIFI_SCAN_IN_PROGRESS,
    FSM_STATE_WIFI_SCAN_COMPLETE,
    FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS,
    FSM_STATE_WIFI_CONNECT_IN_PROGRESS,
    FSM_STATE_WIFI_CONNECT_COMPLETE,
    FSM_STATE_CFG_SET_ADDRESS,
    FSM_STATE_CFG_SET_HELP_NUM,
    FSM_STATE_ACTIVATE_IN_PROGRESS,
} CCU_FSM_STATES;

extern int akbi_get_fsm_state(void);
extern void akbi_set_fsm_state(CCU_FSM_STATES state);
extern int akbi_check_fsm_state_and_respond(char *ep_return_message);
void save_ssids(char *ssid,int indx,int length);
void akbi_clear_ssids();
#define RETURN_MSG_DATA_OFFSET 8
#define AKBI_RC_OFFSET         6
#define RETURN_MSG_LENGTH      20


#endif
