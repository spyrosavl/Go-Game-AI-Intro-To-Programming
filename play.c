#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipgo.h"
#include "board.h"
#include "play.h"
#include "settings.h"
#include "strategy.h"
#define OPPONENT(X, COLOR) ( (X)!=(COLOR) && (X)!= EMPTY )

int is_ko_point(game_info * Game, int x, int y){ /* check the ko rule */

	if (Game->ko_point.x == x && Game->ko_point.y == y)
		return 1;
	else
		return 0;
}

int is_suicide(game_info * Game, int x, int y, int color){ /* check if suicide */
	stone ** p = Game->game_board.stones ;
	int liberties_and_ally = 0; 
	int max=0;
	stone tmp_stone;
	tmp_stone.color = color;
	tmp_stone.cords.x = x;
	tmp_stone.cords.y = y;
	tmp_stone.stone_group = NULL;
	int groups_captured = number_of_enemy_groups_to_be_captured(Game, &tmp_stone );
	if (x>0 && (p[x-1][y].color == EMPTY ||  p[x-1][y].color == color) )
	{	
		liberties_and_ally++;
		if(p[x-1][y].color == color && p[x-1][y].stone_group->liberties > max  )max = p[x-1][y].stone_group->liberties;
		/* max is the max of number of liberties the neighbor allies group have */
	}
	if (y>0 && ( p[x][y-1].color == EMPTY  || p[x][y-1].color == color) )
	{	
		if(p[x][y-1].color == color && p[x][y-1].stone_group->liberties > max )max = p[x][y-1].stone_group->liberties;
		liberties_and_ally++;
	}
	if (x<Game->game_board.size-1 && (p[x+1][y].color == EMPTY || p[x+1][y].color == color)  )
	{	
		if(p[x+1][y].color == color && p[x+1][y].stone_group->liberties > max )max = p[x+1][y].stone_group->liberties;
		liberties_and_ally++;
	}
	if (y<Game->game_board.size-1 && (p[x][y+1].color == EMPTY || p[x][y+1].color ==color ) )
	{	
		if(p[x][y+1].color == color && p[x][y+1].stone_group->liberties>max )max= p[x][y+1].stone_group->liberties;
		liberties_and_ally++;
	}
	if(groups_captured)
	{
		return 0;
	}
	if ( liberties_and_ally==0 || (liberties_and_ally!=0 && check_liberties(Game, &tmp_stone)==0 && max<=1)  )
	{
		return 1;
	}

	
	return 0;
}

int islegal(game_info * Game, int x, int y){
	/* check if legal move */
	if(!(x >= 0) || !(x<Game->game_board.size))
		return 0;
	else if (!(y >= 0) || !(y<Game->game_board.size))
		return 0;
	else if(Game->game_board.stones[x][y].color != EMPTY)
		return 0;
	else if(is_ko_point(Game, x, y))
		return 0;
	else
		return 1;
}
int check_allied_liberties(game_info * Game,int x, int y,  stone * current_stone){
	/* eksetazoyme an H liberty x,y tis current_stone anikei idi (san liberty) sto group tis current_Stone oste na min
	tin ksanametrisoyme */
	stone ** p = Game->game_board.stones ;
	group * stone_group = current_stone->stone_group;
	int color = current_stone->color;
	int c_x = current_stone->cords.x;
	int c_y = current_stone->cords.y;
	if (stone_group == NULL)/* an i petra den einai se kapoio group den exei noima ayti i synartisi */
		return 1;

	if (x>0 && p[x-1][y].color == color && p[x-1][y].stone_group == stone_group && ((x-1)!=c_x || y !=c_y) )
		return 0;
	
	else if (y>0 && p[x][y-1].color == color && p[x][y-1].stone_group == stone_group && (x!=c_x || (y-1)!=c_y) )
		return 0;

	else if (x<Game->game_board.size-1 && p[x+1][y].color == color && p[x+1][y].stone_group == stone_group && ((x+1)!=c_x || y !=c_y) )
		return 0;

	else if (y<Game->game_board.size-1 && p[x][y+1].color == color && p[x][y+1].stone_group == stone_group && (x!=c_x || (y+1) !=c_y) )
		return 0;

	else return 1;
}
int check_liberties(game_info * Game, stone * current_stone){
	/* vriskoyme tis liberties enos stone */
	int liberties=0;
	stone ** p = Game->game_board.stones ;
	int color = current_stone->color;
	int x = current_stone->cords.x;
	int y = current_stone->cords.y;
	group * stone_group = current_stone->stone_group ;

	if (x>0 && p[x-1][y].color == EMPTY && check_allied_liberties(Game, x-1, y, current_stone) )
	{
		liberties++;
	}
	if (y>0 && p[x][y-1].color == EMPTY  && check_allied_liberties(Game, x, y-1, current_stone) )
	{
		liberties++;
	}
	if (x<Game->game_board.size-1 && p[x+1][y].color == EMPTY && check_allied_liberties(Game, x+1, y, current_stone)  )
	{
		liberties++;
	}
	if (y<Game->game_board.size-1 && p[x][y+1].color == EMPTY && check_allied_liberties(Game, x, y+1, current_stone) )
	{
		liberties++;
	}
	return liberties;
}

