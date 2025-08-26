idf_component_register(SRCS "main.c" "ui/ui.c" "ui/ui_helpers.c" "ui/screens/ui_Screen1.c" "ui/images/ui_img_lovely_dash_png.c"
                       PRIV_REQUIRES esp_lcd
                       INCLUDE_DIRS "." "ui")
