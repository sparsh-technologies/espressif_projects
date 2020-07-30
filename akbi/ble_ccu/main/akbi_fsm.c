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
#include "akbi_msg.h"
#include "akbi_bt_msg.h"
#include "akbi_ccu_api.h"
#include "esp_log.h"

#define BT_BLE_COEX_TAG             "AKBI_FSM"

extern CCU this_ccu;

static CCU_FSM_STATES bt_state = FSM_STATE_INIT;

int akbi_get_fsm_state()
{
    return bt_state;
}

// static char wifi_report[50][50];
// static int  wifi_report_length[50];
char        ccu_ap_ssid[20];
char        ccu_ap_passwd[20];
extern      AKBI_WIFI_SCAN_REPORT  wifi_scan_report;


static int ssid_index = 0;
static int no_of_stored_ssids = 0;

extern int akbi_slice_wifi_names();

// void akbi_clear_ssids(){
//     memset(wifi_report,0x00,sizeof(wifi_report));
//     no_of_stored_ssids = 0 ;
//     ssid_index = 0 ;
// }

void akbi_clear_ssids(){
    memset(&wifi_scan_report,0x00,sizeof(wifi_scan_report));
    no_of_stored_ssids = 0 ;
    ssid_index = 0 ;
}

void akbi_set_fsm_state(CCU_FSM_STATES state)
{
    bt_state = state;
}

// void save_ssids(char *ssid,int indx,int length){
//     memcpy(wifi_report[indx],ssid,length);
//     if(no_of_stored_ssids<(indx+1)){
//         no_of_stored_ssids = indx + 1;
//     }
//     wifi_report_length[indx] = length;
// }

int akbi_check_fsm_state_and_respond(char *ep_return_message)
{
    CCU_FSM_STATES    current_state;
    int               ret = 0,end_of_name = 0;
    char              wifi_name_part[15];

    current_state = akbi_get_fsm_state();
    /*
     * Return 1 if waiting for message from ccu. If received the message, return 0
     */
    switch(current_state)
    {

    case FSM_STATE_CCU_READY:
        ep_return_message[0] = 0x47;
        ret = 0;
        break;

    case FSM_STATE_INIT :
        ret = 0;
        break;

    case FSM_STATE_MOB_REGISTERED :
        ret = 0;
        break;

    case FSM_STATE_LOGIN :
        ret = 1;
        break;

    case FSM_STATE_LOGIN_STATUS :
        // printf("return login success\n" );
        this_ccu.paired_mob1.authentication_status = AUTHENTICATED;
        ret = 0;
        break;

    case FSM_STATE_FORGOT_PASSWD :
        ret = 1;
        break;

    case FSM_STATE_FORGOT_PASSWD_RESULT :
        ret = 0;
        break;

    case FSM_STATE_CHANGE_PASSWD :
        ret = 1;
        break;

    case FSM_STATE_CHANGE_PASSWD_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_VOICE_RECORDING_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_VOICE_RECORDING_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_SET_EMER_NUM_SENDING :
        ret = 1;
        break;

    case FSM_STATE_SET_EMER_NUM_RECEIVED :
        ret = 0;
        break;

    case FSM_STATE_SET_PERSONAL_NUM_SENDING :
        ret = 1;
        break;

    case FSM_STATE_SET_PERSONAL_NUM_RECEIVED :
        ret = 0;
        break;

    case FSM_STATE_WIFI_SCAN_IN_PROGRESS :
        // printf(" INFO : Wifi Scanning in progress ,ssid_index = %d\n",ssid_index);
        //set ssid name to packets
        ret = 1;
        break;

    case FSM_STATE_WIFI_SCAN_COMPLETE :
        ssid_index = 0;
        ret = 0;
        akbi_set_fsm_state(FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS);
        break;

    case FSM_STATE_WIFI_NAME_SEND_IN_PROGRESS :

        if (strlen(wifi_scan_report.ap_name[ssid_index])>11) {
            memset(wifi_name_part,0x00,15);
            end_of_name = akbi_slice_wifi_names(wifi_scan_report.ap_name[ssid_index] ,wifi_name_part);
            ep_return_message[RETURN_MSG_DATA_OFFSET] = strlen (wifi_scan_report.ap_name[ssid_index]);
            memcpy(ep_return_message+RETURN_MSG_DATA_OFFSET+1,wifi_name_part,11);
            ssid_index += end_of_name;
        }
        else{
            ep_return_message[RETURN_MSG_DATA_OFFSET] = strlen(wifi_scan_report.ap_name[ssid_index]);
            memcpy(ep_return_message+RETURN_MSG_DATA_OFFSET+1,wifi_scan_report.ap_name[ssid_index],11);
            ssid_index++;
        }
        ret = 0;
        break;


    case FSM_STATE_WIFI_SELECT_IN_PROGRESS :
        ret = 0;
        break;


    case FSM_STATE_WIFI_CONNECT_IN_PROGRESS :
        // printf("wifi connect in progress\n");
        ret = 1;
        break;

    case FSM_STATE_WIFI_CONNECT_COMPLETE :
        // ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
        ret = 0;
        break;

    case FSM_STATE_WIFI_DISCONNECT_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_WIFI_DISCONNECT_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_FW_UPGRADE_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_FW_UPGRADE_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_FW_REBOOT_MSG_SENDING :
        ret = 1;
        break;

    case FSM_STATE_FW_REBOOT_MSG_ACKNOWLEDGED :
        ret = 0;
        break;

    case FSM_STATE_TRIP_INFO_UPLOAD_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_TRIP_INFO_UPLOAD_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_CFG_SET_ADDRESS :
        ret = 1;
        break;

    case FSM_STATE_CFG_SET_ADDRESS_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_CFG_SET_HELP_NUM_SENDING :
        ret = 1;
        break;

    case FSM_STATE_CFG_SET_HELP_NUM_RECEIVED :
        ret = 0;
        break;

    case FSM_STATE_ACTIVATE_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_ACTIVATE_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_WIFI_MODE_SET_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_WIFI_MODE_SET_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_TEST_REMOTE_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_TEST_REMOTE_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_PROGRAM_NEW_REMOTE_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_PROGRAM_NEW_REMOTE_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_SITE_SPECIFIC_STATUS_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_SITE_SPECIFIC_STATUS_COMPLETE :
        ret = 0;
        break;

    case FSM_STATE_TRIP_INFO_STATUS_IN_PROGRESS :
        ret = 1;
        break;

    case FSM_STATE_TRIP_INFO_STATUS_COMPLETE :
        ret = 0;
        break;

    }
    return ret;
}