int capture_group(game_info * Game, group * stone_group){
	int i, j, color ;
	if (stone_group->no_stones==1)/* for ko_point */
	{
		Game->ko_point.x = stone_group->stones[0]->cords.x;
		Game->ko_point.y = stone_group->stones[0]->cords.y;
	}
	/* kanoyme empty ola ta stone toy group */
	for (i = 0; i < stone_group->no_stones; i++)
	{
		color = stone_group->stones[i]->color;
		stone_group->stones[i]->color = EMPTY ;
		check_opponent_liberties_after_remove(Game, stone_group->stones[i], color);
		stone_group->stones[i]->stone_group = NULL ;
		

	}
	/* kai tis prosthetoyme  sta capture toy analogoy paikti */
	if (stone_group->color == WHITE)
		Game->w_captures += stone_group->no_stones;
	else
		Game->b_captures += stone_group->no_stones;
	/* to group pleon einai anenergo */
	stone_group->color = EMPTY ;
}

int number_of_enemy_groups_to_be_captured(game_info * Game, stone * current_stone){
	/* epistrefei 1 an me tin topothetisi tis current_stone ginete capture kapoio antipalo group , allios 0
	XORIS na kanoyme kapoio capture
	Einai xrisimi synartisi gia to check toy suicide */
	int i, j ;
	stone ** p = Game->game_board.stones ;
	int color = current_stone->color;
	int x = current_stone->cords.x;
	int y = current_stone->cords.y;
	if (x>0 && OPPONENT(p[x-1][y].color, color)  )
	{			
		if(!(p[x-1][y].stone_group->liberties-1))
			return 1;
	}
	if (y>0 && OPPONENT(p[x][y-1].color, color) )
	{	
		if(!(p[x][y-1].stone_group->liberties-1))
			return 1;
	}
	if (x<Game->game_board.size-1 && OPPONENT(p[x+1][y].color, color)  )
	{	
		if(!(p[x+1][y].stone_group->liberties-1))
			return 1;
	}
	if (y<Game->game_board.size-1 && OPPONENT(p[x][y+1].color, color)  )
	{	
		if(!(p[x][y+1].stone_group->liberties-1))
			return 1;
	}
	return 0;
}
int check_opponent_liberties_after_add(game_info * Game, stone * current_stone){
	/* koitame an meta tin topothetisi tis current_stone ginetai capture kapoio antipalo group
	KAI to kanoume capture */
	stone ** p = Game->game_board.stones ;
	int color = current_stone->color;
	int x = current_stone->cords.x;
	int y = current_stone->cords.y;
	group * stone_group = current_stone->stone_group ;
	group * modified_groups[4] = {NULL, NULL, NULL, NULL};/* kratame se poia group afairesame oste na min ksanakanoyme kati edw*/
	if (x>0 && OPPONENT(p[x-1][y].color, color)  )
	{
		if(!(--p[x-1][y].stone_group->liberties)){
				capture_group(Game, p[x-1][y].stone_group);
		}
		modified_groups[0] = p[x-1][y].stone_group;

	}
	if (y>0 && OPPONENT(p[x][y-1].color, color) 
		&& p[x][y-1].stone_group != modified_groups[0])
	{
		if(!(--p[x][y-1].stone_group->liberties)){
				capture_group(Game, p[x][y-1].stone_group);
		}
		modified_groups[1] = p[x][y-1].stone_group;
	}
	if (x<Game->game_board.size-1 && OPPONENT(p[x+1][y].color, color)   
		&& p[x+1][y].stone_group != modified_groups[1] &&  p[x+1][y].stone_group != modified_groups[0]  )
	{
		if(!(--p[x+1][y].stone_group->liberties)){
				capture_group(Game, p[x+1][y].stone_group);
		}
		modified_groups[2] = p[x+1][y].stone_group;
	}
	if (y<Game->game_board.size-1 && OPPONENT(p[x][y+1].color, color)  
		&& p[x][y+1].stone_group != modified_groups[2] && p[x][y+1].stone_group != modified_groups[1] 
		&&  p[x][y+1].stone_group != modified_groups[0])
	{
		if(!(--p[x][y+1].stone_group->liberties)){
				capture_group(Game, p[x][y+1].stone_group);
		}
		modified_groups[3] = p[x][y+1].stone_group;
	}
	return 1;
	

	
}
int check_opponent_liberties_after_remove(game_info * Game, stone * current_stone,int color){
	/*prosthetoyme analoga liberties se kathe group meta tin afairesi enos geitoniko toy antipalo stone */
	int i, j ;
	stone ** p = Game->game_board.stones ;
	int x = current_stone->cords.x;
	int y = current_stone->cords.y;
	group * modified_groups[4] = {NULL, NULL, NULL, NULL};/* kratame se poia group prosthesame oste na min ksanakanoyme to idio*/
	if (x>0 && OPPONENT(p[x-1][y].color, color)  )
	{	
		p[x-1][y].stone_group->liberties++;
		modified_groups[0] = p[x-1][y].stone_group;
	}
	if (y>0 && OPPONENT(p[x][y-1].color, color) 
		&& p[x][y-1].stone_group != modified_groups[0])
	{	
		p[x][y-1].stone_group->liberties++;
		modified_groups[1] = p[x][y-1].stone_group;
	}
	if (x<Game->game_board.size-1 && OPPONENT(p[x+1][y].color, color)   
		&& p[x+1][y].stone_group != modified_groups[1] &&  p[x+1][y].stone_group != modified_groups[0]  )
	{	
		p[x+1][y].stone_group->liberties++;
		modified_groups[2] = p[x+1][y].stone_group;
	}
	if (y<Game->game_board.size-1 && OPPONENT(p[x][y+1].color, color)  
		&& p[x][y+1].stone_group != modified_groups[2] && p[x][y+1].stone_group != modified_groups[1] 
		&&  p[x][y+1].stone_group != modified_groups[0])
	{	
		p[x][y+1].stone_group->liberties++;
		modified_groups[3] = p[x][y+1].stone_group;
	}
	return 1;
	

	
}
int add_to_group(game_info * Game, group * neighbor_group, stone * current_stone){
	int i, no;
	group * stone_group = current_stone->stone_group ;
	if ( current_stone->stone_group == NULL )
	{	
		/* realloc more space for stone groups if needed */
		if (neighbor_group->no_stones + 1 > neighbor_group->stones_mem_alloced){
			neighbor_group->stones_mem_alloced += 20;
			if (( neighbor_group->stones = realloc( neighbor_group->stones , (neighbor_group->stones_mem_alloced) * sizeof(stone *) ))  == NULL )return 0;//////warning/////////
		}
		/* add to group */
		current_stone->stone_group = neighbor_group;
		neighbor_group->stones[neighbor_group->no_stones] = current_stone;
		neighbor_group->no_stones++;
		/* update stone && group liberties*/
		current_stone->liberties = check_liberties(Game, current_stone);
		neighbor_group->liberties += (current_stone->liberties - 1);

	}else{
		if (current_stone->stone_group != neighbor_group){

			/* another ally group, merge groups */
			
			if (stone_group->no_stones + neighbor_group->no_stones +1> stone_group->stones_mem_alloced){
				stone_group->stones_mem_alloced += (20 +  neighbor_group->no_stones ) ;
				stone_group->stones = realloc( stone_group->stones , (stone_group->stones_mem_alloced ) * sizeof(stone *));
				if ( stone_group->stones   == NULL )return 0;
			}
			/*merge them into one stone_group*/
			no = stone_group->no_stones;
			for(i=0;i<neighbor_group->no_stones;i++){

				stone_group->stones[no+i] = neighbor_group->stones[i] ;
				stone_group->stones[no+i]->stone_group = stone_group ;
				stone_group->stones[no+i]->liberties = check_liberties(Game, stone_group->stones[no+i]);
				stone_group->liberties += stone_group->stones[no+i]->liberties;
				stone_group->no_stones++;

			}
			neighbor_group->color = EMPTY; //DEACTIVE IT
			
			
		}
	}
	return 1;

}
int create_group(game_info * Game, stone * current_stone){
	/* create new group*/
	group * new_group ;
	/*malloc memory if needed*/
	if (Game->game_board.no_groups +1 > Game->game_board.groups_mem_alloced ){			
			Game->game_board.groups_mem_alloced += 20 ;
			if(!realloc_and_update_groups(Game, Game->game_board.groups_mem_alloced))return 0;
	}

	/* insert into groups array */
	new_group = &(Game->game_board.groups[Game->game_board.no_groups]);
	Game->game_board.no_groups++;
	/* group values */
	new_group->color = current_stone->color;
	new_group->no_stones = 1;
	new_group->stones_mem_alloced =  20;

	current_stone->liberties =  check_liberties(Game, current_stone);
	new_group->liberties =  current_stone->liberties;

	/*maloc memory for stones */
	if (( new_group->stones = malloc( (new_group->stones_mem_alloced ) * sizeof(stone*)))  == NULL )return 0;//////warning/////////

	new_group->stones[0] = current_stone;
	current_stone->stone_group = new_group;
	return 1;
}

