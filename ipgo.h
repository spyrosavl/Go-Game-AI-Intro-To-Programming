#define EMPTY 0
#define BLACK 1
#define WHITE 2
typedef struct go_cords cordinates;
typedef struct go_stone stone;
typedef struct go_groups group;
typedef struct go_board board;
typedef struct go_log board_log;
typedef struct Game_info game_info ;

struct go_cords{
	int x;
	int y;
} ;

struct go_stone{
	int color;
	cordinates  cords;
	int liberties ;
	group * stone_group;
} ;

struct go_groups{
	int color;
	int liberties;
	int no_stones;
	int stones_mem_alloced;
	stone ** stones;
};

struct go_board{
	int size;
	stone ** stones;
	int no_groups;
	int groups_mem_alloced;
	group * groups ;
} ;

struct go_log{
  int color;
  int x;
  int y;
} ;

struct Game_info
{
	board game_board ;
	double komi ;
	int japanese ;
	int b_captures ;
	int w_captures ;
	int seed;
	cordinates ko_point;
	int log_position;
	int log_mem_alloced;
	board_log * game_log ;
};

