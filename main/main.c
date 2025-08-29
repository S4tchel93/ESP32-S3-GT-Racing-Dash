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
#include "screens.h"

static const char *TAG = "example";

// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to protect it
static _lock_t lvgl_api_lock;
static _lock_t simhub_data_lock;

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1) {
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        // in case of task watch dog timeout
        time_till_next_ms = MAX(time_till_next_ms, EXAMPLE_LVGL_TASK_MIN_DELAY_MS);
        // in case of lvgl display not ready yet
        time_till_next_ms = MIN(time_till_next_ms, EXAMPLE_LVGL_TASK_MAX_DELAY_MS);
        usleep(1000 * time_till_next_ms);
    }
}

static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)lv_indev_get_driver_data(indev_drv);
    assert(tp); // Ensure touchpad handle is valid

    uint16_t touchpad_x; // Variable for X coordinate
    uint16_t touchpad_y; // Variable for Y coordinate
    uint8_t touchpad_cnt = 0; // Variable for touch count

    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(tp); // Read data from touch controller

    /* Read data from touch controller */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, &touchpad_x, &touchpad_y, NULL, &touchpad_cnt, 1); // Get touch coordinates
    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x; // Set the X coordinate
        data->point.y = touchpad_y; // Set the Y coordinate
        data->state = LV_INDEV_STATE_PRESSED; // Set state to pressed
        ESP_LOGI(TAG, "Touch position: %d,%d", touchpad_x, touchpad_y); // Log touch position
    } else {
        data->state = LV_INDEV_STATE_RELEASED; // Set state to released
    }
}

#define BUF_SIZE (1024)
#define ECHO_TASK_STACK_SIZE (4096)

static void search_until(uint8_t* in_buf, uint8_t start_idx, uint8_t* out_len, char* out_buf)
{
    uint8_t count = 0;
    while(1)
    {
        if(in_buf[start_idx + count] != ';')
        {
            out_buf[count] = in_buf[start_idx + count];
            count++;
        }
        else
        {
            out_buf[count] = '\0';
            break;
        }
    }
    *out_len = count;
    //printf("Out length %d\n", count);
}


void read_until_delimiter(char*out_data, int* length)
{
    uint32_t index = 0;
    char data[BUF_SIZE];

    while(1)
    {
        int len = usb_serial_jtag_read_bytes(&data[index], 1,  5 / portTICK_PERIOD_MS);
        if(len > 0)
        {
            if(data[index] != ';')
            {
                //printf("RX char: %c \n", data[index]);
                out_data[index] = (char)data[index];
                index++;
            }
            else
            {
                data[index] = '\0';
                break;
            }  
        }
        else
        {
            break;
        }      
    }
    *length = index;
}

static void simhub_task(void *arg)
{
    while (1) 
    {
        //uint32_t timestart = esp_timer_get_time();
        //uint32_t readstart = 0;
        //uint32_t readend = 0;
        //uint32_t updatestart = 0;
        //uint32_t updateend = 0;

        int len_gear = 0;
        int len_speed = 0;
        int len;
        char curr_gear[2] = "N";
        char curr_speed[4] = "0";
        char current_time[9] = "00:00.00";
        char last_time[9] = "00:00.00";
        char best_time[9] = "00:00.00";
        char delta_time[7] = "+0.000";
        char fl_wear[6] = "";
        char fr_wear[6] = "";
        char rl_wear[6] = "";
        char rr_wear[6] = "";
        char fl_tire_temp[6] = "";
        char fr_tire_temp[6] = "";
        char rl_tire_temp[6] = "";
        char rr_tire_temp[6] = "";
        read_until_delimiter(curr_gear, &len_gear);
        if(len_gear > 0)
        {   
            //readstart = esp_timer_get_time();
            read_until_delimiter(curr_speed, &len_speed);
            read_until_delimiter(current_time, &len);
            read_until_delimiter(last_time, &len);
            read_until_delimiter(best_time, &len);
            read_until_delimiter(delta_time, &len);
            read_until_delimiter(fl_wear, &len);
            read_until_delimiter(fr_wear, &len);
            read_until_delimiter(rl_wear, &len);
            read_until_delimiter(rr_wear, &len);
            read_until_delimiter(fl_tire_temp, &len);
            read_until_delimiter(fr_tire_temp, &len);
            read_until_delimiter(rl_tire_temp, &len);
            read_until_delimiter(rr_tire_temp, &len);
            //readend = esp_timer_get_time();

            //updatestart = esp_timer_get_time();
            _lock_acquire(&lvgl_api_lock);
            lv_label_set_text(objects.gear_value, curr_gear);
            lv_label_set_text(objects.speed_value, curr_speed);
            lv_label_set_text(objects.estimated_lap_value, current_time);
            lv_label_set_text(objects.last_lap_value, last_time);
            lv_label_set_text(objects.best_lap_value, best_time);
            lv_label_set_text(objects.lap_delta_value, delta_time);
            lv_label_set_text(objects.fl_tire_wear, fl_wear);
            lv_label_set_text(objects.fr_tire_wear, fr_wear);
            lv_label_set_text(objects.rl_tire_wear, rl_wear);
            lv_label_set_text(objects.rr_tire_wear, rr_wear);
            lv_label_set_text(objects.fl_tire_temp, fl_tire_temp);
            lv_label_set_text(objects.fr_tire_temp, fr_tire_temp);
            lv_label_set_text(objects.rl_tire_temp, rl_tire_temp);
            lv_label_set_text(objects.rr_tire_temp, rr_tire_temp);
            _lock_release(&lvgl_api_lock);
            //updateend = esp_timer_get_time();
        }
        //uint32_t timeend = esp_timer_get_time();
        //printf("Reading Time: %ld \n", ((readend - readstart)/1000));
        //printf("LVGL Update Time: %ld \n", ((updateend - updatestart)/1000));
        //printf("Total Loop Time: %ld \n", ((timeend - timestart)/1000));
        
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    // Configure USB SERIAL JTAG
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .rx_buffer_size = BUF_SIZE,
        .tx_buffer_size = BUF_SIZE,
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
    lv_display_t *display = lv_display_create(EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);

    // Initialize the touchpad input device
    
    /* Register a touchpad input device */
    lv_indev_t *indev = lv_indev_create(); 
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); // Set the input device type
    lv_indev_set_read_cb(indev, touchpad_read); // Set the read callback function
    lv_indev_set_driver_data(indev, tp_handle); // Set driver data to the touch panel handle

    // associate the rgb panel handle to the display
    lv_display_set_user_data(display, panel_handle);
    // set color depth
    lv_display_set_color_format(display, EXAMPLE_LV_COLOR_FORMAT);
    // create draw buffers
    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // set LVGL draw buffers and direct mode
    lv_display_set_buffers(display, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * EXAMPLE_PIXEL_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
#else
    ESP_LOGI(TAG, "Allocate LVGL draw buffers");
    // it's recommended to allocate the draw buffer from internal memory, for better performance
    size_t draw_buffer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * EXAMPLE_PIXEL_SIZE;
    buf1 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf1);
    // set LVGL draw buffers and partial mode
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif // CONFIG_EXAMPLE_USE_DOUBLE_FB

    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_color_trans_done = example_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, display));

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreatePinnedToCore(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL, 0);

    ESP_LOGI(TAG, "Display LVGL UI");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    ui_init();
    _lock_release(&lvgl_api_lock);

    ESP_LOGI(TAG, "Create SimHub task");
    xTaskCreatePinnedToCore(simhub_task, "SimHub", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL, 1);
}
