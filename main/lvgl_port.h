#ifndef LVGL_PORT_H
#define LVGL_PORT_H 

/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/
#include "esp_lcd_panel_rgb.h"
#include "lvgl.h"

/***************************************************************************************************************************
 * PUBLIC DEFINITIONS
 ***************************************************************************************************************************/
#define LVGL_DRAW_BUF_LINES    50 // number of display lines in each draw buffer
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ

/***************************************************************************************************************************
 * PUBLIC DATA & TYPES
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * PUBLIC FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

extern bool lvgl_port_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_ctx);
extern void lvgl_port_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
extern void lvgl_port_increase_lvgl_tick(void *arg);
extern void lvgl_port_touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data);
extern void lvgl_port_task(void *arg);

#endif