/*

map.h - Callum MacDougall & Dylan Moore

File Description:
    This file contains the function definitions and structures necessary for map.c.

*/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAPLAYERS 100
#define MAPROWS 101
#define MAPCOLUMNS 101
#define MAP_ZERO_COL 50
#define MAP_ZERO_ROW 50

#define PORTALCHAR 'X'
#define ABOVEPORTALCHAR '0'
#define CHASMCHAR 'C'
#define UNKNOWNCHAR '?'

#define ALLOCATION_SUCCESS 1
#define ALLOCATION_FAILURE 0

#define CH_DEFAULT '\0'
#define FRIC_DEFAULT 5
#define RAD_DEFAULT 0
#define ICE_DEFAULT 0
#define RITT_DEFAULT 0

/* Structure, containing the x and y coordinates of a given cell */
struct cell_pos {
    uint8_t row;
    uint8_t col;
} typedef cell_pos;

/* Describes state and type of cell */
enum cellType {
    INACTIVE,
    NORMAL,
    PORTAL,
    CHASM,
    PORTAL_ABOVE
};

/* Structure for cell orientaion data */
struct orientation
{
    char ch; /* The character to be displayed */
    uint8_t dir[4]; /* Direction in and out of cell */
};

/* Structure for cell properties */
struct properties
{   
    uint8_t icePercentage;
    uint8_t frictionLevel;
    uint8_t radiationLevel;
    uint8_t ritterbariumLevel;
};

/* Structure contining cell information */
typedef struct cell {
    enum cellType type;
    struct orientation orient;
    struct properties prop;
} cell_t;

extern void getPortals(uint8_t layer, cell_pos *portal_pos_arr, size_t* arr_size);

/* Current layer global initialized in the middle*/
extern cell_t *** map;
extern uint8_t last_layer;
/*
Function Description:
    This function initalizes the layers in the map, sets layers to null.
Returns:
    Initialization success.
*/
extern int map_init();

/*
Function description:
    Allocates memory for a layer, sets rows to null.    
Returns:
    Allocation success.
*/
extern int allocate_layer(uint8_t layer_num);

/*
Function description:
    Allocates a given row, and initializes the cells within it.
Returns:
    Allocation success.
*/
extern int allocate_row(uint8_t layer_num, uint8_t row_num);
/*
Function description:
    Sets or resets values of a cell to defaults
Returns:
    None.
*/
void set_cell_defaults(cell_t * cell);
/*
Function description:
    Allocates layer and or row if they are not already allocated.
Returns: 
    None.
*/
void allocate_if_null(uint8_t layer_num, uint8_t row_num);
/*
Function description:
    Deallocates stored map data.
Returns:
    None.
*/
void dealloc_map();
/*
Function description:
    Finds layers which are allocated. Parameter "layers" MUST have size of at least MAPLAYERS.
Returns:
    Array of allocated layers, and number of allocated layers (through pointers).
*/
void find_allocated_layers(uint8_t * layers, uint8_t * size);

/*
Function description:
    Updates cell types of portal aboves by checking portals.
    Assumes bounds are checked and 
*/
void update_portal_aboves(uint8_t layer);