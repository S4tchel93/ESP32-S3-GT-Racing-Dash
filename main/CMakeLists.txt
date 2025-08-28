idf_component_register(SRCS "main.c" "gt911_touch.c" "st7262_lcd.c" "lvgl_port.c"
                       PRIV_REQUIRES esp_lcd
                       REQUIRES "ui"
                       INCLUDE_DIRS ".")
