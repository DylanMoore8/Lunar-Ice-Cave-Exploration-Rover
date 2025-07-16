#include "FileManager.h"

FILE * input_file;
header_record tmp_header;

void fileInit()
{
    _set_fmode(_O_BINARY);
    input_file = NULL;
}

void fileMenu()
{
    char filename[FILENAMEMAX + FILEEXTSIZE + 1];
    uint8_t filename_len;
    char ch;
    char option = -1; // -1 = no option picked; 0 = read; 1 = write
    char state = 0;
    char tmp_status;

    printf(CSI "%dm" CSI "%dm", BGBLACK, FGWHITE);

    // State machine
    while(state != -1) {
        CLRSCR
        CUP(1, 1);
        switch(state) {
            case 0: // Main Menu
                // Menu Screen:
                printf("LICER Map Creation Software - Dylan Moore and Callum MacDougall\n");
                printf("---------------------------------------------------------------\n");
                printf("[R]ead and Latch File\n");
                printf("[W]rite New and Latch File\n");
                while (_kbhit() == 0);
                state = 1;
                switch(ch = (char)_getch()) {
                    case 'R': case 'r':
                        option = READ_AND_LATCH; // read;
                        break; // state = 1
                    case 'W': case 'w':
                        option = WRITE_AND_LATCH; // write;
                        break; // state = 1
                    default:
                        state = 0;
                        break;
                }

                break;

            case 1: // Read File Name

                tmp_status = fileReadName(filename, &filename_len, FILENAMEMAX);
                
                // Append extension to end of filename
                //abc.lic*
                //  ^filename_len
                //       ^ NUL
                memcpy(filename + filename_len, FILEEXTENSION, FILEEXTSIZE);

                if(tmp_status == 1) { // filename read successful?
                    state = 2;
                }
                else {
                    state = 0;
                }
                
                break;

            case 2: // Read/Write File - Check if file exists

                if(!fileExists(filename)) {
                    if(option == READ_AND_LATCH) { // Trying to read a file that does not exist (create new file, prompt needed):
                        state = 3;
                    }
                    else { // Trying to write to a file that does not exist (create new file):
                        state = 5;
                    }
                }
                else {
                    if(option == READ_AND_LATCH) { // Trying to read a file that does exist (read in file):
                        state = 4;
                    }
                    else { // Trying to write to a file that does exist (overwrite, prompt needed):
                        state = 3;
                    }
                }

                break;
            
            case 3: // Read/Write File - File does not exist
                if(option == READ_AND_LATCH)
                    printf("File >%s< does not exist. Would you like to create a new file? [Y / N]", filename);
                else
                    printf("File >%s< already exists. Would you like to overwrite it? [Y / N]", filename);

                while (_kbhit() == 0);
                switch(ch = (char)_getch()) {
                    case 'Y': case 'y': case ENTER:
                        state = 5;
                        break;
                    case 'N': case 'n': case ESCAPE:
                        state = 0;
                        break;
                }
                break;
            case 4: // Read File - File does exist
                fileOpenRead(filename);
                fileReadLastLayer();
                readLayers((uint8_t[]){0, 1}, 2); // Load layers 0 and 1 (first two layers) into program memory
                state = -1;
                break;

            case 5: // Write File
                fileOpenWrite(filename);
                fileWriteNew();
                state = -1;
                break;
        }
    }
    
}

void fileReadIntoMap()
{
    cell_record tmpRec;
    cell_t tmpCell;
    size_t next_in_layer;

    CUP(1, 1)
    readHeader(&tmp_header);
    /* Read each record into a cell on each allocated layer */
    for(uint8_t layer = 0; layer < MAPLAYERS; layer++) { // 
        next_in_layer = tmp_header.layers_start[layer];
        while(next_in_layer != LIST_END) {
            fileMoveToCellIndex(next_in_layer);
            allocate_if_null(layer, tmpRec.pos.row);
            map[layer][tmpRec.pos.row][tmpRec.pos.col] = tmpRec.cell_data;

            next_in_layer = tmpRec.next_in_layer;
        }
    }
}

void fileReadLastLayer()
{
    fileMoveTo(0, SEEK_SET);
    readHeader(&tmp_header); /* Read last created layer from the header */
    last_layer = tmp_header.stored_last_layer;
}

void fileWriteLastLayer()
{
    readHeader(&tmp_header);
    /* Write last created layer to the header */
    tmp_header.stored_last_layer = last_layer; 
    writeHeader(&tmp_header);
}

