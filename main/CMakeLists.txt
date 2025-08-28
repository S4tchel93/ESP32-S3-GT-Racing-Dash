idf_component_register(SRCS "main.c" "gt911_touch.c" "st7262_lcd.c"
                       PRIV_REQUIRES esp_lcd
                       REQUIRES "ui"
                       INCLUDE_DIRS ".")
