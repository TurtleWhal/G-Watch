#include "Arduino.h"
#include <BluetoothSerial.h>
#include "lvgl.h"
#include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "BThandle.h"
#include "notifications.h"
#include "Preferences.h"

BluetoothSerial SerialBT;
extern TWatchClass *twatch;
extern Notification NotificationList[11];
extern Preferences Storage;
extern int notificationid;
lv_obj_t *ui_Notification_Widgets[1];
LV_IMG_DECLARE(ui_img_bluetooth_small_png);
bool BTon;

bool readStringUntil(String &input, size_t char_limit)
{
  while (SerialBT.available())
  {
    char c = SerialBT.read();
    input += c;
    if (c == 254)
    {
      return true;
    }
    if (input.length() >= char_limit)
    {
      return true;
    }
  }
  return false;
}

void BThandle()
{
  /*    if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }*/

  static String input = "";
  static bool ping = 0;
  static bool waitingforping = 1;
  static int songtime;

  if (readStringUntil(input, 240))
  { // read until find newline or have read 20 chars
    if (input.lastIndexOf(254) >= 0)
    { // could also use check  if (input[input.length()-1] == terminatingChar) {
      // Serial.print(F(" got a line of input '")); Serial.print(input); Serial.println("'");
      if (input.charAt(0) == 1)
      {
        Log.verboseln("Getting Time From Phone: %ih %im %is %imonth %iday %iyear", (int)(input.charAt(1)), (int)(input.charAt(2)), (int)(input.charAt(3)), (int)(input.charAt(4)), (int)(input.charAt(5), (int)(input.charAt(6) + 2000)));
        /* Serial.print("Getting Time From Phone: ");
         Serial.print((int)(input.charAt(1)));
         Serial.print("h ");
         Serial.print((int)(input.charAt(2)));
         Serial.print("m ");
         Serial.print((int)(input.charAt(3)));
         Serial.print("s ");
         Serial.print((int)(input.charAt(4)));
         Serial.print("month ");
         Serial.print((int)(input.charAt(5)));
         Serial.print("day ");
         Serial.print((int)((input.charAt(6) + 2000)));
         Serial.println("year");*/
        // rtc.adjust(input.charAt(1), input.charAt(2), input.charAt(3), input.charAt(6) + 2000, input.charAt(4), input.charAt(5));
        twatch->rtc_set_time(input.charAt(6) + 2000, input.charAt(4), input.charAt(5), input.charAt(1), input.charAt(2), input.charAt(3));
      }
      if (input.charAt(0) == 2)
      {
        Log.verbose("Notification Title: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Title, input.c_str());
        NotificationList[10].Title = input;
      }
      if (input.charAt(0) == 3)
      {
        Log.verbose("Notification Text: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Text, input.c_str());
        NotificationList[10].Text = input;
      }
      if (input.charAt(0) == 4)
      {
        Log.verbose("Notification Source: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Source, input.c_str());
        NotificationList[10].Source = input;
        shownotification(0);
        pushnotification(1);
      }
      if (input.charAt(0) == 5)
      {
        Log.verbose("Now Playing: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        char song[64];
        // sprintf(song, "♪ %s ♪", input.c_str());
        sprintf(song, "%s   •", input.c_str());
#ifdef UPDATE_ELEMENTS
        lv_label_set_text(ui_Now_Playing_Label, song);
#endif
        songtime = millis();
      }

      if (input.charAt(0) == 7)
      {
        // static char componentname[25];
        // sprintf(componentname, "ui_Generic_Notification%d", notificationid);
        // lv_obj_t * ui_Notification_Widgets[1];
        ui_Notification_Widgets[0] = ui_Notification_Widget_create(ui_Notification_Panel);
        lv_obj_set_x(ui_Notification_Widgets[0], 0);
        lv_obj_set_y(ui_Notification_Widgets[0], (((notificationid - 1) * 60) - 50));
        notificationid += 1;
      }
    }
    else
    {
      Serial.print(F(" reached limit without newline '"));
      Serial.print(input);
      Serial.println("'");
    }
    input = ""; // clear after processing for next line
  }

  if (songtime < (millis() - 70000))
  {
    songtime = millis();
#ifdef UPDATE_ELEMENTS
    lv_label_set_text(ui_Now_Playing_Label, "");
#endif
  }

#ifdef UPDATE_ELEMENTS
  if (SerialBT.connected() && lv_img_get_src(ui_Bluetooth_Indicator) != &ui_img_bluetooth_small_png)
    lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_bluetooth_small_png);
  else if (lv_img_get_src(ui_Bluetooth_Indicator) != &ui_img_no_bluetooth_small_png )
    lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_no_bluetooth_small_png);
#endif
}

void ToggleBT(lv_event_t *e)
{
  if (BTon)
    BT_off();
  else
    BT_on();
}

void BT_on()
{
  BTon = 1;
  if (Storage.isKey("BTname"))
  {
    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    SerialBT.begin((String)BTnamechar);
    Log.verboseln("BT Name: ");
    // Serial.print("BT Name: ");
    // Serial.println(BTnamechar);
  }
  else
    SerialBT.begin("Unnamed Watch"); /*
     char BTnamechar[17];
     Storage.getBytes("BTname", BTnamechar, 17);
     Serial.println(BTnamechar);
     Serial.println(Storage.isKey("BTname"));*/
}

void BT_off()
{
  BTon = 0;
  SerialBT.disconnect();
}