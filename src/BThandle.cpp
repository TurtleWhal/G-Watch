#include "Arduino.h"
#include "NimBLEDevice.h"
#include "lvgl.h"
#include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "BThandle.h"
#include "power_managment.h"
#include "notifications.h"
#include "Preferences.h"
#include "hardware/blectl.h"
#include "hardware/ble/gadgetbridge.h"
#include "ArduinoJson.h"
#include "timers.h"
#include "timestuff.h"
#include "screen_management.h"

#define MUSICPOSOFFSET 2

extern TWatchClass *twatch;
extern Notification NotificationList[11];
extern Preferences Storage;
extern int notificationid;
lv_obj_t *ui_Notification_Widgets[1];
LV_IMG_DECLARE(ui_img_bluetooth_small_png);
// LV_IMG_DECLARE(ui_img_pause_button_png);
bool BTon;
bool BTconnected;
int songtime;
static String msg;

int MusicPos;
int MusicLength;
String MusicSong;
String MusicArtist;
String MusicAlbum;
bool MusicPlaying;

LV_IMG_DECLARE(ui_img_mostly_cloudy_png);          // assets\Mostly Cloudy.png
LV_IMG_DECLARE(ui_img_snow_rain_png);              // assets\Snow Rain.png
LV_IMG_DECLARE(ui_img_mostly_sunny_png);           // assets\Mostly Sunny.png
LV_IMG_DECLARE(ui_img_sunny_png);                  // assets\Sunny.png
LV_IMG_DECLARE(ui_img_partly_cloudy_png);          // assets\Partly Cloudy.png
LV_IMG_DECLARE(ui_img_cloudy_png);                 // assets\Cloudy.png
LV_IMG_DECLARE(ui_img_rainy_png);                  // assets\Rainy.png
LV_IMG_DECLARE(ui_img_scattered_showers_png);      // assets\Scattered Showers.png
LV_IMG_DECLARE(ui_img_snowing_png);                // assets\Snowing.png
LV_IMG_DECLARE(ui_img_isolated_thunderstorms_png); // assets\Isolated Thunderstorms.png
LV_IMG_DECLARE(ui_img_blizzard_png);               // assets\Blizzard.png
LV_IMG_DECLARE(ui_img_fog_png);                    // assets\Fog.png

typedef struct
{
  int Temp = 0;
  int High = 0;
  int Low = 0;
  int Humidity = 0;
  int Precip = 0;
  int UV = 0;
  int Code = 0;
  lv_img_dsc_t Img = ui_img_mostly_cloudy_png;
  String Type = "Not Updated";
  int Wind = 0;
  int Winddir = 0;
  String Loc = "";
} weather;

weather Weather;

void ParseGB(char *Message);
int KelvintoF(int Kelvin);
String DegToCompassHeading(int);

void ParseBLE(char *Message)
{
  Serial.println(Message);
  struct timeval val;
  char *settime_str = NULL;
  ulong timevalue;
  short timezone;
  settime_str = strstr(Message, "setTime(");

  if (settime_str)
  {
    /* code */
    // Serial.println("GOT TIME!");
    settime_str = settime_str + 8;
    timevalue = atol(settime_str);
    // Serial.println(settime_str);
    Serial.println(settime_str);

    settime_str = strstr(Message, "setTimeZone(");
    settime_str = settime_str + 12;
    Serial.println(settime_str);
    timezone = atol(settime_str);

    Serial.println(timezone);

    SetTime(timevalue, timezone);
  }
  else
    ParseGB(Message);
}

