
/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/
/*project includes*/
#include "ui_update_task.h"
#include "screens.h"
#include "user_colors.h"
#include "simhub_data.h"
#include "simhub_task.h"
#include "lvgl.h"

/*System/ESP IDF Includes*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "string.h"
#include <sys/lock.h>

/***************************************************************************************************************************
 * PRIVATE DEFINITIONS
 ***************************************************************************************************************************/

/**
 * @brief Number of RPM Leds on the screen
 * 
 */
#define NUM_LEDS (9)

/**
 * @brief Number of ABS Leds on the screen
 * 
 */
#define NUM_ABS_LEDS (4)

/**
 * @brief Number of TC Leds on the screen
 * 
 */
#define NUM_TC_LEDS (4)

/**
 * @brief ABS LED step time in ms
 * 
 */
#define ABS_STEP_MS  (1000)

/**
 * @brief TC LED step time in ms
 * 
 */
#define TC_STEP_MS   (2000)

/**
 * @brief Minimum segments allowed for RPM LED strip
 * Only used whenever we get 0 on redline threshold data from simhub.
 * Used to prevent division by 0
 */
#define MIN_SEGMENTS (1.0)

/**
 * @brief LEDs start lighting at 75% of redline threshold
 * 
 */
#define RPM_START_THRESHOLD 0.75f

/**
 * @brief Blink speed at redline (non-blocking)
 */
#define BLINK_INTERVAL_MS   100

// Temperature color mapping constants
#define COLD_TIRE_TEMP      (50)
#define OPTIMUM_TIRE_TEMP   (85)
#define HOT_TIRE_TEMP       (120)
#define COLD_BRAKE_TEMP     (150)
#define OPTIMUM_BRAKE_TEMP  (550)
#define HOT_BRAKE_TEMP      (1000)

// Minimum interval between temp updates for tire and brakes(ms)
#define TEMP_UPDATE_INTERVAL_MS   (250)

/***************************************************************************************************************************
 * PRIVATE DATA & TYPES
 ***************************************************************************************************************************/

/**
 * @brief Struct that holds LED LVGL object data
 *  for LED manipulation
 */
typedef struct {
    lv_obj_t* led; // lvgl LED object
    uint32_t color; // LED color
    uint8_t brightness; // LED brightness
} led_state_t;

/**
 * @brief Struct that holds LVGL object label data for label
 * manipulation
 */
typedef struct {
    lv_obj_t *label; //lvgl label object
    const char *value; //label value
} ui_label_update_t;

/**
 * @brief Struct that holds temperature mapping data for tires and brakes
 * 
 */
typedef struct {
    lv_obj_t *label;   // label with numeric value
    lv_obj_t *target;  // object whose bg color changes
    int cold, optimum, hot; // temperature thresholds
    lv_color_t last_color; // last color set to avoid redundant updates
} temp_map_t;

/**
 * @brief holds rpm LED data for the current module
 * 
 */
static led_state_t rpm_leds[NUM_LEDS];

/**
 * @brief holds abs LED data for the current module
 * 
 */
static led_state_t abs_leds[NUM_ABS_LEDS];

/**
 * @brief Holds tc LED data for the current module
 * 
 */
static led_state_t tc_leds[NUM_TC_LEDS];

/**
 * @brief Holds tire temperature mapping data for the current module
 * 
 */
static temp_map_t tire_maps[4];

/**
 * @brief Holds brake temperature mapping data for the current module
 * 
 */
static temp_map_t brake_maps[4];

/***************************************************************************************************************************
 * PRIVATE FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

static long map(long x, long in_min, long in_max, long out_min, long out_max);
static uint32_t get_led_color(uint8_t idx);
static void set_led(uint8_t idx, uint32_t color, uint8_t brightness);
static void process_rpm_leds(const char* rpm, const char* redline_threshold);
static void init_rpm_leds(void);
static void update_label_if_changed(lv_obj_t *label, const char *new_val);
static void update_label_if_changed(lv_obj_t *label, const char *new_val);
static void process_car_controls(const char* ignition, const char* wipers, const char* lights);
static void process_delta_color(const char* delta_time);
static void process_throttle_brake_bars(const char* throttle, const char* brake);
static void ui_apply_simhub_data(const simhub_data_t *data);
static void init_abs_leds(void);
static void init_tc_leds(void);
static void init_rpm_leds(void);
static void process_abs_leds(const char* abs_active);
static void process_tc_leds(const char* tc_active);
static void update_all_temps(void);
static inline uint8_t linear_interpolation_u8(uint8_t a, uint8_t b, float t);
static lv_color_t linear_interpolation_color(lv_color_t c1, lv_color_t c2, float t);
static lv_color_t temp_to_color(int temp, int cold, int optimum, int hot);
static void update_temp_from_label(temp_map_t *map);
static void update_all_temps(void);
static void init_temp_mappings(void);


 /***************************************************************************************************************************
 * PRIVATE FUNCTION DEFINITIONS
 ***************************************************************************************************************************/

