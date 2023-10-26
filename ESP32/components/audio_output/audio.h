#ifndef audio_h
#define audio_h

#include "esp_err.h"

esp_err_t audio_init(void);
esp_err_t play_wav_file(char *volume_selection);

#endif