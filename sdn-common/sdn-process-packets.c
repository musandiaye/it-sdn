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

#include "sdn-process-packets.h"
#include "sdn-unknown-route.h"
#ifdef SDN_ENABLED_NODE
#include "sdn-send.h"
#include "sdn-network.h"
#else //SDN_ENABLED_NODE
#include "stdlib.h"
#include "sdn-process-packets-controller.h"
#include "sdn-serial-send.h"
#include "flow-table-cache.h"
#endif //SDN_ENABLED_NODE
#include "sdn-send-packet.h"
#include "sdn-queue.h"
#include "sdn-packetbuf.h"
#include "string.h"

#ifdef DEMO
#include "leds.h"
#endif

extern void (* sdn_callback_receive)(uint8_t*, uint16_t, sdnaddr_t*, uint16_t);
extern uint8_t sdn_state;

static uint8_t *packet_ptr;
static uint16_t packet_len;

static void treat_control_flow_setup();
static void treat_data_flow_setup();
static void treat_src_routed_control_flow_setup();
static void treat_src_routed_data_flow_setup();
static void treat_mult_data_flow_setup();
static void treat_mult_control_flow_setup();
static void treat_data();

#if defined(SDN_ENABLED_NODE) && 0
static void test_control_flow_request();
static void test_data_flow_request();
static void test_neighbor_report();
#endif //SDN_ENABLED_NODE

static void sdn_execute_action_receive() {
  print_packet(packet_ptr, packet_len);

  switch (SDN_HEADER(packet_ptr)->type) {
    case SDN_PACKET_CONTROL_FLOW_SETUP:
      treat_control_flow_setup();
    break;

    case SDN_PACKET_DATA_FLOW_SETUP:
      treat_data_flow_setup();
    break;

    case SDN_PACKET_DATA:
      treat_data();
    break;

    case SDN_PACKET_CONTROL_FLOW_REQUEST:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_CONTROL_FLOW_REQUEST not expected.\n");
      // test_control_flow_request();
#else
      process_control_flow_request((sdn_control_flow_request_t *) packet_ptr);
#endif
    break;

    case SDN_PACKET_DATA_FLOW_REQUEST:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_DATA_FLOW_REQUEST not expected.\n");
      // test_data_flow_request();
#else
      process_data_flow_request_packet((sdn_data_flow_request_t *) packet_ptr);
#endif
    break;

    case SDN_PACKET_NEIGHBOR_REPORT:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_NEIGHBOR_REPORT not expected.\n");
      // test_neighbor_report();
#else
      process_neighbor_report((sdn_neighbor_report_t*) packet_ptr,(void*) (packet_ptr + sizeof(sdn_neighbor_report_t)));
#endif
    break;

    case SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP:
      treat_src_routed_control_flow_setup();
    break;

    case SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP:
      treat_src_routed_data_flow_setup();
    break;

    case SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP:
      treat_mult_control_flow_setup();
    break;

    case SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP:
      treat_mult_data_flow_setup();
    break;

    case SDN_PACKET_ND:
#ifdef SDN_ENABLED_NODE
      SDN_ND.input(packet_ptr, packet_len);
#endif
    break;

    case SDN_PACKET_CD:
#ifdef SDN_ENABLED_NODE
      SDN_CD.input(packet_ptr, packet_len);
#endif
    break;

    case SDN_PACKET_ACK:
      SDN_DEBUG("SDN_PACKET_ACK not expected.\n");
    break;

    case SDN_PACKET_REGISTER_FLOWID:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_REGISTER_FLOWID not expected.\n");
#else
      flow_id_list_table_add(SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), (unsigned char*)&SDN_HEADER(packet_ptr)->source);
#endif
    break;

    default:
      SDN_DEBUG("(sdn_execute_action_receive) Unknown packet type.\n");
    break;
  }

}

static void treat_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_CONTROL_FLOW_SETUP.\n");
  sdn_controlflow_insert( \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, route_destination), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_id) );
  sdn_controlflow_print();
  sdn_send_ack( \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, header).seq_no, \
    SDN_PACKET_CONTROL_FLOW_SETUP \
  );
}

