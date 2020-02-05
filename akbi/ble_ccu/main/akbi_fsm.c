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


static CCU_FSM_STATES bt_state = FSM_STATE_INIT;

int akbi_get_fsm_state()
{
    return bt_state;
}

static char wifi_report[10][20];
static int  wifi_report_length[10];

static int ssid_index = 0;
static int no_of_stored_ssids = 0;

void akbi_clear_ssids(){
    memset(wifi_report,0x00,sizeof(wifi_report));
    no_of_stored_ssids = 0 ;
    ssid_index = 0 ;
}

void akbi_set_fsm_state(CCU_FSM_STATES state)
{
    bt_state = state;
}

void save_ssids(char *ssid,int indx,int length){
    memcpy(wifi_report[indx],ssid,length);
    if(no_of_stored_ssids<(indx+1)){
        no_of_stored_ssids = indx + 1;
    }
    wifi_report_length[indx] = length;
}

int akbi_check_fsm_state_and_respond(char *ep_return_message)
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
        ret = 1;
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
        printf(" INFO : Wifi Scanning in progress ,ssid_index = %d\n",ssid_index);
        //set ssid name to packets
        ret = 1;
        break;

    case FSM_STATE_WIFI_SCAN_COMPLETE :
        ssid_index = 0;
        ret = 0;
        akbi_set_fsm_state(FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS);
        break;

    case FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS :

        memcpy(ep_return_message+RETURN_MSG_DATA_OFFSET,&wifi_report[ssid_index],12);

        if (wifi_report_length[ssid_index]>12) {
            memcpy(ep_return_message+RETURN_MSG_DATA_OFFSET,&wifi_report[ssid_index],10);
            memset(ep_return_message+RETURN_MSG_DATA_OFFSET+10,'~',1);
            memset(ep_return_message+RETURN_MSG_DATA_OFFSET+11,49,1);
            ssid_index++;
        }
        else{
            memcpy(ep_return_message+RETURN_MSG_DATA_OFFSET,&wifi_report[ssid_index],12);
            ssid_index++;
        }
        ret = 0;
        break;


    case FSM_STATE_WIFI_SELECT_IN_PROGRESS :
        ret = 0;
        break;

    case FSM_STATE_WIFI_CONNECT_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_WIFI_CONNECT_COMPLETE :
        ret = 1;
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
    return ret;
}
