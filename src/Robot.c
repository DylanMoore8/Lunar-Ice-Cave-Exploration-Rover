/*

Robot.c - Callum MacDougall & Dylan Moore, original author: Larry Hughes

File Description:
	Includes implementations for Robot.h.

*/

#include "robot.h"

/* Tunnel symbol to display [new dir][old dir] */
char cell_sym[4][5] = {
/*	Old: NO  SO  ET  WT  IDL    New:*/
		{VR, VR, LR, LL, VR}, /* NO */
		{VR, VR, UR, UL, VR}, /* SO */
		{UL, LL, HR, HR, HR}, /* ET */
		{UR, LR, HR, HR, HR}  /* WT */
};

short del_y[] = { -1,  1,  0,  0 };
short del_x[] = {  0,  0,  1, -1 };

ROBOT mylice;

/* ASCII-to-DEC graphic characters */
enum symbol asc_dec[] = {
	'j', 'k', 'l', 'm', 'n', 'q', 't', 'u', 'v', 'w', 'x', ' ' };



void robot_init()
{
/*
 - Initialize robot to center of screen
 - Show robot at center of screen
*/
mylice.x = MAP_ZERO_COL - 1;
mylice.y = MAP_ZERO_ROW - 1;
mylice.z = 0; /* First layer */
mylice.curr_dir = IDLE;

/* Allocated initial position */
allocate_if_null(mylice.z, mylice.y);
if (map[mylice.z][mylice.y][mylice.x].type == INACTIVE)
{
	map[mylice.z][mylice.y][mylice.x].orient.ch = START_SYM;
	map[mylice.z][mylice.y][mylice.x].type = NORMAL;
}

writeCellFromMap(CELL_IN_FILE, mylice.z, mylice.y, mylice.x);
}

void robot_reset()
{
	mylice.x = MAP_ZERO_COL - 1;
	mylice.y = MAP_ZERO_ROW - 1;
	mylice.z = 0; /* First layer */
	mylice.curr_dir = IDLE;
	cell_t * init_cell = &map[mylice.z][mylice.y][mylice.x];

	/* Set starting symbol */
	init_cell->orient.ch = START_SYM;
	if(init_cell->type == INACTIVE)
		changeType(mylice.z, mylice.y, mylice.x, NORMAL);

	writeCellFromMap(CELL_IN_FILE, mylice.z, mylice.y, mylice.x);
	
	return;
}

void robot_move(dir_t direction, enum mode mode)
{
enum symbol new_off; /* Offset into asc_dec[] */
char new_sym;	/* Symbol from asc_dec[] */
int new_x, old_x = mylice.x;
int new_y, old_y = mylice.y;
int old_dir = mylice.curr_dir > 0 ? mylice.curr_dir : -1 * mylice.curr_dir;
int temp_dir;
/* Set current direction */
mylice.curr_dir = direction;
/* If old direction is idle, change it to reverse of current direction */
if (old_dir == IDLE)
	old_dir += direction % 2 == 0 ? 1 : -1;
/* If not moving, retun */
if(direction == IDLE) {
	return;
}

/* DEC Keypad sequence (U, D, R, L) */
new_x = mylice.x + del_x[direction];
new_y = mylice.y + del_y[direction];

EDLDM /* Enable DEC line drawing mode */
/* If move is within bounds of map */
if (new_x < MAPROWS && new_y < MAPCOLUMNS &&
	new_x > 0 		&& new_y > 0)
{

	/* Possible overshoot to invalid position */
	mylice.x = new_x;
	mylice.y = new_y;

	if (mode == CURSOR){
		/* Allocate layer and row of licer */
		allocate_if_null(mylice.z, mylice.y);

		if(map[mylice.z][mylice.y][mylice.x].type == INACTIVE) {
			map[mylice.z][mylice.y][mylice.x].type = NORMAL;
		}

		/* Set the directions into and out of cell */
		map[mylice.z][old_y][old_x].orient.dir[mylice.curr_dir] = 1;
		/* 0 -> 1. 1 -> 0. 2 -> 3. 3 -> 2 */
		old_dir += old_dir % 2 == 0 ? 1 : -1;
		map[mylice.z][old_y][old_x].orient.dir[old_dir] = 1; /* Needs to be the reverse of the old direction */
		/* 0 -> 1. 1 -> 0. 2 -> 3. 3 -> 2 */
		temp_dir = mylice.curr_dir;
		temp_dir += temp_dir % 2 == 0 ? 1 : -1;
		map[mylice.z][mylice.y][mylice.x].orient.dir[temp_dir] = 1;

		/* Update characters */
		update_symbol(old_x, old_y);
		update_symbol(mylice.x, mylice.y);

		writeCellFromMap(CELL_IN_FILE, mylice.z, old_y, old_x);
		writeCellFromMap(CELL_IN_FILE, mylice.z, mylice.y, mylice.x);

	} /* else do nothing */

EAM /* Enable ASCII mode */
}
}

