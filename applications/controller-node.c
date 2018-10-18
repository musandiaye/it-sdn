/*
* Header Here!
*/

#include <stdio.h>
#include "contiki.h"
#include "string.h"
#include "sys/etimer.h"

#include "sdn-protocol.h"
#include "net/packetbuf.h"
#include "sdn-debug.h"
#include "sdn-network.h"
#include "sdn-addr.h"
#include "net/netstack.h"
#include "sdn-constants.h"
#include "sys/ctimer.h"
#include "sdn-serial.h"
#include "sdn-send.h"
#include "sdn-queue.h"
#include "sdn-packetbuf.h"
#include "sdn-send-packet.h"
#include "sdn-receive.h"
#include "powertrace.h"

void sdn_receive();

/*---------------------------------------------------------------------------*/
PROCESS(sdn_core_process, "SDN Controller process");
unsigned seconds = 60*5;
double fixed_perc_energy = 1; //  percentage of energy the node will start (2AA) 0-1\
                                  https://github.com/KineticBattery/Powertrace
unsigned variation = 2; // between 0 and 99
AUTOSTART_PROCESSES(&sdn_core_process);


void sdn_receive() {

  int index;
  sdn_serial_packet_t serial_pkt;
  uint8_t * packet_ptr = packetbuf_dataptr();
  uint8_t packet_len = packetbuf_datalen();

  SDN_DEBUG ("Packet Received from [%02X", packetbuf_addr(PACKETBUF_ADDR_SENDER)->u8[0]);

  for (index = 1; index < SDNADDR_SIZE; index++) {
    SDN_DEBUG (":%02X", packetbuf_addr(PACKETBUF_ADDR_SENDER)->u8[index]);
  }

  SDN_DEBUG ("] \n");

  print_packet((uint8_t*)packetbuf_dataptr(), packetbuf_datalen());

  if (sdn_seqno_is_duplicate(SDN_HEADER(packetbuf_dataptr())) == SDN_YES) {
    SDN_DEBUG("Packet is duplicated, dropping.\n");
    return;
  }
  sdn_seqno_register(SDN_HEADER(packetbuf_dataptr()));

  if (SDN_HEADER(packet_ptr)->thl == 0) {
    SDN_DEBUG ("THL expired.\n");
    return;
  }
  SDN_HEADER(packet_ptr)->thl --;

#ifdef SDN_METRIC
  // assuming the controller receives packets only to SDN_CONTROLLER_FLOW or its own address
  // source routed packets are not expected
  if ( (SDN_ROUTED_BY_ADDR(packet_ptr) && sdnaddr_cmp(&sdn_node_addr, &SDN_GET_PACKET_ADDR(packet_ptr)) == SDN_EQUAL) ||
       (SDN_ROUTED_BY_FLOWID(packet_ptr) && flowid_cmp(&sdn_controller_flow, &SDN_GET_PACKET_FLOW(packet_ptr))== SDN_EQUAL) ) {
    SDN_METRIC_RX(packet_ptr);
  }
#endif // SDN_METRIC

  switch (SDN_HEADER(packet_ptr)->type) {
    case SDN_PACKET_ND:
      SDN_ND.input(packet_ptr, packet_len);
    break;

    case SDN_PACKET_CD:
      SDN_CD.input(packet_ptr, packet_len);
    break;

    default:
      memcpy(serial_pkt.payload, packet_ptr, packet_len);
      // Setting serial packet address as the MAC sender.
      sdnaddr_copy(&serial_pkt.header.node_addr, (sdnaddr_t*)packetbuf_addr(PACKETBUF_ADDR_SENDER));
      serial_pkt.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO;
      serial_pkt.header.payload_len = packet_len;

      sdn_serial_send(&serial_pkt);
    break;

  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_core_process, ev, data)
{
  PROCESS_BEGIN();
  powertrace_start(CLOCK_SECOND * seconds, seconds, fixed_perc_energy, variation);
  //powertrace_start(CLOCK_SECOND * 10);
  printf("Ticks per second: %u\n", RTIMER_SECOND);
  //powertrace_print();

  SDN_CD.init(SDN_YES);
  SDN_ND.init(SDN_YES);

  memcpy(&sdn_node_addr, &linkaddr_node_addr, sizeof(sdnaddr_t));
  sdn_serial_init(&putchar);
  sdn_send_init();


  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == SDN_EVENT_ND_CHANGE) {
      /* send neighbor report through serial connection */
      SDN_DEBUG ("Neighbor Information!\n");
      sdn_send_neighbor_report();
    }

    if (ev == sdn_raw_binary_packet_ev) {
      /* Send packet->payload to Radio Stack */
      sdn_serial_packet_t *packet = (sdn_serial_packet_t*)data;
      struct sdn_packetbuf * pkt_to_enqueue_ptr;
#ifdef SDN_METRIC
      if (sdnaddr_cmp(&sdn_node_addr, &SDN_HEADER(packet->payload)->source) == SDN_EQUAL) {
        SDN_METRIC_TX(packet->payload);
      }
#endif //SDN_METRIC
      pkt_to_enqueue_ptr = sdn_packetbuf_pool_get();
      if (pkt_to_enqueue_ptr == NULL) {
        SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
      } else {
        memcpy(pkt_to_enqueue_ptr, packet->payload, packet->header.payload_len);
        if (sdn_send_queue_enqueue((uint8_t *) pkt_to_enqueue_ptr, packet->header.payload_len) != SDN_SUCCESS) {
          sdn_packetbuf_pool_put((struct sdn_packetbuf *)pkt_to_enqueue_ptr);
          SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
        } else {
          sdnaddr_copy(&sdn_send_queue_tail()->last_next_hop, &packet->header.node_addr);
          sdn_send_down_once();
        }
      }
    }

  }

  PROCESS_END();
}
