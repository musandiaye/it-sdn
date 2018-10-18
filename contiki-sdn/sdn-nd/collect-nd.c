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

#include "contiki.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
//#include "net/rime/collect.h"
//#include "net/rime/collect-neighbor.h"
#include "collect-nd.h"
#include <string.h>
#include <stdio.h>

/*----------------------------------------------*/
extern struct process sdn_core_process;
/*----------------------------------------------*/
/**
 * tc: it is a structure that needs collect to open the communcation
 * neighbors_copy_list: it creates a list to copy the information from
 * the list of neighbors of collect
 */
struct collect_conn tc;
struct collect_neighbor_list neighbors_copy_list;

PROCESS(neighbors_discovery, "Neighbors discovery");

/*---------------------------------------------------------------------------*/
/**
 *callback: requirement of collect
 */
static const struct collect_callbacks callbacks;
/*---------------------------------------------------------------------------*/
void neighbors_init(uint8_t is_controller) {
  printf("Initializing neighbor discovery\n");
  sdn_neighbor_table_init();
  /*opens the collect protocol communication*/
  collect_sdn_open(&tc, 130, COLLECT_ROUTER, &callbacks);

  /*set the sink: requirement of collect protocol*/
  if(is_controller == SDN_YES) {
    printf("I am sink\n");
    collect_sdn_set_sink(&tc, 1);
  }

  process_start(&neighbors_discovery, NULL);
}
/*---------------------------------------------------------------------------*/
void neighbors_close() {

}
/*---------------------------------------------------------------------------*/
void print_neighbors(struct collect_neighbor_list *neighbor_list) {

  struct collect_neighbor *n;
  for(n = list_head(neighbor_list->list); n != NULL; n = list_item_next(n)) {
    printf("Table 1 Neighbor number %d.%d \n", n->addr.u8[0], n->addr.u8[1]);
  }
}
/*----------------------------------------------------------------------------*/

void sdn_neighbors_table_act(struct collect_neighbor_list *neighbor_list) {

  struct collect_neighbor *d;

  for(d = list_head(neighbor_list->list); d != NULL; d = list_item_next(d)) {
    sdn_neighbor_table_insert(d->addr, d->rtmetric);
  }
}
/*----------------------------------------------------------------------------*/

struct collect_conn *collect_pointer() {

  return &tc;
}

/*----------------------------------------------------------------------------*/

PROCESS_THREAD(neighbors_discovery, ev, data){

  PROCESS_BEGIN();

  static struct etimer first_table_copy;

  /*the programm waits 2 minutes while the network is settled (this time could change)*/
  etimer_set(&first_table_copy, CLOCK_SECOND * 30);

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&first_table_copy));

  /*do a copy of the collect's neigbors list*/
  neighbors_copy_list.list = collect_neighbor_list(&tc.neighbor_list);
  /*Update the sdn neigbors list*/
  sdn_neighbors_table_act(&neighbors_copy_list);

  sdn_neighbor_table_print();

  process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);

  while(1){

    /*Waits for a change in the list and actualize the sdn list*/
    PROCESS_WAIT_EVENT_UNTIL(ev == NEIGHBOR_CHANGE);

    sdn_neighbors_table_act(&neighbors_copy_list);
    sdn_neighbor_table_print();
    process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);

  }

  PROCESS_END();
}

void nd_input(uint8_t* p, uint16_t l) {

}

const struct sdn_nd collect_sdn_nd = {
  neighbors_init,
  nd_input
};

/*--------------------------------------------------------------------------------*/