void update_symbol(uint8_t x, uint8_t y)
{
uint8_t symbol_index = 0;
char sim[] = {'*', 'q', 'q', 'q', 'x', 'l', 'k', 'w', 'x', 'm', 'j', 'v', 'x', 't', 'u', 'n'};

/* Ensure cell exists or that it is the current licer cell */
if (map[mylice.z][y] != NULL && ((map[mylice.z][y][x].type != INACTIVE || (x == mylice.x && y == mylice.y))) 
    && map[mylice.z][y][x].type != PORTAL && map[mylice.z][y][x].type != PORTAL_ABOVE)
{
    /* Determine symbol */
    /* Check for surrounding cells UDLR */
    symbol_index |= ((map[mylice.z][y][x].orient.dir[2]) << 0); /* LEFT */
    symbol_index |= ((map[mylice.z][y][x].orient.dir[3]) << 1); /* RIGHT */
    symbol_index |= ((map[mylice.z][y][x].orient.dir[1]) << 2); /* UP */
    symbol_index |= ((map[mylice.z][y][x].orient.dir[0]) << 3); /* DOWN */

    map[mylice.z][y][x].orient.ch = sim[symbol_index]; /* Update symbol */
}
}

void traverseLayer(char layerUp) // Only if your above or below a portal
{
	enum cellType type;
	/* Moving up and layer to move up to? */
	if(mylice.z > 0 && layerUp) {
		type = map[mylice.z][mylice.y][mylice.x].type;
		if(type == PORTAL_ABOVE) { // Is the robot below a portal?
			/* Change layer, and move in current direction */
			if(switchLayerIntoProg(mylice.z-1)) {
				mylice.z--;
				robot_move(mylice.curr_dir, 1);
			}
		}
	}
	/* Moving down and layer to move down to? */
	else if(mylice.z < MAPLAYERS - 1 && !layerUp) {
		/* Get current cell's portal status */
		type = map[mylice.z][mylice.y][mylice.x].type;
		if(type == PORTAL) // Is portal?
		{
			if(switchLayerIntoProg(mylice.z+1)) {
				mylice.z++;	

				//update_portal_aboves(mylice.z);
				/* Allocate layer and row */
				allocate_if_null(mylice.z, mylice.y);

				/* Set cell below portal to type portal above */
				//map[mylice.z][mylice.y][mylice.x].type = PORTAL_ABOVE;

				/* Move LICER in direction it was moving before entering portal */
				robot_move(mylice.curr_dir, 1);
			}
		}
	}
}

void changeLayer(char layerUp)
{
	/* Move a layer up */
	if(mylice.z > 0 && layerUp) {
		if(switchLayerIntoProg(mylice.z-1)) /* Change the layers within program memory */
			mylice.z--;
	}
	/* Move a layer down */
	else if(mylice.z < MAPLAYERS - 1 && !layerUp) {
		if(switchLayerIntoProg(mylice.z+1)) { /* Change the layers within program memory */
			mylice.z++;
			//update_portal_aboves(mylice.z);
		}
	}
}

void changeType(uint8_t layer, uint8_t row, uint8_t col, enum cellType type)
{
	cell_t * portalAboveCell = NULL;
	char inProgram = (map[layer] != NULL);
	cell_t * cell;

	if(inProgram) {
 		if(map[layer][row] == NULL) {
			allocate_row(layer, row);
		}
		cell = &map[layer][row][col];
	}
	else {
		readCell(&cell, layer, row, col); // Find cell in file.
		if(cell == NULL) { // If cell is not in file:
			cell = malloc(sizeof(cell_t));
			set_cell_defaults(cell);
		}
	}

	if(type == PORTAL) {
		cell->type = type;

		if(layer < MAPLAYERS - 1) { // If there is a layer below:
			changeType(layer+1, row, col, PORTAL_ABOVE);
		}
	}
	else {
		if(cell->type == PORTAL) { // If cell is a portal:
			if(layer < MAPLAYERS - 1) { // If there is a layer below:
				changeType(layer+1, row, col, NORMAL);
			}
		}
		cell->type = type;
	}

	writeCell(CELL_IN_FILE, cell, layer, row, col);

	if(!inProgram) free(cell);

	return;
}