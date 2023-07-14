#include "lvgl.h"
#include "ui.h"
#include "themes.h"
#include "Preferences.h"

lv_color_t ThemeColor = lv_palette_darken(LV_PALETTE_AMBER, 4);
extern Preferences Storage;

void UpdateTestTheme(lv_event_t *e)
{
  lv_obj_set_style_bg_color(ui_Theme_Apply_Button, lv_colorwheel_get_rgb(ui_Theme_Colorwheel), LV_PART_MAIN);
  // char color[32];
  // sprintf(color, "Hex: #%i", lv_color_hex(lv_colorwheel_get_hsv(ui_Theme_Colorwheel)));
  // sprintf(color, "Hex: #%X%X%X", lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.red, lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.green_h, lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.blue);
  /*Serial.print(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.red);
  Serial.print(", ");
  Serial.print(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.green_l);
  Serial.print(", ");
  Serial.println(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.blue);*/
  // lv_label_set_text(ui_Theme_Hex_Label, color);
  lv_label_set_text(ui_Theme_Hex_Label, "Hex Code");
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

void ApplyTheme()
{
  lv_disp_t *dispp = lv_disp_get_default();
  if (Storage.isKey("Theme"))
  {
    lv_color16_t color;
    color.full = Storage.getInt("Theme");
    lv_theme_t *theme = lv_theme_default_init(dispp, color, lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ThemeColor = color;
  }
  else
  {
    lv_theme_t *theme = lv_theme_default_init(dispp, ThemeColor, lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
  }

  //////////Apply colors to unthemed items//////////
  extern lv_obj_t *tick_index[62];

  // Clock Screen
#ifdef UPDATE_ELEMENTS
    lv_obj_set_style_img_recolor(ui_Minute_Hand, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Steps_Image, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Step_Counter_Text, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Date_Numerical, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Notification_Timer, ThemeColor, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Notification_Image_Panel, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[60], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[61], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);

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

  // Alarm Set Screen
  if (ui_Set_Alarm != NULL)
  {
    lv_obj_set_style_text_color(ui_AM, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  #endif
}