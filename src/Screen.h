/*

Screen.h - Larry Hughes

File Description:
    Allows high-level screen access to the program. Features drawing characters,
    printing messages, and screen initialization.

*/

#pragma once

/* Position on screen */
typedef struct scr_coord
{
int col;
int row;
} SCR_COORD;

/* Screen size and center */
typedef struct screen
{
SCR_COORD min;
SCR_COORD max;
SCR_COORD center;
} SCREEN;

extern SCREEN screen;

/*
Function description:
    This function draws a char to the screen at a specified row and column.
Returns:
    Status (0 if within bounds, -1 if not within bounds)
*/
extern int draw_object(int col, int row, int symbol);

/*
Function description:
    Displays a message at the bottom left of the screen.
Returns:
    None.
*/
extern void print_msg(char* msg);

/*
Function description:
    Initializes screen. Must be called before use of screen object.
Returns:
    None.
*/
extern void screen_init();
