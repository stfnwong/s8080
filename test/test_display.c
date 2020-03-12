/*
 * TEST_DISPLAY
 * Unit tests for display object
 *
 * Stefan Wong 2020
 */

#include "cpu.h"
#include "display.h"
// testing framework
#include "bdd-for-c.h"


spec("Display")
{
    it("Should init and destroy correctly")
    {
        Display* disp;
        CPUState* test_state;

        test_state = cpu_create();
        // Check the object was allocated 
        check(test_state != NULL);

        disp = display_create();
        check(disp != NULL);
        //check(disp->resize == 0);

        cpu_destroy(test_state);
        display_destroy(disp);
    }
}
