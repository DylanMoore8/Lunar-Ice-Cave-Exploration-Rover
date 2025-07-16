/*

Screen.c - Larry Hughes

File Description:
	Adds implementations for Screen.h

*/

#include "VT100.h"
#include "Screen.h"

SCREEN screen;

void print_msg(char* msg)
{
/*
 - Display message in diagnostic area (LL of active screen)
*/
/* Move to last row */
CUP(screen.min.col, screen.max.row)
/* Change BG colour */
printf(CSI "%dm", BGBLACK);
/* Erase */
EL(screen.max.row)
printf(CSI "%d;%dm%s", BGBLUE, FGWHITE, msg);
printf(CSI "0m"); /* Restore FG & BG colour */
}

void screen_init()
{
CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;

/* Get new screen size */
GetConsoleScreenBufferInfo(scrout, &ScreenBufferInfo);

screen.min.col = 0;
screen.min.row = 0;

screen.max.col = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
screen.max.row = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;

screen.center.col = (screen.max.col - screen.min.col) / 2;
screen.center.row = (screen.max.row - screen.min.row) / 2;

#ifdef DIAG
printf("Screen details: Max: r: %d c: %d Cent: r: %d c: %d\n",
	screen.max.row, screen.max.col, screen.center.row, screen.center.col);
(void) getchar();
#endif

/* Clear the screen */
CLRSCR

/* Resizing displays cursor - hide it */
printf(CSI "?25l");
}

int draw_object(int col, int row, int symbol)
{
/* 
 - Attempts to draw symbol at position col, row
 - Returns -1 if illegal location, 0 otherwise
*/
if (col <= screen.min.col || row <= screen.min.row ||
	col >= screen.max.col || row >= screen.max.row)
	/* Outside view screen - do not draw */
	return -1;

CUP(col, row);
_putch(symbol);

return 0;

}
