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
#include <stdio.h>
#include <string.h>
#include "digraph.h"

#define DIGRAPH_DEBUG

#ifdef DIGRAPH_DEBUG
#define DEBUG_DIGRAPH(...) printf(__VA_ARGS__);
// #define DEBUG_DIGRAPH(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define DEBUG_DIGRAPH(...)
#endif /* DIGRAPH_DEBUG */

/* To implement
number of edges changed
*/

vertice_ptr digraph_header = NULL;

void digraph_print_vertice_id(unsigned char *vertice_id) {

  int index;

  printf("[ ");

  for(index = 0; index < DIGRAPH_VERTICE_ID_SIZE; index++) {
    printf("%02X ", vertice_id[index]);
  }

  printf ("]");
}

vertice_ptr digraph_new_vertice(unsigned char *vertice_id) {

  vertice_ptr vertice = malloc(sizeof *vertice);

  memcpy(vertice->vertice_id, vertice_id, DIGRAPH_VERTICE_ID_SIZE);

  vertice->back = NULL;
  vertice->next = NULL;
  vertice->edge_header = NULL;

  vertice->visited = 0;
  vertice->route_weight = 0;
  vertice->route_root = NULL;

  return vertice;
}

vertice_ptr digraph_add_vertice(unsigned char *vertice_id) {

  // DEBUG_DIGRAPH("FUNCTION digraph_add_vertice\n");

  vertice_ptr vertice_new = digraph_new_vertice(vertice_id);

  if(digraph_header == NULL) {
    // DEBUG_DIGRAPH("Adding vertice to Digraph header.\n");
    digraph_header = vertice_new;
    vertice_new->next = vertice_new;
    vertice_new->back = vertice_new;
  } else {
    vertice_new->next = digraph_header->next;
    digraph_header->next->back = vertice_new;
    vertice_new->back = digraph_header;
    digraph_header->next = vertice_new;
    if(digraph_header->back == digraph_header) {
      digraph_header->back = vertice_new;
    }
  }

//  DEBUG_DIGRAPH("Added Vertice ID: ");
//#ifdef DIGRAPH_DEBUG
//  digraph_print_vertice_id(vertice_new->vertice_id);
//#endif
//  DEBUG_DIGRAPH("\n");

  return vertice_new;
}

vertice_ptr digraph_get_vertice(unsigned char *vertice_id) {

  vertice_ptr vertice = digraph_find_vertice(vertice_id);

  if(vertice == NULL) {
    vertice = digraph_add_vertice(vertice_id);
  }

  return vertice;

}

void digraph_del_vertice(vertice_ptr vertice) {

  digraph_del_edges_from(vertice);
  digraph_del_edges_to(vertice);

  if(vertice->next == vertice) {
    digraph_header = NULL;
  } else {
    // DEBUG_DIGRAPH("before: header %p header->next %p header->back %p vertice %p vertice->back %p vertice->next %p\n", digraph_header, digraph_header->next, digraph_header->back, vertice, vertice->next, vertice->back);
    vertice->back->next = vertice->next;
    vertice->next->back = vertice->back;
    digraph_header = vertice->next;
    // DEBUG_DIGRAPH("after: header %p header->next %p header->back %p\n", digraph_header, digraph_header->next, digraph_header->back);
  }

  DEBUG_DIGRAPH("Vertice removed: ");
#ifdef DIGRAPH_DEBUG
  digraph_print_vertice_id(vertice->vertice_id);
#endif
  DEBUG_DIGRAPH("\n");

  free(vertice);
}

vertice_ptr digraph_find_vertice(unsigned char *vertice_id) {

  // DEBUG_DIGRAPH("FUNCTION digraph_find_vertice\n");

  vertice_ptr vertice_found = NULL;

  if(digraph_header != NULL) {

    vertice_ptr vertice_find = digraph_header;

    do {
// #ifdef DIGRAPH_DEBUG
//       DEBUG_DIGRAPH("Comparing vertices: ");
//       digraph_print_vertice_id(vertice_id);
//       DEBUG_DIGRAPH(" == ");
//       digraph_print_vertice_id(vertice_find->vertice_id);
//       DEBUG_DIGRAPH("\n");
// #endif
      if(memcmp(vertice_find->vertice_id, vertice_id, DIGRAPH_VERTICE_ID_SIZE) == 0) {
        vertice_found = vertice_find;
        // DEBUG_DIGRAPH("Vertice found\n");
      } else {
        vertice_find = vertice_find->next;
      }
    } while(vertice_find != digraph_header && vertice_found == NULL);

    if(vertice_found == NULL) {
      // DEBUG_DIGRAPH("Vertice not found.\n");
    }
  } else {
    // DEBUG_DIGRAPH("There is no vertice on digraph.\n");
  }

  return vertice_found;
}

