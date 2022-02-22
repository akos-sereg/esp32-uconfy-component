idf_component_register(SRCS "config_poll.c" "config_repo.c" "nvs.c" "networking.c" "logging.c" "base64.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash wpa_supplicant)