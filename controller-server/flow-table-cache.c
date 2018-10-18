/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE SOFTWARE PROVIDER OR DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "flow-table-cache.h"

flow_id_table_ptr flow_id_table_head = NULL;
flow_id_table_ptr flow_id_table_tail = NULL;

flow_address_table_ptr flow_address_table_head = NULL;
flow_address_table_ptr flow_address_table_tail = NULL;

flow_id_list_table_ptr flow_id_list_table_head = NULL;
flow_id_list_table_ptr flow_id_list_table_tail = NULL;


flow_id_table_ptr flow_id_table_new(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action) {

  flow_id_table_ptr flow_id_table = malloc(sizeof *flow_id_table);

  memcpy(flow_id_table->source, source, SDNADDR_SIZE);
  flow_id_table->flowid = flowid;
  memcpy(flow_id_table->next_hop, next_hop, SDNADDR_SIZE);
  flow_id_table->action = action;

  flow_id_table->next = NULL;
  flow_id_table->flag = ROUTE_NEW;
  flow_id_table->changed = 1;
  // flow_id_table->recalc = 0;

  return flow_id_table;
}

flow_id_table_ptr flow_id_table_add(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action) {

  flow_id_table_ptr flow_id_table;

  // if(flowid == SDN_NULL_FLOW) {

  flow_id_table = flow_id_table_find(source, flowid);
  // } else {

  //   flow_id_table = flow_table_find_flow(source, flowid, sdnaddr_null);
  // }

  if(flow_id_table == NULL) {

    flow_id_table = flow_id_table_new(source, flowid, next_hop, action);

    if(flow_id_table_head == NULL) {

      flow_id_table_head = flow_id_table;
      flow_id_table_tail = flow_id_table;
      printf("Flow ID Table (new register) (Head):");
    } else {
      flow_id_table_tail->next = flow_id_table;
      flow_id_table_tail = flow_id_table;
      printf("Flow ID Table (new register):");
    }
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" flowid %d ", flowid);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf(" action %d ", action);
      printf("\n");

  } else {

    if(memcmp(next_hop, flow_id_table->next_hop, SDNADDR_SIZE) != 0 || memcmp(&action, &flow_id_table->action, sizeof(action_t)) != 0 ) {

      memcpy(flow_id_table->next_hop, next_hop, SDNADDR_SIZE);

      flow_id_table->action = action;

      flow_id_table->flag = ROUTE_UPDATED;

      flow_id_table->changed = 1;
      // flow_id_table->recalc = 0;

      printf("Flow ID route was updated: source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" -> Flow id %d next_hop ", flowid);
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf("\n");
    } else {

      // flow_id_table->changed = 0;
      // flow_id_table->recalc = 0;

      // printf("Flow ID route has not changed: source ");
      // sdnaddr_print((sdnaddr_t *)source);
      // printf(" -> Flow id %d.\n", flowid);
    }

  }

  return flow_id_table;
}

flow_id_table_ptr flow_id_table_find(unsigned char* source, uint16_t flowid) {

  flow_id_table_ptr flow_id_table = flow_id_table_head;

  while(flow_id_table != NULL) {

    if(memcmp(flow_id_table->source, source, SDNADDR_SIZE) == 0 && flow_id_table->flowid == flowid) {

      return flow_id_table;
    }

    flow_id_table = flow_id_table->next;

  }

  return flow_id_table;
}

flow_id_table_ptr flow_id_table_get() {

  return flow_id_table_head;
}

flow_address_table_ptr flow_address_table_new(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action) {

  flow_address_table_ptr flow_address_table = malloc(sizeof *flow_address_table);

  memcpy(flow_address_table->source, source, SDNADDR_SIZE);
  memcpy(flow_address_table->target, target, SDNADDR_SIZE);
  memcpy(flow_address_table->next_hop, next_hop, SDNADDR_SIZE);
  flow_address_table->action = action;

  flow_address_table->next = NULL;
  flow_address_table->flag = ROUTE_NEW;
  flow_address_table->changed = 1;
  // flow_address_table->recalc = 0;

  return flow_address_table;
}

