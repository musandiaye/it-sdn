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

#include <stdlib.h>
#include "sdn-debug.h"

#include "digraph.h"

#include "flow-table-cache.h"

#include "control-flow-table.h"
#include "data-flow-table.h"
#include "sdn-queue.h"

#include "sdn-send-packet.h"
#include "sdn-serial-send.h"
#include "sdn-process-packets-controller.h"
#include "sdn-packetbuf.h"

void process_control_flow_request(sdn_control_flow_request_t *sdn_control_flow_request);

void process_flow_address_table_changes();

void process_flow_id_table_changes();

void process_data_flow_setup(route_ptr route, uint16_t flowid, flow_id_table_ptr routing_field);

void process_control_flow_setup(route_ptr route, uint8_t *target_request, flow_address_table_ptr routing_field);

void send_src_rtd_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action);

void send_src_rtd_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action);

void enqueue_sdn_data_flow_request_to_controller(uint8_t *source);

void enqueue_sdn_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action);

void enqueue_sdn_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action);

void process_neighbor_report(sdn_neighbor_report_t* sdn_neighbor_report, void* neighbors) {

  vertice_ptr vertice_source = digraph_find_vertice(sdn_neighbor_report->header.source.u8);

  if(vertice_source == NULL) {

    vertice_source = digraph_add_vertice(sdn_neighbor_report->header.source.u8);
    enqueue_sdn_data_flow_request_to_controller(sdn_neighbor_report->header.source.u8);
  }

  printf("Source ");
  sdnaddr_print(&(sdn_neighbor_report->header.source));
  printf(" neighbors (%d)\n", sdn_neighbor_report->num_of_neighbors);

  vertice_ptr vertice_dest;

  //Marks edges to remove before updates.
  digraph_mark_edges_to_del_from(vertice_source);

  edge_ptr edge;

  int digraph_changed = 0;
  int index_neighbor;

  for(index_neighbor = 0; index_neighbor < sdn_neighbor_report->num_of_neighbors; index_neighbor++) {

    sdn_neighbor_report_list_t* neighbor_list = (sdn_neighbor_report_list_t*) (neighbors + (sizeof(sdn_neighbor_report_list_t) * index_neighbor));

    vertice_dest = digraph_find_vertice(neighbor_list->neighbor.u8);

    if(vertice_dest == NULL) {

      vertice_dest = digraph_add_vertice(neighbor_list->neighbor.u8);
      enqueue_sdn_data_flow_request_to_controller(neighbor_list->neighbor.u8);
    }

    edge = digraph_add_edge(vertice_source, vertice_dest, neighbor_list->etx);

    if(edge->status != EDGE_STATIC) {
      digraph_changed = 1;
    }

    edge = digraph_add_edge(vertice_dest, vertice_source, neighbor_list->etx);

    if(edge->status != EDGE_STATIC) {
      digraph_changed = 1;
    }
  }

  //Removes edges not informed.
  if(digraph_del_marked_edges_to_del_from(vertice_source) == 1) {
    digraph_changed = 1;
  }

  printf("Ending routine of neighbor information processing.\n");

  if(digraph_changed == 1) {
    update_flow_table_cache();
   } else {
    sdn_send_down_once();
   }
}

