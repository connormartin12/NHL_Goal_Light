#ifndef parse_JSON_h
#define parse_JSON_h

#include "esp_err.h"

esp_err_t parse_abbreviation(char *buffer);
esp_err_t parse_score(char *bufferStr, char *user_team_name);
esp_err_t parse_football_score(char *liveScore);

#endif