/**
 * @brief Maps an input value from one range to another (as in a rule of three)
 * 
 * @param x Value to be maped from input range to output range
 * @param in_min input minimum value
 * @param in_max input maximum value
 * @param out_min output minimum value
 * @param out_max output maximum value
 * @return long input value mapped to output range
 */
static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Get the led color object
 * 
 * @param idx LED index to get its configured color
 * @return uint32_t LED Color
 */
static uint32_t get_led_color(uint8_t idx) {
    if (idx < 3) return GREEN_COLOR;
    else if (idx < 6) return YELLOW_COLOR;
    else return RED_COLOR;
}

/**
 * @brief Set the led object if color and brightness has changed
 * 
 * @param idx LED index to be set
 * @param color Color to be set
 * @param brightness Brightness to be set
 */
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

/**
 * @brief Main processing function for the RPM LED Bar
 * 
 * @details Determines the number of segments based on the number
 * of LEDs available and the redline_threshold value. Based on that it figures out
 * which LED from each segment has to be lit and how bright it needs to be.
 * 
 * @param rpm RPM percentage value coming from simhub
 * @param redline_threshold RPM Redline threshold percentage value coming from simhub
 */
static void process_rpm_leds(const char* rpm, const char* redline_threshold) {
    uint16_t rpm_val = atoi(rpm);
    uint16_t redline = atoi(redline_threshold);
    //Keeps state for blinking on redline
    static bool blink_state = false;
    static int64_t last_blink_time = 0;

    if (redline == 0) return;

    uint16_t start_rpm = (uint16_t)(RPM_START_THRESHOLD * redline);

    // Case 1: Below threshold → all off
    if (rpm_val < start_rpm) {
        for (int i = 0; i < NUM_LEDS; i++) {
            set_led(i, get_led_color(i), 0);
        }
        return;
    }

    // Case 2: At or above redline → blinking all blue
    if (rpm_val >= redline) {
        int64_t now = esp_timer_get_time() / 1000; // ms
        if (now - last_blink_time >= BLINK_INTERVAL_MS) {
            blink_state = !blink_state;
            last_blink_time = now;
        }

        for (int i = 0; i < NUM_LEDS; i++) {
            set_led(i, BLUE_COLOR, blink_state ? 255 : 0);
        }
        return;
    }

    // Case 3: Between threshold and redline → progressive lighting
    float active_range = (float)(redline - start_rpm);
    float rel_rpm = (float)(rpm_val - start_rpm);

    float segment_size = active_range / NUM_LEDS;
    if (segment_size < 1.0f) segment_size = MIN_SEGMENTS;

    int active_idx = (int)(rel_rpm / segment_size);
    if (active_idx >= NUM_LEDS) active_idx = NUM_LEDS - 1;

    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t color = get_led_color(i);

        if (i < active_idx) {
            set_led(i, color, 255); // Fully lit
        } else if (i == active_idx) {
            float seg_start = i * segment_size;
            float seg_end   = (i + 1) * segment_size;
            uint8_t b = (uint8_t)map(rel_rpm, seg_start, seg_end, 0, 255);
            set_led(i, color, b); // Partial brightness
        } else {
            set_led(i, color, 0); // Off
        }
    }
}

/**
 * @brief Initializes the ABS Led structure to point to the correct LVGL
 * objects and sets default color and brightness
 * 
 */
static void init_abs_leds(void) {
    abs_leds[0].led = objects.abs_led_1;
    abs_leds[1].led = objects.abs_led_2;
    abs_leds[2].led = objects.abs_led_3;
    abs_leds[3].led = objects.abs_led_4;

    // Initialize the state so we don't need to be setting color when updating
    for (int i = 0; i < NUM_ABS_LEDS; i++) {
        abs_leds[i].color = PURPLE_COLOR; 
        lv_led_set_color(abs_leds[i].led, lv_color_hex(abs_leds[i].color));
        abs_leds[i].brightness = 0;    // no brightness
    }
}

