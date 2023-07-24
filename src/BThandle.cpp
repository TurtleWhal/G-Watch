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
#include "hardware/ble/gadgetbridge.h"
#include "ArduinoJson.h"

extern TWatchClass *twatch;
extern Notification NotificationList[11];
extern Preferences Storage;
extern int notificationid;
lv_obj_t *ui_Notification_Widgets[1];
LV_IMG_DECLARE(ui_img_bluetooth_small_png);
LV_IMG_DECLARE(ui_img_pause_button_png);
bool BTon;
int songtime;
int MusicPos;
bool msgAvailible;
static String msg;

void ParseGB(char *Message)
{
  static int MusicLength;
  // GB({t:"notify",id:1689704373,src:"Gadgetbridge",title:"",subject:"Testgh",body:"Testgh",sender:"Testgh",tel:"Testgh"})

  StaticJsonDocument<200> received;

  // char Message2[] = "{t:\"notify\",id:1689704373,src:\"Gadgetbridge\",title:\"\",subject:\"Testgh\",body:\"Testgh\",sender:\"Testgh\",tel:\"Testgh\"}";
  // Message = "GB({t:\"notify\",id:1234567890,src:\"Messages\",title:\"Dad\",body:\"Test\"})";
  // GB({t:"notify",id:1234567890,src:"Messages",title:"Dad",body:"Test"})
  //  char json[] =
  //      "{sensor:\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  const char *TempMessage = Message;
  DeserializationError error = deserializeJson(received, TempMessage + 3);

  // Test if parsing succeeds.
  if (error)
  {
    // lv_label_set_text(ui_Now_Playing_Label, Message);
    return;
  }

  const char *NotifType = received["t"];

  //  if (strcmp(NotifType, "musicstate") != 0)
  lv_label_set_text(ui_Now_Playing_Label, Message);

  if (strcmp(NotifType, "notify") == 0)
  {

    const char *NotifText = "";
    const char *NotifTitle = "";
    const char *NotifSource = "Undefined";
    int NotifID;

    if (received.containsKey("title"))
      NotifTitle = received["title"];
    else if (received.containsKey("subject"))
      NotifTitle = received["subject"];

    if (received.containsKey("body"))
      NotifText = received["body"];

    if (received.containsKey("src"))
      NotifSource = received["src"];

    if (received.containsKey("id"))
      NotifID = received["id"];

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
    int NotifID = received["id"];
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
    const char *MusicAlbum = "";

    MusicArtist = received["artist"];
    MusicSong = received["track"];
    MusicLength = received["dur"];
    MusicAlbum = received["album"];

    // ui_Music_screen_init();

    lv_label_set_text(ui_Music_Artist, MusicArtist);
    lv_label_set_text(ui_Music_Title, MusicSong);
    lv_label_set_text(ui_Music_Album, MusicAlbum);
    // twatch->motor_shake(1, 50);
  }
  else if (strcmp(NotifType, "musicstate") == 0)
  {
    const char *MusicPlaying;

    MusicPlaying = received["musicstate"];
    MusicPos = received["position"];

    /*if (strcmp(MusicPlaying, "play") == 0)
    {
      // lv_img_set_src(ui_Music_Play_Button_Image, &ui_img_play_button_png);
      lv_label_set_text(ui_Now_Playing_Label, MusicPlaying);
    }
    else
    {
      // lv_img_set_src(ui_Music_Play_Button_Image, &ui_img_pause_button_png);
      lv_label_set_text(ui_Now_Playing_Label, MusicPlaying);
    }*/
    lv_label_set_text_fmt(ui_Music_Time, "%i:%02i / %i:%02i", (int)(MusicPos / 60), MusicPos % 60, (int)(MusicLength / 60), MusicLength % 60);
    lv_slider_set_range(ui_Music_Play_Bar, 0, MusicLength);
    lv_slider_set_value(ui_Music_Play_Bar, MusicPos, LV_ANIM_ON);
  }
}

void PauseMusic(lv_event_t *e)
{
  /*StaticJsonDocument<200> Send;
  Send["t"] = "music";
  Send["n"] = "pause";
  String TempJson;
  serializeJson(Send, TempJson);
  char *TempChar;
  sprintf(TempChar, "GB(%s", TempJson);
  lv_label_set_text(ui_Now_Playing_Label, TempChar);
  gadgetbridge_send_msg("json", TempChar);*/
  //gadgetbridge_send_msg("\r\n{t:\"music\", n:\"pause\"}\r\n");
  //BTsend("{\"t\":\"music\", \"n\":\"pause\"}");
}

void BTHandle()
{
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

void BTsend(String message)
{
  Serial.println("BTsend");
  msg = "\r\n" + message + "\r\n";
  msgAvailible = 1;
  Log.verboseln("BTsend: %s", message.c_str());
  //msg = "\r\n{\"t\":\"status\", \"bat\":42}\r\n";
}

void BTmsgloop()
{
  // Serial.println("BTmsgloop");
  // add \r\n to beginning and end when sent
  // Serial.println(msgAvailible);
  unsigned char tempmsg[BLECTL_CHUNKSIZE + 1];
  if (msgAvailible)
  {
    if (msg.length() > BLECTL_CHUNKSIZE)
    {
      msg.getBytes(tempmsg + 1, BLECTL_CHUNKSIZE);
      // tempmsg[0] = 0;
      msg.remove(0, BLECTL_CHUNKSIZE - 1);
      gadgetbridge_send_chunk(tempmsg, BLECTL_CHUNKSIZE);
    }
    else
    {
      msg.getBytes(tempmsg, BLECTL_CHUNKSIZE);
      msgAvailible = false;
      gadgetbridge_send_chunk(tempmsg, msg.length());
    }
    // Serial.println(msg);
  }
}