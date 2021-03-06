/*
 * DISPLAY
 * Some SDL stuff to display the ROM contents (ie: for games)
 *
 * Stefan Wong 2020
 */


#include <stdlib.h>
#include "display.h"

// Display structure internals
struct Display
{
    SDL_Window*  win;
    SDL_Surface* surf;
    SDL_Surface* winsurf;
    int resize;
    //void (*resize_func)(void*, SDL_Event*);
};

int disp_resize_func(void* user_data, SDL_Event* ev)
{
    Display* disp = (Display*) user_data;
    if(ev->type == SDL_WINDOWEVENT)
    {
        if(ev->window.event == SDL_WINDOWEVENT_RESIZED)
            disp->resize = 1;
    }

    return 0;
}

// ======== DISPLAY ======== //
Display* display_create(void)
{
    int status = 0;
    Display* disp;

    disp = malloc(sizeof(*disp));
    if(!disp)
        goto DISP_END;

    disp->resize = 0;

    // We also take care of all the SDL stuff here
    status = SDL_Init(SDL_INIT_VIDEO);
    if(status)
    {
        fprintf(stderr, "[%s] %s\n", __func__, SDL_GetError());
        goto DISP_END;
    }
    status = 1;

    disp->win = SDL_CreateWindow(
            DISP_TITLE,
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED,
            2 * DISP_WIDTH,
            2 * DISP_HEIGHT,
            SDL_WINDOW_RESIZABLE
    );

    if(!disp->win)
    {
        fprintf(stderr, "[%s] failed to create SDL_Window handle\n", __func__);
        goto DISP_END;
    }

    // Create surface
    disp->winsurf = SDL_GetWindowSurface(disp->win);
    if(!disp->winsurf)
    {
        fprintf(stderr, "[%s] failed to get Window surface\n", __func__);
        goto DISP_END;
    }
    // watch for resize
    SDL_AddEventWatch(disp_resize_func, NULL);
    // Create backbuffer surface
    disp->surf = SDL_CreateRGBSurface(
            0, 
            DISP_WIDTH, 
            DISP_HEIGHT, 
            32,
            0,
            0,
            0,
            0
    );

DISP_END:
    if(!disp || !disp->win || !disp->winsurf || !status)
    {
        fprintf(stderr, "[%s] failed to create display\n", __func__);
        return NULL;
    }

    return disp;
}

/*
 * display_draw()
 */
void display_draw(Display* disp, uint8_t* mem)
{
    // TODO : Size of video RAM is hardcoded here. Need to check if 
    // that the case for all 8080 software or just invaders
    int vram = 0x2400;
    uint32_t* pixels = disp->surf->pixels;

    for(int col = 0; col < DISP_WIDTH; ++col)
    {
        for(int row = DISP_HEIGHT; row > 0; row = row-8)
        {
            for(int k = 0; k < 8; ++k)
            {
                int idx = (row - k) * DISP_WIDTH + col;
                if(mem[vram] & 1 << k)
                    pixels[idx] = 0xFFFFFF;
                else
                    pixels[idx]= 0x000000;
            }
            vram++;
        }
    }

    //if(disp->resize)
    //{
    //    disp->winsurf = SDL_GetWindowSurface(disp->win);
    //}
    SDL_BlitScaled(disp->surf, NULL, disp->winsurf, NULL);

    if(SDL_UpdateWindowSurface(disp->win))
    {
        fprintf(stderr, "[%s] %s\n", __func__, SDL_GetError());
    }
}

/*
 * display_destroy()
 */
void display_destroy(Display* disp)
{
    free(disp);
}
