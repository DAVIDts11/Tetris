/*
 * File:   cell.c
 * Author: Tsibulky David & Haham Omri 
 *
 * Created on February 9, 2022, 1:53 AM
 */

#include"xc.h"
#include "cell.h"
#include "../oledDriver/oledC_shapes.h"


void drawCell(int row, int col, char colorID) {
    uint16_t color = 0x0 ;
    switch (colorID) {
        case '1':   
            color = 0xffe0;   //YELLOW
            break;
        case '2':
            color = 0xa145;   //BROWN
            break;
        case '3':
            color = 0xfbea;   //CORAL
            break;
        case '4':
            color = 0x400;    //GREEN
            break;
        case '5':
            color = 0x4810;  //INDIGO
            break;
        case '6':
            color = 0xfd20;  //ORANGE
            break;
        case '7':
            color = 0xf800;  //RED
            break;
    }
    oledC_DrawRectangle(col * 5 + 5, row * 5, col * 5 + 10, row * 5 + 5, color);
}