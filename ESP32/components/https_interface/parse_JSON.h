#ifndef parse_JSON_h
#define parse_JSON_h

#include "esp_err.h"

esp_err_t parse_buffer(char *buffer);
esp_err_t parse_score(char *liveScore);

#endif