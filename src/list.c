/*
 * LIST
 * A linked list
 *
 * Stefan Wong 2020
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"


// ================= BYTE NODE

/*
 * byte_data_create()
 */
ByteData* byte_data_create(uint8_t* data, int len, int addr)
{
    ByteData* node;

    node = malloc(sizeof(*node));
    if(!node)
        goto BYTE_NODE_CREATE_END;

    node->data = malloc(sizeof(uint8_t) * len);
    if(!node->data)
    {
        free(node);
        goto BYTE_NODE_CREATE_END;
    }

    node->start_addr = addr;
    node->len        = len;
    node->next       = NULL;
    node->prev       = NULL;
    memcpy(node->data, data, node->len);

BYTE_NODE_CREATE_END:
    if(!node || !node->data)
    {
        fprintf(stdout, "[%s] failed to create byte node (%d bytes)\n",
                __func__, len);
        return NULL;
    }

    return node;
}

/*
 * byte_data_destroy()
 */
void byte_data_destroy(ByteData* node)
{
    free(node->data);
    free(node);
}

/*
 * byte_data_zero()
 */
void byte_data_zero(ByteData* node)
{
    memset(node->data, 0, node->len);
    node->start_addr = 0;
}

/*
 * byte_data_copy()
 */
void byte_data_copy(ByteData* dst, ByteData* src)
{
    if(dst == NULL || src == NULL)
        return;

    free(dst->data);
    dst->data = malloc(sizeof(uint8_t) * src->len);
    if(!dst->data)
    {
        fprintf(stdout, "[%s] failed to allocate memory for dst->data\n", __func__);
        return;
    }
    memcpy(dst->data, src->data, src->len);
    dst->len = src->len;
}

/*
 * byte_data_set_next()
 */
void byte_data_set_next(ByteData* node, ByteData* n)
{
    node->next = n;
}

/*
 * byte_data_set_prev()
 */
void byte_data_set_prev(ByteData* node, ByteData* p)
{
    node->prev = p;
}


/*
 * byte_data_print()
 */
void byte_data_print(ByteData* node)
{
    fprintf(stdout, "ByteData (%d bytes)\n", node->len);

    for(int b = 0; b < node->len; ++b)
    {
        if((b % 8 == 0) && (b > 0))
            fprintf(stdout, "\n");
        fprintf(stdout, "%02X ", node->data[b]);
    }
    fprintf(stdout, "\n");
}


// ================= BYTE LIST
/*
 * byte_list_create()
 */
ByteList* byte_list_create(void)
{
    ByteList* head;

    head = malloc(sizeof(*head));
    if(!head)
        goto BYTE_LIST_HEAD_END;

    head->len   = 0;
    head->first = NULL;

BYTE_LIST_HEAD_END:
    if(!head)
    {
        fprintf(stdout, "[%s] failed to allocate memory for ByteList\n", __func__);
        return NULL;
    }

    return head;
}

/*
 * byte_list_destroy()
 */
void byte_list_destroy(ByteList* list)
{
    if(list->len > 0)
    {
        ByteData* cur_node = list->first;

        // go to the end
        while(cur_node->next != NULL)
            cur_node = cur_node->next;

        while(cur_node != list->first)
        {
            cur_node = cur_node->prev;
            byte_data_destroy(cur_node->next);
        }
        byte_data_destroy(list->first);
    }

    free(list);
}

/*
 * byte_list_len()
 */
int byte_list_len(ByteList* list)
{
    return list->len;
}

/*
 * byte_list_total_bytes()
 */
int byte_list_total_bytes(ByteList* list)
{
    int total = 0;
    ByteData* cur_node = list->first;

    while(cur_node != NULL)
    {
        total += cur_node->len;
        cur_node = cur_node->next;
    }

    return total;
}

/*
 * byte_list_append_node()
 */
