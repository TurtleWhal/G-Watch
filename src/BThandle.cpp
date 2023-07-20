#include "Arduino.h"
#include "NimBLEDevice.h"
#include "lvgl.h"
#include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "BThandle.h"
#include "notifications.h"
#include "Preferences.h"
#include "hardware/blectl.h"
#include "ArduinoJson.h"

extern TWatchClass *twatch;
extern Notification NotificationList[11];
extern Preferences Storage;
extern int notificationid;
lv_obj_t *ui_Notification_Widgets[1];
LV_IMG_DECLARE(ui_img_bluetooth_small_png);
bool BTon;

void ParseGB(char *Message)
{
  lv_label_set_text(ui_Now_Playing_Label, Message);

  // GB({t:"notify",id:1689704373,src:"Gadgetbridge",title:"",subject:"Testgh",body:"Testgh",sender:"Testgh",tel:"Testgh"})
  //String Notif = Message;

  //Notif.remove(0, 7);
  //Notif.remove(Notif.indexOf("\""), Notif.length());

  //sscanf(Message, "GB({t:%*s,", Notif);
StaticJsonDocument<200> doc;
  //deserializeJson(doc, Message);
  char Message2[] = "{t:\"notify\",id:1689704373,src:\"Gadgetbridge\",title:\"\",subject:\"Testgh\",body:\"Testgh\",sender:\"Testgh\",tel:\"Testgh\"}";
  //char json[] =
  //    "{sensor:\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    DeserializationError error = deserializeJson(doc, Message + 3);

  // Test if parsing succeeds.
  if (error) {
    //Serial.print(F("deserializeJson() failed: "));
    lv_label_set_text(ui_Now_Playing_Label, "deserializeJson() failed");
    //Serial.println(error.f_str());
    return;
  }

  //Notif = doc["t"];
  const char* test = doc["t"];

  lv_label_set_text(ui_Now_Playing_Label, test);
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

  blectl_setup();
  /*
  if (Storage.isKey("BTname"))
  {
    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    SerialBT.begin((String)BTnamechar);
    Log.verboseln("BT Name: %s", BTnamechar);
  }
  else
    SerialBT.begin("Unnamed Watch");
    */
}

void BT_off()
{
  BTon = 0;
  // SerialBT.disconnect();
}