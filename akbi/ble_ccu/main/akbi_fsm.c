/*
 ****************************************************************************************
 * akbi_fsm.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 28-Jan-2020
 *
 * Copyright Infinicomm Solutions (I) Pvt Ltd, 2020
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "akbi_fsm.h"

CCU_FSM_STATES bt_state = FSM_STATE_INIT;

int akbi_get_fsm_state()
{
    return (bt_state);
}

void akbi_set_fsm_state(CCU_FSM_STATES state)
{
    bt_state = state;
}

int akbi_check_fsm_state_and_respond()
{
    CCU_FSM_STATES    current_state;
    int               ret = 0;

    current_state = akbi_get_fsm_state();

    switch(current_state)
    {

    case FSM_STATE_INIT :
        ret = 0;
        break;

    case FSM_STATE_MOB_REGISTERED :
        ret = 0;
        break;

    case FSM_STATE_LOGIN :
        ret = 0;
        break;

    case FSM_STATE_FORGOT_PASSWD :
        ret = 0;
        break;

    case FSM_STATE_CHANGE_PASSWD :
        ret = 0;
        break;

    case FSM_STATE_VOICE_RECORDING_IN_PRORESS :
        ret = 0;
        break;

    case FSM_STATE_SET_EMER_NUM :
        ret = 0;
        break;

    case FSM_STATE_SET_PERSONAL_NUM :
        ret = 0;
        break;

    case FSM_STATE_WIFI_SCAN_IN_PROGRESS :
        printf(" INFO : Wifi Scanning in progress \n");
        ret = 1;
        break;

    case FSM_STATE_WIFI_SCAN_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_WIFI_SELECT_IN_PROGRESS :
        ret = 0;
        break;

    case FSM_STATE_WIFI_CONNECT_IN_PROGRESS :
        ret = 0;
        break;

    case FSM_STATE_WIFI_CONNECT_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_CFG_SET_ADDRESS :
        ret = 0;
        break;

    case FSM_STATE_CFG_SET_HELP_NUM :
        ret = 0;
        break;

    case FSM_STATE_ACTIVATE_IN_PROGRESS :
        ret = 0;
        break;

    }

    return (ret);
}
