idf_component_register(SRCS "audio.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver littlefs esp_partition esp_timer)