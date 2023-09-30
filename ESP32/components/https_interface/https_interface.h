#ifndef https_interface_h
#define https_interface_h

#include <stdbool.h>

extern char *user_team_abbr;
extern char *other_team_abbr;
extern int user_team_id;
extern int other_team_id;
extern int user_team_score;
extern int other_team_score;
extern bool scored;
extern bool init_abbr;
extern bool game_found;

void https_get_score(char *team_name);
void https_get_abbr();

#endif