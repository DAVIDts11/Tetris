/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SHAPE1_H
#define SHAPE1_H

#define TRUE 1
#define FALSE 0
#define ROWS  16                 // 85*80
#define COLS  17

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


typedef struct {
    char **array;
    int width, row, col ;
    char colorId; 
} Shape;


const Shape shape1_ShapesArray[7]= {
	{(char *[]){(char []){'0','1','1'},(char []){'1','1','0'}, (char []){'0','0','0'}}, 3,0,0,'1'},                           //S shape     
	{(char *[]){(char []){'2','2','0'},(char []){'0','2','2'}, (char []){'0','0','0'}}, 3,0,0,'2'},                           //Z shape     
	{(char *[]){(char []){'0','3','0'},(char []){'3','3','3'}, (char []){'0','0','0'}}, 3,0,0,'3'},                           //T shape     
	{(char *[]){(char []){'0','0','4'},(char []){'4','4','4'}, (char []){'0','0','0'}}, 3,0,0,'4'},                           //L shape     
	{(char *[]){(char []){'5','0','0'},(char []){'5','5','5'}, (char []){'0','0','0'}}, 3,0,0,'5'},                           //flipped L shape    
	{(char *[]){(char []){'6','6'},(char []){'6','6'}}, 2,0,0,'6'},                                                 //square shape
	{(char *[]){(char []){'0','0','0','0'}, (char []){'7','7','7','7'}, (char []){'0','0','0','0'}, (char []){'0','0','0','0'}}, 4,0,0,'7'} //long bar shape
};



//Shape shape1_CopyShape(Shape shape){
//    Shape new_shape = shape ;
//    memcpy(&new_shape, &shape, sizeof(Shape));
//    memcpy(&new_shape.array[0][0], &shape.array[0][0],sizeof(shape.array));
////    new_shape.width = shape.width;
////    new_shape.col = shape.col ;
////    new_shape.row = shape.row ;
//    return new_shape;
//}

Shape shape1_CopyShape(Shape shape){
	Shape new_shape = shape;
	char **copyshape = shape.array;
	new_shape.array = (char**)malloc(new_shape.width*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.width; i++){
		new_shape.array[i] = (char*)malloc(new_shape.width*sizeof(char));
		for(j=0; j < new_shape.width; j++) {
			new_shape.array[i][j] = copyshape[i][j];
		}
    }
    return new_shape;
}

void shape1_DeleteShape(Shape * shape){
    int i;
    for(i = 0; i < shape->width; i++){
		free(shape->array[i]);
    }
    free(shape->array);
}

int shape1_CheckPosition(Shape shape,char table[][COLS]){ //Check the position of the copied shape
	char **array = shape.array;
	int i, j;
	for(i = 0; i < shape.width;i++) {
		for(j = 0; j < shape.width ;j++){
			if((shape.col+j < 0 || shape.col+j >= COLS || shape.row+i >= ROWS)){ //Out of borders
				if(array[i][j] && array[i][j] != '0') //but is it just a phantom?
					return FALSE;
				
			}
			else if(table[shape.row+i][shape.col+j] && array[i][j] && table[shape.row+i][shape.col+j] != '0' && array[i][j] != '0')
				return FALSE;
		}
	}
	return TRUE;
}

void shape1_SetNewRandomShape(Shape *current,Shape * next,char table[][COLS],char* GameOn){ //updates [current] with new shape
    Shape new_next_shape = shape1_CopyShape(shape1_ShapesArray[rand()%7]);
    new_next_shape.col = rand()%(COLS-new_next_shape.width+1);
    new_next_shape.row = 0;
   
    if(next->width == 0)
    {
       Shape new_curr_shape = shape1_CopyShape(shape1_ShapesArray[rand()%7]);
       new_curr_shape.col = rand()%(COLS-new_curr_shape.width+1);
       new_curr_shape.row = 0;
       shape1_DeleteShape(current);
	   *current = new_curr_shape;
       shape1_DeleteShape(next);
       *next = new_next_shape ;
    }else{
        shape1_DeleteShape(current);
        *current = shape1_CopyShape(*next);
        shape1_DeleteShape(next);
       *next = new_next_shape ;
    }
        

    
	if(!shape1_CheckPosition(*current,table)){
		*GameOn = FALSE;
	}
}

void shape1_RotateShape_c(Shape *shape){ //rotates clockwise
    char array[4][4];
    int i, j, k, width;
	width = shape->width;
    for(i=0;i<width;i++)
        for(j=0;j<width;j++)
            array[i][j] = shape->array[i][j];
        
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				shape->array[i][j] = array[k][i];
		}
	}
}

void shape1_RotateShape_cc(Shape *shape){ // rotates counterclockwise
    char array[4][4];
    int i, j, k, width;
	width = shape->width;
    for(i=0;i<width;i++)
        for(j=0;j<width;j++)
            array[i][j] = shape->array[i][j];
        
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				shape->array[k][i] = array[i][j];
		}
	}
}

void shape1_WriteToTable(char table[][COLS],Shape *current){
	int i, j;
	for(i = 0; i < current->width ;i++){
		for(j = 0; j < current->width ; j++){
			if(current->array[i][j] && current->array[i][j]!='0')
				table[current->row+i][current->col+j] = current->array[i][j];
		}
	}
}


// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

//#endif	/* XC_HEADER_TEMPLATE_H */
#endif
