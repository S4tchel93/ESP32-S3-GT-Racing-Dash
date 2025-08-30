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
QueueHandle_t xQueue;

typedef struct simhub_data_t
{
    char curr_gear[2];
    char curr_speed[4];
    char rpm_percent[4];
    char rpm_redline_threshold[4] ;
    char current_time[9];
    char last_time[9];
    char best_time[9];
    char delta_time[7];
    char fl_wear[6];
    char fr_wear[6];
    char rl_wear[6];
    char rr_wear[6];
    char fl_tire_temp[6];
    char fr_tire_temp[6];
    char rl_tire_temp[6];
    char rr_tire_temp[6];
}simhub_data_t;

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

#define SIMHUB_SYNC "SH;"
#define SIMHUB_MAX_FIELDS 16

typedef struct {
    char *dest;      // pointer into struct field
    size_t max_size; // including null terminator
} simhub_field_t;

static simhub_field_t simhub_fields[] = {
    { NULL, 2 },   // curr_gear
    { NULL, 4 },   // curr_speed
    { NULL, 4 },   // rpm_percent
    { NULL, 4 },   // rpm_redline_threshold
    { NULL, 9 },   // current_time
    { NULL, 9 },   // last_time
    { NULL, 9 },   // best_time
    { NULL, 7 },   // delta_time
    { NULL, 6 },   // fl_wear
    { NULL, 6 },   // fr_wear
    { NULL, 6 },   // rl_wear
    { NULL, 6 },   // rr_wear
    { NULL, 6 },   // fl_tire_temp
    { NULL, 6 },   // fr_tire_temp
    { NULL, 6 },   // rl_tire_temp
    { NULL, 6 }    // rr_tire_temp
};

static bool wait_for_sync(void) {
    const char *sync = SIMHUB_SYNC;
    int matched = 0;

    while (1) {
        char c;
        int len = usb_serial_jtag_read_bytes(&c, 1, 50 / portTICK_PERIOD_MS);
        if (len <= 0) {
            return false; // timeout
        }

        if (c == sync[matched]) {
            matched++;
            if (matched == 3) {
                return true; // full "SH;" matched
            }
        } else {
            matched = (c == sync[0]) ? 1 : 0; // restart match if 'S' seen
        }
    }
}

static void bind_simhub_fields(simhub_data_t *data) {
    int i = 0;
    simhub_fields[i++].dest = data->curr_gear;
    simhub_fields[i++].dest = data->curr_speed;
    simhub_fields[i++].dest = data->rpm_percent;
    simhub_fields[i++].dest = data->rpm_redline_threshold;
    simhub_fields[i++].dest = data->current_time;
    simhub_fields[i++].dest = data->last_time;
    simhub_fields[i++].dest = data->best_time;
    simhub_fields[i++].dest = data->delta_time;
    simhub_fields[i++].dest = data->fl_wear;
    simhub_fields[i++].dest = data->fr_wear;
    simhub_fields[i++].dest = data->rl_wear;
    simhub_fields[i++].dest = data->rr_wear;
    simhub_fields[i++].dest = data->fl_tire_temp;
    simhub_fields[i++].dest = data->fr_tire_temp;
    simhub_fields[i++].dest = data->rl_tire_temp;
    simhub_fields[i++].dest = data->rr_tire_temp;
}

static bool parse_simhub_packet(simhub_data_t *out_data) {
    char field_buf[64]; // temporary buffer for each field
    size_t field_len = 0;
    int field_idx = 0;

    bind_simhub_fields(out_data);

    while (field_idx < (int)(sizeof(simhub_fields) / sizeof(simhub_fields[0]))) {
        char c;
        int len = usb_serial_jtag_read_bytes(&c, 1, 20 / portTICK_PERIOD_MS);
        if (len <= 0) return false; // timeout

        if (c == ';') {
            field_buf[field_len] = '\0';

            strncpy(simhub_fields[field_idx].dest, field_buf,
                    simhub_fields[field_idx].max_size - 1);
            simhub_fields[field_idx].dest[simhub_fields[field_idx].max_size - 1] = '\0';

            field_idx++;
            field_len = 0;
        } else {
            if (field_len < sizeof(field_buf) - 1) {
                field_buf[field_len++] = c;
            }
        }
    }

    return true;
}

static void simhub_task(void *arg) {
    while (1) {
        simhub_data_t simhub_data = {0};

        // Step 1: Wait until sync marker appears
        if (!wait_for_sync()) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        // Step 2: Parse fields after sync
        if (parse_simhub_packet(&simhub_data)) {
            xQueueSend(xQueue, &simhub_data, 10 / portTICK_PERIOD_MS);
        }
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

static void ui_apply_simhub_data(const simhub_data_t *data) {
    ui_label_update_t updates[] = {
        { objects.gear_value,        data->curr_gear },
        { objects.speed_value,       data->curr_speed },
        { objects.estimated_lap_value, data->current_time },
        { objects.last_lap_value,    data->last_time },
        { objects.best_lap_value,    data->best_time },
        { objects.lap_delta_value,   data->delta_time },
        { objects.fl_tire_wear,      data->fl_wear },
        { objects.fr_tire_wear,      data->fr_wear },
        { objects.rl_tire_wear,      data->rl_wear },
        { objects.rr_tire_wear,      data->rr_wear },
        { objects.fl_tire_temp,      data->fl_tire_temp },
        { objects.fr_tire_temp,      data->fr_tire_temp },
        { objects.rl_tire_temp,      data->rl_tire_temp },
        { objects.rr_tire_temp,      data->rr_tire_temp },
    };

    for (size_t i = 0; i < sizeof(updates)/sizeof(updates[0]); i++) {
        update_label_if_changed(updates[i].label, updates[i].value);
    }

    // RPM LEDs (special case)
    process_rpm_leds(data->rpm_percent, data->rpm_redline_threshold);
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