edge_ptr digraph_new_edge(vertice_ptr vertice_to, int etx) {

  edge_ptr edge_new = malloc(sizeof *edge_new);

  edge_new->vertice = vertice_to;
  edge_new->etx = etx;
  edge_new->status = EDGE_NEW;
  edge_new->back = NULL;
  edge_new->next = NULL;

  return edge_new;
}

edge_ptr digraph_add_edge(vertice_ptr vertice_from, vertice_ptr vertice_to, int etx) {


  edge_ptr edge = digraph_find_edge(vertice_from, vertice_to);

  if(edge == NULL) { // Add new edge

    edge = digraph_new_edge(vertice_to, etx);

    if(vertice_from->edge_header == NULL) {
      vertice_from->edge_header = edge;
      edge->next = edge;
      edge->back = edge;
      DEBUG_DIGRAPH("First edge added: ");
    } else {
      edge->next = vertice_from->edge_header->next;
      vertice_from->edge_header->next->back = edge;
      edge->back = vertice_from->edge_header;
      vertice_from->edge_header->next = edge;
      if(vertice_from->edge_header->back == vertice_from->edge_header) {
        vertice_from->edge_header->back = edge;
      }
      DEBUG_DIGRAPH("Edge added: ");
    }
#ifdef DIGRAPH_DEBUG
    digraph_print_vertice_id(vertice_from->vertice_id);
    DEBUG_DIGRAPH(" -> ");
    digraph_print_vertice_id(vertice_to->vertice_id);
    DEBUG_DIGRAPH(" ETX %d.\n", etx);
#endif
  } else { // update existing edge
    if(edge->etx == etx) { // Egde not changed
      edge->status = EDGE_STATIC;

#ifdef DIGRAPH_DEBUG
      DEBUG_DIGRAPH("Edge not changed: ");
      digraph_print_vertice_id(vertice_from->vertice_id);
      DEBUG_DIGRAPH(" -> ");
      digraph_print_vertice_id(vertice_to->vertice_id);
      DEBUG_DIGRAPH(" ETX %d.\n", edge->etx);
#endif

    } else {
      edge->etx = etx;
      edge->status = EDGE_UPDATED;

#ifdef DIGRAPH_DEBUG
      DEBUG_DIGRAPH("Edge updated: ");
      digraph_print_vertice_id(vertice_from->vertice_id);
      DEBUG_DIGRAPH(" -> ");
      digraph_print_vertice_id(vertice_to->vertice_id);
      DEBUG_DIGRAPH(" ETX %d.\n", edge->etx);
#endif

    }
  }

  return edge;
}

void digraph_del_edge(vertice_ptr vertice_from, vertice_ptr vertice_to) {

  if(vertice_from->edge_header == NULL) {
    DEBUG_DIGRAPH("There are no edges on vertice to remove.\n");
    return;
  }

  edge_ptr edge = vertice_from->edge_header;

  while(edge->next != vertice_from->edge_header && edge->vertice != vertice_to) {
    edge = edge->next;
  }

  if(edge->vertice == vertice_to) {
    if(edge->next == edge) {
      // DEBUG_DIGRAPH("Edge header removed.\n");
      vertice_from->edge_header = NULL;
    } else {
      // DEBUG_DIGRAPH("before edge: header %p vertice_from->edge_header->next %p vertice_from->edge_header->back %p edge %p edge->back %p edge->next %p\n", vertice_from->edge_header, vertice_from->edge_header->next, vertice_from->edge_header->back, edge, edge->next, edge->back);
      if(edge == vertice_from->edge_header) {
        vertice_from->edge_header = edge->next;
      }
      edge->back->next = edge->next;
      edge->next->back = edge->back;
      // DEBUG_DIGRAPH("after edge: header %p vertice_from->edge_header->next %p vertice_from->edge_header->back %p edge %p edge->back %p edge->next %p\n", vertice_from->edge_header, vertice_from->edge_header->next, vertice_from->edge_header->back, edge, edge->next, edge->back);
    }
#ifdef DIGRAPH_DEBUG
    DEBUG_DIGRAPH("Edge removed: ");
    digraph_print_vertice_id(vertice_from->vertice_id);
    DEBUG_DIGRAPH(" -> ");
    digraph_print_vertice_id(edge->vertice->vertice_id);
    DEBUG_DIGRAPH(" ETX %d.\n", edge->etx);
#endif
    free(edge);
  }
}

