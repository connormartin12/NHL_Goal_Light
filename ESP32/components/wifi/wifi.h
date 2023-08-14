#ifndef wifi_h
#define wifi_h

void wifi_init(void);
esp_err_t wifi_connect_sta(const char *ssid, const char *pass);
void wifi_disconnect(void);

#endif