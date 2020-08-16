/*
 * SOURCE_INFO_MEM_CHECK
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "source.h"

int main(int argc, char *argv[])
{
    SourceInfo* info;

    info = source_info_create(8);
    if(!info)
    {
        fprintf(stderr, "[%s] failed to create SourceInfo\n", __func__);
        return -1;
    }

    fprintf(stdout, "[%s] created new SourceInfo containing %d bytes of inline data\n",
            __func__, source_info_num_bytes(info)
    );

    source_info_destroy(info);

    return 0;
}
