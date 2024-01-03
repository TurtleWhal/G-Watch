#include "lvgl.h"
#include "ui.h"
#include "themes.h"
#include "Preferences.h"
#include "ArduinoLog.h"
#include "screen_management.h"

#include "base64.hpp"
#include <vector>

lv_color_t ThemeColor = lv_palette_darken(LV_PALETTE_AMBER, 4);
extern Preferences Storage;

extern ClockInfo info;

void UpdateTestTheme(lv_event_t *e)
{
  lv_color_t TempTheme = lv_colorwheel_get_rgb(ui_Theme_Colorwheel);
  lv_obj_set_style_bg_color(ui_Theme_Apply_Button, TempTheme, LV_PART_MAIN);
  lv_label_set_text_fmt(ui_Theme_Hex_Label, "#%02X%02X%02X", TempTheme.ch.red * 255 / 31, TempTheme.ch.green * 255 / 63, TempTheme.ch.blue * 255 / 31);
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
    lv_obj_set_style_img_recolor(tick_index[60], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[61], ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (!info.theme.darkmode)
    {
      // Serial.println("Light Mode Applytheme");
      lv_obj_set_style_arc_color(ui_Default_Clock_Arc_Battery, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Hour_Hand, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Hour_Hand, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_img_recolor(ui_Default_Clock_Notification_Amount_Image, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_Default_Clock_Notification_Amount_Image, LV_OPA_100, LV_PART_MAIN);
      lv_obj_set_style_bg_color(ui_Default_Clock_Center_dot, lv_color_black(), LV_PART_MAIN);
    }
    else
    {
      // Serial.println("Dark Mode Applytheme");
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

    /*unsigned char * data = (unsigned char *)ps_malloc(15000);
    data = (unsigned char *) "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";

    unsigned char *binary = (unsigned char *)ps_malloc(15000);

    unsigned int binary_length = decode_base64(data, binary);

    LV_ATTRIBUTE_MEM_ALIGN uint8_t imgData[binary_length] = {};

    //Serial.println(binary_length);

    //printf("[");
    for (int i = 0; i < binary_length; i++)
    {
      printf("%c", binary[i]);
      imgData[i] = binary[i];
    }
    printf("\n");

    //lv_res_t res;
    //lv_img_decoder_dsc_t dsc;
    //res = lv_img_decoder_open(&dsc, &imgData, lv_color_black(), 0);

    lv_img_dsc_t img;
    img.header.always_zero = 0;
    img.header.w = 60;
    img.header.h = 60;
    img.data_size = 60 * 60 * LV_COLOR_DEPTH / 8;
    img.header.cf = LV_IMG_CF_RGB565;
    img.data = imgData;
      lv_obj_set_style_bg_img_src(ui_Album_Image_Panel, &img, LV_PART_MAIN);*/
    /*if (res == LV_RES_OK)
    {
      lv_img_decoder_close(&dsc);
      Serial.println("closed decoder");
      lv_obj_set_style_bg_img_src(ui_Album_Image_Panel, &img, LV_PART_MAIN);
      //lv_img_set_src(ui_Album_Image_Panel, &img);
      //Log.verboseln("%i", img.data);
    }*/

    // array size is 5978

    /*lv_img_dsc_t img;
        img.header.always_zero = 0;
        img.header.w = 60;
        img.header.h = 60;
        img.data_size = 60 * 60 * LV_COLOR_DEPTH / 8;
        img.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        img.data = Untitled;*/

    // lv_obj_set_style_bg_img_src(ui_Album_Image_Panel, &Untitled_map, LV_PART_MAIN);
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

// takes in a 16-bit lv_color_t variable and converts it to a 32-bit hex string EX: #FFFFFF
/*auto ColorToHexString(lv_color_t color) {

  Log.verboseln("ColorToHexString R:%i, G:%i, B:%i", color.ch.red, color.ch.green, color.ch.blue);
  char output[100];
  //sprintf(output, "#%02X%02X%02X", color.ch.red * 255 / 31, color.ch.green * 255 / 63, color.ch.blue * 255 / 31);
  sprintf(output, "#rgb", color.ch.red, color.ch.green, color.ch.blue);
  String outputString = output;
  return outputString;
}*/

// 89504e47da1aa000d494844520003c0003c82000b59e4e2500184694343504943432070726f66696c650028917d913d48c3401cc55f3fa4522a1dec202218a13ad945451cb5a45a8106a85561d4c2efd8226d498a8ba3e05a70f063b1eae0e2acab83ab2087e80b8ba3829ba4889ff4ba2d623c38eec7bb7b8fbb7780bf5965aa199c554cd3232a9a490cbafaa15784318c2846109498a9cf89621a9ee3eb1e3ebede257896f7b93f479f523019e0138867996e58c41bc4d39b96ce799f38c6ca92427c4e3c6ed05891fb92ebbfcc6b9e4b09f67c68c6c669e38462c94ba58ee62563654e229e2b8a26a94efcfb9ac70dee2ac56ebac7d4ffec248415b59e63acd21a4b08825881020a38eaaab090a05523c54486f6931efe41c72f924b265758c1cba84185e4f8c1ffe077b7667172c24d8a24819e17dbfe18542bb40ab61dbdfc7b6dd3a12cfc095d6f1d79ac0cc27e98d8e163f2a2dbc0c5754793f780cb1d60e04997cc991234fdc522f07e46df947fa6f81f09adb5b7b1fa7f4096ba4adf0787c05889b2d73ddedddbdddbbf67dafdfd0841572adbe7f784000970485973002e23002e23178a53f76000774494d457e7a1ff23d4e1b92130001974455874436f6d6d656e7404372656174656420776974682047494d505781e17004c64944415468ded59a3d6ee33c1086475917bd212ec45f4c1c556ba33152e7292142e84f0eaa5c18e145c45840d77011e02be8a586c39921a524bb6b55712c53f47f3f3ce48d64c7fcabc31fcdbd9fbbeb7f8c4ffe86ea23fe8afc165fcbdc7b0f83977eca47b37c7b977f1d7eccee415ab178901d923dc4c5d9f58fc67e27834a26d527bc3504659656fe33dbec4fe9b685db231068a5becbfc1e11d7d1e19eeee34af82b60eed92ac65670f1d1ca10967287cd38385cafbdf7fd8cbd96d7318188135f3ef58d6ad5b2be11e186b0a88d27106ef7fc4aadfbf41f3e19ef191f64f384caef923d342cf4f71fcbc2d4d84362f5eb39b69743f0318f60bca67d5b71af2183ed7765fe3683cd4a845ee4debb89fc27b72e68fdce5fa1d9ef2274c8861b08653ae7ce71a5fba66421b2875ae8b97bf9edd393e4f784bba37f5b462f6cf455d0a9f12612f5127a19372db4f5293f8035a31f9d252c969e5cd8bee5a6e5a5ca21bec619292c0223c829dc65abb4a1a81d4cb4a7bb718975edef61137bfc09271c73af9c1e162556cfdd4ac4122e9b43ae95f29d7d31d49d20cfaddbecddf0b9ecdf3eeee068f9b842ea43c2e3a6945c091649f1c2b6024c149a5e14ae526143b1745cb1e6cfd057da23ef38f2de84c4226554f75d65cf73b07d2e89c2bad8bac1547e67228dd47de839a49ca775af48e44749d0b01047e3c9e2e1237b4b71e6ee92fc96fefc8a725e2cbd9115d4f4a6334b3e28e9f9e559b50db5427d624dedfc91e9284f8b7f3f4ad116b2347e6f0e6f29c6a9f9cdf992f94330f4b8b140dae3da47d0d683c4f857c049d42e6438f01a2e8d5fb97565bc7a32c649539c31f3aca421f8d6f92f2f361ffb66fae588a73629c55497f99731f8ddf284d4987878b628d8488c4f970273b26c1d7cb5ddec70b874f99d593bd7abc08dbe3a45ab1f1bad3dc2883983c2d679f92014485914148c3d2a296f5cd1a8ae3ad6e3f295b885af3fbad35cedc50a31ab64223bb5c854c1ef8ca9cb25ee1d837d7eaa9547a5692c762ae3f9998a6e9ac568de14ceabd692aeeac55b8d9e746c177f76e924a5a65a8e0944c430de977dda3e39c2adcc8770a81d6cfe9b0aa32a6f1dcf33e9dc754d4dc552ea45ea3ffd363795a42fa8cc17e287be02110996bad535f75a24443572393b7ca1269f9fe796e6d1f3be19cbb1780699d68197a3c70c31ec5eb6f374c434b6d2821693997445967d4a5ca8ac4c63f86675add69ce155fa1b844facb190c03d58d8a6a1776b63de9532dcdee8cd2c7d7b393bf7206cebc1aea484fa51e2136fe519f4660d76fdd5ab294329f8c717ef5a89fa3102f7ef8be88a2da321ef311c9be57e83f3b2fbec9f9747b1b984b4c4f9fd3d30494d965f3d7e6c8e63ae28bb9ba248c74be31c44c450fca83485eef935c6436c2b236ecdd39fb0ed46f94138358f6bf7fc1a4a17fb3e181ce5aee4928ab5dc443029b8c5765f2a875185726f554c581253368d0c11f84e7652f4a8baf7431ac84d86f7abfe560d34024eefb96d69e9f2dbb8dbe45e78f175bc98b928430d689fd916fb94572b4ec91cb513d46bfb430294fd11ba55bf13bc3bbc41f7f2a4036d0d63a713d68e96519a6d0947a8f2a2649b6bab643458c14a86911f37a4ee75f3afee7f7d50d61fa02c8adc000049454e44ae426082