#include "parse_JSON.h"
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "https_interface.h"

// Included for debugging
#include "esp_heap_caps.h"

static const char *TAG = "PARSE JSON";

int user_team_score = 0;
int other_team_score = 0;
bool scored = false;

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

    scored = false;
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
            cJSON *home_team_score = cJSON_GetObjectItemCaseSensitive(home_team, "points");
            cJSON *away_team_score = cJSON_GetObjectItemCaseSensitive(away_team, "points");
            int home_score = home_team_score->valueint;
            int away_score = away_team_score->valueint;

            if (strcmp(home_team_name->valuestring, "Oklahoma State Cowboys") == 0)
            {
                if (home_score > user_team_score)
                    scored = true;
                user_team_score = home_score;
                other_team_score = away_score;
            }
            else
            {
                if (away_score > user_team_score)
                    scored = true;
                user_team_score = away_score;
                other_team_score = home_score;
            }

            goto end;
        }
    }

end:
    cJSON_Delete(games);
    return ESP_OK;
}