#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipgo.h"
#include "play.h"
#include "score.h"
#include "board.h"
#include "strategy.h"

int valid_moves(game_info * Game, int color){
	int i,j, valid_moves=0;
	for (i = 0; i < Game->game_board.size; i++){
		for (j = 0; j < Game->game_board.size; j++)
			if(Game->game_board.stones[i][j].color == EMPTY){
				if(islegal(Game, i, j) && !is_suicide(Game, i, j, color) ){
					valid_moves++;
					break;
				}
			}
		if (valid_moves)
			break;
	}
	if (!valid_moves)
		return 0; /*pass */
	return 1;
}
int is_strategical_valid(game_info * Game, int x, int y,  int color){

	int valid_moves=0;
	int groups_captured, liberties, allies=0;
	stone ** p = Game->game_board.stones ;
	stone tmp_stone;
	tmp_stone.color = color;
	tmp_stone.cords.x = x;
	tmp_stone.cords.y = y;
	tmp_stone.stone_group = NULL;
	if (x>0 && p[x-1][y].color == color )
		allies++;
	if (y>0 && p[x][y-1].color == color  )
		allies++;
	if (x<Game->game_board.size-1 && p[x+1][y].color == color  )
		allies++;
	if (y<Game->game_board.size-1 && p[x][y+1].color == color  )
		allies++;

	groups_captured = number_of_enemy_groups_to_be_captured(Game, &tmp_stone );
	if(groups_captured==0 && allies==( (x>0) + (y>0) + (x<Game->game_board.size-1) + (y<Game->game_board.size-1) ) ){
		return 0;
	}	
	return 1;
}
int strategical_valid_moves(game_info * Game,  int color){
	int i,j;
	int valid_moves=0;
	int groups_captured, liberties, allies=0;
	stone ** p = Game->game_board.stones ;
	for (i = 0; i < Game->game_board.size; i++){
		for (j = 0; j < Game->game_board.size; j++)
			if(Game->game_board.stones[i][j].color == EMPTY){
				if(islegal(Game, i, j) && !is_suicide(Game, i, j, color) ){
					if (is_strategical_valid(Game, i, j, color)){
						valid_moves++;
						break;
					}

				}
			}
		if (valid_moves)
			break;
	}
	if (!valid_moves)
		return 0; /*pass */
	else
		return 1;
}

double play_random_games(game_info * Game, int color, int comp_color){
	int i, j, win_games=0, player_resp, opponent_resp, moves=0;
        int No_of_games_to_play ;
	int x,y;
	int Game_size = Game->game_board.size;
	if(Game_size < 5)
		No_of_games_to_play = 150;
	else if(Game_size < 10)
		No_of_games_to_play = 100;
	else if(Game_size < 15)
		No_of_games_to_play = 40;
	else 
		No_of_games_to_play = 10;

	for(i=0;i<No_of_games_to_play;i++){
		moves=0;
		do{	
			if(strategical_valid_moves(Game, color) ){
				do{
					x = rand()%Game->game_board.size;
					y = rand()%Game->game_board.size;
				}while(!islegal(Game, x, y) || !is_strategical_valid(Game, x, y, color) || !(player_resp=play(Game, x, y, color)) );
				if(player_resp>=1)moves++;	
			}else{
				player_resp=2;
			}
			if(strategical_valid_moves(Game, (color == BLACK)?(WHITE):(BLACK))){
				do{
					x = rand()%Game->game_board.size;
					y = rand()%Game->game_board.size;
				}while(!islegal(Game, x, y) || !is_strategical_valid(Game, x, y, (color == BLACK)?(WHITE):(BLACK)) ||!(opponent_resp = play(Game, x, y, (color == BLACK)?(WHITE):(BLACK) ))  );
				if(opponent_resp>=1)moves++;
			}else{
				opponent_resp=2;
			}
		}while((player_resp != 2 || opponent_resp != 2 )&& moves<=(Game->game_board.size*Game->game_board.size*3) );
		if( moves<=(Game->game_board.size*Game->game_board.size*3) && ( ((comp_color == BLACK && chinese_score(Game)>=0.0) || (comp_color == WHITE && chinese_score(Game)<=0.0)) ) )
			win_games ++ ;
		
		undo(Game, moves);
	}
	return ((double)win_games/(double)No_of_games_to_play);
}
double monte_carlo(game_info *  Game, int * x, int * y,  int color, int comp_color,  int depth){
	int i, j;
	double win_rate, max;
	double score;
	int tmp_played_games=0, tmp_won_games=0;
	int resp;
	int rand_x, rand_y;
	double minimax=2.0, tmp;
	if(depth== 1) return play_random_games(Game, color, comp_color);
	if(strategical_valid_moves(Game, color)==0) return ( (color == BLACK && chinese_score(Game)>=0.0) || (color == WHITE && chinese_score(Game)<=0.0) );	//printf("color =  %d, depth = %d", color, depth);
	for (i = 0; i < 10; i++){
 					do{
 						rand_x = rand()%Game->game_board.size;
 						rand_y = rand()%Game->game_board.size;
 					}while(!islegal(Game, rand_x, rand_y)  || !is_strategical_valid(Game, rand_x, rand_y, color) ||  !(resp = play(Game, rand_x, rand_y, color )) );
 					//printf("| %d,%d |", rand_x, rand_y);
 					if(color == BLACK){
 						if((tmp = monte_carlo(Game, x, y, WHITE, comp_color, depth+1)) >= minimax || minimax==2.0){
							minimax = tmp;
 							*x = rand_x;
 							*y = rand_y; 
						}
 					}else{
 						if((tmp = monte_carlo(Game, x, y, BLACK, comp_color, depth+1)) >= minimax || minimax==2.0){
							minimax = tmp;
 							*x = rand_x;
 							*y = rand_y; 
						}
 					}
 					
 					undo(Game, 1);
 	}
 	//printf("\n");
	return minimax;
}
