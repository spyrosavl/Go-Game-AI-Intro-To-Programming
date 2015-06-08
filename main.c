#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "ipgo.h"
#include "board.h"
#include "gtp.h"
#include "settings.h"
#define GET_ARG(ARG, VALUE) find_arg(argv, argc, ARG, VALUE)

int main(int argc, char  *argv[])
{	
	int i;
	int seed = time(NULL);
	int size = 18;
	double komi = 0;
	int japanese = 0;
	/* get_arg einai makroentoli poy kalei tin find_arg orismeni sto settings.c*/
	GET_ARG("-seed", &seed);
	GET_ARG("-size", &size);
	get_komi_arg(argv, argc, "-komi", &komi);
	GET_ARG("-japanese", &japanese);
	game_info Game;

	if(!init_board(&Game,  size,  komi, japanese, seed)){fprintf(stderr, "Unable to initialize game.\n"); return 0;}
	/* initialize a new game */
	gtp_get_command(&Game);	/* get gtp commands*/
	return 0;
}
