// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.4
// Project name: My_Watch

/*#include "../ui.h"

void ui_Calculator_screen_init(void)
{
    ui_Calculator = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Calculator, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Calculator_Plus_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Plus_Button, 40);
    lv_obj_set_height(ui_Calculator_Plus_Button, 40);
    lv_obj_set_x(ui_Calculator_Plus_Button, 66);
    lv_obj_set_y(ui_Calculator_Plus_Button, 74);
    lv_obj_set_align(ui_Calculator_Plus_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Plus_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Plus_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Plus_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Plus_Button, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Plus_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Plus_Label = lv_label_create(ui_Calculator_Plus_Button);
    lv_obj_set_width(ui_Calculator_Plus_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Plus_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Plus_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Plus_Label, "+");
    lv_obj_set_style_text_font(ui_Calculator_Plus_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Minus_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Minus_Button, 40);
    lv_obj_set_height(ui_Calculator_Minus_Button, 40);
    lv_obj_set_x(ui_Calculator_Minus_Button, 66);
    lv_obj_set_y(ui_Calculator_Minus_Button, 29);
    lv_obj_set_align(ui_Calculator_Minus_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Minus_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Minus_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Minus_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Minus_Button, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Minus_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Minus_Label = lv_label_create(ui_Calculator_Minus_Button);
    lv_obj_set_width(ui_Calculator_Minus_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Minus_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Minus_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Minus_Label, "-");
    lv_obj_set_style_text_font(ui_Calculator_Minus_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Times_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Times_Button, 40);
    lv_obj_set_height(ui_Calculator_Times_Button, 40);
    lv_obj_set_x(ui_Calculator_Times_Button, 66);
    lv_obj_set_y(ui_Calculator_Times_Button, -16);
    lv_obj_set_align(ui_Calculator_Times_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Times_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Times_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Times_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Times_Button, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Times_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Times_Label = lv_label_create(ui_Calculator_Times_Button);
    lv_obj_set_width(ui_Calculator_Times_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Times_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Times_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Times_Label, "x");
    lv_obj_set_style_text_font(ui_Calculator_Times_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Divide_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Divide_Button, 40);
    lv_obj_set_height(ui_Calculator_Divide_Button, 40);
    lv_obj_set_x(ui_Calculator_Divide_Button, 66);
    lv_obj_set_y(ui_Calculator_Divide_Button, -61);
    lv_obj_set_align(ui_Calculator_Divide_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Divide_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Divide_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Divide_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Divide_Button, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Divide_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Divide_Label = lv_label_create(ui_Calculator_Divide_Button);
    lv_obj_set_width(ui_Calculator_Divide_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Divide_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Divide_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Divide_Label, "/");
    lv_obj_set_style_text_font(ui_Calculator_Divide_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Equals_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Equals_Button, 40);
    lv_obj_set_height(ui_Calculator_Equals_Button, 40);
    lv_obj_set_x(ui_Calculator_Equals_Button, 22);
    lv_obj_set_y(ui_Calculator_Equals_Button, 74);
    lv_obj_set_align(ui_Calculator_Equals_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Equals_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Equals_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Equals_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Equals_Button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Equals_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Calculator_Equals_Button, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Calculator_Equals_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Calculator_Equals_Button, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Equals_Label = lv_label_create(ui_Calculator_Equals_Button);
    lv_obj_set_width(ui_Calculator_Equals_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Equals_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Equals_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Equals_Label, "=");
    lv_obj_set_style_text_color(ui_Calculator_Equals_Label, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Calculator_Equals_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Calculator_Equals_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_3_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_3_Button, 40);
    lv_obj_set_height(ui_Calculator_3_Button, 40);
    lv_obj_set_x(ui_Calculator_3_Button, 22);
    lv_obj_set_y(ui_Calculator_3_Button, 29);
    lv_obj_set_align(ui_Calculator_3_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_3_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_3_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_3_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_3_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_3_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_3_Label = lv_label_create(ui_Calculator_3_Button);
    lv_obj_set_width(ui_Calculator_3_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_3_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_3_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_3_Label, "3");
    lv_obj_set_style_text_font(ui_Calculator_3_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_6_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_6_Button, 40);
    lv_obj_set_height(ui_Calculator_6_Button, 40);
    lv_obj_set_x(ui_Calculator_6_Button, 22);
    lv_obj_set_y(ui_Calculator_6_Button, -16);
    lv_obj_set_align(ui_Calculator_6_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_6_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_6_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_6_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_6_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_6_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_6_Label = lv_label_create(ui_Calculator_6_Button);
    lv_obj_set_width(ui_Calculator_6_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_6_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_6_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_6_Label, "6");
    lv_obj_set_style_text_font(ui_Calculator_6_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_9_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_9_Button, 40);
    lv_obj_set_height(ui_Calculator_9_Button, 40);
    lv_obj_set_x(ui_Calculator_9_Button, 22);
    lv_obj_set_y(ui_Calculator_9_Button, -61);
    lv_obj_set_align(ui_Calculator_9_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_9_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_9_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_9_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_9_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_9_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_9_Label = lv_label_create(ui_Calculator_9_Button);
    lv_obj_set_width(ui_Calculator_9_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_9_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_9_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_9_Label, "9");
    lv_obj_set_style_text_font(ui_Calculator_9_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_8_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_8_Button, 40);
    lv_obj_set_height(ui_Calculator_8_Button, 40);
    lv_obj_set_x(ui_Calculator_8_Button, -22);
    lv_obj_set_y(ui_Calculator_8_Button, -61);
    lv_obj_set_align(ui_Calculator_8_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_8_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_8_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_8_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_8_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_8_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_8_Label = lv_label_create(ui_Calculator_8_Button);
    lv_obj_set_width(ui_Calculator_8_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_8_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_8_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_8_Label, "8");
    lv_obj_set_style_text_font(ui_Calculator_8_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_5_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_5_Button, 40);
    lv_obj_set_height(ui_Calculator_5_Button, 40);
    lv_obj_set_x(ui_Calculator_5_Button, -22);
    lv_obj_set_y(ui_Calculator_5_Button, -16);
    lv_obj_set_align(ui_Calculator_5_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_5_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_5_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_5_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_5_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_5_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_5_Label = lv_label_create(ui_Calculator_5_Button);
    lv_obj_set_width(ui_Calculator_5_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_5_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_5_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_5_Label, "5");
    lv_obj_set_style_text_font(ui_Calculator_5_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_2_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_2_Button, 40);
    lv_obj_set_height(ui_Calculator_2_Button, 40);
    lv_obj_set_x(ui_Calculator_2_Button, -22);
    lv_obj_set_y(ui_Calculator_2_Button, 29);
    lv_obj_set_align(ui_Calculator_2_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_2_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_2_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_2_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_2_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_2_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_2_Label = lv_label_create(ui_Calculator_2_Button);
    lv_obj_set_width(ui_Calculator_2_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_2_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_2_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_2_Label, "2");
    lv_obj_set_style_text_font(ui_Calculator_2_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_0_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_0_Button, 40);
    lv_obj_set_height(ui_Calculator_0_Button, 40);
    lv_obj_set_x(ui_Calculator_0_Button, -22);
    lv_obj_set_y(ui_Calculator_0_Button, 74);
    lv_obj_set_align(ui_Calculator_0_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_0_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_0_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_0_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_0_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_0_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_0_Label = lv_label_create(ui_Calculator_0_Button);
    lv_obj_set_width(ui_Calculator_0_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_0_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_0_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_0_Label, "0");
    lv_obj_set_style_text_font(ui_Calculator_0_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Decimal_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Decimal_Button, 40);
    lv_obj_set_height(ui_Calculator_Decimal_Button, 40);
    lv_obj_set_x(ui_Calculator_Decimal_Button, -66);
    lv_obj_set_y(ui_Calculator_Decimal_Button, 74);
    lv_obj_set_align(ui_Calculator_Decimal_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_Decimal_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_Decimal_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_Decimal_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_Decimal_Button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_Decimal_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Decimal_Label = lv_label_create(ui_Calculator_Decimal_Button);
    lv_obj_set_width(ui_Calculator_Decimal_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Decimal_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_Decimal_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_Decimal_Label, ".");
    lv_obj_set_style_text_color(ui_Calculator_Decimal_Label, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Calculator_Decimal_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Calculator_Decimal_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_1_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_1_Button, 40);
    lv_obj_set_height(ui_Calculator_1_Button, 40);
    lv_obj_set_x(ui_Calculator_1_Button, -66);
    lv_obj_set_y(ui_Calculator_1_Button, 29);
    lv_obj_set_align(ui_Calculator_1_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_1_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_1_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_1_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_1_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_1_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_1_Label = lv_label_create(ui_Calculator_1_Button);
    lv_obj_set_width(ui_Calculator_1_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_1_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_1_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_1_Label, "1");
    lv_obj_set_style_text_font(ui_Calculator_1_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_4_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_4_Button, 40);
    lv_obj_set_height(ui_Calculator_4_Button, 40);
    lv_obj_set_x(ui_Calculator_4_Button, -66);
    lv_obj_set_y(ui_Calculator_4_Button, -16);
    lv_obj_set_align(ui_Calculator_4_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_4_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_4_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_4_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_4_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_4_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_4_Label = lv_label_create(ui_Calculator_4_Button);
    lv_obj_set_width(ui_Calculator_4_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_4_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_4_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_4_Label, "4");
    lv_obj_set_style_text_font(ui_Calculator_4_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_7_Button = lv_btn_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_7_Button, 40);
    lv_obj_set_height(ui_Calculator_7_Button, 40);
    lv_obj_set_x(ui_Calculator_7_Button, -66);
    lv_obj_set_y(ui_Calculator_7_Button, -61);
    lv_obj_set_align(ui_Calculator_7_Button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Calculator_7_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Calculator_7_Button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Calculator_7_Button, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Calculator_7_Button, lv_color_hex(0x292831), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Calculator_7_Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_7_Label = lv_label_create(ui_Calculator_7_Button);
    lv_obj_set_width(ui_Calculator_7_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_7_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Calculator_7_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Calculator_7_Label, "7");
    lv_obj_set_style_text_font(ui_Calculator_7_Label, &ui_font_Comfortaa_26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Calculator_Equation_Panel = lv_obj_create(ui_Calculator);
    lv_obj_set_width(ui_Calculator_Equation_Panel, 120);
    lv_obj_set_height(ui_Calculator_Equation_Panel, 24);
    lv_obj_set_x(ui_Calculator_Equation_Panel, 0);
    lv_obj_set_y(ui_Calculator_Equation_Panel, -95);
    lv_obj_set_align(ui_Calculator_Equation_Panel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(ui_Calculator_Equation_Panel, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(ui_Calculator_Equation_Panel, LV_DIR_HOR);
    lv_obj_set_style_text_font(ui_Calculator_Equation_Panel, &ui_font_Comfortaa_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_pad_left(ui_Calculator_Equation_Panel, 6, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Calculator_Equation_Panel, 6, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Calculator_Equation_Panel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Calculator_Equation_Panel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_Calculator_Equation_Label = lv_label_create(ui_Calculator_Equation_Panel);
    lv_obj_set_width(ui_Calculator_Equation_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Calculator_Equation_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Calculator_Equation_Label, -9);
    lv_obj_set_y(ui_Calculator_Equation_Label, 0);
    lv_obj_set_align(ui_Calculator_Equation_Label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Calculator_Equation_Label, "mathymathymathymath.");

    lv_obj_add_event_cb(ui_Calculator_Equation_Panel, ui_event_Calculator_Equation_Panel, LV_EVENT_ALL, NULL);

}
*/