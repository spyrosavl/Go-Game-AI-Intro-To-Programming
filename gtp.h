struct gtp_command{
	char command_name[40];
	int (*command_function)(int,  game_info *, char *);
};

void gtp_success(int  ,char * );
void gtp_failure(int ,char * );
int gtp_get_command(game_info *);
int gtp_protocol_version(int ,  game_info *, char *);
int gtp_name(int ,  game_info *, char *);
int gtp_version(int , game_info *, char *);
int gtp_known_command(int ,  game_info *, char *);
int gtp_list_commands(int ,  game_info *, char *);
int gtp_quit(int , game_info *,  char *);
int gtp_boardsize(int ,  game_info *, char *);
int gtp_clear_board(int ,  game_info *, char *);
int gtp_komi(int ,  game_info *, char *);
int gtp_play(int ,  game_info *, char *);
int gtp_genmove(int , game_info *,  char *);
int gtp_undo(int ,  game_info *, char *);
int gtp_fixed_handicap(int ,  game_info *, char *);
int gtp_place_free_handicap(int , game_info *,  char *);
int gtp_set_free_handicap(int ,  game_info *, char *);
int gtp_time_settings(int ,  game_info *, char *);
int gtp_time_left(int , game_info *, char *);
int gtp_final_score(int , game_info *, char *);
int gtp_final_status_list(int , game_info *, char *);
int gtp_showboard(int , game_info *, char *);