void ParseGB(char *Message)
{
  // int MusicLength = 600;
  //  GB({t:"notify",id:1689704373,src:"Gadgetbridge",title:"",subject:"Testgh",body:"Testgh",sender:"Testgh",tel:"Testgh"})

  StaticJsonDocument<2048> received;

  // char Message2[] = "{t:\"notify\",id:1689704373,src:\"Gadgetbridge\",title:\"\",subject:\"Testgh\",body:\"Testgh\",sender:\"Testgh\",tel:\"Testgh\"}";
  // Message = "GB({t:\"notify\",id:1234567890,src:\"Messages\",title:\"Dad\",body:\"Test\"})";
  // GB({t:"notify",id:1234567890,src:"Messages",title:"Dad",body:"Test"})
  //  char json[] =
  //      "{sensor:\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  const char *TempMessage = Message;
  // const char TempMessage[] = "GB({t:\"notify\",id:1689704373,src:\"Gadgetbridge\",title:\"Testgh\",subject:\"Testgh\",body:\"Testgh\",sender:\"Testgh\",tel:\"Testgh\"})";
  DeserializationError error = deserializeJson(received, TempMessage + 3); // message +3 to get rid of GB( at beginning of gadgetbridge message

  // Test if parsing succeeds.
  if (error)
  {
    // lv_label_set_text(ui_Now_Playing_Label, Message);
    return;
  }

  const char *NotifType = received["t"];

  //  if (strcmp(NotifType, "musicstate") != 0)
  // lv_label_set_text(ui_Now_Playing_Label, Message);

  // Serial.println(Message);

  if (strcmp(NotifType, "notify") == 0)
  {

    const char *NotifText = "";            // This doesn't initialize enought space
    const char *NotifTitle = "";           // This doesn't initialize enought space
    const char *NotifSource = "Undefined"; // This may not initialize enough space
    static int NotifID;

    if (NotifID != received["id"])
    {
      NotifID = received["id"];

      if (received.containsKey("title"))
        NotifTitle = received["title"];
      else if (received.containsKey("subject"))
        NotifTitle = received["subject"];

      if (received.containsKey("body"))
        NotifText = received["body"];

      if (received.containsKey("src"))
        NotifSource = received["src"];

      Log.verboseln("Received Notification With Title: %s, Text: %s, Source: %s, ID: %i", NotifTitle, NotifText, NotifSource, NotifID);

      // Serial.println(Message);
      if (strcmp(NotifSource, "Android System Intelligence") == 0)
      {
        /*NotifTitle = "Number 179 °@#$* by dot.* and .;:' feetering !@3%6* by ksajfd";
        char NowPlayingTitle[64];
        char NowPlayingArtist[64];
        sscanf(NotifTitle, "%s by %s", NowPlayingTitle, NowPlayingArtist);
        lv_label_set_text_fmt(ui_Now_Playing_Label, "%s\n%s", NowPlayingTitle, NowPlayingArtist);*/
        lv_label_set_text_fmt(ui_Now_Playing_Label, "%s   •", NotifTitle);
        songtime = millis();
        Log.verboseln("Detected Now Playing: %s", NotifTitle);
      }
      else
      {
        shownotification(NotifTitle, NotifText, NotifSource, NotifID);
      }
    }
  }
  else if (strcmp(NotifType, "notify-") == 0)
  {
    int NotifID = received["id"];
    for (int i = 0; i < 10; i++)
    {
      if (NotificationList[i].id == NotifID)
      {
        popnotification(i + 1);
        // lv_label_set_text_fmt(ui_Now_Playing_Label, "%i", i);
        Log.verboseln("Notify: %i", i);
      }
    }
  }
  else if (strcmp(NotifType, "musicinfo") == 0)
  {
    // const char *MusicArtist = "artist";
    // const char *MusicSong = "song";
    // const char *MusicAlbum = "album";

    // if (received.containsKey("artist"))
    const char *localMusicArtist = received["artist"];
    // if (received.containsKey("track"))
    const char *localMusicSong = received["track"];
    // if (received.containsKey("dur"))
    MusicLength = received["dur"];
    // if (received.containsKey("album"))
    const char *localMusicAlbum = received["album"];

    Serial.println(localMusicArtist);
    Serial.println(localMusicSong);
    Serial.println(MusicLength);
    Serial.println(localMusicAlbum);

    MusicSong = localMusicSong;
    MusicArtist = localMusicArtist;
    MusicAlbum = localMusicAlbum;

    // ui_Music_screen_init();

    // lv_label_set_text(ui_Now_Playing_Label, MusicSong);
    DrawMusicInfo(nullptr);
    SetDownScreen(MUSIC_SCREEN);
    // twatch->motor_shake(1, 50);
  }
  else if (strcmp(NotifType, "call") == 0)
  {
    const char *CallType = received["cmd"];
    Serial.println(CallType);
    if (strcmp(CallType, "incoming") == 0)
    {
      _ui_screen_change(&ui_Call, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Call_screen_init);
      lv_label_set_text(ui_Call_Type, "Incoming Call");
      lv_label_set_text(ui_Caller_Name, received["name"]);
      lv_label_set_text(ui_Caller_Number, received["number"]);
    }
    else if (strcmp(CallType, "outgoing") == 0)
    {
      _ui_screen_change(&ui_Call, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Clock_screen_init);
      lv_label_set_text(ui_Call_Type, "Outgoing Call");
      lv_label_set_text(ui_Caller_Name, received["name"]);
      lv_label_set_text(ui_Caller_Number, received["number"]);
      lv_obj_add_flag(ui_Accept_Call_Button, LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_x(ui_Reject_Call_Button, 0);
    }
    else if (strcmp(CallType, "start") == 0)
    {
      lv_label_set_text(ui_Call_Type, "Ongoing Call");
      lv_obj_add_flag(ui_Accept_Call_Button, LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_x(ui_Reject_Call_Button, 0);
    }
    else if (strcmp(CallType, "end") == 0)
    {
      _ui_screen_change(&ui_Clock, LV_SCR_LOAD_ANIM_FADE_OUT, 150, 0, nullptr);
    }
  }
  else if (strcmp(NotifType, "musicstate") == 0)
  {
    const char *MusicState = received["state"];
    MusicPos = received["position"];
    // MusicPos += MUSICPOSOFFSET;
    Serial.println(MusicState);

    if (strcmp(MusicState, "play") == 0)
    {
      Serial.println("Change To Pause Button");
      lv_img_set_src(ui_Music_Play_Button_Image, &ui_img_pause_button_png);
      // lv_label_set_text(ui_Now_Playing_Label, MusicState);
      MusicPlaying = 1;
    }
    else
    {
      Serial.println("Change To Play Button");
      lv_img_set_src(ui_Music_Play_Button_Image, &ui_img_play_button_png);
      // lv_label_set_text(ui_Now_Playing_Label, MusicState);
      MusicPlaying = 0;
    }
    DrawMusicTime(nullptr);
  }
  else if (strcmp(NotifType, "is_gps_active") == 0)
  {
    BTsend("{\"t\":\"gps_power\",\"status\":false}");
  }
  else if (strcmp(NotifType, "weather") == 0)
  {
    // t:"weather", temp,hi,lo,hum,rain,uv,code,txt,wind,wdir,loc
    int Temp = KelvintoF(received["temp"]);
    int High = KelvintoF(received["hi"]);
    int Low = KelvintoF(received["lo"]);
    int Humidity = received["hum"];
    int Precip = received["rain"];
    int UV = received["uv"];
    int Code = received["code"];
    const char *Type = received["txt"];
    int Wind = received["wind"];
    int Winddir = received["wdir"];
    const char *Loc = received["loc"];

    Log.verboseln("%s", Message);

    Log.verboseln("(Weather) Temperature: -- %i", Temp);
    Log.verboseln("(Weather) Daily High: --- %i", High);
    Log.verboseln("(Weather) Daily Low: ---- %i", Low);
    Log.verboseln("(Weather) Humidity: ----- %i", Humidity);
    Log.verboseln("(Weather) Precipitation:  %i", Precip);
    Log.verboseln("(Weather) UV Index: ----- %i", UV);
    Log.verboseln("(Weather) Weather Code:-- %i", Code);
    Log.verboseln("(Weather) Weather:------- %s", Type);
    Log.verboseln("(Weather) Wind Speed:---- %i", Wind);
    Log.verboseln("(Weather) Wind Dir:------ %i", Winddir);
    Log.verboseln("(Weather) Weather Loc: -- %s", Loc);

    Weather.Temp = Temp;
    Weather.High = High;
    Weather.Low = Low;
    Weather.Humidity = Humidity;
    Weather.Precip = Precip;
    Weather.UV = UV;
    Weather.Code = Code;
    Weather.Type = Type;
    Weather.Wind = Wind / 1.609;
    Weather.Winddir = Winddir;
    Weather.Loc = Loc;

    switch (Weather.Code)
    {
    case 200 ... 232:
      Weather.Img = ui_img_isolated_thunderstorms_png;
      break;

    case 300 ... 321:
    case 520 ... 531:
      Weather.Img = ui_img_rainy_png;
      break;

    case 500:
      Weather.Img = ui_img_scattered_showers_png;
      break;

    case 501 ... 504:
      Weather.Img = ui_img_sunny_png;
      break;

    case 511:
    case 612 ... 621:
      Weather.Img = ui_img_snow_rain_png;
      break;

    case 600 ... 601:
    case 611:
      Weather.Img = ui_img_snowing_png;
      break;

    case 602:
    case 622:
      Weather.Img = ui_img_blizzard_png;
      break;

    case 701 ... 781:
      Weather.Img = ui_img_fog_png;
      break;

    case 800:
      Weather.Img = ui_img_sunny_png;
      break;

    case 801:
      Weather.Img = ui_img_mostly_sunny_png;
      break;

    case 802:
      Weather.Img = ui_img_partly_cloudy_png;
      break;

    case 803:
      Weather.Img = ui_img_mostly_cloudy_png;
      break;

    case 804:
      Weather.Img = ui_img_cloudy_png;
      break;

    default:
      Weather.Img = ui_img_mostly_cloudy_png;
      break;
    }

    DrawWeather(nullptr);
  }
  else if (strcmp(NotifType, "find") == 0)
  {
    bool find = received["n"];
    Serial.println(find);
    if (find)
    {
      VibrateStart(100);
      _ui_screen_change(&ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, ui_Alarm_Going_Off_screen_init);
      lv_label_set_text(ui_Alarm_Going_Off_Name, "Find Watch");
      lv_label_set_text(ui_Alarm_Going_Off_Time, "");
    }
    else
    {
      VibrateStop(nullptr);
    }
  }
}

int KelvintoF(int Kelvin)
{
  return ((Kelvin - 273) * 1.8 + 32);
}

String DegToCompassHeading(int degree)
{
  if (degree < 0 || degree > 360)
  {
    return "Invalid degree value";
  }

  const char *directions[] = {
      "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
      "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N"};

  int index = (degree + 11.25) / 22.5; // 360 degrees divided into 16 segments
  return directions[index];
}

void DrawMusicInfo(lv_event_t *e)
{
  if (lv_scr_act() == ui_Music)
  {
    lv_label_set_text(ui_Music_Artist, MusicArtist.c_str());
    lv_label_set_text(ui_Music_Title, MusicSong.c_str());
    lv_label_set_text(ui_Music_Album, MusicAlbum.c_str());
  }
}

void DrawMusicTime(lv_event_t *e)
{
  if (lv_scr_act() == ui_Music)
  {
    lv_label_set_text_fmt(ui_Music_Time, "%i:%02i / %i:%02i", (int)(MusicPos / 60), MusicPos % 60, (int)(MusicLength / 60), MusicLength % 60);
    if (MusicLength)
      lv_slider_set_range(ui_Music_Play_Bar, 0, MusicLength);
    lv_slider_set_value(ui_Music_Play_Bar, MusicPos, LV_ANIM_ON);
  }
}

void PauseMusic(lv_event_t *e)
{
  if (MusicPlaying)
  {
    Serial.println("Pausing");
    String pausestring = "{t:\"music\", n:\"pause\"}";
    BTsend(pausestring, 2);
    // lv_img_set_src(ui_Music_Play_Button, &ui_img_play_button_png);
    // MusicPlaying = !MusicPlaying;
  }
  else
  {
    Serial.println("Resuming");
    String playstring = "{t:\"music\", n:\"play\"}";
    BTsend(playstring, 2);
    // lv_img_set_src(ui_Music_Play_Button, &ui_img_pause_button_png);
    // MusicPlaying = !MusicPlaying;
  }
}

void MusicSkipForward(lv_event_t *e)
{
  Serial.println("Skipping Forward");
  String skipforwardstring = "{t:\"music\", n:\"next\"}";
  BTsend(skipforwardstring, 2);
}

void MusicSkipBackward(lv_event_t *e)
{
  Serial.println("Skipping Backward");
  String skipbackwardstring = "{t:\"music\", n:\"previous\"}";
  BTsend(skipbackwardstring, 2);
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
}

void BT_off()
{
  BTon = 0;
  blectl_off();
}

void onBTConnect()
{
  BTsendpower();
}

void BTsend(String message, int repeat)
{
  for (int i = repeat; i > 0; i--)
  {
    msg = msg + "\r\n" + message + "\r\n" + BTtermchar;
    Log.verboseln("BTsend: %s", message.c_str());
    Log.verboseln("BTsendmsg: %s", msg.c_str());
  }
}

void BTmsgloop()
{
  static int lasttime;
  unsigned char tempmsg[BLECTL_CHUNKSIZE + 1];
  if (msg.length() and BTon)
  {
    if (msg.indexOf(BTtermchar) - 1 > BLECTL_CHUNKSIZE)
    {
      msg.getBytes(tempmsg + 1, BLECTL_CHUNKSIZE);
      msg.remove(0, BLECTL_CHUNKSIZE - 1);
      gadgetbridge_send_chunk(tempmsg, BLECTL_CHUNKSIZE);
    }
    else
    {
      msg.getBytes(tempmsg, BLECTL_CHUNKSIZE);
      gadgetbridge_send_chunk(tempmsg, msg.indexOf(BTtermchar) - 1);
      msg.remove(0, msg.indexOf(BTtermchar) + 1);
    }
    // Serial.println(msg);
  }

  if (BTon and blectl_get_event(BLECTL_CONNECT | BLECTL_AUTHWAIT)) // can't call get event unless BTon = true
  {
    BTconnected = true;
    if (lv_img_get_src(ui_Bluetooth_Indicator) != &ui_img_bluetooth_small_png)
      lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_bluetooth_small_png);
  }
  else
  {
    BTconnected = false;
    if (lv_img_get_src(ui_Bluetooth_Indicator) != &ui_img_no_bluetooth_small_png)
      lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_no_bluetooth_small_png);
  }

  if (lasttime < millis() - 1000)
  {
    if (MusicPlaying)
    {
      MusicPos++;
      DrawMusicTime(nullptr);
    }
    lasttime = millis();
  }
}

