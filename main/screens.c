#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // Gear Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.gear_panel = obj;
            lv_obj_set_pos(obj, 1, -86);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Gear Value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gear_value = obj;
                    lv_obj_set_pos(obj, -1, 43);
                    lv_obj_set_size(obj, 116, 125);
                    lv_obj_add_event_cb(obj, action_rpm_change, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_zen_dots120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "N");
                }
            }
        }
        {
            // ABS Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.abs_panel = obj;
            lv_obj_set_pos(obj, 178, 349);
            lv_obj_set_size(obj, 117, 102);
            lv_obj_add_event_cb(obj, action_abs_change, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xfff7ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ABS Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.abs_title = obj;
            lv_obj_set_pos(obj, 203, 332);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffe400), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "ABS");
        }
        {
            // ABS Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.abs_value = obj;
            lv_obj_set_pos(obj, 221, 374);
            lv_obj_set_size(obj, 32, 52);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff7ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "4");
        }
        {
            // TC Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.tc_panel = obj;
            lv_obj_set_pos(obj, 29, 349);
            lv_obj_set_size(obj, 117, 102);
            lv_obj_add_event_cb(obj, action_tc_change, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff00afff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // TC Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.tc_title = obj;
            lv_obj_set_pos(obj, 67, 332);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00afff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "TC");
        }
        {
            // TC Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.tc_value = obj;
            lv_obj_set_pos(obj, 72, 375);
            lv_obj_set_size(obj, 32, 52);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00afff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "6");
        }
        {
            // BB Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.bb_panel = obj;
            lv_obj_set_pos(obj, 327, 349);
            lv_obj_set_size(obj, 149, 102);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // BB Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.bb_title = obj;
            lv_obj_set_pos(obj, 377, 332);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "BB");
        }
        {
            // BB Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.bb_value = obj;
            lv_obj_set_pos(obj, 350, 375);
            lv_obj_set_size(obj, 103, 52);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "60.0");
        }
        {
            // Map Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.map_panel = obj;
            lv_obj_set_pos(obj, 508, 350);
            lv_obj_set_size(obj, 117, 100);
            lv_obj_add_event_cb(obj, action_map_change, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff00ff1b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Map Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.map_title = obj;
            lv_obj_set_pos(obj, 528, 333);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff1b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "MAP");
        }
        {
            // MAP Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.map_value = obj;
            lv_obj_set_pos(obj, 551, 374);
            lv_obj_set_size(obj, 32, 52);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff1b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "9");
        }
        {
            // Fuel Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.fuel_panel = obj;
            lv_obj_set_pos(obj, 657, 349);
            lv_obj_set_size(obj, 117, 102);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Fuel Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.fuel_title = obj;
            lv_obj_set_pos(obj, 673, 332);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "FUEL");
        }
        {
            // FUEL Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.fuel_value = obj;
            lv_obj_set_pos(obj, 686, 374);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "43");
        }
        {
            // FUEL units
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.fuel_units = obj;
            lv_obj_set_pos(obj, 688, 418);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff818181), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "LITERS");
        }
        {
            // Speed Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.speed_panel = obj;
            lv_obj_set_pos(obj, 322, 263);
            lv_obj_set_size(obj, 159, 54);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Speed Value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.speed_value = obj;
                    lv_obj_set_pos(obj, 16, -15);
                    lv_obj_set_size(obj, 84, 41);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_38, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "60");
                }
            }
        }
        {
            // Indicator bar
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.indicator_bar = obj;
            lv_obj_set_pos(obj, 322, 240);
            lv_obj_set_size(obj, 159, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_add_event_cb(obj, action_speed_change, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff21f337), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff69f321), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff69f321), LV_PART_KNOB | LV_STATE_DEFAULT);
        }
        {
            // Rpm LED 5
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_5 = obj;
            lv_obj_set_pos(obj, 384, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 6
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_6 = obj;
            lv_obj_set_pos(obj, 463, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 4
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_4 = obj;
            lv_obj_set_pos(obj, 304, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 7
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_7 = obj;
            lv_obj_set_pos(obj, 542, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 3
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_3 = obj;
            lv_obj_set_pos(obj, 225, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 8
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_8 = obj;
            lv_obj_set_pos(obj, 621, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 2
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_2 = obj;
            lv_obj_set_pos(obj, 146, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 9
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_9 = obj;
            lv_obj_set_pos(obj, 700, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Rpm LED 1
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.rpm_led_1 = obj;
            lv_obj_set_pos(obj, 67, 12);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // TC LED 1
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.tc_led_1 = obj;
            lv_obj_set_pos(obj, 758, 69);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // TC LED 2
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.tc_led_2 = obj;
            lv_obj_set_pos(obj, 758, 128);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // TC LED 3
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.tc_led_3 = obj;
            lv_obj_set_pos(obj, 758, 186);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // TC LED 4
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.tc_led_4 = obj;
            lv_obj_set_pos(obj, 758, 245);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // ABS Led 1
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.abs_led_1 = obj;
            lv_obj_set_pos(obj, 13, 69);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // ABS Led 2
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.abs_led_2 = obj;
            lv_obj_set_pos(obj, 13, 128);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // ABS Led 3
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.abs_led_3 = obj;
            lv_obj_set_pos(obj, 13, 186);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // ABS Led 4
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.abs_led_4 = obj;
            lv_obj_set_pos(obj, 13, 245);
            lv_obj_set_size(obj, 32, 32);
            lv_led_set_color(obj, lv_color_hex(0xffffffff));
            lv_led_set_brightness(obj, 0);
        }
        {
            // Times Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.times_panel = obj;
            lv_obj_set_pos(obj, 497, 124);
            lv_obj_set_size(obj, 246, 193);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Tires Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.tires_panel = obj;
            lv_obj_set_pos(obj, 63, 124);
            lv_obj_set_size(obj, 246, 193);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // FR brake
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.fr_brake = obj;
                    lv_obj_set_pos(obj, 149, 25);
                    lv_obj_set_size(obj, 9, 21);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // RR brake
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.rr_brake = obj;
                    lv_obj_set_pos(obj, 149, 104);
                    lv_obj_set_size(obj, 9, 21);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // RL brake
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.rl_brake = obj;
                    lv_obj_set_pos(obj, 42, 102);
                    lv_obj_set_size(obj, 9, 21);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FL brake
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.fl_brake = obj;
                    lv_obj_set_pos(obj, 42, 25);
                    lv_obj_set_size(obj, 9, 21);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // RL Tire
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.rl_tire = obj;
                    lv_obj_set_pos(obj, 21, 91);
                    lv_obj_set_size(obj, 19, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // RR Tire
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.rr_tire = obj;
                    lv_obj_set_pos(obj, 160, 94);
                    lv_obj_set_size(obj, 19, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FR Tire
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.fr_tire = obj;
                    lv_obj_set_pos(obj, 160, 15);
                    lv_obj_set_size(obj, 19, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FL Tire
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.fl_tire = obj;
                    lv_obj_set_pos(obj, 21, 15);
                    lv_obj_set_size(obj, 19, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff016694), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FL Pressure
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fl_pressure = obj;
                    lv_obj_set_pos(obj, 2, -9);
                    lv_obj_set_size(obj, 57, 22);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21.8");
                }
                {
                    // RL Pressure
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rl_pressure = obj;
                    lv_obj_set_pos(obj, 2, 135);
                    lv_obj_set_size(obj, 57, 22);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "22.5");
                }
                {
                    // FR Pressure
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fr_pressure = obj;
                    lv_obj_set_pos(obj, 141, -9);
                    lv_obj_set_size(obj, 57, 22);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21.8");
                }
                {
                    // RR Pressure
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rr_pressure = obj;
                    lv_obj_set_pos(obj, 141, 135);
                    lv_obj_set_size(obj, 57, 22);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "22.5");
                }
                {
                    // RR Tire Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rr_tire_temp = obj;
                    lv_obj_set_pos(obj, 186, 94);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "30°");
                }
                {
                    // FR Tire Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fr_tire_temp = obj;
                    lv_obj_set_pos(obj, 186, 15);
                    lv_obj_set_size(obj, 24, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "30°");
                }
                {
                    // FL Tire Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fl_tire_temp = obj;
                    lv_obj_set_pos(obj, -10, 15);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "30°");
                }
                {
                    // RL Tire Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rl_tire_temp = obj;
                    lv_obj_set_pos(obj, -10, 91);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "30°");
                }
                {
                    // FL Brake Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fl_brake_temp = obj;
                    lv_obj_set_pos(obj, 60, 28);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "29°");
                }
                {
                    // RL Brake Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rl_brake_temp = obj;
                    lv_obj_set_pos(obj, 59, 105);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "29°");
                }
                {
                    // FR Brake Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fr_brake_temp = obj;
                    lv_obj_set_pos(obj, 117, 28);
                    lv_obj_set_size(obj, 24, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "29°");
                }
                {
                    // RR Brake Temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rr_brake_temp = obj;
                    lv_obj_set_pos(obj, 117, 105);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "29°");
                }
                {
                    // FL Tire Wear
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fl_tire_wear = obj;
                    lv_obj_set_pos(obj, -16, 36);
                    lv_obj_set_size(obj, 35, 16);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "100%");
                }
                {
                    // RL Tire Wear
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rl_tire_wear = obj;
                    lv_obj_set_pos(obj, -16, 115);
                    lv_obj_set_size(obj, 35, 16);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "100%");
                }
                {
                    // FR Tire Wear
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fr_tire_wear = obj;
                    lv_obj_set_pos(obj, 181, 36);
                    lv_obj_set_size(obj, 35, 16);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "100%");
                }
                {
                    // RR Tire Wear
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rr_tire_wear = obj;
                    lv_obj_set_pos(obj, 181, 117);
                    lv_obj_set_size(obj, 35, 16);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "100%");
                }
            }
        }
        {
            // Tires Panel Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.tires_panel_title = obj;
            lv_obj_set_pos(obj, 162, 118);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "TIRES");
        }
        {
            // Session Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.session_panel = obj;
            lv_obj_set_pos(obj, 497, 60);
            lv_obj_set_size(obj, 246, 54);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Session Type val
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_type_val = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "QUALIFYING");
                }
                {
                    // Session Time Val
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_time_val = obj;
                    lv_obj_set_pos(obj, -13, -10);
                    lv_obj_set_size(obj, 80, 24);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "15:00");
                }
                {
                    // Session Position val
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_position_val = obj;
                    lv_obj_set_pos(obj, 83, -10);
                    lv_obj_set_size(obj, 66, 24);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5/20");
                }
                {
                    // Session Lap Number val
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_lap_number_val = obj;
                    lv_obj_set_pos(obj, 164, -10);
                    lv_obj_set_size(obj, 45, 24);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "3");
                }
                {
                    // Session Position text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_position_text = obj;
                    lv_obj_set_pos(obj, 106, 14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "POS");
                }
                {
                    // Session Lap Text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.session_lap_text = obj;
                    lv_obj_set_pos(obj, 176, 14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "LAP");
                }
            }
        }
        {
            // Car Controls Panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.car_controls_panel = obj;
            lv_obj_set_pos(obj, 60, 60);
            lv_obj_set_size(obj, 246, 54);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Estimated Lap Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.estimated_lap_value = obj;
            lv_obj_set_pos(obj, 528, 148);
            lv_obj_set_size(obj, 185, 40);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff464545), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "01:19.455");
        }
        {
            // Estimated Lap Text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.estimated_lap_text = obj;
            lv_obj_set_pos(obj, 615, 187);
            lv_obj_set_size(obj, 98, 15);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "ESTIMATED LAP");
        }
        {
            // Last Lap Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.last_lap_value = obj;
            lv_obj_set_pos(obj, 528, 200);
            lv_obj_set_size(obj, 185, 40);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff464545), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00:00.000");
        }
        {
            // Last Lap Text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.last_lap_text = obj;
            lv_obj_set_pos(obj, 655, 239);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "LAST LAP");
        }
        {
            // Best Lap Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.best_lap_value = obj;
            lv_obj_set_pos(obj, 528, 252);
            lv_obj_set_size(obj, 185, 40);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff464545), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00:00.000");
        }
        {
            // Best Lap Text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.best_lap_text = obj;
            lv_obj_set_pos(obj, 655, 291);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffa9a9a9), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "BEST LAP");
        }
        {
            // Lap Times Title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lap_times_title = obj;
            lv_obj_set_pos(obj, 577, 118);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "LAP TIMES");
        }
        {
            // Sector 1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.sector_1 = obj;
            lv_obj_set_pos(obj, 323, 51);
            lv_obj_set_size(obj, 51, 9);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8a8a8a), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Sector 2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.sector_2 = obj;
            lv_obj_set_pos(obj, 376, 51);
            lv_obj_set_size(obj, 51, 9);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8a8a8a), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Sector 3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.sector_3 = obj;
            lv_obj_set_pos(obj, 429, 51);
            lv_obj_set_size(obj, 51, 9);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8a8a8a), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Lap Delta Value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lap_delta_value = obj;
            lv_obj_set_pos(obj, 325, 63);
            lv_obj_set_size(obj, 153, 31);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffde0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "+3.546");
        }
    }
    
    tick_screen_main();
}

