
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"

#define MAX_RETURN_MSG_LENGTH       20

AKBI_WIFI_SCAN_REPORT  wifi_scan_report;
extern char ep_return_message[MAX_RETURN_MSG_LENGTH];

void akbi_process_rx_serial_data(char *ccu_msg,int length)
{
    int    index;
    char   *p_payload;

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)ccu_msg;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    p_payload = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);

    switch(p_protocol_hdr->opcode)
    {
        case BT_CP_OPCODE_CID_CCU_READY:
            akbi_set_fsm_state(FSM_STATE_CCU_READY);
            break;

        case BT_CP_OPCODE_CID_SCAN_WIFI_RESULT :
            /*
            * Now check whether this is the last packet. If so, just mark the scanning as completed.
            */
            if (p_protocol_hdr->type == 0) {
              akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_COMPLETE);
              memset(&ep_return_message[BLE_RET_MSG_RC_OFFSET],SUCCESS,BLE_RETURN_RC_SIZE);
              memcpy(&ep_return_message[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],
                     &wifi_scan_report.ap_count,SCANNED_WIFI_COUNT_SIZE);
              return ;
            }
            else {
                if (p_protocol_hdr->type == 1 ) {
                    /*
                     * First check whether this is the first packet. If this is the first packet, then
                     * cleanupp the datastructure here.
                     */
                    memset(&wifi_scan_report, 0x00, sizeof(AKBI_WIFI_SCAN_REPORT));
                }
                memset(&ep_return_message[BLE_RET_MSG_RC_OFFSET],WIFI_SCANNING_IN_PROGRESS,BLE_RETURN_RC_SIZE);
                /*
                * Now, copy all the contents into the local datastructure.
                */
                index = p_protocol_hdr->type;

                strncpy(wifi_scan_report.ap_name[index-1], p_payload, p_protocol_hdr->length);
                save_ssids(wifi_scan_report.ap_name[index-1],index-1,p_protocol_hdr->length);
                wifi_scan_report.ap_count++;
                break;
            }

        case BT_CP_OPCODE_CID_LOGIN_STATUS:
            if (p_payload[0] == SUCCESS) {
                akbi_set_fsm_state(FSM_STATE_LOGIN_SUCCESS);
                memset(&ep_return_message[BLE_RET_MSG_RC_OFFSET],SUCCESS,BLE_RETURN_RC_SIZE);
                return;
            }
        // case BT_CP_OPCODE_CID_FORGOT_PASSWORD_STATUS:
        //       if (p_payload[0] == SUCCESS) {
              //     akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD_SMS_SENT);
              //     memset(&ep_return_message[BLE_RET_MSG_RC_OFFSET],SUCCESS,BLE_RETURN_RC_SIZE);
              //     return;
          //     }else if (p_payload[0] == ERROR_MOB1_NO_NOT_CONFIGURED){
                  // akbi_set_fsm_state(FSM_STATE_INIT);
                  // memset(&ep_return_message[BLE_RET_MSG_RC_OFFSET],ERROR_MOB1_NO_NOT_CONFIGURED,BLE_RETURN_RC_SIZE);
                  // return;
            //  }
            //     else if (p_payload[0] == ERROR_SENDING_TEXT_MSG){
                    // akbi_set_fsm_state(FSM_STATE_INIT);
                    // memset(&ep_return_message[ERROR_SENDING_TEXT_MSG],ERROR_MOB1_NO_NOT_CONFIGURED,BLE_RETURN_RC_SIZE);
                    // return;
              //  }        //      break;
        // default:
        //     break;

        }
    return;
}