flow_address_table_ptr flow_address_table_add(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action) {

  flow_address_table_ptr flow_address_table;

  flow_address_table = flow_address_table_find(source, target);

  if(flow_address_table == NULL) {

    flow_address_table = flow_address_table_new(source, target, next_hop, action);

    if(flow_address_table_head == NULL) {
      flow_address_table_head = flow_address_table;
      flow_address_table_tail = flow_address_table;
      printf("Flow Address Table (new register) (Head):");
    } else {
      flow_address_table_tail->next = flow_address_table;
      flow_address_table_tail = flow_address_table;
      printf("Flow Address Table (new register):");
    }
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" target ");
      sdnaddr_print((sdnaddr_t *)target);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf(" action %d ", action);
      printf("\n");

  } else {

    if(memcmp(next_hop, flow_address_table->next_hop, SDNADDR_SIZE) != 0 || memcmp(&action, &flow_address_table->action, sizeof(action_t)) != 0 ) {

      memcpy(flow_address_table->next_hop, next_hop, SDNADDR_SIZE);

      flow_address_table->action = action;

      flow_address_table->flag = ROUTE_UPDATED;

      flow_address_table->changed = 1;
      // flow_address_table->recalc = 0;

      printf("Flow Address route was updated:");
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" -> target ");
      sdnaddr_print((sdnaddr_t *)target);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf("\n");
    } else {

      // flow_address_table->changed = 0;
      // flow_address_table->recalc = 0;

      // printf("Flow Address route has not changed:");
      // printf(" source ");
      // sdnaddr_print((sdnaddr_t *)source);
      // printf(" -> target ");
      // sdnaddr_print((sdnaddr_t *)target);
      // printf("\n");

    }
  }

  return flow_address_table;
}

flow_address_table_ptr flow_address_table_find(unsigned char* source, unsigned char* target) {

  flow_address_table_ptr flow_address_table = flow_address_table_head;

  while(flow_address_table != NULL) {

    if(memcmp(flow_address_table->source, source, SDNADDR_SIZE) == 0 && memcmp(flow_address_table->target, target, SDNADDR_SIZE) == 0) {

      return flow_address_table;
    }

    flow_address_table = flow_address_table->next;

  }

  return flow_address_table;
}

flow_address_table_ptr flow_address_table_get() {

  return flow_address_table_head;
}
//##############################
// flow_table_field_ptr flow_table_find_flow(unsigned char* source, uint16_t flowid) {

//   flow_table_field_ptr flow_table_field_find = flow_table_head;

//   if(flow_table_field_find != NULL) {

//     do{

//       if(memcmp(flow_table_field_find->source, source, SDNADDR_SIZE) == 0 && flow_table_field_find->flowid == flowid) {

//         return flow_table_field_find;
//       }

//       flow_table_field_find = flow_table_field_find->next;

//     } while(flow_table_field_find != NULL);
//   }

//   return flow_table_field_find;

// }

// void flow_table_mark_recalc_from(unsigned char* source) {

//   flow_table_field_ptr flow_table_field_find = flow_table_head;

//   if(flow_table_field_find != NULL) {

//     do{

//       if(memcmp(flow_table_field_find->source, source, SDNADDR_SIZE) == 0) {

//         flow_table_field_find->recalc = 1;
//         printf("flow_table_mark_recalc_from CHANGED\n");
//       }

//       flow_table_field_find = flow_table_field_find->next;

//     } while(flow_table_field_find != NULL);
//   }

// }

// void flow_table_mark_recalc_to(unsigned char* target) {

//   flow_table_field_ptr flow_table_field_find = flow_table_head;

//   if(flow_table_field_find != NULL) {

//     do{

