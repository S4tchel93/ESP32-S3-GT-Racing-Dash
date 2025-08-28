#ifndef LVGL_PORT_H
#define LVGL_PORT_H 

#include "esp_lcd_panel_rgb.h"
#include "lvgl.h"

#define EXAMPLE_LVGL_DRAW_BUF_LINES    50 // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (5 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ

extern bool example_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_ctx);
extern void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
extern void example_increase_lvgl_tick(void *arg);

#endif