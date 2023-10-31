#include "parse_JSON.h"
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "https_interface.h"
#include "../nvs_storage/nvs_storage.h"

// Included for debugging
#include "esp_heap_caps.h"

static const char *TAG = "PARSE JSON";
char *user_team_abbr;
char *other_team_abbr;
int user_team_id = 0;
int other_team_id = 0;
int user_team_score = 0;
int other_team_score = 0;
bool init_score = true;
bool game_found = false;
bool scored = false;

cJSON *home_team_id = NULL;
cJSON *away_team_id = NULL;

void set_abbr_id(const cJSON * const home_team_info, const cJSON * const away_team_info)
{
    init_score = false;
    init_abbr = true;
    home_team_id = cJSON_GetObjectItemCaseSensitive(home_team_info, "id");
    away_team_id = cJSON_GetObjectItemCaseSensitive(away_team_info, "id");
}

esp_err_t parse_score(char *bufferStr, char *user_team_name)
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

    cJSON *items = cJSON_GetObjectItemCaseSensitive(buffer_json, "dates");
    cJSON *item = NULL;
    cJSON *games = NULL;
    cJSON_ArrayForEach(item, items)
    {
        games = cJSON_GetObjectItemCaseSensitive(item, "games");
        if (games != NULL)
            break;
    }

    cJSON *game = NULL;
    cJSON *teams = NULL;
    scored = false;
    cJSON_ArrayForEach(game, games)
    {
        teams = cJSON_GetObjectItemCaseSensitive(game, "teams");

        cJSON *home_team = cJSON_GetObjectItemCaseSensitive(teams, "home");
        cJSON *home_team_info = cJSON_GetObjectItemCaseSensitive(home_team, "team");
        cJSON *home_team_name = cJSON_GetObjectItemCaseSensitive(home_team_info, "name");

        cJSON *away_team = cJSON_GetObjectItemCaseSensitive(teams, "away");
        cJSON *away_team_info = cJSON_GetObjectItemCaseSensitive(away_team, "team");
        cJSON *away_team_name = cJSON_GetObjectItemCaseSensitive(away_team_info, "name");

        if (strcmp(home_team_name->valuestring, user_team_name) == 0 || 
            strcmp(away_team_name->valuestring, user_team_name) == 0)
        {
            game_found = true;

            cJSON *home_team_score = cJSON_GetObjectItemCaseSensitive(home_team, "score");
            cJSON *away_team_score = cJSON_GetObjectItemCaseSensitive(away_team, "score");
            int home_score = home_team_score->valueint;
            int away_score = away_team_score->valueint;

            if (strcmp(home_team_name->valuestring, user_team_name) == 0)
            {
                if (init_score) { // If this is first time getting score, it will not set off goal_scored function
                    set_abbr_id(home_team_info, away_team_info);
                    
                    user_team_id = home_team_id->valueint;
                    other_team_id = away_team_id->valueint;
                }    
                else if ((home_score > user_team_score))
                    scored = true;

                user_team_score = home_score;
                other_team_score = away_score;
            }
            else
            {
                if (init_score) {
                    set_abbr_id(home_team_info, away_team_info);

                    user_team_id = away_team_id->valueint;
                    other_team_id = home_team_id->valueint;
                }
                else if ((away_score > user_team_score))
                    scored = true;

                user_team_score = away_score;
                other_team_score = home_score;
            }

            goto end;
        }
    }

    ESP_LOGW(TAG, "No game found");
    game_found = false;
    goto end;

end:
    cJSON_Delete(buffer_json);
    return ESP_OK;
}

esp_err_t parse_abbr(char *bufferStr)
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

    cJSON *teams = cJSON_GetObjectItemCaseSensitive(buffer_json, "teams");
    cJSON *item = NULL;
    cJSON *abbreviation = NULL;
    cJSON_ArrayForEach(item, teams)
    {
        abbreviation = cJSON_GetObjectItemCaseSensitive(item, "abbreviation");
        if (abbreviation != NULL) {
            if (user_team_abbr == NULL) {
                user_team_abbr = (char *)calloc(sizeof(abbreviation->valuestring), sizeof(char));
                memcpy(user_team_abbr, abbreviation->valuestring, strlen(abbreviation->valuestring));
            } else {
                other_team_abbr = (char *)calloc(sizeof(abbreviation->valuestring), sizeof(char));
                memcpy(other_team_abbr, abbreviation->valuestring, strlen(abbreviation->valuestring));
            }
            goto end;
        }
    }

    ESP_LOGE(TAG, "Error parsing team abbreviation");
    goto end;

end:
    cJSON_Delete(buffer_json);
    return ESP_OK;
}