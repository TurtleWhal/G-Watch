#include "Preferences.h"
#include "ui.h"
#include "usersettings.h"
#include "ArduinoLog.h"
#include "screen_management.h"

Preferences Storage;
extern ClockInfo info;

#define DEFAULTNOTIFICATIONLENGTH 10
#define DEFAULTSTEPGOAL 10000

lv_obj_t *typingPointer;

void InitUserSettings()
{
  Storage.begin("Settings");
  if (!Storage.isKey("NotifLength") or Storage.getUInt("NotifLength") < 1)
  {
    Storage.putUInt("NotifLength", DEFAULTNOTIFICATIONLENGTH);
    Log.verboseln("WARNING: STORED NOTIFLENGTH UNDEFINED, DEFAULT TO %i", DEFAULTNOTIFICATIONLENGTH);
  }

  // lv_label_set_text_fmt(ui_Step_Counter_Text, "%i", Storage.getUInt("StepGoal"));
  if (!Storage.isKey("StepGoal") or Storage.getUInt("StepGoal") < 1)
  {
    Storage.putUInt("StepGoal", DEFAULTSTEPGOAL);
    Log.verboseln("WARNING: STORED STEPGOAL UNDEFINED, DEFAULT TO %i", DEFAULTSTEPGOAL);
  }
  info.health.stepgoal = Storage.getUInt("StepGoal");

  if (!Storage.isKey("Steps") or Storage.getUInt("Steps") < 1)
  {
    Storage.putUInt("Steps", 0);
    Log.verboseln("WARNING: STORED STEPS UNDEFINED, DEFAULT TO %i", 0);
  }

  PrintSettings();

  // lv_label_set_text_fmt(ui_Step_Counter_Text, "%s", Storage.isKey("StepGoal") ? "TRUE":"FALSE");
}

void UpdateSettings(lv_event_t *e)
{
  if (Storage.getUShort("StepGoal") != atoi(lv_textarea_get_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))))
  {
    Storage.putUShort("StepGoal", atoi(lv_textarea_get_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))));
    Storage.putBool("StepReach", 0);
  }

  Storage.putUInt("NotifLength", atoi(lv_textarea_get_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))));

  String BTnametemp = lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL));
  Storage.putBytes("BTname", BTnametemp.c_str(), 17);
  info.bt.name = BTnametemp;
  // Serial.println(lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));

  Storage.putUInt("Theme", lv_colorwheel_get_rgb(ui_Theme_Colorwheel).full);
  ApplyTheme(nullptr);

  Storage.putBool("DarkMode", lv_obj_has_state(ui_Dark_Mode_Setting_Switch, LV_STATE_CHECKED));
  // Serial.println(Storage.getBool("DarkMode"));

  PrintSettings();
}

void PrintSettings()
{
  char BTnamechar[17];
  Storage.getBytes("BTname", BTnamechar, 17);

  Log.verboseln("");
  Log.verboseln("|-------- Stored Settings ---------");
  Log.verboseln("|- Step Goal ------------ %i", Storage.getUShort("StepGoal"));
  Log.verboseln("|- Notification Length -- %i", Storage.getUInt("NotifLength"));
  Log.verboseln("|- Bluetooth Name ------- %s", BTnamechar);
  Log.verboseln("|- Theme Color ---------- %i", Storage.getUInt("Theme"));
  Log.verboseln("|- Dark Mode ------------ %i", Storage.getBool("DarkMode"));
  Log.verboseln("-----------------------------------");
  Log.verboseln("");
}

void editSetting(lv_event_t *e)
{
  typingPointer = lv_event_get_target(e);
  lv_obj_clear_flag(ui_Numberpad_Panel, LV_OBJ_FLAG_HIDDEN);
  // lv_label_set_text(lv_obj_get_child(ui_Keyboard_Setting_panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(typingPointer), UI_COMP_SETTING_PANEL_SETTING_LABEL)));
  lv_textarea_set_text(lv_obj_get_child(ui_Keyboard_Setting_panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), lv_textarea_get_text(typingPointer));
  lv_label_set_text(lv_obj_get_child(ui_Keyboard_Setting_panel, UI_COMP_SETTING_PANEL_SETTING_PANEL), lv_label_get_text(lv_obj_get_child(lv_obj_get_parent(typingPointer), UI_COMP_SETTING_PANEL_SETTING_PANEL)));
}

void applyKeyboardValue(lv_event_t *e)
{
  lv_textarea_set_text(typingPointer, lv_textarea_get_text(lv_obj_get_child(ui_Keyboard_Setting_panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));
  lv_obj_add_flag(ui_Numberpad_Panel, LV_OBJ_FLAG_HIDDEN);
  UpdateSettings(nullptr);
}