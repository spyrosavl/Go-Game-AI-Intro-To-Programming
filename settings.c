#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipgo.h"
/*find command line arguments*/
int find_arg(char ** argv, int argc, char * str, int * val){
	int i ;
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], str))
		{	
			if (i+1< argc && val != NULL)
			{
				*val = atoi(argv[i+1]);
			}
			if (val == NULL)
			{
				*val = 1;
			}
			return 1;
		}
	}
	return 0;
}
/*find komi argument only because double*/
int get_komi_arg(char ** argv, int argc, char * str, double * val){
	int i ;
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], str))
		{	
			if (i+1< argc && val != NULL)
			{
				*val = atof(argv[i+1]);
			}
			if (val == NULL)
			{
				*val = 1;
			}
			return 1;
		}
	}
	return 0;
}
/*realloc memory for groups and update stones pointers to new groups */
int realloc_and_update_groups(game_info * Game, int size){
	int i,j;
	group * buffer = Game->game_board.groups;
	Game->game_board.groups = realloc(Game->game_board.groups, size * sizeof(group));
	if (Game->game_board.groups == NULL)
	{
		fprintf(stderr, "NO MORE MEMORY\n");
		return 0;
	}
	if(buffer == Game->game_board.groups)return 1;
	for(i = 0; i < Game->game_board.no_groups ; i++){
		if (Game->game_board.groups[i].color!=EMPTY)
				for(j = 0; j < Game->game_board.groups[i].no_stones ; j++)
					Game->game_board.groups[i].stones[j]->stone_group = &Game->game_board.groups[i];
	}
	
				
}
