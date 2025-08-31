#include "ui_update_task.h"
#include "screens.h"
#include "user_colors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/lock.h>
#include "simhub_data.h"
#include "simhub_task.h"
#include "string.h"

#define NUM_LEDS 9
#define MIN_SEGMENTS 3.0

typedef struct {
    lv_obj_t* led;
    uint32_t color;
    uint8_t brightness;
} led_state_t;

typedef struct {
    lv_obj_t *label;
    const char *value;
} ui_label_update_t;

static led_state_t rpm_leds[NUM_LEDS];

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

static void process_rpm_leds(const char* rpm, const char* redline_threshold) {
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

static void init_rpm_leds(void) {
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

void ui_update_task(void *arg) {
    simhub_data_t simhub_data;
    QueueHandle_t xQueue;
    QueueHandle_t* xQueue_p;
    _lock_t* lvgl_api_lock = (_lock_t*) arg;

    xQueue_p = get_simhub_data_queue();
    xQueue = *xQueue_p;

    init_rpm_leds();

    while (1) {
        if (xQueueReceive(xQueue, &simhub_data, portMAX_DELAY) == pdPASS) {
            _lock_acquire(lvgl_api_lock);
            ui_apply_simhub_data(&simhub_data);
            _lock_release(lvgl_api_lock);
        }
        taskYIELD();
        // vTaskDelay(10 / portTICK_PERIOD_MS);  // optional throttle
    }
}