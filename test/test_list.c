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
        ByteData* test_node;
        uint8_t* test_data;
        test_data = malloc(sizeof(uint8_t) * test_data_len);
        check(test_data != NULL);

        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 1) % 256;       

        test_node = byte_data_create(test_data, test_data_len);
        check(test_node != NULL);
        check(test_node->data != NULL);
        check(test_node->next == NULL);
        check(test_node->prev == NULL);
        check(test_node->len == test_data_len);

        // try and set all the data to zero 
        byte_data_zero(test_node);
        for(int i = 0; i < test_node->len; ++i)
            check(test_node->data[i] == 0);

        byte_data_destroy(test_node);
    }

    it("Should init list head correctly")
    {
        int status;
        ByteList* test_list;

        test_list = byte_list_create();
        check(test_list != NULL);
        check(test_list->len == 0);
        check(test_list->first == NULL);

        // Also check the API calls 
        check(byte_list_len(test_list) == 0);
        check(byte_list_total_bytes(test_list) == 0);

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
        ByteData* check_node = test_list->first;
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

        //// Remove the last node
        //byte_list_remove_end(test_list);
        //check(test_list->len == 2);

        //check_node = byte_list_get(test_list, 0);
        //check(check_node != NULL);
        //check(check_node->data[0] == 0x02);
        //check_node = byte_list_get(test_list, 1);
        //check(check_node != NULL);
        //check(check_node->data[0] == 0x03);

        //// Remove the new last node
        //byte_list_remove_end(test_list);
        //check(test_list->len == 1);

        //check_node = byte_list_get(test_list, 0);
        //check(check_node != NULL);
        //check(check_node->data[0] == 0x02);

        byte_list_destroy(test_list);
    }

    it("Should append Nodes to list head")
    {
        int status;
        ByteList* test_list;

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

        ByteData* test_node = byte_data_create(test_data, test_data_len);
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

        ByteData* out_node;
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

    it("Should allow insert and remove from end")
    {
        int status;
        ByteList* test_list;

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
        ByteData* check_node = test_list->first;
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

        // Now remove from the end
        byte_list_remove_end(test_list);
        check(test_list->len == 2);
        check_node = byte_list_get(test_list, 2);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 1);
        check(check_node != NULL);
        check(check_node->data[0] == 0x3);

        // Remove from end again
        byte_list_remove_end(test_list);
        check(test_list->len == 1);
        check_node = byte_list_get(test_list, 2);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 1);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 0);
        check(check_node->data[0] == 0x2);

        // Removing from the end will create an empty list 
        byte_list_remove_end(test_list);
        check(test_list->len == 0);
        check_node = byte_list_get(test_list, 2);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 1);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 0);
        check(check_node == NULL);

        // Calling again has no effect
        byte_list_remove_end(test_list);
        check(test_list->len == 0);
        check_node = byte_list_get(test_list, 2);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 1);
        check(check_node == NULL);
        check_node = byte_list_get(test_list, 0);
        check(check_node == NULL);

        byte_list_destroy(test_list);
    }

    it("Should allow insert and remove from middle")
    {
        int status;
        ByteList* test_list;

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
        ByteData* check_node = test_list->first;
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

        fprintf(stdout, "After removing element 1 of list...:\n\n");
        byte_list_print(test_list);

        // Now remove the first element
        byte_list_remove_idx(test_list, 0);
        check(test_list->len == 1);

        check_node = byte_list_get(test_list, 0);
        check(check_node != NULL);
        check(check_node->data[0] == 0x4);
        check_node = byte_list_get(test_list, 1);
        check(check_node == NULL);

        fprintf(stdout, "After removing element 0 of list...:\n\n");
        byte_list_print(test_list);

        // Removing the end will empty the list 
        byte_list_remove_idx(test_list, 0);
        check(test_list->len == 0);

        // Trying to remove more elements should have no effect 
        byte_list_remove_end(test_list);
        check(test_list->len == 0);

        byte_list_destroy(test_list);
        free(test_data);
    }

    it("Should be copyable")
    {
        int status;
        ByteList* src_list;
        ByteList* dst_list;

        // Create the list to copy from
        src_list = byte_list_create();
        check(src_list != NULL);
        check(src_list->len == 0);
        check(src_list->first == NULL);

        // make some dummy data for the test 
        int test_data_len = 32;
        uint8_t* test_data;
        test_data = malloc(sizeof(uint8_t) * test_data_len);
        check(test_data != NULL);

        // Data for node 1
        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 2) % 256;

        fprintf(stdout, "[%s] adding %d bytes to list\n", __func__, test_data_len);

        // Start adding data to the source list
        status = byte_list_append_data(src_list, test_data, test_data_len);
        check(status == 0);
        check(byte_list_len(src_list) == 1);
        check(byte_list_total_bytes(src_list) == test_data_len);
        check(src_list->first != NULL);

        byte_list_print(src_list);

        // Data for node 2
        for(int i = 0; i < test_data_len; ++i)
            test_data[i] = (i + 3) % 256;
        
        fprintf(stdout, "[%s] adding %d bytes to list\n", __func__, test_data_len);

        status = byte_list_append_data(src_list, test_data, test_data_len);
        check(status == 0);
        check(byte_list_len(src_list) == 2);
        check(byte_list_total_bytes(src_list) == 2 * test_data_len);

        byte_list_print(src_list);
        fprintf(stdout, "[%s] node contains %d bytes total\n", __func__, byte_list_total_bytes(src_list));

        // Create the list to copy to
        dst_list = byte_list_create();
        check(dst_list != NULL);
        check(dst_list != NULL);
        check(dst_list->len == 0);
        check(dst_list->first == NULL);

        // Perform the copy
        byte_list_copy(dst_list, src_list);
        check(byte_list_len(src_list) == byte_list_len(dst_list));
        fprintf(stdout, "[%s] %d bytes in src_list, %d bytes in dst_list\n",
                __func__, 
                byte_list_total_bytes(src_list), 
                byte_list_total_bytes(dst_list)
        );
        check(byte_list_total_bytes(dst_list) == byte_list_total_bytes(src_list));

        // Check that all the elements were copied
        ByteData* src_node;
        ByteData* dst_node;

        for(int i = 0; i < byte_list_len(src_list); ++i)
        {
            src_node = byte_list_get(src_list, i);
            dst_node = byte_list_get(src_list, i);
            check(src_node != NULL);
            check(dst_node != NULL);
            check(src_node->len == dst_node->len);
            for(int d = 0; d < src_node->len; ++d)
                check(src_node->data[d] == dst_node->data[d]);
            // Pointers won't match, but the pattern should be the same
            if(i == 0)
            {
                check(src_node->prev == NULL);
                check(dst_node->prev == NULL);
                check(src_node->next != NULL);
                check(dst_node->next != NULL);
            }
            else if(i == byte_list_len(src_list)-1)
            {
               check(src_node->next == NULL);
               check(dst_node->next == NULL);
               check(src_node->prev != NULL);
               check(dst_node->prev != NULL);
            }
            else
            {
                check(src_node->next != NULL);
                check(src_node->prev != NULL);
                check(dst_node->next != NULL);
                check(dst_node->prev != NULL);
            }
        }

        byte_list_destroy(src_list);
        byte_list_destroy(dst_list);
        free(test_data);
    }
}
