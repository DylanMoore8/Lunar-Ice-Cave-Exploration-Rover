/*

CellEdit.h - Callum MacDougall & Dylan Moore

File Description:
    Provides interface for editing cell properties (editCell). Also provides
    user with dynamic number input field with error correction.

*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Screen.h"
#include "map.h"
#include "VT100.h"
#include "Robot.h"
#include "util.h"

#define MIN_BEC 0
#define MAX_BEC 100
#define MIN_FRIC 1
#define MAX_FRIC 10
#define MIN_RIT 0
#define MAX_RIT 100
#define MIN_ICE 0
#define MAX_ICE 100

#define ENABLESIGN 1
#define DISABLESIGN 0

/*
Function Description:
    Allows users to input numbers dynamically and safely.
Returns:
    Status, meaning if user successfully inputted a number or exited.
*/
int readNum(char * format, int *num, char allowSign, int lBound, int uBound, uint8_t curr, uint8_t curc);
/*
Function Description:
    Menu and redirecter for changing cell properties.
Returns:
    None.
*/
void editCell();
