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

/**
 * \file
 *         Header file for the SDN Send
 * \author
 *         Doriedson A. G. O. <doliveira@larc.usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-send SDN Send
 * @{
 *
 * The sdn-send module is responsible for the send of SDN packets.
 *
 */

#ifndef SDN_SEND_H
#define SDN_SEND_H

#include "sdn-addr.h"

#ifndef SDN_MAX_MAC_NOACK
#ifdef SDN_CONF_MAX_MAC_NOACK
#define SDN_MAX_MAC_NOACK SDN_CONF_MAX_MAC_NOACK
#else // SDN_CONF_MAX_MAC_NOACK
#define SDN_MAX_MAC_NOACK 5
#endif // SDN_CONF_MAX_MAC_NOACK
#endif //SDN_MAX_MAC_NOACK

#define ALLOC_AND_ENQUEUE(queue_ptr, packet_ptr, packet_len) \
  queue_ptr = sdn_packetbuf_pool_get(); \
  \
  if (queue_ptr == NULL) { \
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n"); \
  } else { \
    memcpy(queue_ptr, packet_ptr, packet_len); \
    if (sdn_send_queue_enqueue((uint8_t *) queue_ptr, packet_len) != SDN_SUCCESS) { \
      sdn_packetbuf_pool_put((struct sdn_packetbuf *)queue_ptr); \
      SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    } else { \
      sdn_send_down_once(); \
    } \
  }

#define ENQUEUE_AND_SEND(packet_ptr, packet_len) \
  if (sdn_send_queue_enqueue((uint8_t *) packet_ptr, packet_len) != SDN_SUCCESS) { \
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)packet_ptr); \
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    return SDN_ERROR; \
  } else { \
    SDN_METRIC_TX(packet_ptr); \
    sdn_send_down_once(); \
    return SDN_SUCCESS; \
  }

void sdn_send_init();

void sdn_send_down_once();

#endif //SDN_SEND_H

/** @} */
/** @} */
/** @} */
