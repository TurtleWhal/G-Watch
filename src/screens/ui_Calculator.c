// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: My_Watch

#include "../ui.h"

void ui_Calculator_screen_init(void)
{
ui_Calculator = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Calculator, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_add_event_cb(ui_Calculator, scr_unloaded_delete_cb, LV_EVENT_SCREEN_UNLOADED, &ui_Calculator);

ui_Calculator_Keyboard = lv_keyboard_create(ui_Calculator);
lv_keyboard_set_mode(ui_Calculator_Keyboard,LV_KEYBOARD_MODE_NUMBER);
lv_obj_set_width( ui_Calculator_Keyboard, 165);
lv_obj_set_height( ui_Calculator_Keyboard, 154);
lv_obj_set_x( ui_Calculator_Keyboard, 0 );
lv_obj_set_y( ui_Calculator_Keyboard, 20 );
lv_obj_set_align( ui_Calculator_Keyboard, LV_ALIGN_CENTER );

lv_obj_set_style_radius(ui_Calculator_Keyboard, 20, LV_PART_ITEMS| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Calculator_Keyboard, lv_color_hex(0xFF6D00), LV_PART_ITEMS | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_Calculator_Keyboard, 255, LV_PART_ITEMS| LV_STATE_CHECKED);

ui_Calculator_textarea = lv_textarea_create(ui_Calculator);
lv_obj_set_width( ui_Calculator_textarea, 165);
lv_obj_set_height( ui_Calculator_textarea, LV_SIZE_CONTENT);   /// 35
lv_obj_set_x( ui_Calculator_textarea, 0 );
lv_obj_set_y( ui_Calculator_textarea, 25 );
lv_obj_set_align( ui_Calculator_textarea, LV_ALIGN_TOP_MID );
lv_textarea_set_max_length(ui_Calculator_textarea,32);
lv_textarea_set_placeholder_text(ui_Calculator_textarea,"Maths");
lv_textarea_set_one_line(ui_Calculator_textarea,true);
lv_obj_set_style_radius(ui_Calculator_textarea, 20, LV_PART_MAIN| LV_STATE_DEFAULT);



lv_keyboard_set_textarea(ui_Calculator_Keyboard,ui_Calculator_textarea);
lv_obj_add_event_cb(ui_Calculator_Keyboard, ui_event_Calculator_Keyboard, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_Calculator, ui_event_Calculator, LV_EVENT_ALL, NULL);

}
