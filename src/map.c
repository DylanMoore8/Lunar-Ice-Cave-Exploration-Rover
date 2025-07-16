/*

map.c - Callum MacDougall & Dylan Moore

File Descripition:
    Includes implementations for CellEdit.h.
*/

#include "map.h"

/* Three dimentional array of structures */
cell_t *** map;
uint8_t last_layer;

int map_init()
{
/* Allocate memory for layers */
map = malloc(MAPLAYERS * sizeof(cell_t**));

/* Ensure allocation success */
if (map == NULL)
    return ALLOCATION_FAILURE; /* Allocation failure */

/* Set layers to NULL */
for (uint8_t layer = 0; layer < MAPLAYERS; layer++)
    map[layer] = NULL;

return ALLOCATION_SUCCESS; /* Allocation success */
}

int allocate_layer(uint8_t layer_num)
{
/* Allocated memory for rows in layer */
map[layer_num] = malloc(MAPROWS * sizeof(cell_t *));

/* Ensure allocation success */
if (map[layer_num] == NULL)
    return ALLOCATION_FAILURE;


/* Set rows to null */
for (int row = 0; row < MAPROWS; row++)
    map[layer_num][row] = NULL;

return ALLOCATION_SUCCESS; /* Allocation success */
}

int allocate_row(uint8_t layer_num, uint8_t row_num)
{
/* Allocated memory for cell in row */
map[layer_num][row_num] = malloc(MAPCOLUMNS * sizeof(cell_t));

/* Ensure allocation success */
if (map[layer_num] == NULL)
{
    free(map);
    return ALLOCATION_FAILURE;
}

/* Set defaults for cell */
for (int cell = 0; cell < MAPCOLUMNS; cell++)
{
    set_cell_defaults(&map[layer_num][row_num][cell]);
}

return ALLOCATION_SUCCESS; /* Allocation success */
}

void set_cell_defaults(cell_t * cell)
{
    /* Defaults for cell values */
    cell->orient.dir[0] = 0;
    cell->orient.dir[1] = 0;
    cell->orient.dir[2] = 0;
    cell->orient.dir[3] = 0;
    cell->orient.ch = CH_DEFAULT;
    cell->prop.frictionLevel = FRIC_DEFAULT;
    cell->prop.radiationLevel = RAD_DEFAULT;
    cell->prop.icePercentage = ICE_DEFAULT;
    cell->prop.ritterbariumLevel = RITT_DEFAULT;
    cell->type = INACTIVE;
}

void dealloc_map() {
    /* If map is not allocated, take no action */
    if(map == NULL)
        return;
    
    for(unsigned layer = 0; layer < MAPLAYERS; layer++) { // For each layer:
        if(map[layer] != NULL) { // If layer is allocated:
            for(unsigned row = 0; row < MAPROWS; row++) { // For each row in layer:
                if(map[layer][row] != NULL) { // If row is allocated:
                    free(map[layer][row]); // Deallocate row
                }
            }
            free(map[layer]); // Once allocated rows are deallocated, deallocate layer.
        }
    }
    free(map); // Once all layers deallocated, deallocate map object.

    return;
}

void find_allocated_layers(uint8_t *layers, uint8_t *size)
{
    uint8_t layer_index = 0, allocated_layer_index = 0;
    /* Must be within bounds */
    while(layer_index < MAPLAYERS) {
        /* If layer is not null, add it to the allocated layers list */
        if(map[layer_index] != NULL)
            layers[allocated_layer_index++] = layer_index;
        layer_index++; /* Go to next layer */
    }

    *size = allocated_layer_index;
    return;
}

void allocate_if_null(uint8_t layer_num, uint8_t row_num)
{
    /* Is layer allocated? */
    if (map[layer_num] == NULL)
	    allocate_layer(layer_num);
    /* Is row allocated? */
    if (map[layer_num][row_num] == NULL)
	    allocate_row(layer_num, row_num);
}

void update_portal_aboves(uint8_t layer)
{
    cell_pos *portal_above_arr;
    size_t arr_size;

    getPortals(layer - 1, portal_above_arr, &arr_size);

    for(size_t i = 0; i < arr_size; i++) {
        map[layer][ portal_above_arr[i].col ][ portal_above_arr[i].row ].type = PORTAL_ABOVE;
    }

    free(portal_above_arr);
}