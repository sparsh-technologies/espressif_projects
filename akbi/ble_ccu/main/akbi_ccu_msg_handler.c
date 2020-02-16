
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"
#include <rom/ets_sys.h>


#define MAX_RETURN_MSG_LENGTH       20

AKBI_WIFI_SCAN_REPORT  wifi_scan_report;
extern char ep_return_message[MAX_RETURN_MSG_LENGTH];
extern char firmware_version[10];
extern char ccu_serial_no[20];

void akbi_process_rx_serial_data(char *ccu_msg,int length)
{
    int              index;
    int              type_in_msg;
    int              length_in_msg;
    char             *p_payload;
    BT_CP_TLV_HDR    *p_tlv;

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)ccu_msg;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    p_payload     = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);
    p_tlv         = (BT_CP_TLV_HDR *) (ccu_msg + 0x03);

    type_in_msg   = p_protocol_hdr->type;
    length_in_msg = p_protocol_hdr->length;

    switch(p_protocol_hdr->opcode)
    {
        case BT_CP_OPCODE_CID_CCU_READY:
            akbi_set_fsm_state(FSM_STATE_CCU_READY);

            memset(firmware_version, 0x00, 10);
            memset(ccu_serial_no, 0x00, 20);

            /*
             * Extract the firmware version from the packet
             */

            akbi_dump_serial_pkt(ccu_msg, length);

            memcpy(firmware_version, p_tlv->data, p_tlv->length);
            printf(" INFO : FW-Version      : %s \n", firmware_version );

            p_tlv = (BT_CP_TLV_HDR *)(ccu_msg + 0x03 + p_tlv->length); 
            memcpy(ccu_serial_no, p_tlv->data, p_tlv->length);
			printf(" INFO : SERIAL-NO-TYPE : %x \n", p_tlv->type );
			printf(" INFO : SERIAL-NO      : %d \n", p_tlv->length );
            printf(" INFO : SERIAL-NO      : %s \n", ccu_serial_no );

            break;

        case BT_CP_OPCODE_CID_SCAN_WIFI_RESULT :
            /*
            * Now check whether this is the last packet. If so, just mark the scanning as completed.
            */
            if (p_protocol_hdr->type == 0) {
              akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_COMPLETE);
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
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
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = WIFI_SCANNING_IN_PROGRESS;
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
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                return;
            }

        case BT_CP_OPCODE_CID_SELECT_A_WIFI_RESULT:
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            return;

        case BT_CP_OPCODE_CID_WIFI_CONNECT_COMPLETED:
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            return;

        case BT_CP_OPCODE_CID_CFG_PARAMS_SAVE_STATUS:
            if(type_in_msg == TLV_TYPE_STORED_CFG_EMER_NUM_3){
                akbi_set_fsm_state(FSM_STATE_SET_EMER_NUM_RECEIVED);
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            }
            if(type_in_msg == TLV_TYPE_STORED_CFG_PERSONAL_NUM_3){
                akbi_set_fsm_state(FSM_STATE_SET_PERSONAL_NUM_RECEIVED);
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            }
            break;





        /*case BT_CP_OPCODE_CID_CCU_SAVE_VOICE:
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_CCU_save_voice);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ;
                  return;
              }
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_INIT);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                  return;
              }
              if (p_protocol_hdr->type == 0) {
                  akbi_set_fsm_state(FSM_STATE_save_voice_cmplete);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                  return;
              }
              break;
        */
        /*case BT_CP_OPCODE_CID_CCU_ACTIVATE_STATUS:
              if (p_payload[0] == SUCCESS) {
                  akbi_set_fsm_state(FSM_STATE_CCU_ACTIVATED);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                  return;
              }else if (p_payload[0] == ERROR_){
                  akbi_set_fsm_state(FSM_STATE_INIT);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                  return;
              }
              else if (p_payload[0] == ERROR_){
                  akbi_set_fsm_state(FSM_STATE_INIT);
                  ep_return_message[ERROR_SENDING_TEXT_MSG] = ERROR_;
                  return;
              }
              break;
          */
          /*case BT_CP_OPCODE_CID_FORGOT_PASSWORD_STATUS:
                if (p_payload[0] == SUCCESS) {
                    akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD_SMS_SENT);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                    return;
                }else if (p_payload[0] == ERROR_MOB1_NO_NOT_CONFIGURED){
                    akbi_set_fsm_state(FSM_STATE_INIT);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_MOB1_NO_NOT_CONFIGURED;
                    return;
                }
                else if (p_payload[0] == ERROR_SENDING_TEXT_MSG){
                    akbi_set_fsm_state(FSM_STATE_INIT);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_MOB1_NO_NOT_CONFIGURED;
                    return;
                }
                break;
          */
          /*case BT_CP_OPCODE_CID_CCU_CHANGE_PWD_STATUS:
                if (p_payload[0] == SUCCESS) {
                    akbi_set_fsm_state(FSM_STATE_CCU_PWD_CHANGED);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                    return;
                }else if (p_payload[0] == ERROR_){
                    akbi_set_fsm_state(FSM_STATE_INIT);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                    return;
                }
                else if (p_payload[0] == ERROR_){
                    akbi_set_fsm_state(FSM_STATE_INIT);
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                    return;
                }
                break;
          */
        default:
            printf("undefined opcode from ccu\n" );
            break;

        }
    return;
}
