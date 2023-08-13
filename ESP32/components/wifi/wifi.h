#ifndef wifi_h
#define wifi_h

esp_err_t wifi_connect_sta(const char *ssid, const char *pass);
void wifi_disconnect(void);

#endif