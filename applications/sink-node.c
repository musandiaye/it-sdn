/*
* Header Here!
*/

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "string.h"
#include "sdn-constants.h"
#include "powertrace.h"

#include "leds.h"

#include "sys/etimer.h"

/*---------------------------------------------------------------------------*/
PROCESS(sdn_sink_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_sink_process);
//

sdnaddr_t red_node;
sdnaddr_t green_node;
sdnaddr_t blue_node;

/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  printf("Receiving message from ");
  sdnaddr_print(source_addr);
  printf(" of len %d: %s\n", len, (char*) data);

  if(sdnaddr_cmp(&red_node, &sdnaddr_null) == SDN_EQUAL) {
    sdnaddr_copy(&red_node, source_addr);
    leds_toggle(LEDS_RED);
    return;
  } else {
    if(sdnaddr_cmp(&red_node, source_addr) == SDN_EQUAL) {
      leds_toggle(LEDS_RED);
      return;
    }
  } 

  if(sdnaddr_cmp(&green_node, &sdnaddr_null) == SDN_EQUAL) {
    sdnaddr_copy(&green_node, source_addr);
    leds_toggle(LEDS_GREEN);
    return;
  } else {
    if(sdnaddr_cmp(&green_node, source_addr) == SDN_EQUAL) {
      leds_toggle(LEDS_GREEN);
      return;
    }
  }

  if(sdnaddr_cmp(&blue_node, &sdnaddr_null) == SDN_EQUAL) {
    sdnaddr_copy(&blue_node, source_addr);
    leds_toggle(LEDS_BLUE);
    return;
  } else {
    if(sdnaddr_cmp(&blue_node, source_addr) == SDN_EQUAL) {
      leds_toggle(LEDS_BLUE);
      return;
    }
  }
  /* Put your code here to get data received from/to application. */
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_sink_process, ev, data)
{
  PROCESS_BEGIN();
  powertrace_start(CLOCK_SECOND * 10);
  printf("Ticks per second: %u\n", RTIMER_SECOND);

  sdn_init(receiver);

  flowid_t flowid = 2017;

  sdn_register_flowid(flowid);  

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