void delete_screen_main() {
    lv_obj_delete(objects.main);
    objects.main = 0;
    objects.gear_panel = 0;
    objects.gear_value = 0;
    objects.abs_panel = 0;
    objects.abs_title = 0;
    objects.abs_value = 0;
    objects.tc_panel = 0;
    objects.tc_title = 0;
    objects.tc_value = 0;
    objects.bb_panel = 0;
    objects.bb_title = 0;
    objects.bb_value = 0;
    objects.map_panel = 0;
    objects.map_title = 0;
    objects.map_value = 0;
    objects.fuel_panel = 0;
    objects.fuel_title = 0;
    objects.fuel_value = 0;
    objects.fuel_units = 0;
    objects.speed_panel = 0;
    objects.speed_value = 0;
    objects.indicator_bar = 0;
    objects.rpm_led_5 = 0;
    objects.rpm_led_6 = 0;
    objects.rpm_led_4 = 0;
    objects.rpm_led_7 = 0;
    objects.rpm_led_3 = 0;
    objects.rpm_led_8 = 0;
    objects.rpm_led_2 = 0;
    objects.rpm_led_9 = 0;
    objects.rpm_led_1 = 0;
    objects.tc_led_1 = 0;
    objects.tc_led_2 = 0;
    objects.tc_led_3 = 0;
    objects.tc_led_4 = 0;
    objects.abs_led_1 = 0;
    objects.abs_led_2 = 0;
    objects.abs_led_3 = 0;
    objects.abs_led_4 = 0;
    objects.times_panel = 0;
    objects.tires_panel = 0;
    objects.fr_brake = 0;
    objects.rr_brake = 0;
    objects.rl_brake = 0;
    objects.fl_brake = 0;
    objects.rl_tire = 0;
    objects.rr_tire = 0;
    objects.fr_tire = 0;
    objects.fl_tire = 0;
    objects.fl_pressure = 0;
    objects.rl_pressure = 0;
    objects.fr_pressure = 0;
    objects.rr_pressure = 0;
    objects.rr_tire_temp = 0;
    objects.fr_tire_temp = 0;
    objects.fl_tire_temp = 0;
    objects.rl_tire_temp = 0;
    objects.fl_brake_temp = 0;
    objects.rl_brake_temp = 0;
    objects.fr_brake_temp = 0;
    objects.rr_brake_temp = 0;
    objects.fl_tire_wear = 0;
    objects.rl_tire_wear = 0;
    objects.fr_tire_wear = 0;
    objects.rr_tire_wear = 0;
    objects.tires_panel_title = 0;
    objects.session_panel = 0;
    objects.session_type_val = 0;
    objects.session_time_val = 0;
    objects.session_position_val = 0;
    objects.session_lap_number_val = 0;
    objects.session_position_text = 0;
    objects.session_lap_text = 0;
    objects.car_controls_panel = 0;
    objects.estimated_lap_value = 0;
    objects.estimated_lap_text = 0;
    objects.last_lap_value = 0;
    objects.last_lap_text = 0;
    objects.best_lap_value = 0;
    objects.best_lap_text = 0;
    objects.lap_times_title = 0;
    objects.sector_1 = 0;
    objects.sector_2 = 0;
    objects.sector_3 = 0;
    objects.lap_delta_value = 0;
}

void tick_screen_main() {
}



typedef void (*create_screen_func_t)();
create_screen_func_t create_screen_funcs[] = {
    create_screen_main,
};
void create_screen(int screen_index) {
    create_screen_funcs[screen_index]();
}
void create_screen_by_id(enum ScreensEnum screenId) {
    create_screen_funcs[screenId - 1]();
}

typedef void (*delete_screen_func_t)();
delete_screen_func_t delete_screen_funcs[] = {
    delete_screen_main,
};
void delete_screen(int screen_index) {
    delete_screen_funcs[screen_index]();
}
void delete_screen_by_id(enum ScreensEnum screenId) {
    delete_screen_funcs[screenId - 1]();
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
