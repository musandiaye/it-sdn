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

#include "sdn-send.h"
#include "sdn-network.h"
#include "sdn-addr.h"
#include "net/netstack.h"
#include "sdn-debug.h"
#include "sdn-packetbuf.h"
#include "string.h"
#include "sdn-queue.h"
#include "sdn-constants.h"
#include "sdn-neighbor-table.h"
#include "sdn-process-packets.h"
#include "sys/ctimer.h"
#include "random.h"

#ifdef SDN_CONTROLLER_NODE
#include "sdn-serial.h"
#endif

typedef enum {
  SDN_SEND_IDLE,
  SDN_SEND_ACK_WAIT,
  SDN_SEND_TRICKLE_TIME
} sdn_send_status_t;

struct ctimer trickle_timer;
float trickle_time = 0.050f;
uint8_t count_noack;

PROCESS(sdn_send_process, "SDN Send process");

sdn_send_status_t sdn_send_status;

void trickle_time_reset();

void trickle_time_active();

void trickle_time_callback(void *ptr);

// void sdn_send_to_net_driver();

void sdn_send_init() {

  random_init((unsigned short) sdn_node_addr.u8);
  sdn_send_queue_init();
  trickle_time_reset();
  process_start(&sdn_send_process, NULL);

}

void sdn_send_down_once() {
  if (sdn_send_status == SDN_SEND_IDLE) {
    process_post(&sdn_send_process, SDN_EVENT_NEW_PACKET, 0);
  } else {
    SDN_DEBUG ("Defering send due status not SDN_SEND_IDLE.\n");
  }
}

/**
 * \brief This function is called by sdn-network when packet is delivery by radio.
 *
 * \param ptr pointer to the sent packet
 * \param status The status of the packet delivery.
 * \param transmissions Number of transmission attempts
 */
void sdn_send_done(void *ptr, int status, int transmissions);

PROCESS_THREAD(sdn_send_process, ev, data)
{
  PROCESS_BEGIN();

  while (1) {
    PROCESS_WAIT_EVENT();

     while (sdn_send_status == SDN_SEND_IDLE && !sdn_send_queue_empty() ) {

        sdn_send_queue_data_t *queue_data = sdn_send_queue_head();
#ifdef SDN_ENABLED_NODE
        const sdnaddr_t *dest = sdn_treat_packet(queue_data->data, queue_data->len);
#else
        const sdnaddr_t *dest = &queue_data->last_next_hop;
#endif

        SDN_DEBUG("Trying to send: ");
        print_packet((uint8_t*)queue_data->data, queue_data->len);
        if (dest != NULL) {
          sdn_send_status = SDN_SEND_ACK_WAIT;

#ifdef SDN_ENABLED_NODE
          /* Keeps the last next hop address from attempt of the packet delivery. */
          memcpy(&queue_data->last_next_hop, dest, sizeof(sdnaddr_t));
#endif

          SDN_DEBUG ("Sending Packet to ");
          sdnaddr_print(&sdn_send_queue_head()->last_next_hop);
          print_packet(queue_data->data, sdn_send_queue_head()->len);

          sdn_network_send((void *) queue_data->data, queue_data->len, *dest);
        } else {
          SDN_DEBUG ("send(): dest == NULL\n");
          sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();
          sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);
        }
    }
  }
  PROCESS_END();
}

void sdn_send_done(void *ptr, int status, int transmissions) {

  uint8_t index;

  if (memcmp((uint8_t *) ptr, sdn_send_queue_head()->data, sdn_send_queue_head()->len) != 0) {
    SDN_DEBUG_ERROR ("Ignoring Ack for different packet!\n");
    return;
  }

  if (status == MAC_TX_OK) {
#ifndef SDN_ENABLED_NODE
    sdn_serial_send_ack();
#endif

    SDN_DEBUG ("Packet Sent to [%02X", sdn_send_queue_head()->last_next_hop.u8[0]);
    for (index = 1; index < sizeof(sdnaddr_t); index++) {
      SDN_DEBUG (":%02X", sdn_send_queue_head()->last_next_hop.u8[index]);
    }
    SDN_DEBUG ("] ");
    print_packet((uint8_t *) ptr, sdn_send_queue_head()->len);

    sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();

    if (queue_data->data==NULL) SDN_DEBUG_ERROR("PACKET DEQUEUE NULL\n");
    sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);

    trickle_time_reset();
    sdn_send_down_once();
  } else {

    SDN_DEBUG("Activating trickle time: ");
    switch (status) {
      case MAC_TX_COLLISION:
        SDN_DEBUG("MAC_TX_COLLISION\n");
        break;
      case MAC_TX_NOACK:
        SDN_DEBUG("MAC_TX_NOACK\n");
        count_noack++;
        break;
      case MAC_TX_ERR:
        SDN_DEBUG("MAC_TX_ERR\n");
        break;
      default:
        SDN_DEBUG("Unknown error (%d)\n", status);
        break;
    }

    if (count_noack == SDN_MAX_MAC_NOACK) {
      SDN_DEBUG_ERROR("Reached SDN_MAX_MAC_NOACK, giving up\n");
#ifndef SDN_ENABLED_NODE
      sdn_serial_send_nack();
#endif
      sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();
      if (queue_data->data==NULL) SDN_DEBUG_ERROR("PACKET DEQUEUE NULL\n");
      sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);
      trickle_time_reset();
      sdn_send_down_once();
    } else {
      trickle_time_active();
    }
  }
}

void trickle_time_reset() {
  ctimer_stop(&trickle_timer);
  sdn_send_status = SDN_SEND_IDLE;
  trickle_time = 0.050f;
  count_noack = 0;
}

void trickle_time_active() {
  sdn_send_status = SDN_SEND_TRICKLE_TIME;

  if (trickle_time < 1) {
    trickle_time *= 2;
  }

  float trickle_time_rand = random_rand() * trickle_time / 65535;

  SDN_DEBUG ("TRICKLE_TIME activated %d.\n",(int)(trickle_time_rand*1000));

  ctimer_set(&trickle_timer, (unsigned long) (trickle_time_rand * CLOCK_SECOND), trickle_time_callback, NULL);
}

void trickle_time_callback(void *ptr) {
  sdn_send_status = SDN_SEND_IDLE;
  sdn_send_down_once();
}
