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
  static int songtime;
  // GB({t:"notify",id:1689704373,src:"Gadgetbridge",title:"",subject:"Testgh",body:"Testgh",sender:"Testgh",tel:"Testgh"})

  StaticJsonDocument<200> doc;

  // char Message2[] = "{t:\"notify\",id:1689704373,src:\"Gadgetbridge\",title:\"\",subject:\"Testgh\",body:\"Testgh\",sender:\"Testgh\",tel:\"Testgh\"}";
  // Message = "GB({t:\"notify\",id:1234567890,src:\"Messages\",title:\"Dad\",body:\"Test\"})";
  // GB({t:"notify",id:1234567890,src:"Messages",title:"Dad",body:"Test"})
  //  char json[] =
  //      "{sensor:\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  const char *TempMessage = Message;
  DeserializationError error = deserializeJson(doc, TempMessage + 3);

  // Test if parsing succeeds.
  if (error)
  {
    // lv_label_set_text(ui_Now_Playing_Label, Message);
    return;
  }

  const char *NotifType = doc["t"];

  if (strcmp(NotifType, "notify") == 0)
  {

    const char *NotifText = "";
    const char *NotifTitle = "";
    const char *NotifSource = "Undefined";
    int NotifID;

    if (doc.containsKey("title"))
      NotifTitle = doc["title"];
    else if (doc.containsKey("subject"))
      NotifTitle = doc["subject"];

    if (doc.containsKey("body"))
      NotifText = doc["body"];

    if (doc.containsKey("src"))
      NotifSource = doc["src"];

    if (doc.containsKey("id"))
      NotifID = doc["id"];

    Serial.println(Message);
    if (strcmp(NotifSource, "Android System Intelligence") == 0)
    {
      lv_label_set_text_fmt(ui_Now_Playing_Label, "%s   •", NotifTitle);
      songtime = millis();
    }
    else
      shownotification(NotifTitle, NotifText, NotifSource, NotifID, 1);
  }
  else if (strcmp(NotifType, "notify-") == 0)
  {
    int NotifID = doc["id"];
    for (int i = 0; i < 10; i++)
    {
      if (NotificationList[i].id == NotifID)
      {
        // popnotification(i + 1);
        lv_label_set_text_fmt(ui_Now_Playing_Label, "%i", i);
      }
    }
  }
  else if (strcmp(NotifType, "musicinfo") == 0)
  {
    const char *MusicArtist = "";
    const char *MusicSong = "";
    MusicArtist = doc["artist"];
    MusicSong = doc["track"];
  }

  if (songtime and songtime < (millis() - 70000))
  {
    songtime = 0;
    lv_label_set_text(ui_Now_Playing_Label, "");
  }
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