#include "Arduino.h"
#include "lvgl.h"
//#include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "notifications.h"
#include "power_managment.h"

Notification NotificationList[11];
int NotificationCount = 0;

bool notificationshowing = 0;
int notificationid = 1;
int notificationtime;
bool Donotdisturb;
extern TWatchClass *twatch;
extern int NotificationLength;


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

  // Create the widget in the notifications screen
  if (Store)
  {
    // lv_obj_t *NotificationComp = ui_Notification_Widget_create(ui_Notification_Panel);
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), lv_label_get_text(ui_Notification_Title));
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), lv_label_get_text(ui_Notification_Text));
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), lv_label_get_text(ui_Notification_Source));
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), NotificationList[NotificationCount].Title.c_str());
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), NotificationList[NotificationCount].Text.c_str());
    //  lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), NotificationList[NotificationCount].Source.c_str());
  }
}

void drawnotifications(lv_event_t *e)
{
  ui_Notifications_screen_init();
  _ui_screen_change(ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0);
  for (int i = 0; i < NotificationCount; i++)
  {
    lv_obj_t *NotificationComp = ui_Notification_Widget_create(ui_Notification_Panel);
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), NotificationList[i].Title.c_str());
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), NotificationList[i].Text.c_str());
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), NotificationList[i].Source.c_str());
    // lv_obj_set_user_data(NotificationComp, &NotificationList[i]);
    lv_obj_set_user_data(NotificationComp, (void *)i);
  }
}

void deletenotification(lv_event_t *e)
{
  // lv_obj_del(lv_event_get_target(e));
  // lv_obj_set_style_bg_color(lv_event_get_target(e), lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  // lv_obj_set_style_bg_opa(lv_event_get_target(e), 255, LV_PART_MAIN);
  // lv_obj_set_style_bg_color(ui_Notification_Widget2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  // lv_obj_set_style_bg_opa(ui_Notification_Widget2, 255, LV_PART_MAIN);
  // lv_obj_set_x(lv_event_get_target(e), 10);
  auto index = lv_obj_get_user_data(lv_event_get_target(e));
  // index->Title = "Deleted";
  NotificationList[(int)index].Title = "Deleted";
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

void drawnotificationarc()
{
  if (notificationshowing)
  {
    lv_arc_set_value(ui_Notification_Timer, ((millis() - notificationtime) / (NotificationLength * 10)) * 10);
    if (notificationtime + (NotificationLength * 1000) < millis())
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