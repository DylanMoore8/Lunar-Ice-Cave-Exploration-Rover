/*

CellEdit.c - Callum MacDougall & Dylan Moore

File Description:
    Provides implementations for CellEdit.h.

*/

#include "CellEdit.h"

void editCell()
{   
    // Temp variables
    char ch;
    enum cellType* type;
    int num;
    char state;
    char cellEdited = TRUE;
    // Selection Screen
    print_msg("[F]riction; [B]EC; [I]ce; [R]itterbarium; [T]ype");
    // Wait for input.
    while (_kbhit() == 0);
    switch(ch = (char)_getch()) { // Which character was pressed?
        case 'B': case 'b':
            state = readNum("BEC: %s (beq)", &num, DISABLESIGN, MIN_BEC, MAX_BEC, screen.min.col, screen.max.row); // Goto readNum for number input.
            if(state == 1) // If input was accepted:
                map[mylice.z][mylice.y][mylice.x].prop.radiationLevel = num; // Change property.
            break;
        case 'F': case 'f':
            state = readNum("Friction: %s", &num, DISABLESIGN, MIN_FRIC, MAX_FRIC, screen.min.col, screen.max.row);
            if(state == 1)
                map[mylice.z][mylice.y][mylice.x].prop.frictionLevel = num;
            break;
        case 'R': case 'r':
            state = readNum("Ritterbarium: %s", &num, DISABLESIGN, MIN_RIT, MAX_RIT, screen.min.col, screen.max.row);
            if(state == 1)
                map[mylice.z][mylice.y][mylice.x].prop.ritterbariumLevel = num;
            break;
        case 'I': case 'i':
            state = readNum("Ice: %s%%", &num, DISABLESIGN, MIN_ICE, MAX_ICE, screen.min.col, screen.max.row);
            if(state == 1)
                map[mylice.z][mylice.y][mylice.x].prop.icePercentage = num;
            break;
        case 'T': case 't':
            //type = &map[mylice.z][mylice.y][mylice.x].type;
            print_msg("[N]ormal; [P]ortal; [C]hasm");
            while (_kbhit() == 0);
            switch(ch = (char)_getch()) {
                case 'N': case 'n':
                    changeType(mylice.z, mylice.y, mylice.x, NORMAL);
                    //*type = NORMAL;
                    break;
                case 'P': case 'p':
                    changeType(mylice.z, mylice.y, mylice.x, PORTAL);
                    //*type = PORTAL;
                    break;
                case 'C': case 'c':
                    changeType(mylice.z, mylice.y, mylice.x, CHASM);
                    //*type = CHASM;
                    break;
                default:
                    cellEdited = FALSE;
            }
            break;
        default:
            cellEdited = FALSE;
            break;
    }
    if(cellEdited)
        writeCellFromMap(CELL_IN_FILE, mylice.z, mylice.y, mylice.x);
}

int readNum(char * format, int *num, char allowSign, int lBound, int uBound, uint8_t curr, uint8_t curc)
{
    /* Two stages of input used for negative and positive output */
    int tempNum = 0;
    int currNum = 0;

    /* Switch if number is signed or not */
    char sign = 0;

    char ch;

    char state = 0; // Status of Program. 1->Success, -1->Escaped
    char valid = (currNum <= uBound && currNum >= lBound);
    enum VT100_COLOURS numColor = valid ? FGGREEN : FGRED;
    char outputStr[64];
    char coloredNum[32];

    sprintf(coloredNum, CSI ";%dm%d" CSI ";39m", numColor, currNum);
    sprintf(outputStr, format, coloredNum);
    print_msg(outputStr);

    while(!state) {
        while (_kbhit() == 0);
        ch = (char)_getch();

        if(ch == ENTER) {
            if(valid) {
                state = 1;
                *num = currNum;
            }
        }
        else if(ch != ESCAPE) {
            if(ch >= '0' && ch <= '9') {
                tempNum *= 10;
                tempNum += ch - '0';
            }
            else if(ch == BS) {
                tempNum /= 10;
            }
            else if(ch == '-' && allowSign)
                sign = sign ? 0 : 1;

            if(tempNum > MAX_UCHAR)
                tempNum = MAX_UCHAR;

            currNum = sign ? -tempNum : tempNum;

            valid = (currNum <= uBound && currNum >= lBound);
            numColor = valid ? FGGREEN : FGRED;

            CUP(curr, curc)
            /* DEC colour: ESC [ <n> m */
            sprintf(coloredNum, CSI ";%dm%d" CSI ";39m", numColor, currNum);
            sprintf(outputStr, format, coloredNum);
            print_msg(outputStr);
        } 
        else {
            state = -1;
        }
    }
    return state;
}
