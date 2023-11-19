idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

littlefs_create_partition_image(littlefs ../littlefs FLASH_IN_PROJECT)