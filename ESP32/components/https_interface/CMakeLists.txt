idf_component_register(SRCS "https_interface.c" "parse_JSON.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_client
                    REQUIRES json
                    EMBED_TXTFILES "../certs/NHL_API.crt" "../certs/collegefootballdata.crt")