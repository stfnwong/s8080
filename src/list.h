/*
 * LIST
 * A linked list
 *
 * Stefan Wong 2020
 */

// NOTE : lots of malloc and free here, could be a chance to 
// improve performance in the future.

#ifndef __S8080_LIST_H
#define __S8080_LIST_H

#include <stdint.h>

// ByteNode 
// A linked list where each node holds arbitrary 
// array of bytes.
typedef struct ByteNode ByteNode;

struct ByteNode
{
    uint8_t*  data;
    int       len;
    ByteNode* next;
    ByteNode* prev;
};

ByteNode* byte_node_create(uint8_t* data, int len);
void      byte_node_destroy(ByteNode* node);
void      byte_node_zero(ByteNode* node);
void      byte_node_print(ByteNode* node);

// ListHead
typedef struct
{
    int       len;
    ByteNode* first;
} ByteList;

ByteList* byte_list_create(void);
void          byte_list_destroy(ByteList* list);
int           byte_list_append_node(ByteList* list, ByteNode* node);
int           byte_list_append_data(ByteList* list, uint8_t* data, int len);
ByteNode*     byte_list_get(ByteList* list, int idx);
void          byte_list_remove_end(ByteList* list);
void          byte_list_remove_idx(ByteList* list, int idx);

void          byte_list_print(ByteList* list);


#endif /*__S8080_LIST_H*/
