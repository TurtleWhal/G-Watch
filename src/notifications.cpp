#include "Arduino.h"
#include "lvgl.h"
// #include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "notifications.h"
#include "power_managment.h"
#include "Preferences.h"

Notification NotificationList[11];
lv_obj_t *NotificationComp[10];

int NotificationCount = 0;

bool notificationshowing = 0;
int notificationid = 1;
int notificationtime;
bool Donotdisturb;
extern TWatchClass *twatch;
extern Preferences Storage;

void shownotification(bool Store)
{
  // Create the widget in the Clock screen
  Wakeup("Notification Received");
  lv_obj_set_x(ui_Notification_Popup, 0);
  lv_obj_set_y(ui_Notification_Popup, -160);
  NotificationShow_Animation(ui_Notification_Popup, 0);
  notificationtime = millis();
  notificationshowing = 1;
  if (!Donotdisturb)
    twatch->motor_shake(2, 30);
}

void drawnotifications(lv_event_t *e)
{
  if (lv_scr_act() != ui_Notifications)
  {
    ui_Notifications_screen_init();
    //_ui_screen_change(ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0);
    lv_scr_load_anim(ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 0);
  }
  if (NotificationCount)
  {
    lv_obj_add_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < NotificationCount; i++)
    {
      NotificationComp[i] = ui_Notification_Widget_create(ui_Notification_Panel);
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), NotificationList[i].Title.c_str());
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), NotificationList[i].Text.c_str());
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), NotificationList[i].Source.c_str());
      lv_obj_set_user_data(NotificationComp[i], (void *)i);
    }
  }
  else
    lv_obj_clear_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
}

void deletenotification(lv_event_t *e)
{
  auto index = lv_obj_get_user_data(lv_event_get_target(e));
  popnotification((int)index + 1);
  lv_obj_del_delayed(NotificationComp[(int)index], 350);
  if (!NotificationCount)
    lv_obj_clear_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
}

void notificationdismiss(lv_event_t *e)
{
  lv_obj_set_x(ui_Notification_Popup, 0);
  lv_obj_set_y(ui_Notification_Popup, -60);
  NotificationDismiss_Animation(ui_Notification_Popup, 0);
  notificationtime = 0;
  notificationshowing = 0;
}

void pushnotification(int index)
{
  int i;
  for (i = NotificationCount; index <= i; i--)
  {
    if (i != 10)
      NotificationList[i] = NotificationList[i - 1];
  }
  NotificationList[i] = NotificationList[10];
  if (NotificationCount < 10)
    NotificationCount++;
}

void popnotification(int index)
{
  if (!NotificationCount)
    return;
  int i;
  for (i = index; i < NotificationCount; i++)
  {
    if (i != 10)
      NotificationList[i - 1] = NotificationList[i];
  }
  NotificationCount--;
}

void drawnotificationarc()
{
  if (notificationshowing)
  {
    lv_arc_set_value(ui_Notification_Timer, ((millis() - notificationtime) / (Storage.getUInt("NotifLength") * 10)) * 10);
    if (notificationtime + (Storage.getUInt("NotifLength") * 1000) < millis())
    {
      NotificationHide_Animation(ui_Notification_Popup, 0);
      notificationshowing = 0;
    }
  }
}

void ToggleDoNotDisturb(lv_event_t *e)
{
  /*if (!Donotdisturb)
  {
    Donotdisturb = 1;
    // lv_obj_set_style_bg_color(ui_Do_Not_Disturb_Button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  if (Donotdisturb)
  {
    Donotdisturb = 0;
    // lv_obj_set_style_bg_color(ui_Do_Not_Disturb_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
  }*/

  Donotdisturb = !Donotdisturb;
  // erial.println(Donotdisturb);
}

bool NotificationActive()
{
  return notificationshowing;
}

void FakeNotes()
{
  for (int i = 1; i < 7; i++)
  {
    String temp;
    temp = "Title: ";
    temp += i;
    NotificationList[10].Title = temp;
    temp = "Text: ";
    temp += i;
    NotificationList[10].Text = temp;
    temp = "Source: ";
    temp += i;
    NotificationList[10].Source = temp;
    pushnotification(1);
  }
}