void process_data_flow_request_packet(sdn_data_flow_request_t *sdn_data_flow_request) {

  // printf("FUNCTION process_data_flow_request.\n");

  flow_id_table_ptr routing_field;

  route_ptr route = NULL;

  routing_field = flow_id_table_find(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid);

  if(routing_field != NULL) {

      process_data_flow_setup(NULL, sdn_data_flow_request->flowid, routing_field);
  } else {

    route = get_flow_id_better_route(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid);

    // If there are no routes to flow id:
    if(route == NULL) {

      printf("Route to flowid %d was not found.\n", sdn_data_flow_request->flowid);

      routing_field = flow_id_table_add(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid, sdn_data_flow_request->header.source.u8, SDN_ACTION_DROP);

      if(routing_field->changed == 1) {

        process_data_flow_setup(NULL, sdn_data_flow_request->flowid, routing_field);
        // enqueue_sdn_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
        routing_field->changed = 0;
      }

    // If there is at least one route to flow id:
    } else {
      printf("Route was found to flowId %d.\n", sdn_data_flow_request->flowid);

      process_data_flow_setup(route, sdn_data_flow_request->flowid, NULL);
      // route_ptr route_head = route;

      // while (route != NULL) {
      //   digraph_print_vertice_id(route->vertice_id);
      //   printf("(%d) <- ", route->route_weight);

      //   route = route->next;
      // }

      // printf("\n");

      // route = route_head;

      // while (route != NULL) {

      //   if(route->next != NULL) {
      //     //Using Source Route packet to set data flow.

      //     routing_field = flow_id_table_add(route->next->vertice_id, sdn_data_flow_request->flowid, route->vertice_id, SDN_ACTION_FORWARD);

      //     if(routing_field->changed == 1) {
      //       send_src_rtd_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
      //       // enqueue_sdn_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
      //       routing_field->changed = 0;
      //     }
      //   }

      //   route = route->next;
      // }

      // route = NULL;

      // dijkstra_free_route(route_head);
    }
  }

  sdn_send_down_once();
}

void process_control_flow_request(sdn_control_flow_request_t *sdn_control_flow_request) {

  printf("Control Flow Request: ");
  sdnaddr_print(&(sdn_control_flow_request->header.source));
  printf(" -> ");
  sdnaddr_print(&(sdn_control_flow_request->address));
  printf("\n");

  flow_address_table_ptr routing_field;

  route_ptr route = NULL;

  route = get_better_route(sdn_control_flow_request->header.source.u8, sdn_control_flow_request->address.u8, route);

  if(route == NULL) {
    printf("Route not found.\n");

    routing_field = flow_address_table_add(sdn_control_flow_request->header.source.u8, sdn_control_flow_request->address.u8, sdn_control_flow_request->header.source.u8, SDN_ACTION_DROP);

    if(routing_field->changed == 1) {
      process_control_flow_setup(NULL, NULL, routing_field);
      // sdn_send_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
      routing_field->changed = 0;
    }

  } else {

    printf("Route was found.\n");

    process_control_flow_setup(route, sdn_control_flow_request->address.u8, NULL);
  }
}

route_ptr get_flow_id_better_route(uint8_t *source, uint16_t flowid) {

  route_ptr route_better = NULL;

  flow_id_list_table_ptr flowid_search = flow_id_list_table_get();

  // Getting all possible targets from specific flow id to calculate better route.
  while(flowid_search != NULL) {

    if(flowid_search->flowid == flowid) {
      // printf("Found FlowId %d on address ", flowid);
      // sdnaddr_print((sdnaddr_t *)flowid_search->target);
      // printf("\n");

      route_better = get_better_route(source, flowid_search->target, route_better);
    }

    flowid_search = flowid_search->next;
  }

  return route_better;
}

route_ptr get_better_route(uint8_t *source, uint8_t *target, route_ptr route_better) {

  route_ptr route_search = dijkstra_shortest_path(source, target);

  if(route_search != NULL) {

    if(route_better == NULL) {

      route_better = route_search;
    } else {

      if(route_search->route_weight < route_better->route_weight) {

        dijkstra_free_route(route_better);

        route_better = route_search;
      } else {

        dijkstra_free_route(route_search);
      }
    }
  }

  return route_better;
}

void update_flow_table_cache() {

  printf("\nUpdating flow table cache with digraph changes.\n");

  process_flow_id_table_changes();

  process_flow_address_table_changes();

  sdn_send_down_once();
}

