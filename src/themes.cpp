#include "lvgl.h"
#include "ui.h"
#include "themes.h"
#include "Preferences.h"
#include "screen_management.h"

lv_color_t ThemeColor = lv_palette_darken(LV_PALETTE_AMBER, 4);
extern Preferences Storage;

extern ClockInfo info;

void UpdateTestTheme(lv_event_t *e)
{
  lv_color_t TempTheme = lv_colorwheel_get_rgb(ui_Theme_Colorwheel);
  lv_obj_set_style_bg_color(ui_Theme_Apply_Button, TempTheme, LV_PART_MAIN);
  lv_label_set_text_fmt(ui_Theme_Hex_Label, "#%02X%02X%02X", TempTheme.ch.red * 255 / 31, TempTheme.ch.green * 255 / 63, TempTheme.ch.blue * 255 / 31);
  /*Serial.print("Red:");
  Serial.println(TempTheme.ch.red);
  Serial.print("Green:");
  Serial.println(TempTheme.ch.green);
  Serial.print("Blue:");
  Serial.println(TempTheme.ch.blue);*/
}

void ToggleTheme(lv_event_t *e)
{
  static bool themeexpanded;
  if (themeexpanded)
  {
    ThemeSettingShrink_Animation(ui_Theme_Setting_Panel, 0);
    lv_img_set_angle(ui_Theme_Expand_Arrow, 1800);
    themeexpanded = 0;
    lv_obj_add_flag(ui_Theme_Colorwheel, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    ThemeSettingExpand_Animation(ui_Theme_Setting_Panel, 0);
    lv_img_set_angle(ui_Theme_Expand_Arrow, 0);
    themeexpanded = 1;
    lv_obj_clear_flag(ui_Theme_Colorwheel, LV_OBJ_FLAG_HIDDEN);
  }
}

void ApplyTheme(lv_event_t *e)
{
  Serial.println("ApplyTheme");
  lv_disp_t *dispp = lv_disp_get_default();
  if (Storage.isKey("Theme"))
  {
    lv_color16_t color;
    color.full = Storage.getUInt("Theme");
    lv_theme_t *theme = lv_theme_default_init(dispp, color, lv_palette_main(LV_PALETTE_RED),
                                              Storage.getBool("DarkMode"), LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ThemeColor = color;
    info.theme.color = color;
    info.theme.darkmode = Storage.getBool("DarkMode");
  }
  else
  {
    lv_theme_t *theme = lv_theme_default_init(dispp, ThemeColor, lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
  }

  //////////Apply colors to unthemed items//////////
  extern lv_obj_t *tick_index[62];

  /*if (!Storage.getBool("DarkMode"))
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  else
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x101418), LV_PART_MAIN);*/

  // Clock Screen
#ifdef UPDATE_ELEMENTS
  if (ui_Default_Clock != NULL)
  {
    lv_obj_set_style_img_recolor(ui_Default_Clock_Minute_Hand, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Default_Clock_Steps_Image, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Default_Clock_Step_Counter_Text, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Default_Clock_Date_Numerical, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Default_Clock_Notification_Amount_Number, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Notification_Timer, ThemeColor, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Notification_Image_Panel, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[60], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[61], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (!info.theme.darkmode)
    {
      Serial.println("Light Mode Applytheme");
      lv_obj_set_style_arc_color(ui_Default_Clock_Arc_Battery, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Hour_Hand, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Hour_Hand, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Notification_Amount_Image, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Notification_Amount_Image, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_bg_color(ui_Default_Clock_Center_dot, lv_color_black(), LV_PART_MAIN);
    }
    else
    {
      Serial.println("Dark Mode Applytheme");
      lv_obj_set_style_arc_color(ui_Default_Clock_Arc_Battery, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Hour_Hand, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Hour_Hand, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Notification_Amount_Image, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Notification_Amount_Image, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_bg_color(ui_Default_Clock_Center_dot, lv_color_white(), LV_PART_MAIN);
    }
  }

  if (ui_Blocky_Clock != nullptr)
  {
    lv_obj_set_style_text_color(ui_Blocky_Clock_Clock_Layer_1, ThemeColor, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_Blocky_Clock_Clock_Layer_2, ThemeColor, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_Blocky_Clock_Clock_Layer_3, ThemeColor, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_Blocky_Clock_Clock_Layer_4, ThemeColor, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_Blocky_Clock_Clock_Layer_5, ThemeColor, LV_PART_MAIN);

    lv_obj_set_style_outline_color(ui_Blocky_Clock_Battery_Bar, ThemeColor, LV_PART_MAIN);
    lv_obj_set_style_outline_color(ui_Blocky_Clock_Battery_Bar, ThemeColor, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(ui_Blocky_Clock_Battery_Contact, ThemeColor, LV_PART_MAIN);
  }

  // Compass Screen
  if (ui_Compass != NULL)
  {
    lv_obj_set_style_text_color(ui_Compass_Direction, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Compass_N, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Compass_Arrow, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  // Calculator Screen
  if (ui_Calculator != NULL)
  {
    lv_obj_set_style_bg_color(ui_Calculator_Keyboard, ThemeColor, LV_PART_ITEMS | LV_STATE_CHECKED);
  }

  // Calculator Screen
  if (ui_Music != NULL)
  {
    lv_obj_set_style_img_recolor(ui_Background_Album_Image, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    if (!info.theme.darkmode)
    {
      lv_obj_set_style_bg_color(ui_Control_Panel, lv_color_white(), LV_PART_MAIN);
      lv_obj_set_style_shadow_color(ui_Control_Panel, lv_color_white(), LV_PART_MAIN);
    }
  }

  // Alarm Set Screen
  if (ui_Set_Alarm != NULL)
  {
    lv_obj_set_style_text_color(ui_AM, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  // Stopwatch Screen
  if (ui_Stopwatch != NULL)
  {
    if (!info.theme.darkmode) // if light mode, fix the black box under the stopwatch time
      lv_obj_set_style_bg_color(ui_Stopwatch_Time_Underline_Black, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    // else
    // lv_obj_set_style_bg_color(ui_Stopwatch_Time_Underline_Black, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  }

  // Health Screen
  if (ui_Health != NULL)
  {
    lv_chart_set_series_color(ui_Steps_Chart, lv_chart_get_series_next(ui_Steps_Chart, NULL), ThemeColor);
  }
#endif
}

lv_color_t GetTheme()
{
  return ThemeColor;
}