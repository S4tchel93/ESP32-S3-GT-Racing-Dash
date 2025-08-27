#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_speed_change(lv_event_t * e);
extern void action_map_change(lv_event_t * e);
extern void action_tc_change(lv_event_t * e);
extern void action_abs_change(lv_event_t * e);
extern void action_rpm_change(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/