bool isBTconnected()
{
  return BTconnected;
}

void pairBT(int pin)
{
  _ui_screen_change(&ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Alarm_Going_Off_screen_init);
  lv_label_set_text(ui_Alarm_Going_Off_Stop_Button_Text, "Ok");
  lv_label_set_text(ui_Alarm_Going_Off_Name, "Pairing Pin");
  lv_label_set_text_fmt(ui_Alarm_Going_Off_Time, "%03i %03i", pin / 1000, pin % 1000);
  // lv_scr_load_anim(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
}

void DrawWeather(lv_event_t *e)
{
  if (ui_Weather != NULL)
  {
    lv_label_set_text_fmt(ui_Temperature, "%i°", Weather.Temp);
    lv_label_set_text_fmt(ui_High_Temp, "%i°", Weather.High);
    lv_label_set_text_fmt(ui_Low_Temp, "%i°", Weather.Low);
    lv_label_set_text_fmt(ui_Humidity_Label, "%i%%", Weather.Humidity);
    lv_label_set_text_fmt(ui_Precepitation_Label, "%i%%", Weather.Precip);
    lv_label_set_text_fmt(ui_UV_Index_Label, "UV: %i", Weather.UV);
    lv_label_set_text_fmt(ui_Wind_Info, "%i mph\n%s", Weather.Wind, DegToCompassHeading(Weather.Winddir).c_str());
    lv_img_set_angle(ui_Wind_Image, Weather.Winddir * 10 - 900);
    lv_label_set_text(ui_Weather_State, Weather.Type.c_str());
    lv_label_set_text(ui_Weather_Location, Weather.Loc.c_str());
    // lv_label_set_text_fmt(ui_Weather_Id, "%i", Weather.Code);
    lv_img_set_src(ui_Weather_Image, &Weather.Img);
  }
}