int check_for_group(game_info * Game, stone * current_stone){
	/* check for ally group after add current_stone into board*/
	stone ** p =  Game->game_board.stones ;
	int color = current_stone->color;
	int x = current_stone->cords.x;
	int y = current_stone->cords.y;
	int flag=0;
	if (x>0 && p[x-1][y].color == color)
	{	
		add_to_group(Game, p[x-1][y].stone_group , current_stone);
		flag = 1;
	}
	if (y>0 && p[x][y-1].color == color)
	{	
		add_to_group(Game, p[x][y-1].stone_group , current_stone);
		flag = 1;
	}
	if (x<Game->game_board.size-1 && p[x+1][y].color == color )
	{	
		add_to_group(Game, p[x+1][y].stone_group , current_stone);
		flag = 1;
	}
	if (y<Game->game_board.size-1 && p[x][y+1].color == color )
	{	
		add_to_group(Game, p[x][y+1].stone_group , current_stone);
		flag = 1;
	}
	if (!flag)/*if no ally group near create an new one and add this stone*/
	{

		create_group( Game,  current_stone);
	}
	return 1;

}
int add_stone(game_info * Game, int x, int y, int color){
	/* add a new stone into  game board*/
	Game->game_board.stones[x][y].color = color ;
	Game->game_board.stones[x][y].cords.x = x ;
	Game->game_board.stones[x][y].cords.y = y ;
	Game->game_board.stones[x][y].stone_group =  NULL  ;
	/*reset ko handler*/
	Game->ko_point.x = -1;
	Game->ko_point.y = -1;
	return 1;
}
int add_log(game_info * Game, int x, int y, int color){
	/* add move to game log*/
	if (Game->log_position + 1 > Game->log_mem_alloced ){			
			Game->log_mem_alloced += 30 ;
			Game->game_log = realloc( Game->game_log , (Game->log_mem_alloced ) * sizeof(board_log));
	}

	Game->game_log[Game->log_position].x = x;
	Game->game_log[Game->log_position].y = y;
	Game->game_log[Game->log_position].color = color;

	Game->log_position ++;
	return 1;
}
int play(game_info * Game, int x, int y, int color){
	int groups_captured;
	/* if x==y==-1 then pass */
	if (x==y && y==-1){
		add_log(Game, x, y, color);
		return 2;/* pass */
	}
	if (islegal(Game, x, y) && (color==BLACK || color==WHITE) &&  !is_suicide(Game, x, y, color))
	{
		add_stone(Game,  x,  y,  color);

		check_for_group(Game,  &Game->game_board.stones[x][y] ); /*check for allied groups for the played stone */
		/* first add the stone to group, and then remove captured groups so we can add liberties after the remove to neighbor groups */
		check_opponent_liberties_after_add(Game, &Game->game_board.stones[x][y] );

		add_log(Game, x, y, color);
		return 1;
	}
	return 0;
}

