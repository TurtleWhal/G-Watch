#include "TWatch_hal.h"
#include <CST816S.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include "ui.h"
#include "ui_helpers.h"

#include "power_managment.h"
#include "timestuff.h"
#include "screen_management.h"
#include "calculator.h"
#include "Functions.h"
#include "alarms.h"
#include "notifications.h"
#include "timers.h"
#include "BThandle.h"
#include "themes.h"
#include "compass.h"
#include "usersettings.h"
#include "steps.h"
#include "schedule.h"

#include "Preferences.h"
#include "ArduinoLog.h"
#include "ArduinoOTA.h"
#include "hardware/ble/gadgetbridge.h"

#include "mutex"

#include "ArduinoJson.h"
#include <thread>

const char *ssid = "ThisNetworkIsOWN3D";
const char *password = "10244096";
const char *ssid2 = "IT-Test";
const char *password2 = "";

// #define USESPLASHSCREEN

TWatchClass *twatch = nullptr;

CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

int xaccel;

extern Preferences Storage;

extern ClockInfo info;

bool useOTA;
extern bool BTon;
bool Timer0Triggered;
bool BTTimerTriggered;
bool StepGraphTriggered;

void LogoSpin(lv_obj_t *TargetObject);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
  Log.verbose(buf);
  Serial.flush();
}
#endif

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch.available())
  {

    TickleSleep();

    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    xaccel = touch.data.x - data->point.x;
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;

    Log.verboseln("Touch event. Data X: %i, Data Y: %i, X accel: %i", data->point.x, data->point.y, xaccel);

    /*
    lv_obj_t *dot = lv_obj_create(lv_scr_act());
    lv_obj_set_width(dot, 40);
    lv_obj_set_height(dot, 40);
    lv_obj_set_x(dot, touch.data.x - 20);
    lv_obj_set_y(dot, touch.data.y - 20);
    lv_obj_set_style_radius(dot, 20, LV_PART_MAIN);
    */
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

void btn1_click(void *param)
{
  Log.verboseln("BTN1 Click. Power Percent: %i", twatch->power_get_percent());
  // twatch->motor_shake(1, 60);
  if (IsSleeping())
    Wakeup("Button 1 Pressed");
  else
    Sleep();
}
void btn2_click(void *param)
{
  Log.verboseln("BTN2 Click. MilliVolts: %i", (int)twatch->power_get_volt());
  // twatch->motor_shake(1, 60);
  if (IsClockScreen() or lv_scr_act() == ui_Default_Clock)
    _ui_screen_change(&ui_Schedule, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Schedule_screen_init);
  else if (lv_scr_act() == ui_Stopwatch)
    ToggleStopwatch(nullptr);
  else if (lv_scr_act() == ui_Timers)
    ToggleTimer(nullptr);

  Wakeup("Button 2 Pressed");
}
void btn3_click(void *param)
{
  Log.verboseln("BTN3 Click");
  Wakeup("Button 3 Pressed");
  if (IsClockScreen())
  {
    // if (NotificationActive())
    //   NotificationDismiss(nullptr);
  }
  else
    ScreenBack(nullptr);
}
void btn1_held(void *param)
{
  Log.verboseln("BTN1 Held");
  if (lv_scr_act() != ui_Settings)
    // tosettingsscreen(nullptr);
    Wakeup("Button 1 Held");
  twatch->motor_shake(1, 30);
}

void btn2_held(void *param)
{
  Log.verboseln("BTN2 Held");
  Wakeup("Button 2 Held");
  if (lv_scr_act() == ui_Stopwatch)
    lv_event_send(ui_Stopwatch_Reset_Button, LV_EVENT_CLICKED, NULL); // Tell LVGL that we clicked the stopwatch reset button
  twatch->motor_shake(1, 30);
}

void btn3_held(void *param)
{
  Log.verboseln("BTN3 Held");
  Wakeup("Button 3 Held");
  ClockRight(nullptr);
  twatch->motor_shake(1, 30);
}

void InitOTA()
{
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP-Watch");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Log.verboseln("Connection Failed! Rebooting...");
    delay(2000);
    WiFi.disconnect();
    WiFi.begin(ssid2, password2);
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]()
                     {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Log.verboseln("Start updating %s", type); 
      
      _ui_screen_change(&ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Alarm_Going_Off_screen_init);
      lv_label_set_text(ui_Alarm_Going_Off_Name, "OTA Uploading");
      lv_obj_set_style_text_font(ui_Alarm_Going_Off_Name, &ui_font_Comfortaa_26, LV_PART_MAIN);
      lv_label_set_text(ui_Alarm_Going_Off_Time, "Starting");
      lv_obj_del(ui_Alarm_Going_Off_Stop_Button);
      Wakeup("Ota Begin Upload");
      twatch->motor_shake(1, 50); })
      .onEnd([]()
             { Log.verboseln("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Log.verbose("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Log.verboseln("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Log.verboseln("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Log.verboseln("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Log.verboseln("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Log.verboseln("Receive Failed");
      else if (error == OTA_END_ERROR) Log.verboseln("End Failed"); });

  ArduinoOTA.begin();

  Log.verboseln("Ready");
  Log.verbose("IP address: ");
  Log.verboseln(WiFi.localIP());

  info.OTA.useOTA = true;
  info.OTA.ip = WiFi.localIP().toString();

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { uint8_t percent = (progress / (total / 100));
                          Log.verboseln("Progress: %u%%", percent); 
                          lv_label_set_text_fmt(ui_Alarm_Going_Off_Time, "%i%%", percent);
                          TickleSleep(); });
}

void setup()
{
  // setCpuFrequencyMhz(240);

  // need to change uart_config.source_clk to UART_SCLK_REF_TICK in esp32-hal-uart.c in arduino framework https://github.com/espressif/arduino-esp32/issues/8122
  Serial.begin(115200); /* prepare for possible serial debug */
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  /*String str1 = "//////////////////////////////////////////////////////////////////////////////////";
  String str2 = "////    //////////  //////  ////    ////      ////    ////  ////  //////      ////";
  String str3 = "//  //////////////  //////  //  ////  ////  ////  ////  //  ////  ////  ////    //";
  String str4 = "//  //    //    //  //////  //  ////  ////  ////  ////////        ////  //  //  //";
  String str5 = "//  ////  ////////  //  //  //        ////  ////  ////  //  ////  ////  //////  //";
  String str6 = "////    ////////////  //  ////  ////  ////  //////    ////  ////  //////      ////";
  String str7 = "//////////////////////////////////////////////////////////////////////////////////";
  String str8 = "------------------ Made By Garrett Jordan Using The T-Watch 2021 -----------------";

  String output = str1 + "\n" + str2 + "\n" + str3 + "\n" + str4 + "\n" + str5 + "\n" + str6 + "\n" + str7;

  for (size_t i = 0; i < output.length(); i++)
  {
    if (output.charAt(i) == ' ')
    {
      output.setCharAt(i, random(32, 127));
    }
  }

  Log.verboseln(output);
  Log.verboseln(str8);
  Log.verboseln("");*/

  //  _____/\\\\\\\\\\\\________________/\\\______________/\\\_____/\\\\\\\\\_____/\\\\\\\\\\\\\\\________/\\\\\\\\\__/\\\________/\\\_
  //   ___/\\\//////////________________\/\\\_____________\/\\\___/\\\\\\\\\\\\\__\///////\\\/////______/\\\////////__\/\\\_______\/\\\_
  //    __/\\\___________________________\/\\\_____________\/\\\__/\\\/////////\\\_______\/\\\_________/\\\/___________\/\\\_______\/\\\_
  //     _\/\\\____/\\\\\\\__/\\\\\\\\\\\_\//\\\____/\\\____/\\\__\/\\\_______\/\\\_______\/\\\________/\\\_____________\/\\\\\\\\\\\\\\\_
  //      _\/\\\___\/////\\\_\///////////___\//\\\__/\\\\\__/\\\___\/\\\\\\\\\\\\\\\_______\/\\\_______\/\\\_____________\/\\\/////////\\\_
  //       _\/\\\_______\/\\\_________________\//\\\/\\\/\\\/\\\____\/\\\/////////\\\_______\/\\\_______\//\\\____________\/\\\_______\/\\\_
  //        _\/\\\_______\/\\\__________________\//\\\\\\//\\\\\_____\/\\\_______\/\\\_______\/\\\________\///\\\__________\/\\\_______\/\\\_
  //         _\//\\\\\\\\\\\\/____________________\//\\\__\//\\\______\/\\\_______\/\\\_______\/\\\__________\////\\\\\\\\\_\/\\\_______\/\\\_
  //          __\////////////_______________________\///____\///_______\///________\///________\///______________\/////////__\///________\///__

  Log.verboseln("");
  Log.verboseln("_____/\\\\\\\\\\\\\\\\\\\\\\\\________________/\\\\\\______________/\\\\\\_____/\\\\\\\\\\\\\\\\\\_____/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\________/\\\\\\\\\\\\\\\\\\__/\\\\\\________/\\\\\\_        ");
  Log.verboseln(" ___/\\\\\\//////////________________\\/\\\\\\_____________\\/\\\\\\___/\\\\\\\\\\\\\\\\\\\\\\\\\\__\\///////\\\\\\/////______/\\\\\\////////__\\/\\\\\\_______\\/\\\\\\_       ");
  Log.verboseln("  __/\\\\\\___________________________\\/\\\\\\_____________\\/\\\\\\__/\\\\\\/////////\\\\\\_______\\/\\\\\\_________/\\\\\\/___________\\/\\\\\\_______\\/\\\\\\_      ");
  Log.verboseln("   _\\/\\\\\\____/\\\\\\\\\\\\\\__/\\\\\\\\\\\\\\\\\\\\\\_\\//\\\\\\____/\\\\\\____/\\\\\\__\\/\\\\\\_______\\/\\\\\\_______\\/\\\\\\________/\\\\\\_____________\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_     ");
  Log.verboseln("    _\\/\\\\\\___\\/////\\\\\\_\\///////////___\\//\\\\\\__/\\\\\\\\\\__/\\\\\\___\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_______\\/\\\\\\_______\\/\\\\\\_____________\\/\\\\\\/////////\\\\\\_    ");
  Log.verboseln("     _\\/\\\\\\_______\\/\\\\\\_________________\\//\\\\\\/\\\\\\/\\\\\\/\\\\\\____\\/\\\\\\/////////\\\\\\_______\\/\\\\\\_______\\//\\\\\\____________\\/\\\\\\_______\\/\\\\\\_   ");
  Log.verboseln("      _\\/\\\\\\_______\\/\\\\\\__________________\\//\\\\\\\\\\\\//\\\\\\\\\\_____\\/\\\\\\_______\\/\\\\\\_______\\/\\\\\\________\\///\\\\\\__________\\/\\\\\\_______\\/\\\\\\_  ");
  Log.verboseln("       _\\//\\\\\\\\\\\\\\\\\\\\\\\\/____________________\\//\\\\\\__\\//\\\\\\______\\/\\\\\\_______\\/\\\\\\_______\\/\\\\\\__________\\////\\\\\\\\\\\\\\\\\\_\\/\\\\\\_______\\/\\\\\\_ ");
  Log.verboseln("        __\\////////////_______________________\\///____\\///_______\\///________\\///________\\///______________\\/////////__\\///________\\///__");
  Log.verboseln("");
  Log.verboseln("Ascii art made using: https://patorjk.com/software/taag/#p=display&f=Slant%20Relief&t=G-WATCH");
  Log.verboseln("Project made by: Garrett Jordan (https://github.com/TurtleWhal/G-Watch, https://garrettjordan.xyz/projects/#watch)");
  Log.verboseln("");

  twatch = TWatchClass::getWatch(); // Does HALinit for us
  pinMode(TWATCH_CHARGING, INPUT_PULLUP);

  if (!digitalRead(TWATCH_BTN_2)) // Enable OTA if B2 is held at boot
    useOTA = 1;

  // twatch->hal_auto_update(true, 1);

  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_CLICK, btn1_click);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_CLICK, btn2_click);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_CLICK, btn3_click);
  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_LONG_PRESS_START, btn1_held);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_LONG_PRESS_START, btn2_held);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_LONG_PRESS_START, btn3_held);

  touch.begin();

  Log.verboseln("Running LVGL V%i.%i.%i", lv_version_major(), lv_version_minor(), lv_version_patch());

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // initialize ArduinoOTA
  if (useOTA)
    InitOTA();

  //////////Initalize UI//////////
  LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

  SetClockScreen(SCREEN_CLOCK_DEFAULT);
  InitClockScreen();

  Log.verboseln("Init clock Screen");

  ui____initial_actions0 = lv_obj_create(NULL);
  Log.verboseln("ui____initial_actions0");

  DispLoadClockScreen();
  Log.verboseln("lv_disp_load_scr");

  InitUserSettings();

  InitNotifications();

  ApplyTheme(nullptr);

  hw_timer_t *timer = NULL;
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, Timer0Handle, true);
  timerAlarmWrite(timer, 10007 * 1000, true); // 10 seconds, plus prime mS to not
  timerAlarmEnable(timer);

  // BTmsgloop 50ms timer
  hw_timer_t *timer2 = NULL;
  timer2 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer2, BTTimerHandle, true);
  timerAlarmWrite(timer2, BLECTL_CHUNKDELAY * 1000, true);
  timerAlarmEnable(timer2);

  // Steps Graph 10m timer
  hw_timer_t *timer3 = NULL;
  timer3 = timerBegin(2, 80, true);
  timerAttachInterrupt(timer3, StepGraphHandle, true);
  timerAlarmWrite(timer3, 600000 * 1000, true);
  timerAlarmEnable(timer3);

  //////////////////////////Fake Notifications///////////
  // AddFakeNotifications();

  Log.verboseln("Lv Timer");
  lv_timer_handler();

  Log.verboseln("BT on");
  BT_on();

