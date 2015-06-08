#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipgo.h"
#include "board.h"
#include "gtp.h"
#include "play.h"
#include "score.h"
/* list of gtp commands */
struct gtp_command gtp_commands[] = {
	{"protocol_version" , &gtp_protocol_version},
	{"name" , &gtp_name},
	{"version" , &gtp_version},
	{"known_command" , &gtp_known_command},
	{"list_commands" , &gtp_list_commands},
	{"quit" , &gtp_quit},
	{"boardsize" , &gtp_boardsize},
	{"clear_board" , &gtp_clear_board},
	{"komi" , &gtp_komi},
	{"play" , &gtp_play},
	{"genmove" , &gtp_genmove},
	{"undo" , &gtp_undo},
	/*tournament*/
	//{"fixed_handicap" , &gtp_fixed_handicap},
	//{"place_free_handicap" , &gtp_place_free_handicap},
	//{"set_free_handicap" , &gtp_set_free_handicap},
	{"time_settings" , &gtp_time_settings},
	{"time_left" , &gtp_time_left},
	{"final_score" , &gtp_final_score },
	//{"final_status_list" , &gtp_final_status_list },
	/*debug*/
	{"showboard" , &gtp_showboard},
	{"STOP_HERE" , NULL},
}; 
/*eksodos se periptosi epitixias*/
void gtp_success(int id ,char * message){
	if (id!=-1)
		printf("=%d", id);
	else
		printf("=");
	if(message != NULL)
		printf(" %s", message);
	printf("\n\n");
	fflush(stdout);
	return ;
}
/*eksodos se periptosi apotyixias*/
void gtp_failure(int id ,char * message){
	if (id!=-1)
		printf("?%d", id);
	else
		printf("?");
	if(message != NULL)
		printf(" %s", message);
	printf("\n\n");
	fflush(stdout);
	return ;
}
/*klisi katallilon entolwn gtp*/
int gtp_get_command(game_info  * Game){
	char line[200];
	char * c;
	int id=-1, i, j, n=0;
	char char_id[10], command_name[40]; 
	
	while ( fgets (line, 200, stdin) !=NULL )
	{	
		i = 0;
		id = -1;
		n = 0 ;
		c = line;
		/*remove comment line*/
		if(*c == '#')continue;
		if(*c == '\n')continue;
		
		/* search for command */
		sscanf(c, "%39s", command_name);

		if(strlen(command_name)+1 < strlen(line))
			c += strlen(command_name)+1;
		else
			c = NULL ;

		while(gtp_commands[i].command_function!=NULL){
			if (!strcmp(gtp_commands[i].command_name, command_name))
			{
				(*gtp_commands[i].command_function)(id, Game, c);

				break;
			}else
				i++;
		}
		if(gtp_commands[i].command_function == NULL){
			gtp_failure(id, "unknown command");
		}
		if(!strcmp( command_name ,"quit")){
			return 1;
		}
	}
	return 1;	
}


int gtp_protocol_version(int id, game_info  * Game, char * argv){
		gtp_success(id, "2.0");
		return 1 ;
}


int gtp_name(int id, game_info  * Game, char * argv){
		gtp_success(id, "ipGo Avlonitis Spiridon");
		return 1;
}

int gtp_version(int id, game_info  * Game, char * argv){
		gtp_success(id, "1.0");
		return 1 ;
}

int gtp_known_command(int id, game_info  * Game, char * argv){
		int i = 0;
		char  command_name[25] ;
		sscanf(argv, "%24s", command_name);
		while(gtp_commands[i].command_function!=NULL){
			if (!strcmp(gtp_commands[i].command_name, command_name))
			{
				gtp_success(id,"true");
				return 1;
			}else
				i++;
		}
		
		gtp_failure(id, "false");
		return 1 ;
}

int gtp_list_commands(int id, game_info  * Game, char * argv){
		int i = 0;
		if (id!=-1)
			printf("=%d\n", id);
		else
			printf("=\n");
		while(gtp_commands[i].command_function!=NULL){
			printf("%s\n", gtp_commands[i].command_name );
			i++;
		}
		printf("\n");
		fflush(stdout);
		return 1 ;
}

int gtp_quit(int id, game_info  * Game, char * argv){
		free(Game->game_board.stones);
		free(Game->game_board.groups);
		gtp_success(id, "");
		return 1 ;
}

