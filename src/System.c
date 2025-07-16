/*

System.c - Callum MacDougall & Dylan Moore, original author: Larry Hughes

File Description:
	Directs flow of main program. Redirects program based on user input (either
	escape sequence or key).

*/
#include "VT100.h"
#include "Robot.h"
#include "status_bar.h"
#include "viewport.h"
#include "CellEdit.h"
#include "FileManager.h"

#define CHARTODIR(ch) ch-'A'
#define PG_UP '5'
#define PG_DWN '6'
#define HOME 'H'
#define END 'F'
#define INSERT '2'
#define DEL '3'
#define ARR_UP 'A'
#define ARR_DWN 'B'
#define ARR_LEFT 'C'
#define ARR_RIGHT 'D'
#define CAP_EDIT_KEY 'C'
#define LOW_EDIT_KEY 'c'
#define CAP_RESZ_KEY 'R'
#define LOW_RESZ_KEY 'r'
#define LAYER_DOWN 0 
#define LAYER_UP 1

#define DEBUG

void go_robot_go()
{
int done;
char ch;
int * last_coords;
enum cellType tmp_type;
enum mode mode = DRAW;

done = FALSE;

while (!done)
{
	/* Allocate layer and row of licer */
	if (mode == DRAW)
		allocate_if_null(mylice.z, mylice.y);

	/* Draw Map */
	update_viewport_pos(mylice.x, mylice.y);
	display_viewport(mode);

	updateLastLayer(mylice.z);

	#ifdef DEBUG
	printDebugFile();

	FILE * fout = fopen("system_status.txt", "w");
    fprintf(fout, "Licer direction: [%d] (IDLE = -1, NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3)\n", mylice.curr_dir);
    fprintf(fout, "Currently: Layer [%hu]\n", mylice.z);
	for(int i = 0; i < MAPLAYERS; i++) {
    	fprintf(fout, "`>\tLayer [%d]: %p\n", i, map[i]);
	}
    fclose(fout);
	#endif

	/* Print the status of the robot */
	print_status_bar(mode);

	while (_kbhit() == 0); /* Busy wait */

	if ((ch = (char)_getch()) == ESCAPE)
	{
		if ((char)_getch() == '[')
		{
			switch ((ch = (char)_getch()))
			{
			case END:
				if (mode == CURSOR)
				{
					/* Move cursor to last map layer created */
					if(switchLayerIntoProg(last_layer)) {
						mylice.z = last_layer;
						allocate_if_null(mylice.z, mylice.y);
					}
				}
				else
				{
					/* Exit program */
					done = TRUE;
				}
				break;
			case ARR_UP:
			case ARR_DWN:
			case ARR_LEFT:
			case ARR_RIGHT:
				robot_move(CHARTODIR(ch), mode); /* Move robot */
				break;
			case PG_UP:
			case PG_DWN:
				char tmp = ch;
				_getch(); /* Get ~ VT character */
				if (mode == CURSOR)
				{
					traverseLayer(ch == PG_UP ? 1 : 0);
				}
				else
				{
					/* Change layer without moving through portal */
					changeLayer(ch == PG_UP ? LAYER_UP : LAYER_DOWN);
				}
				break;
			case INSERT:
				/* Change to draw mode */
				mode = !mode; // Optimization of mode = (mode == CURSOR) ? DRAW : CURSOR
				break;
			case DEL:
				tmp_type = map[mylice.z][mylice.y][mylice.x].type;
				if (tmp_type != INACTIVE && tmp_type != PORTAL_ABOVE) /* If cell exists */
				{
					changeType(mylice.z, mylice.y, mylice.x, INACTIVE);
					/* Delete cell */
					set_cell_defaults(&map[mylice.z][mylice.y][mylice.x]);
					/* Delete cell from file */
					deleteCellInFile(mylice.z, mylice.y, mylice.x);
				}
				break;
			case HOME:
				/* Move map to layer 0 (MUST BE THE LAST KNOWN LOCATION) */
				if (mode == CURSOR) {
					if(switchLayerIntoProg(0))
						robot_reset();
				} 
				else
				{
					mylice.x = MAP_ZERO_COL - 1;
					mylice.y = MAP_ZERO_ROW - 1;
				}
				break;
			}
		}
	}
	else if (ch == LOW_EDIT_KEY || ch == CAP_EDIT_KEY) /* Edit cell */
	{
		tmp_type = map[mylice.z][mylice.y][mylice.x].type;
		if (mode == CURSOR && tmp_type != PORTAL_ABOVE)
			editCell();
	}
	else if (ch == LOW_RESZ_KEY || ch == CAP_RESZ_KEY)
	{
		/* Resize screen (re-initilize viewport and screen) */
		screen_init();
		initialize_viewport(mylice.x, mylice.y, screen.max.row, screen.max.col);
	}
}
}