/**
 * @brief Initializes the TC Led structure to point to the correct LVGL
 * objects and sets default color and brightness
 * 
 */
static void init_tc_leds(void) {
    tc_leds[0].led = objects.tc_led_1;
    tc_leds[1].led = objects.tc_led_2;
    tc_leds[2].led = objects.tc_led_3;
    tc_leds[3].led = objects.tc_led_4;

    // Initialize the state so we don't need to be setting color when updating
    for (int i = 0; i < NUM_TC_LEDS; i++) {
        tc_leds[i].color = LIGHT_BLUE_COLOR;   
        lv_led_set_color(tc_leds[i].led, lv_color_hex(tc_leds[i].color));
        tc_leds[i].brightness = 0;    // no brightness
    }
}

/**
 * @brief Turns on ABS LEDs progressively when ABS is active depending
 * on how long ABS has been active, determined by ABS_STEP_MS
 * 
 * @param abs_active ABS active string pointer from received simhub data
 */
static void process_abs_leds(const char* abs_active)
{
    static int prev_abs = 0;
    static int64_t abs_start_time = 0;
    static uint8_t last_brightness[NUM_ABS_LEDS] = {0};

    int curr_abs = atoi(abs_active);
    int64_t now = esp_timer_get_time() / 1000; // ms

    if (curr_abs == 1) {
        if (prev_abs == 0) {
            // Just turned ON
            abs_start_time = now;
        }

        int64_t elapsed = now - abs_start_time;
        int leds_on = elapsed / ABS_STEP_MS + 1;
        if (leds_on > NUM_ABS_LEDS) leds_on = NUM_ABS_LEDS;

        for (int i = 0; i < NUM_ABS_LEDS; i++) {
            uint8_t brightness = (i < leds_on) ? 255 : 0;
            if (brightness != last_brightness[i]) {
                lv_led_set_brightness(abs_leds[i].led, brightness);
                last_brightness[i] = brightness;
            }
        }
    } else {
        for (int i = 0; i < NUM_ABS_LEDS; i++) {
            if (last_brightness[i] != 0) {
                lv_led_set_brightness(abs_leds[i].led, 0);
                last_brightness[i] = 0;
            }
        }
    }

    prev_abs = curr_abs;
}

/**
 * @brief Turns on tc LEDs progressively when tc is active depending
 * on how long tc has been active, determined by tc_STEP_MS
 * 
 * @param tc_active TC active string pointer from received simhub data
 */
static void process_tc_leds(const char* tc_active)
{
    static int prev_tc = 0;
    static int64_t tc_start_time = 0;
    static uint8_t last_brightness[NUM_TC_LEDS] = {0};

    int curr_tc = atoi(tc_active);
    int64_t now = esp_timer_get_time() / 1000; // ms

    if (curr_tc == 1) {
        if (prev_tc == 0) {
            // Just turned ON
            tc_start_time = now;
        }

        int64_t elapsed = now - tc_start_time;
        int leds_on = elapsed / TC_STEP_MS + 1;
        if (leds_on > NUM_TC_LEDS) leds_on = NUM_TC_LEDS;

        for (int i = 0; i < NUM_TC_LEDS; i++) {
            uint8_t brightness = (i < leds_on) ? 255 : 0;
            if (brightness != last_brightness[i]) {
                lv_led_set_brightness(tc_leds[i].led, brightness);
                last_brightness[i] = brightness;
            }
        }
    } else {
        for (int i = 0; i < NUM_TC_LEDS; i++) {
            if (last_brightness[i] != 0) {
                lv_led_set_brightness(tc_leds[i].led, 0);
                last_brightness[i] = 0;
            }
        }
    }

    prev_tc = curr_tc;
}

/**
 * @brief Initializes the RPM Led structure to point to the correct LVGL
 * objects
 */
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

/**
 * @brief Updates the labels (text) on screen if the new value coming
 * from simhub has changed from previously received data
 * 
 * @param label LVGL Label (text) object
 * @param new_val New value received from simhub
 */
static void update_label_if_changed(lv_obj_t *label, const char *new_val) {
    const char *old_val = lv_label_get_text(label);
    if (strcmp(old_val, new_val) != 0) {
        lv_label_set_text(label, new_val);
    }
}

