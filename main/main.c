/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"
#include "st7262_lcd.h"
#include "gt911_touch.h"
#include "lvgl_port.h"
#include "driver/usb_serial_jtag.h"
#include "simhub_task.h"
#include "ui_update_task.h"

#define USB_JTAG_UART_BUFF_SIZE (1024)

static const char *TAG = "SimHub-Dash";
// LVGL library is not thread-safe, this app will call LVGL APIs from different tasks, so use a mutex to protect it
static _lock_t lvgl_api_lock;

void app_main(void)
{
    // Configure USB SERIAL JTAG
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .rx_buffer_size = USB_JTAG_UART_BUFF_SIZE,
        .tx_buffer_size = USB_JTAG_UART_BUFF_SIZE,
    };

    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_serial_jtag_config));
    ESP_LOGI("usb_serial_jtag", "USB_SERIAL_JTAG init done");

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t* panel_handle_p = st7262_lcd_init();
    esp_lcd_panel_handle_t panel_handle = *panel_handle_p;
    
    ESP_LOGI(TAG, "Install TOUCH panel driver");
    esp_lcd_touch_handle_t* tp_handle_p = gt911_touch_init();
    esp_lcd_touch_handle_t tp_handle = *tp_handle_p;
    
    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // create a lvgl display
    lv_display_t *display = lv_display_create(LCD_H_RES, LCD_V_RES);

    // Initialize the touchpad input device
    /* Register a touchpad input device */
    lv_indev_t *indev = lv_indev_create(); 
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); // Set the input device type
    lv_indev_set_read_cb(indev, lvgl_port_touchpad_read); // Set the read callback function
    lv_indev_set_driver_data(indev, tp_handle); // Set driver data to the touch panel handle

    // associate the rgb panel handle to the display
    lv_display_set_user_data(display, panel_handle);
    // set color depth
    lv_display_set_color_format(display, LV_COLOR_FORMAT);
    // create draw buffers
    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // set LVGL draw buffers and direct mode
    lv_display_set_buffers(display, buf1, buf2, LCD_H_RES * LCD_V_RES * PIXEL_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
#else
    ESP_LOGI(TAG, "Allocate LVGL draw buffers");
    // it's recommended to allocate the draw buffer from internal memory, for better performance
    size_t draw_buffer_sz = LCD_H_RES * LVGL_DRAW_BUF_LINES * PIXEL_SIZE;
    buf1 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_DMA| MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf1);
    buf2 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_DMA| MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf2);
    // set LVGL draw buffers and partial mode
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif // CONFIG_EXAMPLE_USE_DOUBLE_FB

    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, lvgl_port_lvgl_flush_cb);

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_color_trans_done = lvgl_port_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, display));

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_port_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create Queue for simhub and UI update tasks");

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreatePinnedToCore(lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, &lvgl_api_lock, LVGL_TASK_PRIORITY, NULL, 0);

    ESP_LOGI(TAG, "Display LVGL UI");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    // This initializes the screen with the UI created on EEZ Studio
    ui_init();
    _lock_release(&lvgl_api_lock);

    ESP_LOGI(TAG, "Create SimHub task");
    xTaskCreatePinnedToCore(simhub_task, "SimHub", LVGL_TASK_STACK_SIZE, NULL, 1, NULL, 1);

    ESP_LOGI(TAG, "Create UI Update task");
    xTaskCreatePinnedToCore(ui_update_task, "UI_Update", LVGL_TASK_STACK_SIZE, &lvgl_api_lock, 3, NULL, 1);
}