//       if(memcmp(flow_table_field_find->target, target, SDNADDR_SIZE) == 0) {

//         flow_table_field_find->recalc = 1;
//         printf("flow_table_mark_recalc_to CHANGED\n");
//       }

//       flow_table_field_find = flow_table_field_find->next;

//     } while(flow_table_field_find != NULL);
//   }

// }

// void flow_table_mark_recalc_next_hop(unsigned char* next_hop) {

//   flow_table_field_ptr flow_table_field_find = flow_table_head;

//   if(flow_table_field_find != NULL) {

//     do{

//       if(memcmp(flow_table_field_find->next_hop, next_hop, SDNADDR_SIZE) == 0) {

//         flow_table_field_find->recalc = 1;
//         printf("flow_table_mark_recalc_next_hop CHANGED\n");
//       }

//       flow_table_field_find = flow_table_field_find->next;

//     } while(flow_table_field_find != NULL);
//   }

// }

// void flow_table_mark_recalc_from_to(unsigned char* source, unsigned char* target) {

//   flow_table_field_ptr flow_table_field_find = flow_table_find(source, target);

//   if(flow_table_field_find != NULL) {
//     flow_table_field_find->recalc = 1;
//     printf("flow_table_mark_recalc_from_to CHANGED\n");
//   }
// }

// void flow_table_mark_recalc_all() {

//   printf("flow_table_table_mark_changes_all CHANGED\n");

//   flow_table_field_ptr flow_table_field_find = flow_table_head;

//   if(flow_table_field_find != NULL) {

//     do{

//       flow_table_field_find->recalc = 1;

//       flow_table_field_find = flow_table_field_find->next;

//     } while(flow_table_field_find != NULL);
//   }
// }

flow_id_list_table_ptr flow_id_list_table_new(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = malloc(sizeof *flow_id_list);

  flow_id_list->flowid = flowid;

  memcpy(flow_id_list->target, target, SDNADDR_SIZE);

  flow_id_list->next = NULL;

  return flow_id_list;

}

flow_id_list_table_ptr flow_id_list_table_add(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = flow_id_list_table_find(flowid, target);

  if(flow_id_list == NULL) {

    flow_id_list = flow_id_list_table_new(flowid, target);

    if(flow_id_list_table_head == NULL) {

      flow_id_list_table_head = flow_id_list;
      flow_id_list_table_tail = flow_id_list;
      printf("Flow ID List Table (new register) (Head):");
    } else {

      flow_id_list_table_tail->next = flow_id_list;
      flow_id_list_table_tail = flow_id_list;
      printf("Flow ID List Table (new register):");
    }

    printf(" FlowID %d", flowid);
    printf(" target ");
    sdnaddr_print((sdnaddr_t *)target);
    printf("\n");
  } else {

    printf("Flow ID List Table field was not added because Its already exists.\n");
  }

  return flow_id_list;
}

flow_id_list_table_ptr flow_id_list_table_find(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = NULL;

  flow_id_list_table_ptr flow_id_list_search = flow_id_list_table_head;

  while(flow_id_list_search != NULL && flow_id_list == NULL) {

    if(flow_id_list_search->flowid == flowid && memcmp(flow_id_list_search->target, target, SDNADDR_SIZE) == 0) {

      flow_id_list = flow_id_list_search;
    }

    flow_id_list_search = flow_id_list_search->next;

  }

  return flow_id_list;
}

flow_id_list_table_ptr flow_id_list_table_get() {

  return flow_id_list_table_head;
}

uint8_t flow_id_list_table_has_target(unsigned char* target) {

  uint8_t ret = 0;

  flow_id_list_table_ptr flow_id_list_search = flow_id_list_table_head;

  while(flow_id_list_search != NULL && ret == 0) {

    if(memcmp(flow_id_list_search->target, target, SDNADDR_SIZE) == 0) {

      ret = 1;
    }

    flow_id_list_search = flow_id_list_search->next;

  }

  return ret;
}