/**
 * @brief Updates the car control panel colors (only if
 * data has changed from previously received data)
 * 
 * @param ignition ignition status pointer from received simhub data
 * @param wipers wipers status pointer from received simhub data
 * @param lights lights status pointer from received simhub data
 */
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

/**
 * @brief Updates the delta label background colors (only if
 * data has changed from previously received data)
 * 
 * @param delta_time delta time string pointer from received simhub data
 */
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

/**
 * @brief Updates the throttle and brake bars with new data (only if
 * data has changed from previously received data)
 * 
 * @param throttle throttle string pointer from received simhub data
 * @param brake brake string pointer from received simhub data
 */
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

/**
 * @brief Applies newly received simhub data to the different
 * elements on screen.
 * 
 * @param data simhub data packet pointer
 */
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


    process_rpm_leds(data->rpm_percent, data->rpm_redline_threshold);
    process_car_controls(data->engine_ignition, data->wipers, data->headlights);
    process_delta_color(data->delta_time);
    process_throttle_brake_bars(data->throttle, data->brake);
    process_abs_leds(data->abs_active);
    process_tc_leds(data->tc_active);
}

/**
 * @brief Linear interpolation between two uint8_t values
 * 
 * @param a Input value 1
 * @param b Input value 2
 * @param s Slope between 0.0 and 1.0
 * @return uint8_t Interpolated value
 */
static inline uint8_t linear_interpolation_u8(uint8_t a, uint8_t b, float s) {
    if(s < 0.0f) s = 0.0f;
    if(s > 1.0f) s = 1.0f;
    return (uint8_t)(a + (b - a) * s);
}

/**
 * @brief Linear interpolation between two lv_color_t colors
 * 
 * @param c1 Color 1
 * @param c2 Color 2
 * @param s  Slope
 * @return lv_color_t Interpolated color
 */
static lv_color_t linear_interpolation_color(lv_color_t c1, lv_color_t c2, float s) {
    uint32_t c1_u32 = lv_color_to_u32(c1);
    uint32_t c2_u32 = lv_color_to_u32(c2);

    uint8_t r1 = (c1_u32 >> 16) & 0xFF;
    uint8_t g1 = (c1_u32 >> 8)  & 0xFF;
    uint8_t b1 = (c1_u32 >> 0)  & 0xFF;

    uint8_t r2 = (c2_u32 >> 16) & 0xFF;
    uint8_t g2 = (c2_u32 >> 8)  & 0xFF;
    uint8_t b2 = (c2_u32 >> 0)  & 0xFF;

    return lv_color_make(
        linear_interpolation_u8(r1, r2, s),
        linear_interpolation_u8(g1, g2, s),
        linear_interpolation_u8(b1, b2, s)
    );
}

/**
 * @brief Converts a temperature value to a color
 * 
 * @param temp temperature value in degrees celsius
 * @param cold cold temperature threshold
 * @param optimum optimum temperature threshold
 * @param hot hot temperature threshold
 * @return lv_color_t Output color
 */
static lv_color_t temp_to_color(int temp, int cold, int optimum, int hot)
{
    lv_color_t cold_c    = lv_color_make(0,   0, 255);   // Blue
    lv_color_t optimum_c = lv_color_make(0, 255,   0);   // Green
    lv_color_t hot_c     = lv_color_make(255, 0,   0);   // Red

    if (temp <= cold) {
        return cold_c;
    } else if (temp < optimum) {
        float slope = (float)(temp - cold) / (float)(optimum - cold);
        return linear_interpolation_color(cold_c, optimum_c, slope);
    } else if (temp < hot) {
        float slope = (float)(temp - optimum) / (float)(hot - optimum);
        return linear_interpolation_color(optimum_c, hot_c, slope);
    } else {
        return hot_c;
    }
}

/**
 * @brief Updates the background color of a target LVGL object
 * based on the temperature value read from a label LVGL object
 * 
 * @param map temperature mapping structure pointer
 */
static void update_temp_from_label(temp_map_t *map)
{
    const char *txt = lv_label_get_text(map->label);
    if(!txt) return;

    int temp = atoi(txt);
    lv_color_t new_col = temp_to_color(temp, map->cold, map->optimum, map->hot);

    if(lv_color_to_u32(new_col) != lv_color_to_u32(map->last_color)) {
        lv_obj_set_style_bg_color(map->target, new_col, LV_PART_MAIN);
        map->last_color = new_col;
    }
}

