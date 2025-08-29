#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *gear_panel;
    lv_obj_t *gear_value;
    lv_obj_t *abs_panel;
    lv_obj_t *abs_title;
    lv_obj_t *abs_value;
    lv_obj_t *tc_panel;
    lv_obj_t *tc_title;
    lv_obj_t *tc_value;
    lv_obj_t *bb_panel;
    lv_obj_t *bb_title;
    lv_obj_t *bb_value;
    lv_obj_t *map_panel;
    lv_obj_t *map_title;
    lv_obj_t *map_value;
    lv_obj_t *fuel_panel;
    lv_obj_t *fuel_title;
    lv_obj_t *fuel_value;
    lv_obj_t *fuel_units;
    lv_obj_t *speed_panel;
    lv_obj_t *speed_value;
    lv_obj_t *indicator_bar;
    lv_obj_t *rpm_led_5;
    lv_obj_t *rpm_led_6;
    lv_obj_t *rpm_led_4;
    lv_obj_t *rpm_led_7;
    lv_obj_t *rpm_led_3;
    lv_obj_t *rpm_led_8;
    lv_obj_t *rpm_led_2;
    lv_obj_t *rpm_led_9;
    lv_obj_t *rpm_led_1;
    lv_obj_t *tc_led_1;
    lv_obj_t *tc_led_2;
    lv_obj_t *tc_led_3;
    lv_obj_t *tc_led_4;
    lv_obj_t *abs_led_1;
    lv_obj_t *abs_led_2;
    lv_obj_t *abs_led_3;
    lv_obj_t *abs_led_4;
    lv_obj_t *times_panel;
    lv_obj_t *tires_panel;
    lv_obj_t *fr_brake;
    lv_obj_t *rr_brake;
    lv_obj_t *rl_brake;
    lv_obj_t *fl_brake;
    lv_obj_t *rl_tire;
    lv_obj_t *rr_tire;
    lv_obj_t *fr_tire;
    lv_obj_t *fl_tire;
    lv_obj_t *fl_pressure;
    lv_obj_t *rl_pressure;
    lv_obj_t *fr_pressure;
    lv_obj_t *rr_pressure;
    lv_obj_t *rr_tire_temp;
    lv_obj_t *fr_tire_temp;
    lv_obj_t *fl_tire_temp;
    lv_obj_t *rl_tire_temp;
    lv_obj_t *fl_brake_temp;
    lv_obj_t *rl_brake_temp;
    lv_obj_t *fr_brake_temp;
    lv_obj_t *rr_brake_temp;
    lv_obj_t *fl_tire_wear;
    lv_obj_t *rl_tire_wear;
    lv_obj_t *fr_tire_wear;
    lv_obj_t *rr_tire_wear;
    lv_obj_t *tires_panel_title;
    lv_obj_t *session_panel;
    lv_obj_t *session_type_val;
    lv_obj_t *session_time_val;
    lv_obj_t *session_position_val;
    lv_obj_t *session_lap_number_val;
    lv_obj_t *session_position_text;
    lv_obj_t *session_lap_text;
    lv_obj_t *car_controls_panel;
    lv_obj_t *estimated_lap_value;
    lv_obj_t *estimated_lap_text;
    lv_obj_t *last_lap_value;
    lv_obj_t *last_lap_text;
    lv_obj_t *best_lap_value;
    lv_obj_t *best_lap_text;
    lv_obj_t *lap_times_title;
    lv_obj_t *sector_1;
    lv_obj_t *sector_2;
    lv_obj_t *sector_3;
    lv_obj_t *lap_delta_value;
    lv_obj_t *ignition_status;
    lv_obj_t *wipers_status;
    lv_obj_t *lights_status;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void delete_screen_main();
void tick_screen_main();

void create_screen_by_id(enum ScreensEnum screenId);
void delete_screen_by_id(enum ScreensEnum screenId);
void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#define GREEN_COLOR (0xff00ff1b)
#define YELLOW_COLOR (0xfff7ff00)
#define RED_COLOR (0xffff0000)
#define BLUE_COLOR (0xff00afff)
#define WHITE_COLOR (0xffffffff)

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/