int gtp_boardsize(int id, game_info  * Game, char * argv){
		int size;
		if(argv==NULL){
			gtp_failure(id, "invalid syntax");
			return 1;
		}
		sscanf(argv, "%d", &size);
		if(!set_board_size(Game,  size)){gtp_failure(id, "Too big board size"); return 0;}
		gtp_success(id, "");
		return 1;
}

int gtp_clear_board(int id, game_info  * Game, char * argv){
		gtp_success(id, "");
		clear_board(Game);
		return 1;
}

int gtp_komi(int id, game_info  * Game, char * argv){
		double komi;
		if(argv==NULL){
			gtp_failure(id, "invalid syntax");
			return 1;
		}
		if(sscanf(argv, "%lf", &komi)>0)
			Game->komi = komi;
		gtp_success(id, "");
		return 1;
}
int get_color(char player[10]){
	char  * ch;
	ch = player;
	while(*ch!='\0'){
		*ch = tolower(*ch);
		ch++; 
	}
	if (!strcmp(player, "white") || !strcmp(player, "w"))
		return WHITE;
	else if (!strcmp(player, "black") || !strcmp(player, "b"))
		return BLACK;
	else 
	    return EMPTY;
}

int gtp_play(int id, game_info  * Game, char * argv){
		int x, y;
		char player[6], vertex[5];
		char ch;
		if(argv==NULL){
			gtp_failure(id, "invalid syntax");
			return 1;
		}
		sscanf(argv,"%5s %4s", player, vertex);
		if(get_color(player)==EMPTY){
			gtp_failure(id, "illegal move");
			return 0;
		}
		x=y=-1; /*pass default*/
		if (strcmp(vertex, "pass") != 0)
		{
			y = tolower(vertex[0]) - 'a';
			if(tolower(vertex[0]) > 'i')y = y-1; /* because I letter missing */
			x = Game->game_board.size - atoi(&vertex[1]);
		}
		if(play(Game, x, y, get_color(player))){/* if valid play */
			gtp_success(id, "");
		}else{
			gtp_failure(id, "illegal move");
		}
		
		return 1;
}

int gtp_genmove(int id, game_info  * Game, char * argv){
		char Alphabet[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
		'Z' 
		};
		int x,y;
		char player[6], vertex[5];
		if(argv==NULL){
			gtp_failure(id, "invalid syntax");
			return 1;
		}
		sscanf(argv,"%5s", player);
		if(get_color(player)==EMPTY){
			gtp_failure(id, "illegal move");
			return 0;
		}
		if(genmove(Game, get_color(player) , &x, &y))
			sprintf(vertex, "%c%d", Alphabet[y], Game->game_board.size-x );
		else
			sprintf(vertex, "PASS");
		gtp_success(id, vertex);
		
		return 1;
}

int gtp_undo(int id, game_info  * Game, char * argv){
		if(undo(Game, 1)){
			gtp_success(id, "");
		}
		else{
			gtp_failure(id, "cannot undo");
		}
		return 1;
}

int gtp_fixed_handicap(int id, game_info  * Game, char * argv){
		gtp_success(id, "2.0");/*NOT WORKING*/
}

int gtp_place_free_handicap(int id, game_info  * Game, char * argv){
		gtp_success(id, "2.0");/*NOT WORKING*/
}

int gtp_set_free_handicap(int id, game_info  * Game, char * argv){
		gtp_success(id, "2.0");/*NOT WORKING*/
}

int gtp_time_settings(int id, game_info  * Game, char * argv){
		gtp_success(id, "");/*NOT WORKING*/
}

int gtp_time_left(int id, game_info  * Game, char * argv){
		gtp_success(id, "");/*NOT WORKING*/
}

int gtp_final_score(int id, game_info  * Game, char * argv){
		double score = chinese_score(Game);
		char winner[10];
		if (!score)
			gtp_success(id, "0");
		else if (score>0){
			sprintf(winner, "B+%2.1f", score);
			gtp_success(id, winner);
		}
		else {
			sprintf(winner, "W+%2.1f", score*(-1));
			gtp_success(id, winner);
		}
		return 1 ;
}

int gtp_final_status_list(int id, game_info  * Game, char * argv){
		gtp_success(id, "");/*NOT WORKING*/
		return 1 ;
}

int gtp_showboard(int id, game_info  * Game, char * argv){
		if (id!=-1)
			printf("=%d\n", id);
		else
			printf("=\n");
		print_board(Game);
		printf("\n");
		fflush(stdout);
		return 1;


}





