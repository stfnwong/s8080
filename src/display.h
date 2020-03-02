/*
 * DISPLAY
 * Some SDL stuff to display the ROM contents (ie: for games)
 *
 * Stefan Wong 2020
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>


// These are the specs for invaders. 
#define DISP_TITLE "Space Invaders"
#define DISP_HEIGHT 256
#define DISP_WIDTH 224
#define DISP_TIC (1000.0 / 60.0)       // ms per tic
#define DISP_CYCLES_PER_MS 2000         // 8080 clock @2Mhz
#define DISP_CYCLES_PER_TIC (DISP_CYCLES_PER_MS * DISP_TIC)

// Display forward declaration
typedef struct Display Display;

int disp_resize_func(void* user_data, SDL_Event* ev);
// NOTE: Maybe refactor this file to be something like IO.h (since
// display and input aren't really related)
//int disp_handle_input(void);

Display* display_create(void);
void     display_destroy(Display* disp);
void     display_draw(Display* disp, uint8_t* mem);

#endif /*__DISPLAY_H*/
