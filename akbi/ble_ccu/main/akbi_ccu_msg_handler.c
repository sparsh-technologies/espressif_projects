
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"
#include "akbi_ccu_api.h"

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

    switch(p_protocol_hdr->opcode)
    {

    case BT_CP_OPCODE_CID_SCAN_WIFI_RESULT :

        /*
         * First check whether this is the first packet. If this is the first packet, then
         * cleanupp the datastructure here.
         */
        if (p_protocol_hdr->type == 1) {
            memset(&wifi_scan_report, 0x00, sizeof(AKBI_WIFI_SCAN_REPORT));
        }

        /*
         * Now check whether this is the last packet. If so, just mark the scanning as completed.
         */
        if (p_protocol_hdr->type == 0 ) {
            return ;
        }

        /*
         * Now, copy all the contents into the local datastructure.
         */
        index = p_protocol_hdr->type;
        p = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);

        strncpy(wifi_scan_report.ap_name[index-1], p, p_protocol_hdr->length);
        printf(" INFO : AP-%d : %s \n", index, wifi_scan_report.ap_name[index-1]);
        wifi_scan_report.ap_count++;
        break;

    default:
        break;

    }
    return;
}
