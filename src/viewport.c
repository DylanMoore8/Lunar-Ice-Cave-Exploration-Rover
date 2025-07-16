/*

viewport.h - Callum MacDougall & Dylan Moore

File description:
    This file includes the function implementions for viewport.h.

*/

#include "viewport.h"

viewport_t VIEWPORT;

void initialize_viewport(uint8_t start_x, uint8_t start_y, uint8_t height, uint8_t width)
{
if (height < MAPROWS) /* Set viewport height */
    VIEWPORT.height = height;
else
    VIEWPORT.height = MAPROWS - 1;

if (width < MAPCOLUMNS) /* Set viewport width */
    VIEWPORT.width = width;
else
    VIEWPORT.width = MAPCOLUMNS - 1;

/* Set viewport starting position if it is within the bounds of the screen and the map */
if (start_x - VIEWPORT.width / 2 >= 0 && start_x + VIEWPORT.width / 2 < MAPCOLUMNS &&
    start_y - VIEWPORT.height / 2 >= 0 && start_y + VIEWPORT.height / 2 < MAPROWS) /*  in range of map*/

    if (start_x - VIEWPORT.width / 2 > screen.min.col && start_x + VIEWPORT.width / 2 < screen.max.col &&
        start_y - VIEWPORT.height / 2 > screen.min.col && start_y + VIEWPORT.height / 2 < screen.max.row)
    {
    VIEWPORT.TL_x = start_x - VIEWPORT.height;
    VIEWPORT.TL_y = start_y - VIEWPORT.width;
    }
    else
    {  
    /* Default position for x (far left or far right) */
    if (start_x - VIEWPORT.width / 2 < MAP_ZERO_COL)
    {
        VIEWPORT.TL_x = 0;  
    }
    else 
    {
        int x = mylice.x;
        while (x-- + VIEWPORT.width < MAPCOLUMNS);
        VIEWPORT.TL_x = x; /* Set to bottom of screen */
    }
    /* Default position for y (far left or far right) */
    if (start_y - VIEWPORT.height / 2 < MAP_ZERO_ROW)
    {
        VIEWPORT.TL_y = 0;  
    }
    else 
    {
        int y = mylice.y;
        while (y-- + VIEWPORT.height < MAPROWS);
        VIEWPORT.TL_y = y; /* Set to bottom of screen */
    }
} 
}

void display_viewport(enum mode mode)
{
int layer = mylice.z;
int aboveLayer = layer - 1;
cell_t** layerPtr;
cell_t* rowPtr;
char symbolCurr;
enum cellType typeCurr;
char cursorChar;
cell_t* cellPtr;
cell_pos tmpPos;

/* Clear screen */
printf(CSI "%dm" CSI "%dm", BGBLACK, FGWHITE); /* BG black and FG white */
CLRSCR
printf(CSI "%dm" CSI "%dm", BGCYAN, FGBLACK); /* BG cyan and FG black */

EDLDM /* DEC line drawing mode */

for (int y = VIEWPORT.TL_y + 1; y <= VIEWPORT.TL_y + VIEWPORT.height; y++)
{
    /* If row exists and is within map boundaries */
    rowPtr = map[layer][y];
    if (rowPtr != NULL)
    {
        for (int x = VIEWPORT.TL_x + 1; x <= VIEWPORT.TL_x + VIEWPORT.width; x++)
        {
            cellPtr = &rowPtr[x];
            /* If cell has been initialzied, and column is within map boundarires */
            typeCurr = cellPtr->type;
            if(typeCurr != NORMAL && typeCurr != INACTIVE) { // If cell is portal or chasm:
                //symbolCurr = typeCurr == PORTAL ? PORTALCHAR : CHASMCHAR; // Select which symbol to use:
                if      (typeCurr == PORTAL)     symbolCurr = PORTALCHAR;
                else if (typeCurr == CHASM)      symbolCurr = CHASMCHAR;
                else if (typeCurr == PORTAL_ABOVE) symbolCurr = ABOVEPORTALCHAR;
                else symbolCurr = UNKNOWNCHAR;
                CUP(x - VIEWPORT.TL_x, y - VIEWPORT.TL_y)
                _putch(symbolCurr); 
            }
            else {
                symbolCurr = cellPtr->orient.ch;
                if (symbolCurr != NUL)
                {
                    /* Place cursor realative to center of viewport */
                    CUP(x - VIEWPORT.TL_x, y - VIEWPORT.TL_y)
                    _putch(symbolCurr);
                }
                else {
                    symbolCurr = ' ';
                }
            }
            /* If cell is where lice is, set cursor character to that cell */
            if(x == mylice.x && y == mylice.y) cursorChar = symbolCurr; 
        }   
    } 
}

/* Print curson position afterwards */
CUP(mylice.x - VIEWPORT.TL_x, mylice.y - VIEWPORT.TL_y)
printf(CSI "%dm", BGWHITE); /* When printing the LICER position set BG to white */
if (mode == CURSOR)
{
    //cellPtr = &map[layer][mylice.y][mylice.x];
    _putch(cursorChar);
}
else
{
    _putch(' ');
}

EAM /* Reset to ascii print mode */
}

void update_viewport_pos(uint8_t x, uint8_t y)
{
/* Set the viewport's top left to repective coordinate */
/* Horizontal */
if (x - (VIEWPORT.width / 2) >= screen.min.col && x + (VIEWPORT.width / 2) < MAPCOLUMNS - 1)
    VIEWPORT.TL_x = x - (VIEWPORT.width / 2);

/* Vertical */
if (y - (VIEWPORT.height / 2) >= screen.min.row && y + (VIEWPORT.height / 2) < MAPROWS - 1)
    VIEWPORT.TL_y = y - (VIEWPORT.height / 2);
}