void process_flow_address_table_changes() {

  route_ptr route = NULL;

  flow_address_table_ptr flow_address_table_head = flow_address_table_get();
//TODO: comment this
  while(flow_address_table_head != NULL) {

    if(flow_address_table_head->changed == 1) {
      printf("ERROR: (inconsistency) SOURCE ");
      sdnaddr_print((sdnaddr_t *)flow_address_table_head->source);
      printf(" TARGET ");
      sdnaddr_print((sdnaddr_t *)flow_address_table_head->target);
      printf(" CHANGED = %d\n", flow_address_table_head->changed);
    }

    flow_address_table_head = flow_address_table_head->next;
  }

  flow_address_table_head = flow_address_table_get();
//end TODO
  while(flow_address_table_head != NULL) {

    if(flow_address_table_head->changed == 0) { //Control for it does not read and change the registry more than once.

      route = get_better_route(flow_address_table_head->source, flow_address_table_head->target, route);

      // If there are no routes to address:
      if(route == NULL) {

        // printf("Route was not found:");
        // printf(" source ");
        // sdnaddr_print((sdnaddr_t *)flow_address_table_head->source);
        // printf(" -> flowid %d\n", flow_address_table_head->flowid);

        // flow_id_table_field =
        flow_address_table_add(flow_address_table_head->source, flow_address_table_head->target, flow_address_table_head->source, SDN_ACTION_DROP);

      // If there is at least one route to flow id:
      } else {
        // printf("Route was found:");
        // printf(" source ");
        // sdnaddr_print((sdnaddr_t *)flow_address_table_head->source);
        // printf(" -> flowid %d\n", flow_address_table_head->flowid);

// route_ptr route_head = route;

// while (route != NULL) {
//   digraph_print_vertice_id(route->vertice_id);
//   printf("(%d) <- ", route->route_weight);
//   route = route->next;
// }

// printf("\n");

// route = route_head;

        while (route != NULL) {

          if(route->next != NULL) {
            // flow_id_table_field =
            flow_address_table_add(route->next->vertice_id, flow_address_table_head->target, route->vertice_id, SDN_ACTION_FORWARD);
          }

          route = route->next;
        }
// route = route_head;
// route_head = NULL;
        dijkstra_free_route(route);
        route = NULL;
      }
    }

    flow_address_table_head = flow_address_table_head->next;
  }

  flow_address_table_head = flow_address_table_get();

  while(flow_address_table_head != NULL) {

    if(flow_address_table_head->changed == 1) {
      // printf("!!!! SOURCE ");
      // sdnaddr_print((sdnaddr_t *)flow_address_table_head->source);
      // printf(" FLOW ID [%d] CHANGED = %d.\n", flow_address_table_head->flowid, flow_address_table_head->changed);

      enqueue_sdn_control_flow_setup((sdnaddr_t*)flow_address_table_head->source, (sdnaddr_t*)flow_address_table_head->target, (sdnaddr_t*)flow_address_table_head->next_hop, flow_address_table_head->action);
      flow_address_table_head->changed = 0;
    }

    flow_address_table_head = flow_address_table_head->next;
  }
}

