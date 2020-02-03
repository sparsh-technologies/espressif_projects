
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "akbi_ccu_api.h"
#include "akbi_fsm.h"

AKBI_WIFI_SCAN_REPORT  wifi_scan_report;

void akbi_process_rx_serial_data(char *ccu_msg,int length,char *return_pointer)
{
    int    index;
    char   *p;

    BT_CP_PROTOCOL_HDR  *p_protocol_hdr;

    p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)ccu_msg;

    printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
    printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
    printf(" Type        :  %02x\n", p_protocol_hdr->type);
    printf(" Length      :  %02x\n", p_protocol_hdr->length);

    p = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);

    printf("\ndata :" );
    for (int i = 0; i < p_protocol_hdr->length; i++) {
        printf("%02x ", p[i]);
    }
    printf("\n" );


    switch(p_protocol_hdr->opcode)
    {

    case BT_CP_OPCODE_CID_SCAN_WIFI_RESULT :// 70

        /*
         * First check whether this is the first packet. If this is the first packet, then
         * cleanupp the datastructure here.
         */
        if (p_protocol_hdr->type == 1) {
            memset(&wifi_scan_report, 0x00, sizeof(AKBI_WIFI_SCAN_REPORT));
            //memset(&return_pointer[BLE_RET_MSG_RC_OFFSET],WIFI_SCANNING_IN_PROGRESS,BLE_RETURN_RC_SIZE);
            printf("first ssid\n");

        }

        /*
         * Now check whether this is the last packet. If so, just mark the scanning as completed.
         */
        else if (p_protocol_hdr->type == 0 ) {
            akbi_set_fsm_state(FSM_STATE_WIFI_SCAN_COMPLETE);
            memset(&return_pointer[BLE_RET_MSG_RC_OFFSET],SUCCESS,BLE_RETURN_RC_SIZE);
            memcpy(&return_pointer[BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET],&wifi_scan_report.ap_count,SCANNED_WIFI_COUNT_SIZE);
            printf("STATE set : FSM_STATE_WIFI_SCAN_COMPLETE ,count = %d\n",wifi_scan_report.ap_count);
            for (int i = 0; i < wifi_scan_report.ap_count; i++) {
                printf(" ssid>%s\n", wifi_scan_report.ap_name[i]);
            }
            return ;
        }
        else{
            memset(&return_pointer[BLE_RET_MSG_RC_OFFSET],WIFI_SCANNING_IN_PROGRESS,BLE_RETURN_RC_SIZE);
            printf(" %d th ssid\n",p_protocol_hdr->type );
        }

        /*
         * Now, copy all the contents into the local datastructure.
         */
        index = p_protocol_hdr->type;
        p = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);

        strncpy(wifi_scan_report.ap_name[index-1], p, p_protocol_hdr->length);
        save_ssids(wifi_scan_report.ap_name[index-1],index-1,p_protocol_hdr->length);
        printf(" INFO : AP-%d : %s \n", index, wifi_scan_report.ap_name[index-1]);
        wifi_scan_report.ap_count++;
        break;

    default:
        break;

    }
    return;
}