int byte_list_append_node(ByteList* list, ByteData* node)
{
    if(node == NULL)
        return -1;

    ByteData* list_end;
    if(list->first == NULL)
        list->first = node;
    else
    {
        while(list_end->next != NULL)
            list_end = list_end->next;
        list_end->next = node;
    }
    node->prev  = list_end;
    list->len++;

    return 0; // Can this fail?
}

/*
 * byte_list_append_data()
 */
int byte_list_append_data(ByteList* list, uint8_t* data, int len, int addr)
{
    ByteData* node;
    ByteData* list_end;

    if(data == NULL)
        return -1;

    node = byte_data_create(data, len, addr);
    if(!node)
        return -1;

    if(list->first == NULL)
    {
        list->first = node;
        node->prev = NULL;
    }
    else
    {
        list_end = list->first;
        while(list_end->next != NULL)
            list_end = list_end->next;

        list_end->next = node;
        node->prev     = list_end;
    }
    list->len++;

    return 0;
}

/*
 * byte_list_get()
 */
ByteData* byte_list_get(ByteList* list, int idx)
{
    if(idx < 0 || idx >= list->len)
        return NULL;
    
    ByteData* node = list->first;
    for(int i = 0; i < idx; ++i)
        node = node->next;

    return node;
}

/*
 * byte_list_remove_end()
 */
void byte_list_remove_end(ByteList* list)
{
    ByteData* node;

    if(list->first == NULL)
        return;

    if(list->len == 1)
    {
        byte_data_destroy(list->first);
        list->first = NULL;
    }
    else
    {
        node = list->first;
        while(node->next != NULL)
            node = node->next;

        node = node->prev;
        byte_data_destroy(node->next);
        node->next = NULL;
    }
    list->len--;
}

/*
 * byte_list_remove_idx()
 */
void byte_list_remove_idx(ByteList* list, int idx)
{
    // TODO : It feels like if I sit down and think about 
    // this some more that there should be a simpler way to 
    // do this....
    if(idx < 0)
        return;

    if(list->len == 0 || list->first == NULL)
        return;

    if(idx >= list->len)
    {
        byte_list_remove_end(list);
        return;
    }

    if(idx == 0 && list->len > 1)
    {
        ByteData* del_node = list->first;
        ByteData* node = list->first->next;

        byte_data_destroy(del_node);
        list->first = node;
        list->first->prev = NULL;
    }
    else if(list->len == 1)
    {
        byte_data_destroy(list->first);
        list->first = NULL;
    }
    else
    {
        ByteData* node;
        ByteData* node_after;
        node = list->first;
        for(int i = 0; i < idx; ++i)
            node = node->next;

        node_after = node->next;
        node = node->prev;

        byte_data_destroy(node->next);
        node->next = node_after;
        node_after->prev = node;
    }
    list->len--;
}

/*
 * byte_list_copy()
 */
void byte_list_copy(ByteList* dst, ByteList* src)
{
    if(dst == NULL || src == NULL)
        return;

    int status = 0;
    int cur_node_num = 0;

    ByteData* src_node = src->first;
    while(src_node != NULL)
    {
        status = byte_list_append_data(
                dst, 
                src_node->data, 
                src_node->len,
                src_node->start_addr
        );
        if(status < 0)
        {
            fprintf(stdout, "[%s] failed to append node to dst list\n", __func__);
            return;
        }
        cur_node_num++;
        src_node = src_node->next;
    }
}

/*
 * byte_list_print()
 */
void byte_list_print(ByteList* list)
{
    fprintf(stdout, "ByteList : ");
    if(list->len == 0)
    {
        fprintf(stdout, "[]");
        return;
    }
    else 
        fprintf(stdout, " %d nodes\n", list->len);

    ByteData* cur_node;

    cur_node = list->first;
    for(int i = 0; i < list->len; ++i)
    {
        fprintf(stdout, "%3d " , i);
        byte_data_print(cur_node);
        cur_node = cur_node->next;
    }
}