void process_flow_id_table_changes() {

  route_ptr route;

  flow_id_table_ptr flow_id_table_head = flow_id_table_get();
//TODO: comment this
  while(flow_id_table_head != NULL) {

    if(flow_id_table_head->changed == 1) {
      printf("ERROR: (inconsistency) SOURCE ");
      sdnaddr_print((sdnaddr_t *)flow_id_table_head->source);
      printf(" FLOW ID [%d] CHANGED = %d.\n", flow_id_table_head->flowid, flow_id_table_head->changed);
    }

    flow_id_table_head = flow_id_table_head->next;
  }

  flow_id_table_head = flow_id_table_get();
//end TODO
  while(flow_id_table_head != NULL) {

    if(flow_id_table_head->changed == 0) {

      route = get_flow_id_better_route(flow_id_table_head->source, flow_id_table_head->flowid);

      // If there are no routes to flow id:
      if(route == NULL) {

        // printf("Route was not found:");
        // printf(" source ");
        // sdnaddr_print((sdnaddr_t *)flow_id_table_head->source);
        // printf(" -> flowid %d\n", flow_id_table_head->flowid);

        // flow_id_table_field =
        flow_id_table_add(flow_id_table_head->source, flow_id_table_head->flowid, flow_id_table_head->source, SDN_ACTION_DROP);

      // If there is at least one route to flow id:
      } else {
        // printf("Route was found:");
        // printf(" source ");
        // sdnaddr_print((sdnaddr_t *)flow_id_table_head->source);
        // printf(" -> flowid %d\n", flow_id_table_head->flowid);

// route_ptr route_head = route;

// while (route != NULL) {
//   digraph_print_vertice_id(route->vertice_id);
//   printf("(%d) <- ", route->route_weight);
//   route = route->next;
// }

// printf("\n");

// route = route_head;

        while (route != NULL) {

          if(route->next != NULL) {
            // flow_id_table_field =
            flow_id_table_add(route->next->vertice_id, flow_id_table_head->flowid, route->vertice_id, SDN_ACTION_FORWARD);
          }

          route = route->next;
        }
// route = route_head;
// route_head = NULL;
        dijkstra_free_route(route);
      }
    }

    flow_id_table_head = flow_id_table_head->next;
  }

  flow_id_table_head = flow_id_table_get();

  while(flow_id_table_head != NULL) {

    if(flow_id_table_head->changed == 1) {
      // printf("!!!! SOURCE ");
      // sdnaddr_print((sdnaddr_t *)flow_id_table_head->source);
      // printf(" FLOW ID [%d] CHANGED = %d.\n", flow_id_table_head->flowid, flow_id_table_head->changed);

      // enqueue_sdn_data_flow_setup((sdnaddr_t*)flow_id_table_head->source, flow_id_table_head->flowid, (sdnaddr_t*)flow_id_table_head->next_hop, flow_id_table_head->action);
      process_data_flow_setup(NULL, flow_id_table_head->flowid, flow_id_table_head);
      flow_id_table_head->changed = 0;
    }

    flow_id_table_head = flow_id_table_head->next;
  }
}

void process_data_flow_setup(route_ptr route, uint16_t flowid, flow_id_table_ptr routing_field) {

  if (route == NULL) {

    // enqueue_sdn_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
    send_src_rtd_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
  } else {
    route_ptr route_head = route;
//TODO: comment this, used only for debug.
    while (route != NULL) {
      digraph_print_vertice_id(route->vertice_id);
      printf("(%d) <- ", route->route_weight);

      route = route->next;
    }

    printf("\n");

    route = route_head;
//end TODO
    while (route != NULL) {

      if(route->next != NULL) {
        //Using Source Route packet to set data flow.

        routing_field = flow_id_table_add(route->next->vertice_id, flowid, route->vertice_id, SDN_ACTION_FORWARD);

        if(routing_field->changed == 1) {

          // enqueue_sdn_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
          send_src_rtd_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);

          routing_field->changed = 0;
        }
      }

      route = route->next;
    }

    route = NULL;

    dijkstra_free_route(route_head);
  }
}

void process_control_flow_setup(route_ptr route, uint8_t *target_request, flow_address_table_ptr routing_field) {

  if(route == NULL) {

    send_src_rtd_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
    // sdn_send_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
  } else {

    route_ptr route_head = route;
//TODO: Comment this, use only for debug.
    while (route != NULL) {
      digraph_print_vertice_id(route->vertice_id);
      printf("(%d) <- ", route->route_weight);
      route = route->next;
    }

    printf("\n");

    route = route_head;
//end TODO
    while (route != NULL) {

      if(route->next != NULL) {

        routing_field = flow_address_table_add(route->next->vertice_id, target_request, route->vertice_id, SDN_ACTION_FORWARD);

        if(routing_field->changed == 1) {

          send_src_rtd_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
          // sdn_send_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
          routing_field->changed = 0;
        }
      }
      route = route->next;
    }
    route = NULL;

    dijkstra_free_route(route_head);
  }
}

