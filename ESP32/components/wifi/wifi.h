#ifndef wifi_h
#define wifi_h

#include "esp_err.h"

void wifi_init(void);
esp_err_t wifi_connect_sta(const char *ssid, const char *pass, int timeout);
void wifi_disconnect(void);

#endif