void digraph_del_edges_from(vertice_ptr vertice_from) {

  // DEBUG_DIGRAPH("FUNCTION digraph_delete_edges_from");
  // digraph_print_vertice_id(vertice_from->vertice_id);
  // DEBUG_DIGRAPH("\n");

  if(vertice_from->edge_header == NULL) {
//    DEBUG_DIGRAPH("There are no edges to remove.\n");
    return;
  }

  // edge_ptr edge = vertice_from->edge_header;

  while(vertice_from->edge_header != NULL) {
    digraph_del_edge(vertice_from, vertice_from->edge_header->vertice);
  }
}

void digraph_mark_edges_to_del_from(vertice_ptr vertice_from) {

  // DEBUG_DIGRAPH("FUNCTION digraph_delete_edges_from");
  // digraph_print_vertice_id(vertice_from->vertice_id);
  // DEBUG_DIGRAPH("\n");

  if(vertice_from->edge_header == NULL) {
//    DEBUG_DIGRAPH("There are no edges to mark to remove.\n");
    return;
  }

  edge_ptr edge = vertice_from->edge_header;

  do {
    edge->status = EDGE_DELETED;

    edge = edge->next;
  } while(edge != vertice_from->edge_header);
}

//TODO: to implement
// edge_ptr digraph_get_marked_edges_to_del_from(vertice_ptr vertice_from) {

//   edge_ptr edge_header = NULL;
//     //TODO: Retornar listas de endereços marcados para remoção.
//   return edge_header;
// }

int digraph_del_marked_edges_to_del_from(vertice_ptr vertice_from) {

  // DEBUG_DIGRAPH("FUNCTION digraph_delete_edges_from");
  // digraph_print_vertice_id(vertice_from->vertice_id);
  // DEBUG_DIGRAPH("\n");

  int edge_deleted = 0;

  if(vertice_from->edge_header == NULL) {
//    DEBUG_DIGRAPH("There are no edges to mark to remove.\n");
    return edge_deleted;
  }

  edge_ptr edge = vertice_from->edge_header;
  edge_ptr edge_next;

  do {
    edge_next = edge->next;

    if(edge->status == EDGE_DELETED) {

      digraph_del_edge(vertice_from, edge->vertice);
      edge_deleted = 1;
    }

    edge = edge_next;

  } while(edge != vertice_from->edge_header && vertice_from->edge_header != NULL);

  if(vertice_from->edge_header != NULL && vertice_from->edge_header->status == EDGE_DELETED) {

    digraph_del_edge(vertice_from, vertice_from->edge_header->vertice);
  }

  return edge_deleted;
}

void digraph_del_edges_to(vertice_ptr vertice_to) {

  if(digraph_header == NULL) {
    return;
  }

  vertice_ptr vertice = digraph_header;

  do {
    edge_ptr edge = vertice->edge_header;

    if(edge != NULL) {
      do {
        if(edge->vertice == vertice_to) {
          edge = edge->next;
          digraph_del_edge(vertice, vertice_to);
        } else if(vertice->edge_header != NULL) {
          edge = edge->next;
        }
      } while (vertice->edge_header != NULL && edge != vertice->edge_header);
    }

    vertice = vertice->next;
  } while(vertice != digraph_header);
}

edge_ptr digraph_find_edge(vertice_ptr vertice_from, vertice_ptr vertice_to) {
// #ifdef DIGRAPH_DEBUG
//   DEBUG_DIGRAPH("Finding Edge ");
//   digraph_print_vertice_id(vertice_from->vertice_id);
//   DEBUG_DIGRAPH(" -> ");
//   digraph_print_vertice_id(vertice_to->vertice_id);
//   DEBUG_DIGRAPH("\n");
// #endif
  edge_ptr edge_found = NULL;

  if(vertice_from->edge_header != NULL) {
    edge_ptr edge_find = vertice_from->edge_header;

    do {
      if(edge_find->vertice == vertice_to) {
        edge_found = edge_find;
      } else {
        edge_find = edge_find->next;
      }
    } while(edge_find != vertice_from->edge_header && edge_found == NULL);
  }

  return edge_found;
}

vertice_ptr digraph_get() {
  return digraph_header;
}

void digraph_free() {

  DEBUG_DIGRAPH("Destroying Digraph!\n");

  while(digraph_header != NULL) {
    digraph_del_vertice(digraph_header);
  }
}
