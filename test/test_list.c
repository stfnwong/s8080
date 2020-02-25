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
        //check(&test_list->first == &test_node);


        byte_list_destroy(test_list);
    }
}
