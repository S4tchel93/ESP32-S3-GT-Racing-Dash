idf_component_register(SRCS "simhub_task.c" "main.c" "gt911_touch.c" "st7262_lcd.c" "lvgl_port.c" "simhub_task.c"
                       PRIV_REQUIRES esp_lcd
                       REQUIRES "ui" usb
                       INCLUDE_DIRS ".")
