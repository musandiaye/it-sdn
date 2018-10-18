#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "sdn-neighbor-table.h"

/**
* \brief The struct to define sensor node resource types
*  takes in 4 arguments
*/
struct node_resource{
    uint16_t battery;
    uint16_t num_of_tasks;
    uint16_t RSSI;       
    uint16_t num_of_neighbors;
};

typedef struct node_resource node_resource_t;


struct context_entry {
    /*Declare context parameters here*/
    sdnaddr_t           source_addr;//from header in SDN context packet
    //flowid_t            flowid;
    node_resource_t     resource;
    /*Declare ->next pointer node to allow for 
    linked list operation in contiki list*/
    struct context_entry* next;

};

/**
*  \brief    Initializes the Context_table
*   \retval  SDN_SUCCESS OR SDN_ERROR
*
*/
 uint8_t context_table_init();

 /**
 * \brief   Function to insert or update an entry into the context table
 *  \param  source_addr   Source node address
 *  \param  resource    Node resource context metric values
  * \retval SDN_SUCCESS  Entry was added or updated
 * \retval SDN_ERROR    It was not possible to create new entry
 *
 */

 uint8_t context_table_insert(sdnaddr_t source_addr, node_resource_t resource);

 /**
 * \brief Remove an entry in the the context table based on address
 * \param source_addr   Source node address
 * \retval SDN_SUCCESS  Entry was removed
 * \retval SDN_ERROR    Entry not found
 *
 */
 uint8_t context_table_remove(sdnaddr_t source_addr);

 /**
 * \brief Access context information from the context table
 * \param source_addr   Source node address
 * \retval  Non-NULL if the entry was found, NULL otherwise
 *
 */

 struct context_entry* context_table_get(sdnaddr_t source_addr);

 /**
 * \brief Access the first element from context table
 * \retval  Non-NULL if table is not empty, NULL otherwise
 *
 */
struct context_entry * context_table_head();

/**
 * \brief Access next element in the context table
 * \param source_addr source node address
 * \retval  Non-NULL if there is another element, NULL otherwise
 *
 */
struct context_entry * context_table_next(sdnaddr_t source_addr);

/**
 * \brief Prints all context data, for debugging purposes
 *
 */
 void context_table_print();

 #endif //CONTEXT_H_

 /** @} */
/** @} */
