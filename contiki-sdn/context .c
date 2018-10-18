#include "stdio.h"
#include "stdlib.h"
#include "lib/memb.h"
#include "lib/list.h"
#include "context.h"
//#include "sdn-neighbor-table.h"


#ifndef CONF_CONTEXT_TABLE_SIZE
#define CONTEXT_TABLE_SIZE (sizeof(struct context_entry))//revise this value
#else 
#define CONTEXT_TABLE_SIZE  CONF_CONTEXT_TABLE_SIZE
#endif

/*Declare a memory block to hold the context table data in dynamic memory*/
MEMB(context_table_memb, struct context_entry, CONTEXT_TABLE_SIZE);

LIST(context_table);

/*We initialize the context table before we insert any items*/
uint8_t context_table_init() {
    list_init(context_table);
    return SDN_SUCCESS;
}
/*---------The following is a sequence of steps required to enter a context item----*/
uint8_t context_table_insert(sdnaddr_t source_addr, node_resource_t resource){
    struct context_entry *n;

    /*Check if an entry already exists. */

    n = context_table_get(source_addr);

    /*If n==NULL the entry was not found in the list therefore we allocate a new memory block,*/

    if (n == NULL) {
        n = memb_alloc(&context_table_memb);
    }

    /*If the entry exists we delete that entry and replace it with the new one*/

    if (n != NULL) {
        list_remove(context_table, n);
        memb_free(&context_table_memb, n);
        n = memb_alloc(&context_table_memb);
        return SDN_SUCCESS; // returns a 0
    }

    /*We initialize the resource fields*/
    sdnaddr_copy(&n->source_addr, &source_addr);
    n->resource = resource;

    /*Finally place the entry in the context table list*/
    list_add(context_table, n);

    return SDN_SUCCESS;
}
/*--------------END OF INSERT-------------------------------*/

/*------Steps to remove a target source address item-----*/

uint8_t  context_table_remove(sdnaddr_t source_addr) {
    struct context_entry *n;
    n = context_table_get(source_addr);

    if (n==NULL) {
        list_remove(context_table, n);
        memb_free(&context_table_memb, n);
        return SDN_SUCCESS; // returns a 0
    }

    return SDN_ERROR; // Returns a 1
}//End remove 

/*--To access context given source address*/
struct context_entry * context_table_get(sdnaddr_t source_addr) {
    struct context_entry *n;
    for(n = list_head(context_table);n != NULL; n = list_item_next(n)) {
        /*We break out of the loop if the address matches*/
        if (sdnaddr_cmp(&n->source_addr, &source_addr) == SDN_EQUAL) {
            return n;
        }

    }

    return NULL;
}//end context access

/*To access the first item in the context table*/

struct context_entry* context_table_get() {
    return list_head(context_table);
}

/*To retrieve the proceeding element in the table*/

struct context_entry* context_table_next(sdnaddr_t source_addr) {
    return list_item_next(context_table_get(source_addr));
}//end proceeding 

/*Function to print the context table*/

void context_table_print() {
    struct context_entry *n;
    printf("source, context info, next ptr\n");
    for(n = list_head(context_table); n != NULL; n = list_item_next(n)) {
    printf("(");
    sdnaddr_print(&n->source_addr);
    printf(") (%02d) (%p)\n", n->resource, n->next);

    }
    
}//print function ends here








