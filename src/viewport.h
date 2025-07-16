/*

viewport.c - Callum MacDougall & Dylan Moore

File description:
    Provides camera control to map by creating a liason between the map data
    and the screen relative to the cursor position.
    
*/

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "map.h"
#include "Robot.h"
#include "VT100.h"
#include "Screen.h"

/* Structure and typedef for camera */
typedef struct viewport {
    uint8_t TL_y;
    uint8_t TL_x;
    uint8_t height;
    uint8_t width;
} viewport_t;

extern viewport_t VIEWPORT;

/*
Function description:
    This function initializes values for the viewport.
Returns:
    None.
*/
extern void initialize_viewport(uint8_t start_x, uint8_t start_y, uint8_t height, uint8_t width);

/*
Function description:
    This function prints the cells within the viewport to the screen.
Returns:
    None.
*/
extern void display_viewport(enum mode mode);

/*
Function description:
    This function changes the cells being displayed within the viewport, as long as the position change is within the map boundaries.
    The arguments received are the requested center of the viewport.
Returns:
    None.
*/
extern void update_viewport_pos(uint8_t x, uint8_t y);
