#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "Screen.h"
#include "map.h"
#include "VT100.h"
#include "util.h"
//#include "Robot.h"

/* Definintions */
#define FILENAMEMAX 32
#define FILEEXTENSION ".licermap"
#define FILEEXTSIZE ((sizeof FILEEXTENSION)) 
#define READ_AND_LATCH 0
#define WRITE_AND_LATCH 1
#define MAXPROGLAYERS 2
#define LIST_END (-1ull)
#define BASE_PORTAL_ARRAY_SIZE 16

#define CELL_IN_FILE TRUE
#define CELL_NOT_IN_FILE FALSE

/* Enum describing the status of record */
enum cell_record_status {
    ACTIVE,
    DELETED
} typedef cell_record_status;

/* Structure containing the relavent information for storing a cell record */
struct cell_record {
    cell_record_status status; /* Is the cell active? */
    union data { /* Either used for cell data, or offset to next available cell */
        struct active { /* Cell data */
            cell_t cell_data;
            cell_pos pos;
            size_t next_in_layer;
        };
        struct deleted { /* Next available cell offset */
            size_t next_available;
        };
    };
} typedef cell_record;

/* structure for header format for file, contains the next available position to be written to (offset) */
struct header_record {
    size_t next_available;
    size_t layers_start[MAPLAYERS];
    uint8_t stored_last_layer;
} typedef header_record;

/* wokring file */
extern FILE * input_file;

/* 
Function description:
    Configures the file mode to binary mode, to work with fixed length files
Returns:
    None.
 */
void fileInit();
/* 
Function description:
    Displays screen for file picking. Requests for either the name of a new file to be written
    or the name of an existing file to be edited. 
Returns:
    None.
 */
void fileMenu();
/*  
Function description:
    Writes the cells stored within a file to the map array in program memory.
Returns:
    None.
 */
void fileReadIntoMap();
/*
Function description:
    Loads last_layer from file into progmem.
Returns:
    None.
*/
void fileReadLastLayer();
/*
Function description:
    Writes last_layer from progmem into file.
Returns:
    None.
*/
void fileWriteLastLayer();
/* 
Function description:
    Generates a new header, and writes it to the new file.
Returns:
    None.   
 */
void fileWriteNew();
/* 
Function description:
    Prompts the user for the file name in the format "Filename "Name".licermap"
    then gets the name typed into the keyboard.
Returns:
    None, pointers passed for file name string and string length.
 */ 
char fileReadName(char * filename, uint8_t* _strlen, unsigned maxFileLength);
/* 
Function description:
    Tries to open the file with the name of the passed string, in the cwd.
Returns:
    If the file was able to be opened (if it exists or not).
 */
char fileExists(char * filename);
/* 
Function description:
    Opens the file with the name of the string in its argument to be written to.
    Mode to write is "wb+" (write bytes). 
Returns:
    None.
 */
void fileOpenWrite(char * filename);
/* 
Function description:
    Opens the file with the name of the sting in the argument to be read.
    Mode to read is "rb+" (read bytes).
Returns:
    None.
 */
void fileOpenRead(char * filename);
/* 
Function description:
    Opens the debug file for writing. Then writes the contents of the working file
    to the debug file in a structured and reabable format.
Returns:
    None.
 */
void printDebugFile();
/* 
Function description:
    Moves the read/write location within the file to a given offset.
Returns:
    None.
 */
void fileMoveTo(long int offset, int whence);
/* 
Function description:
    Moves the read/write location within the file to the start of the cell records.
    !WARNING! Do not use with SEEK_SET! The header is sized differently, so the file location will become
    misaligned. Instead, use fileMoveToCellRecords, then use relative offsets to navigate file.
Returns:
    None.
 */
void fileMoveToCellRecords();
/* 
Function description:
    Checks if the position of the file pointer is at the end of the file. 
Returns:
    None
 */
void fileMoveToCellIndex(long int offset);
/* 
Function description:
    Checks if the position of the file pointer is at the end of the file. 
Returns:
    None
 */
char fileEOF();
/* 
Function description:
    Reads a record from the current working file into an instance of the record structure.
Returns:
    None, but the record being read into is passed as a pointer.
 */
void readRecord(cell_record* rec);
/* 
Function description:
    Writes a record into the current working file at the current file pointer's position.
Returns:
    None.
 */
void writeRecord(cell_record* rec);
/* 
Function description:
    Writes a header record (passed as an argument) to the current working file. 
    Can be written with same rules as fseek. 
Returns:
    None.
*/
void writeHeader(header_record* header);
/* 
Function description:
    Reads a header record from the current working file. 
Returns:
    None.
*/
void readHeader(header_record* header);
/* 
Function description:
    Updates the values of a cell within the file to match the values of the cell within the 
    program memory. 
Returns:
    None.
 */
void writeCellFromMap(char inFile, uint8_t layer, uint8_t row, uint8_t col);
/*
Function description:
    Writes a given cell from the map in program memory to the file
Returns:
    None.
*/
void writeCell(char inFile, cell_t * cell, uint8_t layer, uint8_t row, uint8_t col);
/*
Function description:
    Reads a cell from the file specified by a position into the cell pointer 
Returns:
    None. NULL pointer indicates cell not found in file.
*/
void readCell(cell_t ** cell, uint8_t layer, uint8_t row, uint8_t col);
/*
Function description:
    Gets the record index the file is pointing to.
Returns:
    Record index.
*/
size_t fileGetIndex();
/* 
Function description:
    Checks if the positions of two cells are equal. Used when searching for cells.
Returns:
    If the positions are equal.
 */
char positionsEqual(cell_pos *a, cell_pos *b);
/*
Function description:
    Deletes a cell within the file.
Returns:
    None.
*/
void deleteCellInFile(uint8_t layer, uint8_t row, uint8_t col);

/*
Function description:
    Prompts user to reduce layers allocated from 2 to 1. This leaves
    room to allocate another layer back up to 2.
Returns:
    Status
*/
char reduceLayersInProg(uint8_t *alloc_layers, uint8_t alloc_layer_count);

/*
Function description:
    Reads layer(s) into program memory.
Returns:
    None.
*/
void readLayers(uint8_t * layers, uint8_t size);

/*
Function description:
    Handles allocation checks, layer switch menu, deallocation and layer reading.
Returns:
    None.
*/
char switchLayerIntoProg(uint8_t layer);
/*
Function description:
    Changes the last layer in the header 
Returns:
    None.
*/
void updateLastLayer(uint8_t current_layer);

/*
Function description:
    Get all portals in a contiguous array. portal_pos_arr is stored in heap, assumed to point to nothing when passed.
Returns:
    None.
*/
extern void getPortals(uint8_t layer, cell_pos* portal_pos_arr, size_t* arr_size);