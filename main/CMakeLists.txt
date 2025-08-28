idf_component_register(SRCS "main.c" "gt911_touch.c"
                       PRIV_REQUIRES esp_lcd
                       REQUIRES "ui"
                       INCLUDE_DIRS ".")
