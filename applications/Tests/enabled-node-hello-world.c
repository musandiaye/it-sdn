/*
* Header Here!
*/

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "string.h"

#ifdef DEMO
#include "leds.h"
#endif

#include "sys/etimer.h"

/*---------------------------------------------------------------------------*/
PROCESS(sdn_test_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_test_process);

/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {  /*function receive() already has one printf() statement to print incoming messages*/

  printf("Receiving message from ");
  sdnaddr_print(source_addr);
  printf(" of len %d: %s\n", len, (char*) data);

  /* Put your code here to get data received from/to application. */
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_test_process, ev, data) {
  PROCESS_BEGIN();

  sdn_init(receiver);
  static struct etimer periodic_timer;
  etimer_set (&periodic_timer, 30 * CLOCK_SECOND);
  
  if (sdn_node_addr.u8[0]==2){   /*configures the flow id 2017 to the node with the last byte 2*/
    sdn_register_flowid{2017};   /*the node with final address 2 is ready to receive messages from flow id 2017*/
    }
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_restart(&periodic_timer);
  if (sdn_node_addr.u8[0]== 3) {
  char *data = "Hello_World!";
  uint16_t data_len = 13;
  sdn_send((uint8_t*) &data, data_len, 2017);
   }    	
 
  }

  PROCESS_END();
}
/*This function is equivalent to int main() in C. */
/* sdn_init(receiver) is necessary to inform IT-SDN to retrive a callback function to process incoming messages to the node.*/

