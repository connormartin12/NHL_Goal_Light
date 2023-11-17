#ifndef https_interface_h
#define https_interface_h

#include <stdbool.h>

extern char *user_team_abbr;
extern char *other_team_abbr;
extern int user_team_score;
extern int other_team_score;
extern bool scored;
extern bool init_score;
extern bool game_found;

void https_get_score(char *team_abbrev);

#endif