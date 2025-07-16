/*

status_bar.c - Callum MacDougall & Dylan Moore

File Description:
   Contains implementation for status_bar.h
    
*/

#include "status_bar.h"

void print_status_bar(enum mode mode)
{
/* Initialize variables */
char * status_msg; /* Prints on up to two lines */
cell_t curr_cell;
status_msg = malloc(sizeof(char) * STATUSBARSIZE);
if(mode == CURSOR) {
    curr_cell =  map[mylice.z][mylice.y][mylice.x];
    /* Generate the status string */
    sprintf(status_msg, "(X,Y,L): (%d,%d,%d). VIEW: (%d,%d). (f): %d. (BEC): %d. (ICE): %d%%. (RIT): %d. (TYPE): %d", 
        MAP_ZERO_COL - mylice.x, 
        MAP_ZERO_ROW - mylice.y,
        mylice.z + 1,
        VIEWPORT.TL_x + (VIEWPORT.width / 2),
        VIEWPORT.TL_y + (VIEWPORT.height / 2),
        curr_cell.prop.frictionLevel, 
        curr_cell.prop.radiationLevel, 
        curr_cell.prop.icePercentage, 
        curr_cell.prop.ritterbariumLevel,
        curr_cell.type
    );
}
else {
    sprintf(status_msg, "(X,Y,L): (%d,%d,%d). VIEW: (%d,%d).", 
        MAP_ZERO_COL - mylice.x, 
        MAP_ZERO_ROW - mylice.y,
        mylice.z + 1,
        VIEWPORT.TL_x + (VIEWPORT.width / 2),
        VIEWPORT.TL_y + (VIEWPORT.height / 2)
    );
}

/* Print the status bar */
print_msg(status_msg);
free(status_msg);
/* Reset screen colour */
printf(CSI "%dm" CSI "%dm", BGCYAN, FGBLACK);
}