/**
 * @brief Updates all tire and brake temperatures on screen
 * 
 */
static void update_all_temps(void)
{
    for(int i=0;i<4;i++) {
        update_temp_from_label(&tire_maps[i]);
        update_temp_from_label(&brake_maps[i]);
    }
}

/**
 * @brief Initializes the tire and brake temperature mappings with
 * the correct LVGL objects and temperature thresholds
 * 
 */
static void init_temp_mappings(void)
{
    // Tires
    tire_maps[0] = (temp_map_t){ objects.fl_tire_temp, objects.fl_tire, COLD_TIRE_TEMP, OPTIMUM_TIRE_TEMP, HOT_TIRE_TEMP, lv_color_black()};
    tire_maps[1] = (temp_map_t){ objects.fr_tire_temp, objects.fr_tire, COLD_TIRE_TEMP, OPTIMUM_TIRE_TEMP, HOT_TIRE_TEMP, lv_color_black()};
    tire_maps[2] = (temp_map_t){ objects.rl_tire_temp, objects.rl_tire, COLD_TIRE_TEMP, OPTIMUM_TIRE_TEMP, HOT_TIRE_TEMP, lv_color_black()};
    tire_maps[3] = (temp_map_t){ objects.rr_tire_temp, objects.rr_tire, COLD_TIRE_TEMP, OPTIMUM_TIRE_TEMP, HOT_TIRE_TEMP, lv_color_black()};

    // Brakes
    brake_maps[0] = (temp_map_t){ objects.fl_brake_temp, objects.fl_brake, COLD_BRAKE_TEMP, OPTIMUM_BRAKE_TEMP, HOT_BRAKE_TEMP, lv_color_black()};
    brake_maps[1] = (temp_map_t){ objects.fr_brake_temp, objects.fr_brake, COLD_BRAKE_TEMP, OPTIMUM_BRAKE_TEMP, HOT_BRAKE_TEMP, lv_color_black()};
    brake_maps[2] = (temp_map_t){ objects.rl_brake_temp, objects.rl_brake, COLD_BRAKE_TEMP, OPTIMUM_BRAKE_TEMP, HOT_BRAKE_TEMP, lv_color_black()};
    brake_maps[3] = (temp_map_t){ objects.rr_brake_temp, objects.rr_brake, COLD_BRAKE_TEMP, OPTIMUM_BRAKE_TEMP, HOT_BRAKE_TEMP, lv_color_black()};
}

 /***************************************************************************************************************************
 * PUBLIC FUNCTION DEFINITIONS
 ***************************************************************************************************************************/

/**
 * @brief Task that updates the LVGL UI elements on screen.
 * 
 * @details This task is dormant until it gets a simhub_data packet
 * on xQueue coming from the simhub_task.
 * 
 * @param arg holds data passed from main.c, in this case it contains
 * the lvgl_api_lock that is used to control accesses to lvgl data from
 * different tasks
 */
void ui_update_task(void *arg) {
    simhub_data_t simhub_data;
    QueueHandle_t xQueue;
    QueueHandle_t* xQueue_p;
    _lock_t* lvgl_api_lock = (_lock_t*) arg;

    xQueue_p = get_simhub_data_queue();
    xQueue = *xQueue_p;

    init_rpm_leds();
    init_abs_leds();
    init_tc_leds();
    init_temp_mappings();

    // For temperature update rate limiting
    TickType_t last_temp_update = 0;
    const TickType_t temp_update_interval_ticks =
        pdMS_TO_TICKS(TEMP_UPDATE_INTERVAL_MS);

    while (1) {
        if (xQueueReceive(xQueue, &simhub_data, portMAX_DELAY) == pdPASS) {
            _lock_acquire(lvgl_api_lock);
            ui_apply_simhub_data(&simhub_data);

            // Rate limit temperature updates, limited to TEMP_UPDATE_INTERVAL_MS
            // This prevents excessive CPU usage due to frequent updates which may cause FPS drops
            TickType_t now = xTaskGetTickCount();
            if ((now - last_temp_update) >= temp_update_interval_ticks) {
                update_all_temps();
                last_temp_update = now;
            }

            _lock_release(lvgl_api_lock);
        }
        taskYIELD();
    }
}