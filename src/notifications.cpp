#include "Arduino.h"
#include "lvgl.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "notifications.h"
#include "power_managment.h"
#include "Preferences.h"
#include "ArduinoLog.h"
#include "BThandle.h"
#include "themes.h"
#include "screen_management.h"

#define NOTIFPOPUP_IMAGE ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_IMAGE_PANEL_NOTIFICATION_IMAGE)
#define NOTIFPOPUP_MAIN ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET)

extern ClockInfo info;

LV_IMG_DECLARE(ui_img_discord_icon_png);
LV_IMG_DECLARE(ui_img_youtube_icon_png);
LV_IMG_DECLARE(ui_img_sms_icon_png);
LV_IMG_DECLARE(ui_img_gmail_icon_png);
LV_IMG_DECLARE(ui_img_steps_large_png);   // assets\Steps Large.png
LV_IMG_DECLARE(ui_img_messages_icon_png); // assets\Messages Icon.png

Notification NotificationList[11];
lv_obj_t *NotificationComp[10];

int NotificationCount = 0;

bool notificationshowing = 0;
int notificationid = 1;
int notificationtime;
bool Donotdisturb;
extern TWatchClass *twatch;
extern Preferences Storage;

int TempID;

lv_obj_t *NotifPopup;

void ShowNotification(String Title, String Text, String Source, int id)
{
  // Create the widget in the Clock screen
  Wakeup("Notification Received");
  if (notificationshowing)
    NotificationHide(LV_ANIM_OFF);

  NotifPopup = ui_Notification_Widget_create(lv_scr_act());
  lv_obj_set_x(NOTIFPOPUP_MAIN, 0);
  lv_obj_set_y(NOTIFPOPUP_MAIN, -160);

  // lv_label_set_text(ui_Notification_Title, Title.c_str());
  // lv_label_set_text(ui_Notification_Text, Text.c_str());
  // lv_label_set_text(ui_Notification_Source, Source.c_str());

  lv_label_set_text(ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), Title.c_str());
  lv_label_set_text(ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), Text.c_str());
  lv_label_set_text(ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), Source.c_str());
  lv_obj_set_style_bg_color(ui_comp_get_child(NotifPopup, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_IMAGE_PANEL), info.theme.color, LV_PART_MAIN);

  if (Source == "Messages")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_messages_icon_png);
  }
  else if (Source == "SMS")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_sms_icon_png);
  }
  else if (Source == "YouTube")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_youtube_icon_png);
  }
  else if (Source == "Discord")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_discord_icon_png);
  }
  else if (Source == "Gmail")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_gmail_icon_png);
  }
  else if (Source == "local.stephandle")
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_steps_large_png);
  }
  else
  {
    lv_img_set_src(NOTIFPOPUP_IMAGE, &ui_img_notificationbell_png);
  }

  // sources : Gmail, YouTube, Messages

  NotificationList[10].Title = Title;
  NotificationList[10].Text = Text;
  NotificationList[10].Source = Source;
  NotificationList[10].id = TempID;
  NotificationList[10].img = lv_img_get_src(NOTIFPOPUP_IMAGE);

  info.notification.lasttitle = NotificationList[10].Title;
  info.notification.lasttext = NotificationList[10].Text;
  info.notification.lastimg = NotificationList[10].img;

  TempID = id;

  NotificationShow_Animation(NOTIFPOPUP_MAIN, 0);

  notificationtime = millis();
  notificationshowing = 1;

  if (!Donotdisturb)
    twatch->motor_shake(2, 30);

  /*if (lv_scr_act() == ui_Notifications)
  {
    // lv_obj_clean(ui_Notification_Panel);
    // DrawNotifications(nullptr);
    ui_Notifications_screen_init();
  }*/
}

void DrawNotifications(lv_event_t *e)
{
  /*if (lv_scr_act() != ui_Notifications)
  {
    // ui_Notifications_screen_init();
    //_ui_screen_change(ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0);
    // lv_scr_load_anim(ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 0);
    //_ui_screen_change( &ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Notifications_screen_init);
  }*/
  if (NotificationCount)
  {
    lv_obj_add_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < NotificationCount; i++)
    {
      NotificationComp[i] = ui_Notification_Widget_create(ui_Notification_Panel);
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), NotificationList[i].Title.c_str());
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), NotificationList[i].Text.c_str());
      lv_label_set_text(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), NotificationList[i].Source.c_str());
      lv_obj_set_style_bg_color(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_IMAGE_PANEL), GetTheme(), LV_PART_MAIN);
      lv_img_set_src(ui_comp_get_child(NotificationComp[i], UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_IMAGE_PANEL_NOTIFICATION_IMAGE), NotificationList[i].img);
      lv_obj_set_user_data(NotificationComp[i], (void *)i);

      NotificationList[10].Title = lv_label_get_text(ui_Notification_Title);
      NotificationList[10].Text = lv_label_get_text(ui_Notification_Text);
      NotificationList[10].Source = lv_label_get_text(ui_Notification_Source);
      NotificationList[10].id = TempID;
      NotificationList[10].img = lv_img_get_src(ui_Notification_Image);
      // PushNotification(1);
    }
  }
  else
    lv_obj_clear_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
}

