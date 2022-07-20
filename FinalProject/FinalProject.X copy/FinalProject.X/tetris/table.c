/*
 * File:   Table.c
 * Author: Tsibulky David & Haham Omri 
 *
 * Created on February 13, 2022, 12:49 PM
 */


#include"xc.h"
#include"table.h"
//#include "shape1.h"


int ta_RemoveFullRowsAndUpdateScore(char table[][COLS],int* score)
{
    int rowHaveChanged  = -1; 
	int i, j, sum, count=0;  
	for(i=0;i<ROWS;i++){
        sum = 0; 
		for(j=0;j< COLS;j++) {
			if (table[i][j] != 0 && table[i][j] != '0')
            sum++;
		}
		if(sum==COLS){
			count++;
			int l, k;
			for(k = i;k >=1;k--)
				for(l=0;l<COLS;l++)
					table[k][l]=table[k-1][l];
			for(l=0;l<COLS;l++)
				table[k][l]='0';
            rowHaveChanged = i ; 
		}
	}
	*score += 100*count;
    return rowHaveChanged ; 
}