static void treat_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_DATA_FLOW_SETUP.\n");
  sdn_dataflow_insert( \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_id) );

  sdn_dataflow_print();
#ifdef SDN_ENABLED_NODE
  if (sdn_state == SDN_AWAITING_CONTROLLER_ROUTE && \
      flowid_cmp( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), \
        &sdn_controller_flow) == SDN_EQUAL) {
    sdn_state = SDN_STEADY_STATE;
    #ifdef DEMO
    printf("LED RED\n");
    leds_on(LEDS_RED);
    #endif
    sdn_send_neighbor_report();
  } else {
    #ifdef DEMO
    printf("LED GREEN\n");
    leds_on(LEDS_GREEN);
    #endif
  }
#endif

  sdn_send_ack(\
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, header).seq_no, \
    SDN_PACKET_DATA_FLOW_SETUP \
  );
}

static void treat_src_routed_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {
      treat_control_flow_setup();
    } else {
      SDN_DEBUG("Source route header not long enough... lets use flow table\n");
      SDN_HEADER(packet_ptr)->type = SDN_PACKET_CONTROL_FLOW_SETUP;
      sdnaddr_copy(
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, destination), \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, real_destination)
      );
      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, sizeof(sdn_control_flow_setup_t));
    }

  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_control_flow_setup_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);
  }
}

static void treat_src_routed_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {
      treat_data_flow_setup();
    } else {
      SDN_DEBUG("Source route header not long enough... lets use flow table\n");
      SDN_HEADER(packet_ptr)->type = SDN_PACKET_DATA_FLOW_SETUP;
      sdnaddr_copy(
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, destination), \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, real_destination)
      );
      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, sizeof(sdn_data_flow_setup_t));
    }
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_data_flow_setup_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);
  }
}

static void treat_mult_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, path_len) == 0 ) {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, set_len) --;

    sdn_dataflow_insert( \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid), \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_parameter, \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_id \
    );

    sdn_dataflow_print();
#ifdef SDN_ENABLED_NODE
    if (sdn_state == SDN_AWAITING_CONTROLLER_ROUTE && \
        flowid_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid), \
          &sdn_controller_flow) == SDN_EQUAL) {
      sdn_state = SDN_STEADY_STATE;
      #ifdef DEMO
        printf("LED RED\n");

      leds_on(LEDS_RED);
      #endif
    }
#endif

    // if there still are set flows in the packet, resend it
    if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, set_len)) {
      // next destination is the action_parameter from the current sdn_mult_data_flow_elem_t
      sdnaddr_copy( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, destination), \
        &SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_parameter \
      );
      packet_len -= sizeof(sdn_mult_flow_elem_t);

      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);
    }
  // still on source routed segment
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR_MULT(packet_ptr, sdn_mult_data_flow_setup_t)
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);

  }
}

static void treat_mult_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  uint8_t set_len;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, path_len) == 0 ) {
    set_len = SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, set_len) - 1;

    sdn_controlflow_insert( \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, route_destination), \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_parameter, \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_id \
    );
    sdn_controlflow_print();

    // if there still are set flows in the packet, resend it
    if (set_len) {
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, set_len) --;
      // next destination is the action_parameter from the current sdn_mult_data_flow_elem_t
      sdnaddr_copy( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, destination), \
        &SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_parameter \
      );
      packet_len -= sizeof(sdn_mult_flow_elem_t);

      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);
    }
  // still on source routed segment
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR_MULT(packet_ptr, sdn_mult_control_flow_setup_t)
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len);

  }
}

static void treat_data() {
#ifdef SDN_ENABLED_NODE
  SDN_DEBUG("Processing SDN_PACKET_DATA.\n");
  sdn_callback_receive( packet_ptr + sizeof(sdn_data_t), \
    packet_len - sizeof(sdn_data_t), \
    &SDN_HEADER(packet_ptr)->source, \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_t, flowid) );
#endif
}

