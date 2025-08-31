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
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
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
#include "user_colors.h"
#include "simhub_data.h"
#include "simhub_task.h"

#define USB_JTAG_UART_BUFF_SIZE (1024)

static const char *TAG = "example";

// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to protect it
static _lock_t lvgl_api_lock;
static QueueHandle_t xQueue;

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

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define NUM_LEDS 9
#define MIN_SEGMENTS 3.0

typedef struct {
    lv_obj_t* led;
    uint32_t color;
    uint8_t brightness;
} led_state_t;

static led_state_t rpm_leds[NUM_LEDS];

// Choose LED colors by index
static uint32_t get_led_color(uint8_t idx) {
    if (idx < 3) return GREEN_COLOR;
    else if (idx < 6) return YELLOW_COLOR;
    else return RED_COLOR;
}

static void set_led(uint8_t idx, uint32_t color, uint8_t brightness) {
    if (idx >= NUM_LEDS) return;

    led_state_t* l = &rpm_leds[idx];

    if (l->color != color) {
        lv_led_set_color(l->led, lv_color_hex(color));
        l->color = color;
    }

    if (l->brightness != brightness) {
        lv_led_set_brightness(l->led, brightness);
        l->brightness = brightness;
    }
}

void process_rpm_leds(const char* rpm, const char* redline_threshold) {
    uint16_t rpm_val = atoi(rpm);
    uint16_t redline = atoi(redline_threshold);

    if (rpm_val > redline) {
        // Over redline → flash all blue
        for (int i = 0; i < NUM_LEDS; i++) {
            set_led(i, BLUE_COLOR, 255);
        }
        return;
    }

    // Size of each segment
    float segment_size = (float)redline / NUM_LEDS;

    if(segment_size < 1.0 )
    {
        segment_size = MIN_SEGMENTS;
    }

    // Figure out which segment we're in
    int active_idx = (int)(rpm_val / segment_size);

    if (active_idx >= NUM_LEDS) active_idx = NUM_LEDS - 1;

    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t color = get_led_color(i);

        if (i < active_idx) {
            // Fully lit
            set_led(i, color, 255);
        } else if (i == active_idx) {
            // Partial brightness within segment
            float seg_start = i * segment_size;
            float seg_end   = (i + 1) * segment_size;
            uint8_t b = (uint8_t)map(rpm_val, seg_start, seg_end, 0, 255);
            set_led(i, color, b);
        } else {
            // Off
            set_led(i, color, 0);
        }
    }
}

void init_rpm_leds(void) {
    rpm_leds[0].led = objects.rpm_led_1;
    rpm_leds[1].led = objects.rpm_led_2;
    rpm_leds[2].led = objects.rpm_led_3;
    rpm_leds[3].led = objects.rpm_led_4;
    rpm_leds[4].led = objects.rpm_led_5;
    rpm_leds[5].led = objects.rpm_led_6;
    rpm_leds[6].led = objects.rpm_led_7;
    rpm_leds[7].led = objects.rpm_led_8;
    rpm_leds[8].led = objects.rpm_led_9;

    // Initialize the state so set_led() won’t skip first updates
    for (int i = 0; i < NUM_LEDS; i++) {
        rpm_leds[i].color = 0xFFFFFFFF;   // invalid color
        rpm_leds[i].brightness = 0xFF;    // invalid brightness
    }
}

typedef struct {
    lv_obj_t *label;
    const char *value;
} ui_label_update_t;

static void update_label_if_changed(lv_obj_t *label, const char *new_val) {
    const char *old_val = lv_label_get_text(label);
    if (strcmp(old_val, new_val) != 0) {
        lv_label_set_text(label, new_val);
    }
}

static void process_car_controls(const char* ignition, const char* wipers, const char* lights)
{
    static int prev_ignition = 0;
    static int prev_wipers = 0;
    static int prev_lights = 0;
    int curr_ignition = atoi(ignition);
    int curr_wipers = atoi(wipers);
    int curr_lights = atoi(lights);
    lv_color_t color;

    if(curr_ignition != prev_ignition)
    {
        color = curr_ignition == 1 ? lv_color_hex(GREEN_COLOR):lv_color_hex(GRAY_CAR_CONTROLS_COLOR);
        lv_obj_set_style_bg_color(objects.ignition_status, color, LV_PART_MAIN | LV_STATE_DEFAULT);
        prev_ignition = curr_ignition;
    }

    if(curr_wipers != prev_wipers)
    {
        color = curr_wipers == 1 ? lv_color_hex(GREEN_COLOR):lv_color_hex(GRAY_CAR_CONTROLS_COLOR);
        lv_obj_set_style_bg_color(objects.wipers_status, color, LV_PART_MAIN | LV_STATE_DEFAULT);
        prev_wipers = curr_wipers;
    }

    if(curr_lights != prev_lights)
    {
        color = curr_lights == 1 ? lv_color_hex(GREEN_COLOR):lv_color_hex(GRAY_CAR_CONTROLS_COLOR);
        lv_obj_set_style_bg_color(objects.lights_status, color, LV_PART_MAIN | LV_STATE_DEFAULT);
        prev_lights = curr_lights;
    }
}

