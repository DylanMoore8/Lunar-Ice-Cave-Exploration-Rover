/* 

Robot.h - Callum MacDougall & Dylan Moore, original author: Larry Hughes

File Description:
    Provides basic robot structure, movement and entry points. Includes global
    robot "mylice" used by the rest of the program.

*/

#pragma once

#include "VT100.h"
#include "Screen.h"
#include "map.h"
#include "FileManager.h"

#define START_SYM	'*'

/* Directions */
enum direction { IDLE = -1, NORTH = 0, SOUTH, EAST, WEST} typedef dir_t;

/* Position and direction of robot */
struct robot
{
    int x;
    int y;
    int z;
    dir_t curr_dir;
};

/* Editing modes */
enum mode {
	DRAW,
	CURSOR
};

typedef struct robot ROBOT;

extern ROBOT mylice;

/*
Function description:
    Reset the position, and direction of the robot
Returns:
    None.
*/
extern void robot_reset();
/*
Function description:
    Initializes the robot with values for x, y, z and direction. Also allocates the starting position of robot.
Returns:
    None.
*/
extern void robot_init();
/*
Function description:
    Moves the robot in the direction that is indicated. When in draw mode, updates the symbols based on the move
    when in cursor mode, just moves the cursor to a new position, not drawing a path.
Returns:
    None.
*/
extern void robot_move(dir_t direction, enum mode mode);

/*
Function description:
    This function gets the character in a given cell.
Returns:
    None.
*/
extern void update_symbol(uint8_t x, uint8_t y);

/* In system.c: */
/*
Function description:
    Main controlling function of the robot. Loops, gets input from the keyboard, and instructs the robot based
    on the input.
Returns:
    None.
*/
extern void go_robot_go();

/*
Function description:
    Changes the layer the robot is on by + or - one when the robot is above or below a portal.
Returns:
    None.
*/
extern void traverseLayer(char layerUp);
/*
Function description:
    Changes the layer being displayed.
Returns:
    None.
*/
extern void changeLayer(char layerUp);

/*
Function description:
    Changes the type of a cell. Handles certain processes like
    setting portals along with above portals.
    Assumes:
        - If layer is in program memory, then the cell is allocated.
Returns:
    None.
*/
extern void changeType(uint8_t layer, uint8_t row, uint8_t col, enum cellType type);