#if 0
#ifdef SDN_ENABLED_NODE
static void test_control_flow_request() {
  // TESTING PURPOSES:
  sdnaddr_t temp_addr1 = {{1,0}};
  if (sdnaddr_cmp(&temp_addr1, &sdn_node_addr) == SDN_EQUAL) {
    uint8_t b;
    SDN_DEBUG("... but sending a control flow setup for testing...\n");

    sdnaddr_getbyte(&sdn_node_addr, 0, &b);
    sdnaddr_copy(&temp_addr1, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr1, 0, b + 1);

    sdn_controlflow_insert( \
      SDN_HEADER(packet_ptr)->source, \
      temp_addr1, \
      SDN_ACTION_FORWARD );

    sdnaddr_getbyte(&SDN_HEADER(packet_ptr)->source, 0, &b);
    sdnaddr_copy(&temp_addr1, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr1, 0, b + 1);

    sdn_send_control_flow_setup(&SDN_HEADER(packet_ptr)->source, \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_request_t, address), \
      &temp_addr1,\
      SDN_ACTION_FORWARD \
    );
  }
  // <END> TESTING PURPOSES
}

static void test_data_flow_request() {
  // TESTING PURPOSES:
  sdnaddr_t temp_addr = {{1,0}};
  if (sdnaddr_cmp(&temp_addr, &sdn_node_addr) == SDN_EQUAL) {
    uint8_t b;
    SDN_DEBUG("... but sending a flow setup for testing...\n");

    sdnaddr_getbyte(&sdn_node_addr, 0, &b);
    sdnaddr_copy(&temp_addr, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr, 0, b + 1);

    sdn_controlflow_insert( \
      SDN_HEADER(packet_ptr)->source, \
      temp_addr, \
      SDN_ACTION_FORWARD );

    sdnaddr_getbyte(&SDN_HEADER(packet_ptr)->source, 0, &b);
    sdnaddr_copy(&temp_addr, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr, 0, b - 1);

    sdn_send_data_flow_setup(&SDN_HEADER(packet_ptr)->source, \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_request_t, flowid), \
      &temp_addr,\
      SDN_ACTION_FORWARD \
    );
  }
  // <END> TESTING PURPOSES
}

static void test_neighbor_report() {
  // TESTING PURPOSES:
  uint8_t *nl_ptr;
  uint8_t i;
  SDN_DEBUG("... but printing packet for testing...\n");
  SDN_DEBUG("Source: ");
  sdnaddr_print(&SDN_HEADER(packet_ptr)->source);
  SDN_DEBUG("\nNumber of neighbors: %d\n", SDN_PACKET_GET_FIELD(packet_ptr, sdn_neighbor_report_t, num_of_neighbors));
  for (i = 0; i < SDN_PACKET_GET_FIELD(packet_ptr, sdn_neighbor_report_t, num_of_neighbors); i++) {
    nl_ptr = packet_ptr + sizeof(sdn_neighbor_report_t) + i*sizeof(sdn_neighbor_report_list_t);
    SDN_DEBUG("Neighbor [%d]: ", i);
    sdnaddr_print( &SDN_PACKET_GET_FIELD(nl_ptr, sdn_neighbor_report_list_t, neighbor) );
    SDN_DEBUG(" etx: %d\n", SDN_PACKET_GET_FIELD(nl_ptr, sdn_neighbor_report_list_t, etx));
  }
  // <END> TESTING PURPOSES
}
#endif //SDN_ENABLED_NODE
#endif //0

