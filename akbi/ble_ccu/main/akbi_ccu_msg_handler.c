
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"
#include "akbi_serial_task.h"
#include <rom/ets_sys.h>


#define MAX_RETURN_MSG_LENGTH       20
// #define DEBUG_ENABLE


AKBI_WIFI_SCAN_REPORT  wifi_scan_report;
extern char ep_return_message[MAX_RETURN_MSG_LENGTH];
extern char firmware_version[10];
extern char ccu_serial_no[20];
unsigned char post_result = 0;
extern char   ccu_ap_ssid[20];
extern char   ccu_ap_passwd[20];

void akbi_process_rx_serial_data(char *ccu_msg,int length)
{
    int              index;
    int              type_in_msg;
    int              length_in_msg;
    char             *p_payload;
    BT_CP_TLV_HDR    *p_tlv;
    char *p;

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)ccu_msg;
    #ifdef DEBUG_ENABLE
    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);
    #endif

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

            memcpy(firmware_version, p_tlv->data, length_in_msg);
            // printf(" INFO : FW-Version      : %s \n", firmware_version );

            p = (char *)p_tlv ;
            p = p + 0x02 + p_tlv->length;
            p_tlv = (BT_CP_TLV_HDR *) p ;

            memcpy(ccu_serial_no, p_tlv->data, p_tlv->length);
      			// printf(" INFO : SERIAL-NO-TYPE : %x \n", p_tlv->type );
      			// printf(" INFO : SERIAL-NO len  : %d \n", p_tlv->length );
            // printf(" INFO : SERIAL-NO      : %s \n", ccu_serial_no );

            p = (char *)p_tlv ;
            p = p + 0x02 + p_tlv->length;
            p_tlv = (BT_CP_TLV_HDR *) p ;

            memcpy(&post_result, p_tlv->data, p_tlv->length);
            // printf(" INFO : length           : %02x\n",  p_tlv->length );
            // printf(" INFO : POST result      : %02x \n", post_result );



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

                /*
                 * workaround to tackle problem with ssid length 13
                 */
                if (p_protocol_hdr->length == 0x0a) {
                    if(p_payload[0x0b] != 0x00){
                        p_protocol_hdr->length = 0x0d;
                    }
                }
                /*
                 * workaround to tackle problem with ssid ssid_index
                 */
                 if (index==0x0a) {
                     if (strlen(wifi_scan_report.ap_name[0x0b])) {
                         index = 0x0d;
                     }
                 }
                memcpy(wifi_scan_report.ap_name[index-1], p_payload, p_protocol_hdr->length);
                save_ssids(wifi_scan_report.ap_name[index-1],index-1,p_protocol_hdr->length);
                wifi_scan_report.ap_count++;
            }
            break;

        case BT_CP_OPCODE_CID_LOGIN_STATUS:
            akbi_set_fsm_state(FSM_STATE_LOGIN_STATUS);

            if (p_payload[0] == SUCCESS) {

                ep_return_message[BLE_RET_MSG_DATA_TYPE_OFFSET] = post_result;
                if(post_result != ALL_SUCCESS_POST_VALUE){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = BLE_RET_POST_DATA_ERROR;
                    ep_return_message[BLE_RET_MSG_RC_OFFSET+1] = post_result;
                }
                else{
                    memcpy(&ep_return_message[BLE_RET_MSG_FIRMWARE_VERSION_OFFSET],firmware_version,strlen(firmware_version));
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                }
                return;
            }
            else if (p_payload[0] ==ERROR_LOGIN_PASSWORD_MISMATCH){
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_LOGIN_PASSWORD_MISMATCH;
            }
            else if (p_payload[0] ==ERROR_CCU_LOCKED)
            {
                ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_CCU_LOCKED;
            }
            break;

        case BT_CP_OPCODE_CID_SELECT_A_WIFI_RESULT:
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_COMPLETE);
            // ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            switch(p_payload[0]) {

                case SERVER_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
                    break;

                case INET_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x30;
                    break;

                case WIFI_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x31;
                    break;

                case NOT_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] =  0x33;
                    break;

                case PAIRED_WIFI_UNREACHABLE:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] =  0x34;
                    break;

                default:
                    printf("undefined wifi result code \n" );
                    break;
            }


            break;

        case BT_CP_OPCODE_CID_WIFI_CONNECT_COMPLETED:
            akbi_set_fsm_state(FSM_STATE_WIFI_CONNECT_COMPLETE);
            // ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            switch(p_payload[0]) {

                case SERVER_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x00;
                    break;

                case INET_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x30;
                    break;

                case WIFI_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x31;
                    break;

                case NOT_CONNECTED:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x33;
                    break;

                case PAIRED_WIFI_UNREACHABLE:
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] =  0x34;
                    break;

                default:
                    printf("undefined wifi result code \n" );
                    break;
            }
            break;

        case BT_CP_OPCODE_CID_DISCONNECT_FROM_WIFI_STATUS:
            akbi_set_fsm_state(FSM_STATE_WIFI_DISCONNECT_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            break;

        case BT_CP_OPCODE_CID_CFG_PARAMS_SAVE_STATUS:
            if((type_in_msg == TLV_TYPE_STORED_CFG_EMER_NUM_2)||(type_in_msg == TLV_TYPE_STORED_CFG_EMER_NUM_3)){
                akbi_set_fsm_state(FSM_STATE_SET_EMER_NUM_RECEIVED);
                if(p_payload[0] == 0){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x0B;
                }
                else if(p_payload[0] == 0x01){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0;
                }
            }
            if((type_in_msg == TLV_TYPE_STORED_CFG_PERSONAL_NUM_2)||(type_in_msg == TLV_TYPE_STORED_CFG_PERSONAL_NUM_3)){
                akbi_set_fsm_state(FSM_STATE_SET_PERSONAL_NUM_RECEIVED);
                if(p_payload[0] == 0){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x0C;
                }
                else if(p_payload[0] == 0x01){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0;
                }
            }
            if((type_in_msg == TLV_TYPE_STORED_CFG_LOCAL_NUM_1)||(type_in_msg == TLV_TYPE_STORED_CFG_LOCAL_NUM_2)){
                akbi_set_fsm_state(FSM_STATE_CFG_SET_HELP_NUM_RECEIVED);
                if(p_payload[0] == 0){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x1C;
                }
                else if(p_payload[0] == 0x01){
                    ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0;
                }
            }
            break;

        case BT_CP_OPCODE_CID_CHANGE_PASSWORD_STATUS:
            if (p_payload[0] == 0x06) {
              akbi_set_fsm_state(FSM_STATE_CHANGE_PASSWD_COMPLETE);
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x09;
              return;
            }
            if (p_payload[0] == 0x00) {
              akbi_set_fsm_state(FSM_STATE_CHANGE_PASSWD_COMPLETE);
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
              return;
            }
            else{
              printf("error message type from ccu\n");
            }
            break;

        case BT_CP_OPCODE_CID_FORGOT_PASSWORD:
            if (p_payload[0] == 0x07) {
              akbi_set_fsm_state(FSM_STATE_FORGOT_PASSWD_RESULT);
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = 0x07;
              return;
            }

              break;

        case BT_CP_OPCODE_CID_UPDATE_CCU_SW_STATUS:
            akbi_set_fsm_state(FSM_STATE_FW_UPGRADE_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            if(type_in_msg == TLV_TYPE_UPDATE_FW_REBOOT){
                abort();
            }
            break;

        case BT_CP_OPCODE_CID_WIFI_SET_MODE_STATUS:
            akbi_set_fsm_state(FSM_STATE_WIFI_MODE_SET_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            break;

        case BT_CP_OPCODE_CID_RECORD_PERSONAL_VOICE_MSG_STATUS:
            akbi_set_fsm_state(FSM_STATE_VOICE_RECORDING_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            break;

        case BT_CP_OPCODE_CID_ADDRESS_VISITING_STATUS:
            akbi_set_fsm_state(FSM_STATE_CFG_SET_ADDRESS_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            break;

        case BT_CP_OPCODE_CID_REQUEST_REBOOT_STATUS:
            akbi_set_fsm_state(FSM_STATE_FW_REBOOT_MSG_ACKNOWLEDGED);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            ets_delay_us(3000000);
            abort();
            break;

        case BT_CP_OPCODE_CID_UPLOAD_TRIP_INFO_STATUS:
            akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_COMPLETE);
            if (p_payload[0] == 0x0f) {
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            }
            else{
              ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_INFO_NOT_UPLOADED;
            }
            break;

        case BT_CP_OPCODE_CID_CCU_ACTIVATE_STATUS:
            akbi_set_fsm_state(FSM_STATE_ACTIVATE_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = p_payload[0];
            break;

        case BT_CP_OPCODE_CID_STOP_BLE_ADV_CMD:
            printf("Received BT_CP_OPCODE_CID_STOP_BLE_ADV_CMD\n" );
            break;

        case BT_CP_OPCODE_CID_PROGRAM_NEW_REMOTE_STATUS:
            akbi_set_fsm_state(FSM_STATE_PROGRAM_NEW_REMOTE_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            break;

        case BT_CP_OPCODE_CID_TEST_CURRENT_REMOTE_STATUS:
            akbi_set_fsm_state(FSM_STATE_TEST_REMOTE_COMPLETE);
            ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
            break;

        /*case BT_CP_OPCODE_CID_CCU_ACTIVATE:
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_ACTIVATE_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ;
                  return;
              }
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_ACTIVATE_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                  return;
              }
              if (p_protocol_hdr->type == 0) {
                  akbi_set_fsm_state(FSM_STATE_ACTIVATE_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                  return;
              }
              break;
        */

        /*case BT_CP_OPCODE_CID_UPLOAD_TRIP_INFO:
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ;
                  return;
              }
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                  return;
              }
              if (p_protocol_hdr->type == 0) {
                  akbi_set_fsm_state(FSM_STATE_TRIP_INFO_UPLOAD_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
                  return;
              }
              break;
        */
        /*case BT_CP_OPCODE_CID_DISCONNECT_FROM_WIFI:
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_WIFI_DISCONNECT_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ;
                  return;
              }
              if (p_protocol_hdr->type == ) {
                  akbi_set_fsm_state(FSM_STATE_WIFI_DISCONNECT_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = ERROR_;
                  return;
              }
              if (p_protocol_hdr->type == 0) {
                  akbi_set_fsm_state(FSM_STATE_WIFI_DISCONNECT_COMPLETE);
                  ep_return_message[BLE_RET_MSG_RC_OFFSET] = SUCCESS;
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
