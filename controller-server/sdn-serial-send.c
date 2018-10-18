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

#include "sdn-serial-send.h"
#include "sdn-addr.h"
#include "sdn-debug.h"
#include "sdn-packetbuf.h"
#include "string.h"
#include "sdn-queue.h"
#include "sdn-constants.h"
#include "sdn-process-packets.h"
#include "mainwindow_wrapper.h"

typedef enum {
  SDN_SEND_IDLE,
  SDN_SEND_ACK_WAIT
} sdn_send_status_t;

static uint8_t count_noack = 0;

sdn_send_status_t sdn_send_status;

/**
 * \brief                Sends packet to the serial port from controller
 * \param sdn_packet     A pointer to the SDN packet to be transmitted
 * \param packet_len     Number of data bytes
 * \param destination    Destination address value
 * \retval SDN_SUCCESS   If the packet was sent through of the serial port
 * \retval SDN_ERROR     otherwise
 *
 */
uint8_t sdn_send_to_serial(uint8_t *sdn_packet, uint16_t packet_len, sdnaddr_t destination);

void sdn_send_down();

static int send_flag = 0;
void sdn_send_down_once() {
    send_flag ++;
    if (send_flag == 1) {
      sdn_send_down();
    }
    send_flag--;
}

void sdn_send_down() {
  if (sdn_send_status == SDN_SEND_IDLE) {

    if (!sdn_send_queue_empty()) {

      sdn_send_queue_data_t *queue_data = sdn_send_queue_head();
      const sdnaddr_t *dest = sdn_treat_packet(queue_data->data, queue_data->len);

      // print_packet((uint8_t*)queue_data->data, queue_data->len);
      if (dest != NULL) {
        // uint8_t index;

        sdn_send_status = SDN_SEND_ACK_WAIT;

        /* Keeps the last next hop address from attempt of the packet delivery. */
        memcpy(&queue_data->last_next_hop, dest, sizeof(sdnaddr_t));

        // SDN_DEBUG ("Sending Packet to [%02X", sdn_send_queue_head()->last_next_hop.u8[0]);
        // for (index = 1; index < sizeof(sdnaddr_t); index++) {
        //   SDN_DEBUG (":%02X", sdn_send_queue_head()->last_next_hop.u8[index]);
        // }
        // SDN_DEBUG ("] ");
        // print_packet(queue_data->data, sdn_send_queue_head()->len);

//        sdn_network_send((void *) queue_data->data, queue_data->len, *dest);
        sdn_send_to_serial((uint8_t *)queue_data->data, queue_data->len, *dest);
      } else {
        SDN_DEBUG ("send(): dest == NULL\n");
        sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();
        sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);
        sdn_send_down();
      }
    } else {
      SDN_DEBUG ("Send queue is empty!\n");
    }
  } else {
    SDN_DEBUG ("Aborting send due status not ACK_IDLE.\n");
  }
}

void sdn_send_done(int status) {
//sdn_send_queue_data_t* queue_data;
//  if (memcmp((uint8_t *) ptr, sdn_send_queue_head()->data, sdn_send_queue_head()->len) != 0) {
//    SDN_DEBUG_ERROR ("Ignoring Ack for different packet!\n");
//    return;
//  }
  sdn_send_status = SDN_SEND_IDLE;

  switch (status) {
    case SERIAL_TX_ACK:
      sdn_send_queue_dequeue();
      sdn_send_down_once();
      break;
    case SERIAL_TX_NACK:
      sdn_send_down_once();
      count_noack++;
      break;
    default:
      SDN_DEBUG("Unknown error (%d)\n", status);
      sdn_send_down_once();
      break;
  }

//    if (count_noack == SDN_MAX_MAC_NOACK) {
//      SDN_DEBUG_ERROR("Reached SDN_MAX_MAC_NOACK, giving up\n");
//      sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();
//      if (queue_data->data==NULL) SDN_DEBUG_ERROR("PACKET DEQUEUE NULL\n");
//      sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);
//      trickle_time_reset();
//      sdn_send_to_net_driver();
//    } else {
//      trickle_time_active();
//    }
}


uint8_t sdn_send_to_serial(uint8_t *sdn_packet, uint16_t packet_len, sdnaddr_t destination) {

  uint8_t index;

  printf ("Sending Packet to [%02X", destination.u8[0]);

  for (index = 1; index < SDNADDR_SIZE; index++) {
      printf (":%02X", destination.u8[index]);
  }

  printf ("] ");

  print_packet((uint8_t*) sdn_packet, packet_len);

  callSendPacket(sdn_packet, packet_len, destination);

  return SDN_SUCCESS;
}
