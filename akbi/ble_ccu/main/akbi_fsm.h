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

enum {
    FSM_STATE_MOB_REGISTERED,
    FSM_STATE_LOGIN,
    FSM_STATE_FORGOT_PASSWD,
    FSM_STATE_CHANGE_PASSWD,
    FSM_STATE_VOICE_RECORDING_IN_PRORESS,
    FSM_STATE_SET_EMER_NUM,
    FSM_STATE_SET_PERSONAL_NUM,
    FSM_STATE_WIFI_SCAN_IN_PROGRESS,
    FSM_STATE_WIFI_SCAN_COMPLETE,
    FSM_STATE_WIFI_SELECT_IN_PROGRESS,
    FSM_STATE_WIFI_CONNECT_IN_PROGRESS,
    FSM_STATE_WIFI_CONNECT_COMPLETE,
    FSM_STATE_CFG_SET_ADDRESS,
    FSM_STATE_CFG_SET_HELP_NUM,
    FSM_STATE_ACTIVATE_IN_PROGRESS,
} CCU_FSM_STATES;

extern int akbi_get_fsm_state();
extern void akbi_set_fsm_state();

#endif
