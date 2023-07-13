idf_component_register(SRCS "ota.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_client
                    REQUIRES esp_https_ota
                    REQUIRES esp_partition
                    REQUIRES app_update
                    EMBED_TXTFILES "../certs/google.crt")