uint8_t check_packet_len() {
  uint8_t ret = SDN_SUCCESS;
  switch (SDN_HEADER(packet_ptr)->type) {
    case SDN_PACKET_CONTROL_FLOW_SETUP:
      if (packet_len < sizeof(sdn_control_flow_setup_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA_FLOW_SETUP:
      if (packet_len < sizeof(sdn_data_flow_setup_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA:
      if (packet_len < sizeof(sdn_data_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_CONTROL_FLOW_REQUEST:
      if (packet_len < sizeof(sdn_control_flow_request_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA_FLOW_REQUEST:
      if (packet_len < sizeof(sdn_data_flow_request_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_NEIGHBOR_REPORT:
      if (packet_len <
            sizeof(sdn_neighbor_report_t)
            + sizeof(sdn_neighbor_report_list_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_neighbor_report_t,
                                    num_of_neighbors)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_src_rtd_control_flow_setup_t)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_src_rtd_control_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_src_rtd_data_flow_setup_t)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_src_rtd_data_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_mult_control_flow_setup_t)
            + sizeof(sdn_mult_flow_elem_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_control_flow_setup_t,
                                    set_len)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_control_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_mult_data_flow_setup_t)
            + sizeof(sdn_mult_flow_elem_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_data_flow_setup_t,
                                    set_len)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_data_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_ND:
      if (packet_len < sizeof(sdn_header_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_CD:
      if (packet_len < sizeof(sdn_header_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_ACK:
      if (packet_len < sizeof(sdn_ack_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_REGISTER_FLOWID:
      if (packet_len < sizeof(sdn_data_flow_request_t))
        ret = SDN_ERROR;
    break;

    default:
      ret = SDN_ERROR;
    break;
  }
  return ret;
}

sdnaddr_t * sdn_treat_packet(uint8_t * packet, uint16_t len) {
  action_t action = SDN_ACTION_DROP;
  sdnaddr_t *next_hop = NULL;
  packet_ptr = packet;
  packet_len = len;
  // SDN_DEBUG ("sdn_treat_packet %x\n", ((sdn_header_t*) packet)->type);
//   sdn_dataflow_print();

  if (check_packet_len() != SDN_SUCCESS) {
    SDN_DEBUG_ERROR("check_packet_len failed\n");
    print_packet(packet_ptr, packet_len);
  /* Treat packets routed by flowid */
  } else if (SDN_ROUTED_BY_FLOWID(packet)) {
    struct data_flow_entry *dfe;
    dfe = sdn_dataflow_get(SDN_GET_PACKET_FLOW(packet));
    if (dfe != NULL) {
      action = dfe->action;
      next_hop = &dfe->next_hop;
      dfe->times_used++;
    } else {
      SDN_DEBUG ("dfe == NULL\n");
      sdn_unknown_flow(packet, len);
    }
  /* Treat packets routed by address */
  } else if (SDN_ROUTED_BY_ADDR(packet)) {
    struct control_flow_entry *cfe;
    cfe = sdn_controlflow_get(SDN_GET_PACKET_ADDR(packet));
    if (cfe != NULL) {
      action = cfe->action;
      next_hop = &cfe->next_hop;
      cfe->times_used++;
    } else {
      SDN_DEBUG ("cfe == NULL\n");
      sdn_unknown_dest(packet, len);
    }
  /* The packet is not routed by address neither flowid, but by the source */
  } else if (SDN_ROUTED_BY_SRC(packet)) {
    next_hop = &SDN_GET_PACKET_ADDR(packet);
    sdnaddr_print( &SDN_GET_PACKET_REAL_DEST(packet) );
    if (sdnaddr_cmp(next_hop, &sdn_node_addr) == SDN_EQUAL) {
      action = SDN_ACTION_RECEIVE;
    } else {
      action = SDN_ACTION_FORWARD;
    }
  } else if (SDN_ROUTED_NOT(packet)) {
    if (sdnaddr_cmp(&((sdn_header_t*)packet_ptr)->source, &sdn_node_addr) == SDN_EQUAL) {
      next_hop = sdn_addr_broadcast;
      action = SDN_ACTION_FORWARD;
    } else {
      action = SDN_ACTION_RECEIVE;
    }
  }else {
    SDN_DEBUG ("(sdn_treat_packet) Unknown packet type.\n");
  }

  // sdn_send_queue_data_t * queue_data;
  switch (action) {
    case SDN_ACTION_FORWARD:
      SDN_DEBUG("\nSDN_ACTION_FORWARD\n");
      /* do nothing, the outer function will enqueue and TX the packet */
    break;
    case SDN_ACTION_RECEIVE:
      SDN_DEBUG("\nSDN_ACTION_RECEIVE\n");
      if (! (SDN_ROUTED_BY_SRC(packet_ptr) && sdnaddr_cmp(&SDN_GET_PACKET_REAL_DEST(packet_ptr), &sdn_node_addr) != SDN_EQUAL) ) {
        SDN_METRIC_RX(packet_ptr);
      }
      next_hop = NULL;
      sdn_execute_action_receive();
    break;
    case SDN_ACTION_DROP:
      SDN_DEBUG("\nSDN_ACTION_DROP\n");
      next_hop = NULL;
    break;
  }

  return next_hop;
}
