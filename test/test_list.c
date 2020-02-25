/*
 * TEST_LIST
 * Unit tests for the ByteArray linked list
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "list.h"
// testing framework
#include "bdd-for-c.h"

spec("List")
{
    it("Should init a node correctly")
    {
        int test_data_len = 64;
        ByteNode* test_node;
        uint8_t* test_data;
        test_data = malloc(sizeof(uint8_t) * test_data_len);
        check(test_data != NULL);

        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 1) % 256;       

        test_node = byte_node_create(test_data, test_data_len);
        check(test_node != NULL);
        check(test_node->data != NULL);
        check(test_node->next == NULL);
        check(test_node->prev == NULL);
        check(test_node->len == test_data_len);

        // try and set all the data to zero 
        byte_node_zero(test_node);
        for(int i = 0; i < test_node->len; ++i)
            check(test_node->data[i] == 0);

        byte_node_destroy(test_node);
    }

    it("Should init list head correctly")
    {
        ByteListHead* test_list;

        test_list = byte_list_create();
        check(test_list != NULL);
        check(test_list->len == 0);
        check(test_list->first == NULL);

        byte_list_destroy(test_list);
    }

    it("Should append Nodes to list head")
    {
        int status;
        ByteListHead* test_list;

        test_list = byte_list_create();
        check(test_list != NULL);
        check(test_list->len == 0);
        check(test_list->first == NULL);

        // make some dummy data for the test 
        int test_data_len = 128;
        uint8_t* test_data;
        test_data = malloc(sizeof(uint8_t) * test_data_len);
        check(test_data != NULL);

        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 2) % 256;

        ByteNode* test_node = byte_node_create(test_data, test_data_len);
        check(test_node != NULL); 

        status = byte_list_append_node(test_list, test_node);
        check(status == 0);

        check(test_list->len == 1);
        check(test_list->first != NULL);

        byte_list_print(test_list);

        // Check the data 
        uint8_t* ref_data = malloc(sizeof(uint8_t) * test_data_len);
        check(ref_data != NULL);

        for(int i = 0; i < test_data_len; ++i)
            ref_data[i] = (i + 2) % 256;

        ByteNode* out_node;
        out_node = byte_list_get(test_list, 5);
        check(out_node == NULL);
        out_node = byte_list_get(test_list, 0);
        check(out_node != NULL);
        check(out_node->len == test_data_len);
        
        for(int i = 0; i < test_data_len; ++i)
            check(test_data[i] == out_node->data[i]);

        byte_list_destroy(test_list);
        free(ref_data);
    }

    //it("Should allow insert and remove from end")
    //{
    //}

    it("Should allow insert and remove from middle")
    {
        int status;
        ByteListHead* test_list;

        test_list = byte_list_create();
        check(test_list != NULL);
        check(test_list->len == 0);
        check(test_list->first == NULL);

        // make some dummy data for the test 
        int test_data_len = 32;
        uint8_t* test_data;
        test_data = malloc(sizeof(uint8_t) * test_data_len);
        check(test_data != NULL);

        // Data for first node
        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 2) % 256;

        status = byte_list_append_data(test_list, test_data, test_data_len);
        check(status == 0);
        check(test_list->len == 1);
        check(test_list->first != NULL);

        // Adjust all the data for the next node
        for(int i = 0; i < test_data_len; ++i)
            test_data[i]++;

        status = byte_list_append_data(test_list, test_data, test_data_len);
        check(status == 0);
        check(test_list->len == 2);

        // Adjust all the data for the next node
        for(int i = 0; i < test_data_len; ++i)
            test_data[i]++;

        status = byte_list_append_data(test_list, test_data, test_data_len);
        check(status == 0);
        check(test_list->len == 3);

        // Check pointers 
        ByteNode* check_node = test_list->first;
        check(check_node != NULL);
        check(check_node->prev == NULL);
        check(check_node->next != NULL);
        check(check_node->data[0] == 0x2);
        // move to second node
        check_node = check_node->next;
        check(check_node != NULL);
        check(check_node->prev != NULL);
        check(check_node->next != NULL);
        check(check_node->prev == test_list->first);
        check(check_node->data[0] == 0x3);
        // finally the third node 
        check_node = check_node->next;
        check(check_node != NULL);
        check(check_node->prev != NULL);
        check(check_node->next == NULL);
        check(check_node->data[0] == 0x4);

        byte_list_print(test_list);

        // Now remove the middle element
        byte_list_remove_idx(test_list, 1);
        check(test_list->len == 2);

        check_node = byte_list_get(test_list, 0);
        check(check_node != NULL);
        check(check_node->data[0] == 0x02);
        check_node = byte_list_get(test_list, 1);
        check(check_node != NULL);
        check(check_node->data[0] == 0x04);

        byte_list_destroy(test_list);
    }
}