char fileReadName(char * filename, uint8_t* _strlen, unsigned maxFileLength)
{
    uint8_t strlen = 0; 
    char state = 0;
    char ch;

    CLRSCR
    filename[0] = NUL;

    do {
        EL(1)
        CUP(1, 1);
        printf("Filename: %s" FILEEXTENSION, filename); /* Print file extention, and the part of the name written so far */

        while (_kbhit() == 0);
        ch = (char)_getch();

        switch(ch) {
            case ENTER: /* Name has been entered, return from function */
                state = 1;
                break;
            case BS: /* Back space received, print one less character in name */
                if(strlen > 0) {
                    filename[--strlen] = NUL;
                }
                break;
            case ESCAPE: /* Return */
                state = -1;
                break;
            default: /* Letter has been entered, increase the length of string anf append character */
                if(strlen < maxFileLength) {
                    filename[strlen++] = ch;
                    filename[strlen] = NUL;
                }
                break;
        }
    }
    while (!state);

    *_strlen = strlen;
    return state;

}

char fileExists(char *filename)
{
    /* Open file, return if it was able to be opened */
    FILE * file = fopen(filename, "r");
    if(file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void fileOpenWrite(char *filename)
{
    input_file = fopen(filename, "wb+"); /* Open file for writing */
    return;
}

void fileOpenRead(char *filename)
{
    input_file = fopen(filename, "rb+"); /* Open file for reading */
    return;
}

void printDebugFile()
{
    cell_record tmp;

    FILE * debug_out = fopen("debug.txt", "w");

    /* Print the header infromation */
    readHeader(&tmp_header);
    fprintf(debug_out, "Header:\n");
    fprintf(debug_out, "\tnext_available: %llu\n", tmp_header.next_available);
    fprintf(debug_out, "\tstored_last_layer: %hu\n", tmp_header.stored_last_layer);
    fprintf(debug_out, "\tlayers_start:\n");
    for(uint8_t i = 0; i < MAPLAYERS; i++) {
        fprintf(debug_out, "\t[%hu]: %llu\n", i, tmp_header.layers_start[i]);
    }

    fileMoveToCellRecords();
    int i = 0;
    while(!fileEOF()) { /* Print all the record characteristics for each record in the file */
        readRecord(&tmp);
        fprintf(debug_out, "Cell [%d]:\n", i);
        fprintf(debug_out, "\tStatus: %s\n", tmp.status == ACTIVE ? "ACTIVE" : "DELETED");
        if(tmp.status == ACTIVE) {
            fprintf(debug_out, "\tnext_in_layer: %llu\n", tmp.next_in_layer);
            fprintf(debug_out, "\tColumn: %hu\n", tmp.pos.col);
            fprintf(debug_out, "\tRow: %hu\n", tmp.pos.row);
            fprintf(debug_out, "\tChar: '%c'\n", tmp.cell_data.orient.ch);
            fprintf(debug_out, "\tType: %d\n", tmp.cell_data.type);
            fprintf(debug_out, "\tProperties:\n");
            fprintf(debug_out, "\t\tIce: %hu\n", tmp.cell_data.prop.icePercentage);
            fprintf(debug_out, "\t\tFriction: %hu\n", tmp.cell_data.prop.frictionLevel);
            fprintf(debug_out, "\t\tRadiation: %hu\n", tmp.cell_data.prop.radiationLevel);
            fprintf(debug_out, "\t\tRitterbarium: %hu\n", tmp.cell_data.prop.ritterbariumLevel);
        }
        else {
            fprintf(debug_out, "\tnext_available: %lld\n", tmp.next_available);
        }
        i++;
    }

    fclose(debug_out);
    return;
}

void fileMoveTo(long int offset, int whence)
{
    fseek(input_file, offset * sizeof(cell_record), whence); /* Move to specified record in file */
    return;
}

void fileMoveToCellRecords()
{
    fseek(input_file, sizeof(header_record), SEEK_SET); /* Move to specified record in file */
    return;
}

void fileMoveToCellIndex(long int offset)
{
    fileMoveToCellRecords();
    fileMoveTo(offset, SEEK_CUR);
    return;
}

char fileEOF()
{
    char ch, tmp;

    /* Refreshes file position */
    fseek(input_file, 0, SEEK_CUR);

    tmp = getc(input_file);
    if(!feof(input_file)) { /* Checks if at end of file, and returns result */
        fseek(input_file, -1, SEEK_CUR);
        return 0;
    }
    return 1;
}

void readRecord(cell_record* rec)
{
    fread(rec, sizeof(cell_record), 1, input_file); /* Reads record from input file*/
    return;
}

void writeRecord(cell_record *rec)
{
    fwrite(rec, sizeof(cell_record), 1, input_file); /* Writes record to input file */
    return;
}

void writeHeader(header_record* header)
{
    fseek(input_file, 0, SEEK_SET);
    fwrite(header, sizeof(header_record), 1, input_file); /* Writes record as a header record */
}

void readHeader(header_record *header)
{
    fseek(input_file, 0, SEEK_SET);
    fread(header, sizeof(header_record), 1, input_file);
}

void writeCell(char inFile, cell_t * cell, uint8_t layer, uint8_t row, uint8_t col)
{
    cell_record cellRec, tmp;
    size_t next_in_layer;
    char foundCell = FALSE;

    if (cell->type == INACTIVE)
        return; /* Cell is inactive */

    cellRec.status = ACTIVE;
    cellRec.cell_data = *cell;
    cellRec.pos.row = row;
    cellRec.pos.col = col;

    readHeader(&tmp_header);

    if(inFile) {
        /* Look for cell in file... */
        next_in_layer = tmp_header.layers_start[layer];
        while(next_in_layer != LIST_END && !foundCell) {
            fileMoveToCellIndex(next_in_layer);
            readRecord(&tmp);
            if(positionsEqual(&tmp.pos, &cellRec.pos)) {
                foundCell = TRUE;

                /* Add cell to list */
                
                /* Maintain list information */
                cellRec.next_in_layer = tmp.next_in_layer;

                /* Replace record tmp with cellRec */
                fileMoveTo(-1, SEEK_CUR);
                writeRecord(&cellRec);
            }
            else {
                next_in_layer = tmp.next_in_layer;
            }
        }
    }

    /*  Called if program thought cell was in file (through inFile parameter) but it wasn't found,  
        or if program said cell was not the file */
    if(!foundCell) {
        // Finding availbility...

        if(tmp_header.next_available != LIST_END) { // If there are available deleted records:
            fileMoveToCellIndex(tmp_header.next_available); // Align with first deleted record in list
            readRecord(&tmp); // Read in next deleted record

            // Pop from front of deleted list
            tmp_header.next_available = tmp.next_available;
            // Push to start of layers[layer] list
            cellRec.next_in_layer = tmp_header.layers_start[layer];

            fileMoveTo(-1, SEEK_CUR);
            tmp_header.layers_start[layer] = fileGetIndex();

        }
        else { // Move to end and write record there.

            // Push to start of layers[layer] list
            cellRec.next_in_layer = tmp_header.layers_start[layer];
            
            fileMoveTo(0, SEEK_END);
            tmp_header.layers_start[layer] = fileGetIndex();
        }

        writeRecord(&cellRec);
        writeHeader(&tmp_header);
        printDebugFile();
    }
    return;
}

void writeCellFromMap(char inFile, uint8_t layer, uint8_t row, uint8_t col)
{
    writeCell(inFile, &map[layer][row][col], layer, row, col);
    return;
}

void readCell(cell_t ** cell, uint8_t layer, uint8_t row, uint8_t col)
{
    size_t next_in_layer;
    cell_record tmp_record;
    cell_pos pos;

    pos.row = row;
    pos.col = col;

    readHeader(&tmp_header);

    next_in_layer = tmp_header.layers_start[layer];

    while(next_in_layer != LIST_END) {
        fileMoveToCellIndex(next_in_layer);
        readRecord(&tmp_record);
        if(positionsEqual(&pos, &tmp_record.pos)) {
            *cell = malloc(sizeof(cell_t));
            **cell = tmp_record.cell_data;
            return;
        }
        next_in_layer = tmp_record.next_in_layer;
    }

    *cell = NULL;
    return;

}

size_t fileGetIndex()
{
    return (ftell(input_file) - sizeof(header_record)) / sizeof(cell_record);
}

char positionsEqual(cell_pos *a, cell_pos *b)
{
    return /* Return if in all dimensions the positions are equal */
    a->row == b->row &&
    a->col == b->col;
}

void fileWriteNew()
{
    tmp_header.stored_last_layer = 0;
    tmp_header.next_available = LIST_END;
    for(uint8_t i = 0; i < MAPLAYERS; i++) {
        tmp_header.layers_start[i] = LIST_END;
    }
    writeHeader(&tmp_header); /* Write empty header to file */
    return;
}

void deleteCellInFile(uint8_t layer, uint8_t row, uint8_t col)
{
    cell_record curr_rec, prev_rec;
    cell_pos del_rec_pos;
    char foundCell;
    size_t curr_rec_index, prev_rec_index;

    /* Initialize del_rec comparator */
    del_rec_pos.row = row;
    del_rec_pos.col = col;

    /*  This state should be impossible for the previous record (because the current record comes after it). 
        If prev_rec has this property after going through the layer, then the program will know that prev_rec
        only copied curr_rec once, meaning the cell is the first in the list (special case).  */
    prev_rec_index = LIST_END;

    /* Get the offset of the header's next available cell */
    readHeader(&tmp_header);
    
    /* Initialize current record index at the start of the layer */
    curr_rec_index = tmp_header.layers_start[layer];

    /* Set the deleted cell's next available cell to the header's next available cell */
    /* Search for record to delete */
    foundCell = FALSE;
    while(curr_rec_index != LIST_END && !foundCell)
    {   
        fileMoveToCellIndex(curr_rec_index);
        readRecord(&curr_rec); /* Get next record in layer */
        if(positionsEqual(&curr_rec.pos, &del_rec_pos)) {
            foundCell = TRUE;
        }
        else {
            prev_rec_index = curr_rec_index; /* Update previous record */
            // Get index of next cell in file.
            curr_rec_index = curr_rec.next_in_layer;
        }
    };


    if (!foundCell) {
        return; /* Not in file */
    }
        
    if(prev_rec_index == LIST_END) { // If found cell is first item in layer list:
        tmp_header.layers_start[layer] = curr_rec.next_in_layer; // If only one element, still works
    }
    else { // If found cell is either in the middle or the end of the list:
        /* Replace link of "previous record" from "current record" to "record after current record", and update */
        fileMoveToCellIndex(prev_rec_index);
        readRecord(&prev_rec);

        prev_rec.next_in_layer = curr_rec.next_in_layer;

        fileMoveToCellIndex(prev_rec_index);
        writeRecord(&prev_rec);        
    }

    // Push current record to start of deleted list
    curr_rec.status = DELETED;
    curr_rec.next_available = tmp_header.next_available;
    tmp_header.next_available = curr_rec_index;

    // Update header and current record.
    writeHeader(&tmp_header);

    fileMoveToCellIndex(curr_rec_index);
    writeRecord(&curr_rec);
    
    return;
}

char reduceLayersInProg(uint8_t *alloc_layers, uint8_t alloc_layer_count)
{
    char msg[32];
    char ch;
    
    sprintf(msg, "Discard: [1] Layer %hu or [2] Layer %hu", alloc_layers[0]+1, alloc_layers[1]+1); /* Promt */
    print_msg(msg);
    while (_kbhit() == 0);
    ch = (char)_getch(); // Which character was pressed?

    for(int i = 0; i < 2; i++) {
        if(ch == DIGITTOCHAR(i+1)) {
            free(map[alloc_layers[i]]);
            map[alloc_layers[i]] = NULL;
            return 1;
        }
    }

    return -1; // Unsuccessful reduction
    
}

void readLayers(uint8_t *layers, uint8_t size)
{
    cell_record tmpRec;
    cell_t tmpCell;
    size_t next_in_layer;

    readHeader(&tmp_header);


    for(uint8_t i = 0; i < size; i++) { /* Read each record within layer into program memory */
        next_in_layer = tmp_header.layers_start[layers[i]];

        while(next_in_layer != LIST_END) {
            fileMoveToCellIndex(next_in_layer);   
            readRecord(&tmpRec); 
            allocate_if_null(layers[i], tmpRec.pos.row);
            map[layers[i]][tmpRec.pos.row][tmpRec.pos.col] = tmpRec.cell_data;
            
            next_in_layer = tmpRec.next_in_layer;
        }
    }
}

char switchLayerIntoProg(uint8_t layer)
{
    uint8_t alloc_layers[MAPLAYERS];
    uint8_t alloc_layer_count = 0;
    char status; 

    find_allocated_layers(alloc_layers, &alloc_layer_count);

    if(alloc_layer_count < MAXPROGLAYERS) { // No need to reduce, less than two layers allocated
        readLayers(&layer, 1); // Read desired layer into program memory.
        return 1; 
    }

    if(map[layer] != NULL) // No need to reduce, desired layer is already allocated
        return 1;
    
    status = reduceLayersInProg(alloc_layers, alloc_layer_count);

    if(status == -1) 
        return 0; // User did not wish to free layer.

    readLayers(&layer, 1); // Read desired layer into program memory.

    return 1;

}

void updateLastLayer(uint8_t current_layer)
{
    /* Update value stored as last layer */
    if(current_layer > last_layer) {
        last_layer = current_layer;
        fileWriteLastLayer();
    }
}

void getPortals(uint8_t layer, cell_pos *portal_pos_arr, size_t* arr_size)
{
    cell_record tmp;
    size_t index = 0;
    size_t next_in_layer;

    *arr_size = BASE_PORTAL_ARRAY_SIZE;

    portal_pos_arr = malloc(*arr_size * sizeof(cell_pos));
    
    readHeader(&tmp_header);
    next_in_layer = tmp_header.layers_start[layer];

    fileMoveToCellRecords(); /* Start after pointers */
    while(next_in_layer != LIST_END) { /* For each cell in the layer, if its a portal add it to the array of portals */
        readRecord(&tmp);
        if(tmp.cell_data.type == PORTAL) {
            if(index >= *arr_size) {
                *arr_size *= 2;
                portal_pos_arr = realloc(portal_pos_arr, *arr_size * sizeof(cell_pos));
            }
            portal_pos_arr[index] = tmp.pos;
            index++;
        }
    }

    return;
}
