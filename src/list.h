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
// TODO : forward declare structs and move to implementation

// ByteData 
// A linked list where each node holds arbitrary 
// array of bytes.
typedef struct ByteData ByteData;

struct ByteData
{
    uint8_t*  data;
    uint16_t  start_addr;
    int       len;
    ByteData* next;
    ByteData* prev;
};


// TODO: bytedata object, which links addresses and data. 
// This way we can move it out of the LineInfo, and therefore not need
// some odd trick to make a vector of LineInfos (which is essentially 
// what a source info is)

ByteData* byte_data_create(uint8_t* data, int len, int addr);
void      byte_data_destroy(ByteData* node);
void      byte_data_zero(ByteData* node);
void      byte_data_copy(ByteData* dst, ByteData* src);

// FIXME: this is for structure hiding API
void      byte_data_set_next(ByteData* node, ByteData* n);
void      byte_data_set_prev(ByteData* node, ByteData* p);
// display
void      byte_data_print(ByteData* node);

// List of ByteData
typedef struct ByteList ByteList;

struct ByteList
{
    int       len;
    ByteData* first;
};

ByteList*     byte_list_create(void);
void          byte_list_destroy(ByteList* list);
int           byte_list_len(ByteList* list);
int           byte_list_total_bytes(ByteList* list);
int           byte_list_append_node(ByteList* list, ByteData* node);
int           byte_list_append_data(ByteList* list, uint8_t* data, int len, int addr);
ByteData*     byte_list_get(ByteList* list, int idx);
void          byte_list_remove_end(ByteList* list);
void          byte_list_remove_idx(ByteList* list, int idx);
void          byte_list_copy(ByteList* dst, ByteList* src);

void          byte_list_print(ByteList* list);


#endif /*__S8080_LIST_H*/
