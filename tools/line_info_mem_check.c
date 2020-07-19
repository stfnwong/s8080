/*
 * LINE_INFO_MEM_CHECK
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "source.h"

int main(int argc, char *argv[])
{
    LineInfo* info;

    info = line_info_create();
    if(!info)
    {
        fprintf(stderr, "[%s] failed to create LineInfo\n", __func__);
        return -1;
    }

    fprintf(stdout, "[%s] created new LineInfo containing %d bytes of inline data\n",
            __func__, line_info_num_bytes(info)
    );

    line_info_destroy(info);

    return 0;
}