void send_src_rtd_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action) {

  //If this packet is for me, sends the control flow setup
  if(memcmp(&sdn_node_addr, packet_destination, SDNADDR_SIZE) == 0) {

    sdn_send_data_flow_setup(packet_destination, dest_flowid, route_nexthop, action);
    return;
  }

  //Data flow setup packet.
  sdn_data_flow_setup_t *sdn_packet = (sdn_data_flow_setup_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_data_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  //Source Routed data flow setup packet
  route_ptr route = NULL;
  route = get_better_route(sdn_node_addr.u8, packet_destination->u8, route);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn source data flow setup to address: ");
    sdnaddr_print(packet_destination);
    printf("\n");

    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    return;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;

    digraph_print_vertice_id(route->vertice_id);
    printf("(%d) <- ", route->route_weight);
    route = route->next;
  }

  printf("\n");

  route = route_head;

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);

  flowid_copy(&sdn_packet->flowid, &dest_flowid);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  sdnaddr_copy( &((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;

  ((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->path_len = index_hops;

// printf("Count Hops %d.\n", count_hops);

  // route = route->next;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void send_src_rtd_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action) {

  //If this packet is for me, sends the control flow setup
  if(memcmp(&sdn_node_addr, packet_destination, SDNADDR_SIZE) == 0) {

    sdn_send_control_flow_setup(packet_destination, route_destination, route_nexthop, action);
    return;
  }

  // SDN_DEBUG("test_send_src_rt_csf\n");
  sdn_control_flow_setup_t *sdn_packet = (sdn_control_flow_setup_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_control_flow_setup_t);

  if (sdn_packet == NULL) {

    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  //Source Routed data flow setup packet
  route_ptr route = NULL;
  route = get_better_route(sdn_node_addr.u8, packet_destination->u8, route);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn source routed control flow setup: ");
    sdnaddr_print(&sdn_node_addr);
    printf(" -> ");
    sdnaddr_print(packet_destination);
    printf("\n");

    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    return;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;

    digraph_print_vertice_id(route->vertice_id);
    printf("(%d) <- ", route->route_weight);
    route = route->next;
  }

  printf("\n");

  route = route_head;

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->route_destination, route_destination);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  sdnaddr_copy( &((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;

  ((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->path_len = index_hops;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void enqueue_sdn_data_flow_request_to_controller(uint8_t *source) {

  sdn_data_flow_request_t *sdn_packet = malloc(sizeof(sdn_data_flow_request_t));

  MAKE_SDN_HEADER(SDN_PACKET_DATA_FLOW_REQUEST, SDN_DEFAULT_TTL);
  sdn_packet->flowid = SDN_CONTROLLER_FLOW;
  sdn_packet->dest_flowid = SDN_CONTROLLER_FLOW;

  memcpy(sdn_packet->header.source.u8, source, SDNADDR_SIZE);

  // process_data_flow_request(sdn_packet);

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, sizeof(sdn_data_flow_request_t)) != SDN_SUCCESS) {
    printf("ERROR: It was not possible enqueue the packet sdn_data_flow_request_t.\n");
    free(sdn_packet);
  }

}

void enqueue_sdn_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action) {

  sdn_data_flow_setup_t *sdn_packet = malloc(sizeof(sdn_data_flow_setup_t));

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);
  flowid_copy(&sdn_packet->flowid, &dest_flowid);
  sdnaddr_copy(&sdn_packet->destination, packet_destination);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, sizeof(sdn_data_flow_setup_t)) != SDN_SUCCESS) {
    printf("ERROR: It was not possible enqueue the packet sdn_data_flow_setup_t.\n");
    free(sdn_packet);
  }
}

void enqueue_sdn_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action) {

  sdn_control_flow_setup_t *sdn_packet = malloc(sizeof(sdn_control_flow_setup_t));

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->route_destination, route_destination);
  sdnaddr_copy(&sdn_packet->destination, packet_destination);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, sizeof(sdn_control_flow_setup_t)) != SDN_SUCCESS) {
    printf("ERROR: It was not possible enqueue the packet sdn_control_flow_setup_t.\n");
    free(sdn_packet);
  }
}
