/*

CCMain.c - Callum MacDougall & Dylan Moore, original author: Larry Hughes

File Description: 
	Includes initialization and entry point to main control function, go_robot_go().

*/

#include "VT100.h"
#include "Screen.h"
#include "Robot.h"
#include "status_bar.h"
#include "viewport.h"
#include "map.h"
#include "FileManager.h"

#define DEBUG

HANDLE scrout;	/* Output stream */
HANDLE kbin;	/* Input stream */

void terminate(char* msg)
{
/* Fatal error detected - terminate */
printf("Error: %s\n", msg);
(void)getchar();
exit(1);
}

int main(void)
{
/*
 - Setup Main screen based on window size
 - initialize screen and robot
 - run robot until Home key pressed
*/
long outmode, inmode;

/* TEMP TESTING */
/*
input_file = fopen("abc.licermap", "wb+");
header_record header;
record rec;
char tmp = 0xCA;
header.next_available = 123987;
writeHeader(&header);

fileMoveTo(1, SEEK_SET);
writeRecord(&rec);
writeRecord(&rec);
writeRecord(&rec);

rec.cell.pos.layer = 100;

unsigned i = 0;
fileMoveTo(0, SEEK_END);
fileMoveTo(-1, SEEK_CUR);
//fileEOF();
//fwrite(&tmp, 1, 1, input_file);
writeRecord(&rec);
//writeRecord(&rec);
//writeRecord(&rec);
//fileMoveTo(-1, SEEK_CUR);
//fileMoveTo(0, SEEK_CUR);

while(!fileEOF()) {
	fileMoveTo(1, SEEK_CUR);
	i++;
}


fclose(input_file);

return 0;
*/
/* END OF TESTING */

/* Get input and output handles: */
if ((scrout = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	terminate("Can't open output");

if ((kbin = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	terminate("Can't open input");

/* Set terminal mode for screen input: */
if (!GetConsoleMode(scrout, &outmode))
	terminate("Can't get old console mode");

outmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
if (!SetConsoleMode(scrout, outmode))
	terminate("Can't set console mode");

/* Set terminal mode for screen input: */
if (!GetConsoleMode(kbin, &inmode))
	terminate("Can't get old console mode");

inmode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
if (!SetConsoleMode(kbin, inmode))
	terminate("Can't set console mode");

/* Initialize File */
fileInit();

/* Switch to VT-100 mode */
screen_init();

/* Initialize cells in map */
if (!map_init())
{
	print_msg("Failure initializing map.\n");
	return 1;
}

/* File Menu */
fileMenu();

/* Enable robot */
robot_init();

/* Initialize the viewport camera */
initialize_viewport(mylice.x, mylice.y, screen.max.row, screen.max.col);

/* Enable keypad escape sequence */
KPNM
/* Check robot for movement */
go_robot_go();

dealloc_map();

print_msg("Done!");

(void) getchar();

}
