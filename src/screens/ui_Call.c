// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: My_Watch

#include "../ui.h"

void ui_Call_screen_init(void)
{
ui_Call = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Call, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_add_event_cb(ui_Call, scr_unloaded_delete_cb, LV_EVENT_SCREEN_UNLOADED, &ui_Call);

ui_Accept_Call_Button = lv_btn_create(ui_Call);
lv_obj_set_width( ui_Accept_Call_Button, 60);
lv_obj_set_height( ui_Accept_Call_Button, 60);
lv_obj_set_x( ui_Accept_Call_Button, -40 );
lv_obj_set_y( ui_Accept_Call_Button, 65 );
lv_obj_set_align( ui_Accept_Call_Button, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Accept_Call_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Accept_Call_Button, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Accept_Call_Button, 50, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Accept_Call_Button, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Accept_Call_Button, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Accept_Call_Image = lv_img_create(ui_Accept_Call_Button);
lv_img_set_src(ui_Accept_Call_Image, &ui_img_call1_png);
lv_obj_set_width( ui_Accept_Call_Image, LV_SIZE_CONTENT);  /// 20
lv_obj_set_height( ui_Accept_Call_Image, LV_SIZE_CONTENT);   /// 19
lv_obj_set_align( ui_Accept_Call_Image, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Accept_Call_Image, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_Accept_Call_Image, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_img_set_zoom(ui_Accept_Call_Image,500);

ui_Reject_Call_Button = lv_btn_create(ui_Call);
lv_obj_set_width( ui_Reject_Call_Button, 60);
lv_obj_set_height( ui_Reject_Call_Button, 60);
lv_obj_set_x( ui_Reject_Call_Button, 40 );
lv_obj_set_y( ui_Reject_Call_Button, 65 );
lv_obj_set_align( ui_Reject_Call_Button, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Reject_Call_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Reject_Call_Button, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Reject_Call_Button, 50, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Reject_Call_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Reject_Call_Button, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Reject_Call_Image = lv_img_create(ui_Reject_Call_Button);
lv_img_set_src(ui_Reject_Call_Image, &ui_img_call1_png);
lv_obj_set_width( ui_Reject_Call_Image, LV_SIZE_CONTENT);  /// 20
lv_obj_set_height( ui_Reject_Call_Image, LV_SIZE_CONTENT);   /// 19
lv_obj_set_align( ui_Reject_Call_Image, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Reject_Call_Image, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_Reject_Call_Image, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_img_set_angle(ui_Reject_Call_Image,1800);
lv_img_set_zoom(ui_Reject_Call_Image,500);

ui_Call_Type = lv_label_create(ui_Call);
lv_obj_set_width( ui_Call_Type, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Call_Type, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Call_Type, 0 );
lv_obj_set_y( ui_Call_Type, -100 );
lv_obj_set_align( ui_Call_Type, LV_ALIGN_CENTER );
lv_label_set_text(ui_Call_Type,"Incoming Call");
lv_obj_set_style_text_font(ui_Call_Type, &ui_font_Comfortaa_12, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Caller_Name = lv_label_create(ui_Call);
lv_obj_set_width( ui_Caller_Name, 215);
lv_obj_set_height( ui_Caller_Name, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Caller_Name, 0 );
lv_obj_set_y( ui_Caller_Name, -40 );
lv_obj_set_align( ui_Caller_Name, LV_ALIGN_CENTER );
lv_label_set_long_mode(ui_Caller_Name,LV_LABEL_LONG_SCROLL);
lv_label_set_text(ui_Caller_Name,"Garrett Jordan");
lv_obj_set_style_text_align(ui_Caller_Name, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Caller_Name, &ui_font_Comfortaa_26, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Caller_Number = lv_label_create(ui_Call);
lv_obj_set_width( ui_Caller_Number, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Caller_Number, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Caller_Number, LV_ALIGN_CENTER );
lv_label_set_text(ui_Caller_Number,"206 833 9969");
lv_obj_set_style_text_font(ui_Caller_Number, &ui_font_Comfortaa_26, LV_PART_MAIN| LV_STATE_DEFAULT);

}
