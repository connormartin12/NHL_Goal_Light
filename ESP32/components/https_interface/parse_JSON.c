#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "parse_JSON.h"

// Included for debugging
#include "esp_heap_caps.h"

static const char *TAG = "PARSE JSON";

CJSON_PUBLIC(cJSON *) get_abbr(const cJSON * const items)
{
    cJSON *item = NULL;

    cJSON_ArrayForEach(item, items)
    {
        cJSON *this_item = cJSON_GetObjectItemCaseSensitive(item, "abbreviation");
        if (this_item != NULL)
            return this_item;
    }

    return NULL;
}

esp_err_t parse_abbreviation(char *bufferStr)
{
    // Checking available heap memory
    int dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    printf("dram = %d\n", dram);

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

    cJSON *teamData = cJSON_GetObjectItemCaseSensitive(buffer_json, "teams");
    // char *team = cJSON_Print(teamData);
    // printf("%s\n", team);

    cJSON *teamAbbr = get_abbr(teamData);
    
    printf("Team Abbreviation: %s\n", teamAbbr->valuestring);

    cJSON_Delete(buffer_json);

    return ESP_OK;
}

CJSON_PUBLIC(cJSON *) find_game(const cJSON * const games)
{
    const cJSON *game = NULL;

    cJSON_ArrayForEach(game, games)
    {
        cJSON *home_team = cJSON_GetObjectItemCaseSensitive(game, "homeTeam");
        cJSON *home_team_name = cJSON_GetObjectItemCaseSensitive(home_team, "name");

        cJSON *away_team = cJSON_GetObjectItemCaseSensitive(game, "awayTeam");
        cJSON *away_team_name = cJSON_GetObjectItemCaseSensitive(away_team, "name");

        if (strcmp(home_team_name->valuestring, "Oklahoma State Cowboys") == 0 || 
            strcmp(away_team_name->valuestring, "Oklahoma State Cowboys") == 0)
        {
            return (cJSON * const) game;
        }
    }

    return NULL;
}

esp_err_t parse_score(char *liveScore)
{
    // Printing memory that is left
    int dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    printf("dram = %d\n", dram);

    
    cJSON *games = cJSON_Parse(liveScore);
    if (games == NULL)
    {
        const char *err = cJSON_GetErrorPtr();
        if(err)
        {
            ESP_LOGE(TAG, "Error parsing json before %s", err);
            return -1;
        }
    }

    cJSON *game = find_game(games);

    cJSON *homeTeam = cJSON_GetObjectItemCaseSensitive(game, "homeTeam");
    cJSON *homeTeamName = cJSON_GetObjectItemCaseSensitive(homeTeam, "name");
    cJSON *homeTeamScore = cJSON_GetObjectItemCaseSensitive(homeTeam, "points");
    
    cJSON *awayTeam = cJSON_GetObjectItemCaseSensitive(game, "awayTeam");
    cJSON *awayTeamName = cJSON_GetObjectItemCaseSensitive(awayTeam, "name");
    cJSON *awayTeamScore = cJSON_GetObjectItemCaseSensitive(awayTeam, "points");
    
    printf("%s: %d\n%s: %d\n", homeTeamName->valuestring, homeTeamScore->valueint,
                               awayTeamName->valuestring, awayTeamScore->valueint);

    cJSON_Delete(games);

    return ESP_OK;
}