void DeleteNotification(lv_event_t *e)
{
  if (lv_scr_act() == ui_Notifications)
  {
    int index = (int)lv_obj_get_user_data(lv_event_get_target(e));
    PopNotification(index + 1);
    lv_obj_del_delayed(lv_event_get_target(e), 350);

    if (!NotificationCount)
      lv_obj_clear_flag(ui_No_New_Notifications_Label, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    notificationshowing = 0;
    lv_obj_del_delayed(lv_event_get_target(e), 350);
    Serial.println("Notification Dismiss");
  }
}

void NotificationDismiss(lv_event_t *e)
{
  notificationshowing = 0;
  NotificationHide_Animation(ui_Notification_Popup, 300);
  Serial.println("Notification Dismiss");
}

void NotificationHide(bool anim)
{
  notificationshowing = 0;
  if (anim)
    NotificationHide_Animation(NOTIFPOPUP_MAIN, 0);
  else
    lv_obj_set_y(NOTIFPOPUP_MAIN, -160);
  PushNotification(1);
  lv_obj_del_delayed(NotifPopup, 350);
  //lv_obj_del_async(NotifPopup);
  Serial.println("Notification Hide");
}

void PushNotification(int index)
{
  int i;
  for (i = NotificationCount; index <= i; i--)
  {
    if (i != 10)
      NotificationList[i] = NotificationList[i - 1];
  }
  NotificationList[i] = NotificationList[10];
  if (NotificationCount++ < 10)
    info.notification.count = NotificationCount;
  // lv_label_set_text_fmt(ui_Notification_Amount_Number, "%i", NotificationCount);
  // NotificationCount++;

  if (lv_scr_act() == ui_Notifications)
  {
    //_ui_screen_change(&ui_Notifications, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_Notifications_screen_init);
    for (int i = 2; i <= lv_obj_get_child_cnt(ui_Notification_Panel); i++)
    {
      // lv_obj_del(lv_obj_get_child(ui_Notification_Panel, i));
      Log.verboseln("I have %i Childeren", lv_obj_get_child_cnt(ui_Notification_Panel));
      lv_obj_t *child = lv_obj_get_child(ui_Notification_Panel, i);
      lv_obj_del(child);
      Log.verboseln("I just MurDeReD the %ith Child", i);
      // szlv_obj_set_style_bg_color(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
      // if (lv_obj_get_user_data(child))
      // Serial.println(lv_obj_get_child_cnt(ui_Notification_Panel));
    }
    Log.verboseln("Imma DrawNotifications and probably die");
    DrawNotifications(nullptr);

    // DrawNotifications(nullptr);
  }

  Log.verboseln("Pushed Notification with id %i", index);
}

void PopNotification(int index)
{
  Log.verboseln("Popped Notification with id %i", index);
  if (!NotificationCount)
    return;
  /*char temp[50];
  sprintf(temp, "{\"t\":\"notify\",\"id\":%i,\"n\":\"DISMISS\"}", NotificationList[index - 1].id);
  BTsend(temp);*/
  int i;
  for (i = index; i < NotificationCount; i++)
  {
    if (i != 10)
      NotificationList[i - 1] = NotificationList[i];
    if (NotificationComp[1] != NULL)
      lv_obj_set_user_data(NotificationComp[i], (void *)i - 1);
  }
  // NotificationCount--;
  // lv_label_set_text_fmt(ui_Notification_Amount_Number, "%i", --NotificationCount);
  info.notification.count = --NotificationCount;
}

void NotificationHandle()
{
  if (notificationtime + (Storage.getUInt("NotifLength") * 1000) < millis())
    if (notificationshowing)
      NotificationHide();
}

void ToggleDoNotDisturb(lv_event_t *e)
{
  Donotdisturb = !Donotdisturb;
}

bool NotificationActive()
{
  return notificationshowing;
}

void AddFakeNotifications()
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
    PushNotification(1);
  }
}