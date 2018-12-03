/*
* Header Here!
*/

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "string.h"
#include "powertrace.h"

#ifdef DEMO
#include "leds.h"
#endif

#include "sys/etimer.h"



/*---------------------------------------------------------------------------*/
PROCESS(sdn_test_process, "Contiki SDN example process");

unsigned seconds = 60*10;//measure battery every 1 minute
double fixed_perc_energy = 0.4; //  percentage of energy the node will start (4 for 2AA)\
                                  https://github.com/KineticBattery/Powertrace
unsigned variation = 2; // between 0 and 99

AUTOSTART_PROCESSES(&sdn_test_process);


/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  printf("Receiving message from ");
  sdnaddr_print(source_addr);
  printf(" of len %d: %s\n", len, (char*) data);

  /* Put your code here to get data received from/to application. */
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_test_process, ev, data)
{
  PROCESS_BEGIN();

  powertrace_start(CLOCK_SECOND * seconds, seconds, fixed_perc_energy, variation);
   //powertrace_start(CLOCK_SECOND * 10);
   //printf("Ticks per second: %u\n", RTIMER_SECOND);
   //powetrace_print();

  sdn_init(receiver);

  static flowid_t sink = 2017;

  static struct etimer periodic_timer;
  

  etimer_set(&periodic_timer, 60 * CLOCK_SECOND);
  // flow_tables_test();

  static char data[10];
  static uint8_t i = 0;

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    etimer_restart(&periodic_timer);

    printf("Timer reset\n");
    sprintf(data, "test %d", i++);
    // if (i==2)
    sdn_send((uint8_t*) data, 10, SDN_CONTROLLER_FLOW);
    
  // #ifdef DEMO
  //   leds_toogle(LEDS_BLUE);
  // #endif
    

    // if (sdn_node_addr.u8[0] == 1) {
    //   if (i%12 == 1)
    //     test_send_src_rt_csf();
//         test_send_src_rt_csf_incomplete();
//       if (i%12 == 3)
//         test_send_src_rt_dsf();
//         test_send_src_rt_dsf_incomplete();
//       if (i%12 == 7)
//         test_send_mult_dsf();
//       if (i%12 == 11)
//         test_send_mult_csf();
    // }
  }

  PROCESS_END();
}
