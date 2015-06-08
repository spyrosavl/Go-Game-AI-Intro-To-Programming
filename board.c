#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipgo.h"
#include "board.h"
int init_board(game_info  * Game, int size, double komi, int japanese, int seed){
	int i, j;
	if (size>25){fprintf(stderr, "Too big board size\n"); return 0;}
	Game->game_board.size = size;
	Game->game_board.no_groups = 0;
	Game->komi = komi;
	Game->japanese = japanese;
	Game->seed = seed;
	Game->ko_point.x = -1;
	Game->ko_point.y = -1;
	Game->b_captures = 0;
	Game->w_captures = 0;
	Game->log_position = 0;
	/*malloc for stones*/
	if( ((Game->game_board.stones) = malloc(size*sizeof(stone *))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	for (i = 0; i < size; i++)
	{
		if( ((Game->game_board.stones[i]) = malloc(size*sizeof(stone))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	}
	/*malloc for stone groups*/
	if( ((Game->game_board.groups) = malloc(20*sizeof(group ))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	Game->game_board.groups_mem_alloced = 20;

	/*malloc for log*/
	if( ((Game->game_log) = malloc(30*sizeof(board_log ))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	Game->log_mem_alloced = 30;
	return 1;
}
int set_board_size(game_info  * Game, int size){
	int i,j;
	if (size>25){ return 0;}
	Game->game_board.size = size;
	if( ((Game->game_board.stones) = malloc(size*sizeof(stone *))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	for (i = 0; i < size; i++)
	{
		if( ((Game->game_board.stones[i]) = malloc(size*sizeof(stone))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	}

	clear_board(Game);
	return 1;
}

int clear_board(game_info  * Game){
	int i,j;
	Game->game_board.no_groups = 0;
	Game->ko_point.x = -1;
	Game->ko_point.y = -1;
	Game->b_captures = 0;
	Game->w_captures = 0;
	Game->log_position = 0;
	if( ((Game->game_board.groups) = malloc(20*sizeof(group ))) == NULL){fprintf(stderr, "No more memory\n"); return 0;}
	Game->game_board.groups_mem_alloced = 20;
	for (i = 0; i < Game->game_board.size ; i++)
	{
		for (j = 0; j < Game->game_board.size ; j++)
		{
			Game->game_board.stones[i][j].color = EMPTY ;
		}
	}
	return 1;
}

int print_board(game_info  * Game){
	char Alphabet[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
		'Z' 
	};
	int i, j;
	printf("   ");
	for (i = 0; i < Game->game_board.size ; i++)
	{
		printf("%c ", Alphabet[i]);
	}
	printf("   \n");
	for (i = 0; i < Game->game_board.size ; i++)
	{	
		printf("%2d ", Game->game_board.size - i);
		for (j = 0; j < Game->game_board.size ; j++){
			switch(Game->game_board.stones[i][j].color){
				case BLACK:
					printf("X ");
					break;
				case WHITE:
					printf("O ");
					break;
				case EMPTY:
					printf(". ");
					break;
			};
		}
		printf("%-2d\n", Game->game_board.size - i);
	}
	printf("   ");
	for (i = 0; i < Game->game_board.size ; i++)
	{
		printf("%c ", Alphabet[i]);
	}
	printf("   \n");
	fflush(stdout);
	return 1;
}
