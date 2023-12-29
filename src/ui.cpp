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

#include "ArduinoJson.h"

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

bool touchingnotif;
lv_obj_t *notiftouched;

void notifslide(lv_event_t *e);
void LogoSpin(lv_obj_t *TargetObject);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch.available())
  {
    data->state = LV_INDEV_STATE_PR;
    /*Set the coordinates*/
    // if (GetSleepTimer() < millis() - 200)
    //{

    xaccel = touch.data.x - data->point.x;
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;

    if (!IsSleeping()) // If Awake
    {
      if (IsClockScreen) // Only run this if on the main screen
      {
        notifslide(nullptr);
      }
    }

    //}
    Wakeup("Screen Touched");

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

void notifslide(lv_event_t *e)
{
  if (touchingnotif)
  {
    // lv_obj_set_x(lv_event_get_target(e), touch.data.x - (lv_obj_get_width(lv_event_get_target(e)) * 0.65));
    lv_obj_set_x(notiftouched, touch.data.x - (lv_obj_get_width(notiftouched) * 0.7));
    if (xaccel >= 40 or xaccel <= -40 or touch.data.x >= 220 or touch.data.x <= 40)
    {
      NotificationDismiss(nullptr);
      notifslideoff(nullptr);

      if (xaccel >= 40 or touch.data.x >= 220)
        NotificationDismissRight_Animation(notiftouched, 0);
      else
        NotificationDismissLeft_Animation(notiftouched, 0);
    }
  }
}

void notifslideon(lv_event_t *e)
{
  Serial.println("slideon");
  touchingnotif = 1;
  notiftouched = lv_event_get_target(e);
}
void notifslideoff(lv_event_t *e)
{
  touchingnotif = 0;
  Serial.println("slideoff");
  if (NotificationActive())
    CenterNotif_Animation(notiftouched, 0);
}

void CenterNotif_Animation(lv_obj_t *TargetObject, int delay)
{
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
  lv_anim_set_var(&a, TargetObject);
  lv_anim_set_time(&a, 300);
  lv_anim_set_values(&a, lv_obj_get_x(TargetObject), 0);
  lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
  lv_anim_start(&a);
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
  Serial.println(IsClockScreen());
  if (IsClockScreen())
  {
    if (NotificationActive())
      NotificationDismiss(nullptr);
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
    Serial.println("Connection Failed! Rebooting...");
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
      Serial.println("Start updating " + type); 
      
      _ui_screen_change(&ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Alarm_Going_Off_screen_init);
      lv_label_set_text(ui_Alarm_Going_Off_Name, "OTA Uploading");
      lv_obj_set_style_text_font(ui_Alarm_Going_Off_Name, &ui_font_Comfortaa_26, LV_PART_MAIN);
      lv_label_set_text(ui_Alarm_Going_Off_Time, "Starting");
      lv_obj_del(ui_Alarm_Going_Off_Stop_Button);
      Wakeup("Ota Begin Upload");
      twatch->motor_shake(1, 50); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  info.OTA.useOTA = true;
  info.OTA.ip = WiFi.localIP().toString();

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { uint8_t percent = (progress / (total / 100));
                          Serial.printf("Progress: %u%%\r", percent); 
                          lv_label_set_text_fmt(ui_Alarm_Going_Off_Time, "%i%%", percent);
                          TickleSleep(); });
}

void setup()
{
  // setCpuFrequencyMhz(240);

  // need to change uart_config.source_clk to UART_SCLK_REF_TICK in esp32-hal-uart.c in arduino framework https://github.com/espressif/arduino-esp32/issues/8122
  Serial.begin(115200); /* prepare for possible serial debug */
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

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

  Log.verboseln("Hello Arduino! V%i.%i.%i", lv_version_major(), lv_version_minor(), lv_version_patch());
  Log.verboseln("I am LVGL_Arduino");

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

  SetClockScreen(ui_SimplisticWatchFace);
  InitClockScreen();

  Log.verboseln("Init clock Screen");

#ifdef USESPLASHSCREEN
  lv_obj_clear_flag(ui_Logo_Arc, LV_OBJ_FLAG_HIDDEN);
#endif

  ui____initial_actions0 = lv_obj_create(NULL);
  Log.verboseln("ui____initial_actions0");

  DispLoadClockScreen();
  Log.verboseln("lv_disp_load_scr");

  twatch->backlight_set_value(100);
  // twatch->backlight_gradual_light(255,1000);

  InitUserSettings();

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

  // WriteTime();
  // lv_label_set_text(ui_Notification_Amount_Number, "0");
  Serial.println("Lv Timer");
  lv_timer_handler();

  Serial.println("BT on");
  BT_on();

#if defined(CONFIG_BMA423_LATER)
  Serial.println("Bma423 begin");
  twatch->bma423_begin(); // This takes 2 seconds

  // enable the wrist tilt interrupt
  twatch->bma423_feature(BMA423_WRIST_WEAR, true);
  twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, false);
#endif

  Serial.println("HAL Update");
  twatch->hal_auto_update(true, 1);

  Timer0Triggered = 1;

  twatch->motor_shake(1, 100);
  Log.verboseln("Setup done");
  Log.verboseln("Total PSRAM: %d", ESP.getPsramSize());
  Log.verboseln("Free PSRAM: %d", ESP.getFreePsram());

  info.flag.refresh = 1;

#ifdef USESPLASHSCREEN
  LogoSpin(ui_Logo_Arc);
#endif
}

