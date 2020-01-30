
#include <stdio.h>
#include <string.h>
#include "akbi_bt_msg.h"
#include "akbi_msg.h"


void set_data_to_mobile(char *ccu_msg,int length,char *return_pointer){

     char *p;

      BT_CP_PROTOCOL_HDR  *p_protocol_hdr;

      p_protocol_hdr = (BT_CP_PROTOCOL_HDR *)ccu_msg;

      printf(" Opcode      :  %02x\n",p_protocol_hdr->opcode );
      printf(" Trans ID    :  %02x\n", p_protocol_hdr->trans_id);
      printf(" Type        :  %02x\n", p_protocol_hdr->type);
      printf(" Length      :  %02x\n", p_protocol_hdr->length);

      p = ccu_msg + sizeof(BT_CP_PROTOCOL_HDR);

      memcpy(return_pointer+BLE_RET_MSG_SCANNED_SSID_COUNT_OFFSET, p , p_protocol_hdr->length);

}
