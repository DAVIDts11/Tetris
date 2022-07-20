/*
 * File:   newmainXC16.c
 * Author: Tsibulky David & Haham Omri 
 *
 * Created on January 4, 2022, 4:08 PM
 */
;

#define TRUE 1
#define FALSE 0
#define ROWS  16                 // 85*80
#define COLS  17


#include "FinalProject.X/tetris/shape1.h"
#include "xc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "FinalProject.X/System/system.h"
#include "FinalProject.X/System/delay.h"
#include "FinalProject.X/oledDriver/oledC.h"
#include "FinalProject.X/oledDriver/oledC_colors.h" 
#include "FinalProject.X/oledDriver/oledC_shapes.h" 
#include "FinalProject.X/tetris/cell.h"
#include "FinalProject.X/tetris/table.h"
#include "FinalProject.X/tetris/led3.h"
#include "FinalProject.X/tetris/initialize.h"

typedef struct Move {
    char MoveRight;
    char MoveLeft;
} Move;

//Global variables
char playAgain = TRUE;
char Pressed = FALSE;
Shape current, next;
char Table[ROWS][COLS]; // = {{0}};
int score = 0;
int moveToSideCounter = 0;
char GameOn = TRUE;
char colorsSet[3];
Move move;
char moved = TRUE;
int removed = -1;

void showTable(char table[][COLS], Shape* current, int row) {
    char score_string[7];
    char buffer[ROWS][COLS] = {0};
    int i, j;
    for (i = 0; i < current->width; i++) {
        for (j = 0; j < current->width; j++) {
            if (current->array[i][j])
                buffer[current->row + i][current->col + j] = current->array[i][j];
        }
    }

    oledC_clearScreen();
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++) {
            if (i != row) {
                if (table[i][j] != 0 && table[i][j] != '0') {
                    drawCell(i, j, table[i][j]);
                } else if (buffer[i][j] != 0 && buffer[i][j] != '0') {
                    drawCell(i, j, buffer[i][j]);
                }
            }
        }
    sprintf(score_string, "%d", score);
    oledC_DrawString(30, 81, 2, 2, score_string, OLEDC_COLOR_YELLOW);
}

void lightLed() {
    setColorLed(next.colorId, colorsSet);
    OC1R = colorsSet[0];
    OC2R = colorsSet[1];
    OC3R = colorsSet[2];
}

void blink(int removed) {
    int i;
    for (i = 0; i < 3; i++) {
        showTable(Table, &current, removed);
        showTable(Table, &current, -1);
    }
}

void __attribute__((interrupt)) _T1Interrupt(void) {
    int removed = -1;
    if (GameOn) {
        Shape temp = shape1_CopyShape(current);


        if (move.MoveLeft && moveToSideCounter < 3 && temp.col > 0 ) {
            temp.col--; //move left
            moved = TRUE;
            moveToSideCounter++;
        } else if (move.MoveRight && moveToSideCounter < 3 && (temp.col + temp.width < COLS - 1)) {
            temp.col++; //move right 
            moved = TRUE;
            moveToSideCounter++;
        } else {
            temp.row++; //move down
            moveToSideCounter = 0;
        }

        if (shape1_CheckPosition(temp, Table)) {
            shape1_DeleteShape(&current);
            current = shape1_CopyShape(temp);
        } else {
            shape1_WriteToTable(Table, &current);
            removed = ta_RemoveFullRowsAndUpdateScore(Table, &score);
            if (removed >=0 )
                blink(removed);
            shape1_SetNewRandomShape(&current, &next, Table, &GameOn);
            lightLed();
        }
        shape1_DeleteShape(&temp);
        showTable(Table, &current, -1);

    }
    IFS0bits.T1IF = 0;
}

void __attribute__((interrupt)) _IOCInterrupt(void) {

    if (GameOn) {
        Shape temp1 = shape1_CopyShape(current);

        if (IOCFAbits.IOCFA11 == 1 && IOCFAbits.IOCFA12 == 1) {
            T1CONbits.TCKPS = 0b01;
            IOCFAbits.IOCFA11 = 0;
            IOCFAbits.IOCFA12 = 0;
        } else {
            if (IOCFAbits.IOCFA11 == 1) {
                shape1_RotateShape_c(&temp1);
                IOCFAbits.IOCFA11 = 0;
            } else if (IOCFAbits.IOCFA12 == 1) {
                shape1_RotateShape_cc(&temp1);
                IOCFAbits.IOCFA12 = 0;
            }
            if (shape1_CheckPosition(temp1, Table)) {
                shape1_DeleteShape(&current);
                current = shape1_CopyShape(temp1);
            }
            shape1_DeleteShape(&temp1);
            showTable(Table, &current, -1);
        }
    } else {
        if (IOCFAbits.IOCFA12 == 1) {
            playAgain = TRUE;
        } else if (IOCFAbits.IOCFA11 == 1) {
            playAgain = FALSE;
            oledC_clearScreen();
            oledC_DrawString(10, 30, 2, 2, "BYE BYE ", OLEDC_COLOR_RED);
        }
        Pressed = TRUE;
    }
    IFS1bits.IOCIF = 0;
}

void initGame() {
    moved = TRUE;
    score = 0;
    inittable(Table);
    oledC_clearScreen();
    oledC_setBackground(OLEDC_COLOR_BLACK);
    GameOn = TRUE;
    next.width = 0;
    shape1_SetNewRandomShape(&current, &next, Table, &GameOn);
    Pressed = FALSE;
    lightLed();
}

int main(void) {

    SYSTEM_Initialize();
    User_initialize();

    int result, b;
    char mesurePoten;
    char score_string[6];

    //start game
    while (playAgain) {
        initGame();
        mesurePoten = FALSE;
        
        //loop for one game 
        while (GameOn) {

            if (moved)
                move.MoveLeft = move.MoveRight = FALSE;

            AD1CON1bits.SAMP = 1;
            for (b = 0; b < 20; b++) {
                DELAY_milliseconds(1);
            }
            AD1CON1bits.SAMP = 0;
            
            //Wait for  Potentiometer switched or game done 
            while (!AD1CON1bits.DONE && GameOn) {
            }

            // hold interrupts and if Potentiometer at one of the edges -  move  
            IFS0bits.T1IF = 1;
            result = (int) ADC1BUF0;
            if (result > 873) {
                move.MoveRight = TRUE;
                moved = FALSE;

            } else if (result < 150) {
                move.MoveLeft = TRUE;
                moved = FALSE;

            }
            IFS0bits.T1IF = 0;  
            
        }

        //after game has ended -  ask if continue 
        oledC_clearScreen();
        oledC_setBackground(OLEDC_COLOR_CHOCOLATE);
        sprintf(score_string, " %d", score);
        oledC_DrawString(10, 20, 1, 1, score_string, OLEDC_COLOR_YELLOW);
        oledC_DrawString(10, 40, 1, 1, "Play again ?", OLEDC_COLOR_GREEN);
        oledC_DrawString(10, 60, 1, 1, "press->", OLEDC_COLOR_GREEN);
        shape1_DeleteShape(&current);
        shape1_DeleteShape(&next);
        while (!Pressed) {
        }
    }
}