void loop()
{
  if (useOTA)
    ArduinoOTA.handle();
  // Log.verboseln("%i%% CPU",100 - lv_timer_get_idle());
  if (!IsSleeping()) // If Awake
  {
    // lv_timer_handler(); /* let the GUI do its work */
    // delay(5);
    // delay(lv_timer_handler());

    if (IsClockScreen or 1) // Only run this if on the main screen////////////////////////////////////////////////////////////////////////////////////////
    {
      UpdateTime();
      ScreenHandleHandle();
      UpdateTime();
      Powerhandle();
      // notifslide(nullptr);
    }
    else
    {
      Compass();
    }
  }
  else // If Asleep
  {
    if (touch.available()) // Normally handled by lv_timer_handler
      Wakeup("Screen Touched");
    delay(100);
  }
  // alarmhandle();
  BTHandle();
  Sleephandle();
  VibrateHandle();
  TimersHandle();
  NotificationHandle();
  // Serial.println(twatch->power_get_volt());

  // this runs every 50ms
  if (BTTimerTriggered)
  {
    BTmsgloop();
    BTTimerTriggered = 0;
  }

  // This stuff runs every 10 seconds
  if (Timer0Triggered)
  {
    Timer0Triggered = 0;
    StepHandle();
    UpdatePower();
    ScheduleHandle();

    if (millis() < 20000)
    {
      info.flag.refresh = 1;
    }
  }

  // This stuff runs every 1 second
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
  Log.verboseln("Timer 0 Fired");
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

void ResetFlags()
{
  info.flag.hourchanged = 0;
  info.flag.minutechanged = 0;
  info.flag.secondchanged = 0;
}

#ifdef USESPLASHSCREEN
void LogoSpin(lv_obj_t *TargetObject)
{
  lv_anim_t a;
  lv_anim_init(&a);
  // lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_arc_set_value);
  // lv_anim_set_custom_exec_cb(&a, (lv_anim_custom_exec_cb_t)lv_anim_set_user_data);
  // Log.verboseln("Anim User Data: %i", (int)lv_anim_get_user_data(&a));
  // lv_anim_set_get_value_cb(&a, (lv_anim_get_value_cb_t)Serial.println());
  Serial.println(lv_anim_get_playtime(&a));
  // lv_arc_set_value(TargetObject, lv_anim_get_playtime(&a) / 3);
  lv_anim_set_var(&a, TargetObject);
  lv_anim_set_time(&a, 300);
  lv_anim_set_values(&a, 0, 100);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_start(&a);
}
#endif