int genmove(game_info * Game, int color, int * x, int * y){
	srand(Game->seed);
	int i, j, valid_moves=0;
	for (i = 0; i < Game->game_board.size; i++){
		for (j = 0; j < Game->game_board.size; j++)
			if(Game->game_board.stones[i][j].color == EMPTY){
				if(islegal(Game, i, j) && is_strategical_valid(Game, i, j, color)  && !is_suicide(Game, i, j, color) ){
					valid_moves++;
					break;
				}
			}
		if(valid_moves)break;
	}
	if (!valid_moves){
		return 0; /*pass */
	}
	monte_carlo(Game, x, y, color, color, 0);
	play(Game, *x, *y, color);
	/*do {
	 	*x = rand()%Game->game_board.size;
	 	*y = rand()%Game->game_board.size;

	 }while(!islegal(Game, *x, *y) || !is_strategical_valid(Game, *x, *y, color) || !play(Game, *x, *y, color) );*/
	return 1;
}

int undo(game_info * Game, int level){
	/* leve is the number of move to go back*/
	if(Game->log_position==0)return 0;
	int i;
	int log_position = Game->log_position-level;
	clear_board(Game);
	/* clar the board and play again moves from 0 to current log position - leve */
	for(i=0;i<log_position;i++){
		play(Game, Game->game_log[i].x, Game->game_log[i].y, Game->game_log[i].color);
	}
	return 1;
}

