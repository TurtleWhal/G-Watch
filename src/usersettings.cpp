#include "Preferences.h"
#include "ui.h"
#include "usersettings.h"

Preferences Storage;

#define defaultnotificationlength 10
#define defaultStepgoal 10000

void InitUserSettings()
{
  Storage.begin("Settings");
  if (!Storage.isKey("NotifLength") or Storage.getUInt("NotifLength") < 1)
    Storage.putUInt("NotifLength", defaultnotificationlength);

  // lv_label_set_text_fmt(ui_Step_Counter_Text, "%i", Storage.getUInt("StepGoal"));
  if (!Storage.isKey("StepGoal") or Storage.getUInt("StepGoal") < 1)
    Storage.putUInt("StepGoal", defaultStepgoal);

  // lv_label_set_text_fmt(ui_Step_Counter_Text, "%s", Storage.isKey("StepGoal") ? "TRUE":"FALSE");
}

void UpdateSettings(lv_event_t *e)
{
  if (Storage.getUInt("StepGoal") != atoi(lv_textarea_get_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))))
  {
    Storage.putUInt("StepGoal", atoi(lv_textarea_get_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))));
    Storage.putBool("StepReach", 0);
  }

  Storage.putUInt("NotifLength", atoi(lv_textarea_get_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL))));

  Storage.putBytes("BTname", lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)), 17);
  // Serial.println(lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));

  Storage.putUInt("Theme", lv_colorwheel_get_rgb(ui_Theme_Colorwheel).full);
  ApplyTheme(nullptr);
}