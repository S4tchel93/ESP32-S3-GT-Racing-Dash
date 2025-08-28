#include "actions.h"
#include "screens.h"
#include "string.h"

/*Changing the speed based on Green slider*/
void action_speed_change(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    /*Refresh the text*/
    lv_label_set_text_fmt(objects.speed_value, "%" LV_PRId32, lv_slider_get_value(slider));
}

void action_map_change(lv_event_t * e)
{
    char* map_value = lv_label_get_text(objects.map_value);

    uint32_t map_value_int = atoi(map_value);
    map_value_int++;
    if (map_value_int == 10){
        map_value_int = 1;
    }

    /*Refresh the text*/
    lv_label_set_text_fmt(objects.map_value, "%" LV_PRId32, map_value_int);
}

void action_tc_change (lv_event_t * e)
{
    char* tc_value = lv_label_get_text(objects.tc_value);

    uint32_t tc_value_int = atoi(tc_value);
    tc_value_int++;
    if (tc_value_int == 10){
        tc_value_int = 1;
    }

    /*Refresh the text*/
    lv_label_set_text_fmt(objects.tc_value, "%" LV_PRId32, tc_value_int);
}

void action_abs_change (lv_event_t * e)
{
    char* abs_value = lv_label_get_text(objects.abs_value);

    uint32_t abs_value_int = atoi(abs_value);
    abs_value_int++;
    if (abs_value_int == 10){
        abs_value_int = 1;
    }

    /*Refresh the text*/
    lv_label_set_text_fmt(objects.abs_value, "%" LV_PRId32, abs_value_int);
}

void action_rpm_change(lv_event_t * e)
{
    char* gear_value = lv_label_get_text(objects.gear_value);
    uint32_t gear_value_int = atoi(gear_value);
    bool is_text = false;

    if(strcmp(gear_value, "N") == 0)
    {
        gear_value_int = 1;
    }
    else if(gear_value_int >= 1 && gear_value_int < 8)
    {
        gear_value_int++;
    }
    else if (gear_value_int == 8)
    {
        gear_value = "R";
        is_text = true;
    }
    else if (strcmp(gear_value, "R") == 0)
    {
        gear_value = "N";
        is_text = true;
    }
    else
    {
        gear_value = "N";
        is_text = true;
    }

    if(is_text == false)
    {
        lv_label_set_text_fmt(objects.gear_value, "%" LV_PRId32, gear_value_int);
    }
    else
    {
        lv_label_set_text(objects.gear_value, gear_value);
    }
}