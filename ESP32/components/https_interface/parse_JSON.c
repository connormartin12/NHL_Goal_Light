#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "parse_JSON.h"

// Included for debugging
#include "esp_heap_caps.h"

static const char *TAG = "PARSE JSON";

esp_err_t parse_buffer(char *bufferStr)
{
    // Printing buffer to check passed value is the same
    printf("%s\n", bufferStr);

    // Checking available heap memory
    int dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    printf("dram = %d\n", dram);

    cJSON *buffer_json = cJSON_Parse(bufferStr);
    // cJSON *buffer_json = cJSON_ParseWithLength(bufferStr, sizeof(bufferStr));
    // printf(buffer_json->valuestring);
    // char *buffer_string = cJSON_PrintUnformatted(buffer_json);
    // printf("%s\n", buffer_string);

    if(buffer_json == NULL)
    {
        const char *err = cJSON_GetErrorPtr();
        if(err)
        {
            ESP_LOGE(TAG, "Error parsing json before %s", err);
            return -1;
        }
    }
    cJSON *team_object = cJSON_GetObjectItemCaseSensitive(buffer_json, "teams");
    if (team_object == NULL) {
        printf("team_object is null\n");
    } else {
        char *team_object2 = cJSON_PrintUnformatted(team_object);
        printf("%s\n", team_object2);
    }
    // printf(team->valuestring);
    cJSON *teamAbbr = cJSON_GetObjectItemCaseSensitive(team_object, "name");
    // printf("name %s\n", teamID->valuestring);
    if (teamAbbr == NULL) {
        printf("teamID is null\n");
    } else {    
        char *team_id = cJSON_Print(teamAbbr);
        printf(team_id);
    }

    return ESP_OK;
}