static void process_delta_color(const char* delta_time)
{
    static char prev_sign = '-';
    char curr_sign = delta_time[0];
    lv_color_t color;

    if(curr_sign != prev_sign)
    {
        color = curr_sign != '-' ? lv_color_hex(RED_COLOR):lv_color_hex(GREEN_COLOR);
        lv_obj_set_style_bg_color(objects.lap_delta_value, color, LV_PART_MAIN | LV_STATE_DEFAULT);
        prev_sign = curr_sign;
    }

}

static void process_throttle_brake_bars(const char* throttle, const char* brake)
{
    static int prev_throttle = 0;
    static int prev_brake = 0;
    int curr_throttle = atoi(throttle);
    int curr_brake = atoi(brake);

    if(curr_throttle != prev_throttle)
    {
        lv_bar_set_value(objects.throttle_indicator, curr_throttle, LV_ANIM_ON);
        prev_throttle = curr_throttle;
    }
    if(curr_brake != prev_brake)
    {
        lv_bar_set_value(objects.brake_indicator, curr_brake, LV_ANIM_ON);
        prev_brake = curr_brake;
    }
}

static void ui_apply_simhub_data(const simhub_data_t *data) {

    char temp_session_num[15] = "";
    char temp_separator[2] = "/";
    char temp_sesion_time[10]="";

    // Prepare first the full session pos/opp count
    strcat(temp_session_num, data->position);
    strcat(temp_session_num, temp_separator);
    strcat(temp_session_num, data->opponent_count);

    //Remove hours from session time left, only keep minutes and seconds
    strcpy(temp_sesion_time, &data->session_time_left[3]);

    ui_label_update_t updates[] = {
        { objects.gear_value,           data->curr_gear },
        { objects.speed_value,          data->curr_speed },
        { objects.estimated_lap_value,  data->current_time },
        { objects.last_lap_value,       data->last_time },
        { objects.best_lap_value,       data->best_time },
        { objects.lap_delta_value,      data->delta_time },
        { objects.fl_tire_wear,         data->fl_wear },
        { objects.fr_tire_wear,         data->fr_wear },
        { objects.rl_tire_wear,         data->rl_wear },
        { objects.rr_tire_wear,         data->rr_wear },
        { objects.fl_tire_temp,         data->fl_tire_temp },
        { objects.fr_tire_temp,         data->fr_tire_temp },
        { objects.rl_tire_temp,         data->rl_tire_temp },
        { objects.rr_tire_temp,         data->rr_tire_temp },
        { objects.tc_value,             data->tc_level  },
        { objects.abs_value,            data->abs_level },
        { objects.map_value,            data->engine_map},
        { objects.bb_value,             data->bb_level  },
        { objects.fuel_value,           data->fuel      },
        { objects.fl_pressure,          data->fl_tyre_pressure },
        { objects.fr_pressure,          data->fr_tyre_pressure },
        { objects.rl_pressure,          data->rl_tyre_pressure },
        { objects.rr_pressure,          data->rr_tyre_pressure },
        { objects.fl_brake_temp,        data->fl_brake_temp },
        { objects.fr_brake_temp,        data->fr_brake_temp },
        { objects.rl_brake_temp,        data->rl_brake_temp },
        { objects.rr_brake_temp,        data->rr_brake_temp },
        { objects.session_type_val,     data->session_name },
        { objects.session_time_val,     temp_sesion_time },
        { objects.session_position_val, temp_session_num},
        { objects.session_lap_number_val,  data->curr_lap},
    };

    for (size_t i = 0; i < sizeof(updates)/sizeof(updates[0]); i++) {
        update_label_if_changed(updates[i].label, updates[i].value);
    }

    // RPM LEDs (special case)
    process_rpm_leds(data->rpm_percent, data->rpm_redline_threshold);

    process_car_controls(data->engine_ignition, data->wipers, data->headlights);

    process_delta_color(data->delta_time);

    process_throttle_brake_bars(data->throttle, data->brake);
}

static void ui_update_task(void *arg) {
    simhub_data_t simhub_data;
    init_rpm_leds();

    while (1) {
        if (xQueueReceive(xQueue, &simhub_data, portMAX_DELAY) == pdPASS) {
            _lock_acquire(&lvgl_api_lock);
            ui_apply_simhub_data(&simhub_data);
            _lock_release(&lvgl_api_lock);
        }
        taskYIELD();
        // vTaskDelay(10 / portTICK_PERIOD_MS);  // optional throttle
    }
}


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
    buf1 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_DMA| MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf1);
    buf2 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_DMA| MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf2);
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

    ESP_LOGI(TAG, "Create Queue for simhub and UI update tasks");
    xQueue = xQueueCreate(5U, sizeof(simhub_data_t)); // QUEUE_LENGTH is the depth, DataType is the type of data to be sent

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreatePinnedToCore(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL, 0);

    ESP_LOGI(TAG, "Display LVGL UI");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    ui_init();
    _lock_release(&lvgl_api_lock);

    ESP_LOGI(TAG, "Create SimHub task");
    xTaskCreatePinnedToCore(simhub_task, "SimHub", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, 1, NULL, 1);

    ESP_LOGI(TAG, "Create UI Update task");
    xTaskCreatePinnedToCore(ui_update_task, "UI_Update", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, 3, NULL, 1);
}

QueueHandle_t* Get_simhub_data_queue(void)
{
    return &xQueue;
}
