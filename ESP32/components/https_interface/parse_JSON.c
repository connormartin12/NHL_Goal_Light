#include "parse_JSON.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "esp_log.h"
#include "https_interface.h"
#include "../nvs_storage/nvs_storage.h"

// Included for debugging
#include "esp_heap_caps.h"

static const char *TAG = "PARSE JSON";
char game_month[3];
char game_day[3];
char game_year[5];
char *user_team_abbr;
char *other_team_abbr;
int user_team_score = 0;
int other_team_score = 0;
bool init_score = true;
bool game_found = false;
bool scored = false;

void set_game_date(char *date)
{
    memcpy(game_year, strtok(date, "-"), sizeof(game_year));
    memcpy(game_month, strtok(NULL, "-"), sizeof(game_month));
    memcpy(game_day, strtok(NULL, "-"), sizeof(game_day));
}

void set_team_abbreviations(char *userTeam, char *otherTeam)
{
    user_team_abbr = (char *)calloc(sizeof(userTeam), sizeof(char));
    memcpy(user_team_abbr, userTeam, strlen(userTeam));
    other_team_abbr = (char *)calloc(sizeof(otherTeam), sizeof(char));
    memcpy(other_team_abbr, otherTeam, strlen(otherTeam));
}

esp_err_t parse_score(char *bufferStr, char *user_team_abbrev)
{
    int dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    printf("\ndram = %d\n", dram);

    cJSON *buffer_json = cJSON_Parse(bufferStr);
    if(buffer_json == NULL)
    {
        const char *err = cJSON_GetErrorPtr();
        if(err)
        {
            ESP_LOGE(TAG, "Error parsing json before %s", err);
            return -1;
        }
    }

    scored = false;

    // Checking Date Matches
    cJSON *games = cJSON_GetObjectItemCaseSensitive(buffer_json, "games");
    cJSON *game = cJSON_GetArrayItem(games, 0);
    cJSON *date = cJSON_GetObjectItemCaseSensitive(game, "gameDate");
    printf("Game Date: %s\n", date->valuestring);
    set_game_date(date->valuestring);
    if (game == NULL) {
        game_found = false;
        goto end;
    } else {
        game_found = true;
    }

    cJSON *gameState = cJSON_GetObjectItemCaseSensitive(game, "gameState");
    printf("%s\n\n", gameState->valuestring);

    cJSON *awayTeam = cJSON_GetObjectItemCaseSensitive(game, "awayTeam");
    cJSON *awayTeamAbbreviation = cJSON_GetObjectItemCaseSensitive(awayTeam, "abbrev");
    cJSON *homeTeam = cJSON_GetObjectItemCaseSensitive(game, "homeTeam");
    cJSON *homeTeamAbbreviation = cJSON_GetObjectItemCaseSensitive(homeTeam, "abbrev");

    int homeScore;
    int awayScore;

    if (strcmp(gameState->valuestring, "LIVE") == 0 || strcmp(gameState->valuestring, "OFF") == 0 ||
        strcmp(gameState->valuestring, "FINAL") == 0 || (strcmp(gameState->valuestring, "CRIT") == 0)) {
        cJSON *awayTeamScore = cJSON_GetObjectItemCaseSensitive(awayTeam, "score");
        cJSON *homeTeamScore = cJSON_GetObjectItemCaseSensitive(homeTeam, "score");
        homeScore = homeTeamScore->valueint;
        awayScore = awayTeamScore->valueint;
    } else {
        homeScore = 0;
        awayScore = 0;
    }

    if (strcmp(homeTeamAbbreviation->valuestring, user_team_abbrev) == 0) {
        if (!init_score && (homeScore > user_team_score)) {
            scored = true;
        }
        set_team_abbreviations(homeTeamAbbreviation->valuestring, awayTeamAbbreviation->valuestring);

        user_team_score = homeScore;
        other_team_score = awayScore;
    } else {
        if (!init_score && (awayScore > user_team_score)) {
            scored = true;
        }
        set_team_abbreviations(awayTeamAbbreviation->valuestring, homeTeamAbbreviation->valuestring);

        user_team_score = awayScore;
        other_team_score = homeScore;
    }
    if (init_score) 
        init_score = false;
    goto end;

end:
    cJSON_Delete(buffer_json);
    return ESP_OK;
}
