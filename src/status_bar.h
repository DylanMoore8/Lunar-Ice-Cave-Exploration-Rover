/*

status_bar.h - Callum MacDougall & Dylan Moore

File Description:
    Contains function for printing cell data. 
    
*/

#pragma once

#include <stdio.h>
#include "map.h"
#include "Robot.h"
#include "Screen.h"
#include "VT100.h"
#include <string.h>
#include "viewport.h"

#define NUM_ATTR 4
#define TEMP_LEN 10
#define PORTALABOVESTR " (Portal Above) "
#define NUL '\0'
#define STATUSBARSIZE (screen.max.col * 2)

/*
Function description:
    This function prints the data for a given cell to the terminal.
Returns:
    None.
*/
extern void print_status_bar(enum mode mode);