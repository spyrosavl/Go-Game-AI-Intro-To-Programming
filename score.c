#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipgo.h"



double chinese_score(game_info * Game){
	int black_area = 0, white_area = 0;
	int i,j;
	int game_size = Game->game_board.size;
	stone ** game_board = Game->game_board.stones;
	for (i = 0; i < game_size; i++)
		for (j = 0; j < game_size; j++)
		{
			if(game_board[i][j].color == BLACK)black_area ++; /*count black point if stone color is black */
			else if(game_board[i][j].color == WHITE)white_area ++; /*count white point if stone color is white */
			else if(game_board[i][j].color == EMPTY){ 
				if( !((i>0 && game_board[i-1][j].color != BLACK) ||
				(j>0 && game_board[i][j-1].color != BLACK) ||
				(i<game_size-1 && game_board[i+1][j].color != BLACK) ||
				(j<game_size-1 && game_board[i][j+1].color != BLACK)) )
					black_area ++; /* or black point if surrounded by black stones only */
				else if( !((i>0 && game_board[i-1][j].color != WHITE) ||
				(j>0 && game_board[i][j-1].color != WHITE) ||
				(i<game_size-1 && game_board[i+1][j].color != WHITE) ||
				(j<game_size-1 && game_board[i][j+1].color != WHITE) ))
					white_area ++; /* or white point if surrounded by white stones only */
			}
			
		}
	if(black_area-Game->komi - white_area != 0){
		return (double)(black_area - Game->komi - white_area);/*someone won if positive black if negative white*/
	}else{
		return 0.0;/*draw*/
	}
	
}
