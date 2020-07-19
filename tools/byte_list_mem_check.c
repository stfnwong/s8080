// Sanity check for byte_list object 

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(int argc, char *argv[])
{
    ByteList* list;
    uint8_t* data;
    uint16_t start_addr = 0xDEED;
    int data_size = 32;
    int status;

    list = byte_list_create();
    if(!list)
    {
        fprintf(stderr, "[%s] failed to create byte list\n", __func__);
        return -1;
    }

    data = malloc(sizeof(uint8_t) * data_size);
    for(int i = 0; i < data_size; ++i)
        data[i] = (i+1) % 256;

    status = byte_list_append_data(list, data, data_size, start_addr);
    if(status < 0)
    {
        fprintf(stderr, "[%s] error code %d when inserting %d bytes of data into list\n", __func__, status, data_size);
        return -1;
    }

    fprintf(stdout, "[%s] inserted %d bytes into list\n", __func__, data_size);
    fprintf(stdout, "[%s] list contains %d nodes\n", __func__, byte_list_len(list));

    byte_list_destroy(list);
    free(data);

    return 0;
}
