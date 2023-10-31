idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_TARGET} "NHL_Auth_Key.txt" TEXT)
littlefs_create_partition_image(littlefs ../littlefs FLASH_IN_PROJECT)