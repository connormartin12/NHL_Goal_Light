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
char *user_team_abbr;
char *other_team_abbr;
int user_team_score = 0;
int other_team_score = 0;
bool init_score = true;
bool game_found = false;
bool scored = false;

esp_err_t parse_score(char *bufferStr, char *user_team_abbrev)
{
    int dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    printf("\ndram = %d\n\n", dram);

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

    /* Get Time Function, Factor out in future */
    setenv("TZ", "GMT-6", 1);
    tzset();
    time_t current_date = time(NULL);
    struct tm time = *localtime(&current_date);
    char return_date[50]; // "yyyy-mm-dd"
    
    int month = time.tm_mon + 1;
    char month_str[15];
    if (month < 10) {
        sprintf(month_str, "0%d", month);
    } else {
        sprintf(month_str, "%d", month);
    }
    int day = time.tm_mday - 1;
    char day_str[15];
    if (day < 10) {
        sprintf(day_str, "0%d", day);
    } else {
        sprintf(day_str, "%d", day);
    }

    printf("Time: %d:%d\n", time.tm_hour, time.tm_min);

    sprintf(return_date, "%d-%s-%s", time.tm_year + 1900, month_str, day_str);
    /* End of get time function */

    printf("Current date: %s\nGame Date: %s\n", return_date, date->valuestring);
    if (strcmp(return_date, date->valuestring) == 0) {
        game_found = true;
    } else {
        goto end;
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
        user_team_abbr = (char *)calloc(sizeof(homeTeamAbbreviation->valuestring), sizeof(char));
        memcpy(user_team_abbr, homeTeamAbbreviation->valuestring, strlen(homeTeamAbbreviation->valuestring));
        other_team_abbr = (char *)calloc(sizeof(awayTeamAbbreviation->valuestring), sizeof(char));
        memcpy(other_team_abbr, awayTeamAbbreviation->valuestring, strlen(awayTeamAbbreviation->valuestring));

        user_team_score = homeScore;
        other_team_score = awayScore;
    } else {
        if (!init_score && (awayScore > user_team_score)) {
            scored = true;
        }
        user_team_abbr = (char *)calloc(sizeof(awayTeamAbbreviation->valuestring), sizeof(char));
        memcpy(user_team_abbr, awayTeamAbbreviation->valuestring, strlen(awayTeamAbbreviation->valuestring));
        other_team_abbr = (char *)calloc(sizeof(homeTeamAbbreviation->valuestring), sizeof(char));
        memcpy(other_team_abbr, homeTeamAbbreviation->valuestring, strlen(homeTeamAbbreviation->valuestring));

        user_team_score = awayScore;
        other_team_score = homeScore;
    }
    goto end;

end:
    cJSON_Delete(buffer_json);
    return ESP_OK;
}