#if defined(CONFIG_BMA423_LATER)
  Log.verboseln("Bma423 begin");
  twatch->bma423_begin(); // This takes 2 seconds

  // enable the wrist tilt interrupt
  twatch->bma423_feature(BMA423_WRIST_WEAR, true);
  twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, false);
#endif

  Log.verboseln("HAL Update");
  twatch->hal_auto_update(true, 1);

  twatch->backlight_gradual_light(100, 2000);

  Timer0Triggered = 1;

  twatch->motor_shake(1, 100);
  Log.verboseln("Setup done");
  // Log.verboseln("Total PSRAM: %d", ESP.getPsramSize());
  // Log.verboseln("Free PSRAM: %d", ESP.getFreePsram());

  info.flag.refresh = 1;
}

void loop()
{
  if (useOTA)
    ArduinoOTA.handle();

  if (!IsSleeping()) // If Awake
  {
    lv_timer_handler(); /* let the GUI do its work */

    if (IsClockScreen()) // Only run this if on the main screen
      ScreenHandleHandle();
  }
  else // If Asleep
  {
    if (touch.available())
    { // Normally handled by lv_timer_handler
      if (touch.data.x == 0 && touch.data.y == 0)
        Wakeup("Tilted Wrist");
      else
        Wakeup("Screen Touched");

      //Log.verboseln("Touched at %i,%i", touch.data.x, touch.data.y);
    }

    delay(100);
  }

  UpdateTime();

  Powerhandle();
  BTHandle();
  Sleephandle();
  NotificationHandle();
  TimersHandle();
  VibrateHandle();

  // this runs every BLECTL_CHUNKDELAY ms
  if (BTTimerTriggered)
  {
    BTmsgloop();
    BTTimerTriggered = 0;
  }

  // This stuff runs every 10 seconds
  if (Timer0Triggered)
  {
    Log.verboseln("Timer 0 Fired");
    Timer0Triggered = 0;
    StepHandle();
    UpdatePower();
    ScheduleHandle();

    if (millis() < 20000)
    {
      info.flag.refresh = 1;
    }
  }

  // This stuff runs every 10 minutes
  if (StepGraphTriggered)
  {
    StepGraphTriggered = 0;

    AdvanceStepArray();
    if (lv_scr_act() == ui_Health)
    {
      WriteStepGraph();
    }
  }
}

void Timer0Handle()
{
  Timer0Triggered = 1;
}

void BTTimerHandle()
{
  BTTimerTriggered = 1;
}

void StepGraphHandle()
{
  StepGraphTriggered = 1;
}

void testbutton1(lv_event_t *e)
{
  Log.verboseln("test1");
  // BTsendf("{t:\"notify\", tel:\"%s\", n:\"REPLY\", msg:\"Testmsg\"}", "+12069790103");
}

void testbutton2(lv_event_t *e)
{
  Log.verboseln("test2");
}

void testbutton3(lv_event_t *e)
{
  Log.verboseln("test3");
}

void StartOTA(lv_event_t *e)
{
  useOTA = true;
  InitOTA();
}