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

#include "Preferences.h"
#include "ArduinoLog.h"
#include "ArduinoOTA.h"
#include "hardware/ble/gadgetbridge.h"

#include "ArduinoJson.h"

const char *ssid = "ThisNetworkIsOWN3D";
const char *password = "10244096";

TWatchClass *twatch = nullptr;

CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

extern Preferences Storage;

bool useOTA;
extern bool BTon;
bool Timer0Triggered = 1;
bool BTTimerTriggered;

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
    // if (getSleepTimer() < millis() - 200)
    //{
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;
    //}
    Wakeup("Screen Touched");

    Log.verboseln("Touch event. Data X: %i, Data Y: %i", data->point.x, data->point.y);
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
  Wakeup("Button 1 Pressed");
}
void btn2_click(void *param)
{
  Log.verboseln("BTN2 Click. MilliVolts: %i", (int)twatch->power_get_volt());
  // twatch->motor_shake(1, 60);
  if (lv_scr_act() == ui_Stopwatch)
    ToggleStopwatch(nullptr);
  if (lv_scr_act() == ui_Timers)
    ToggleTimer(nullptr);
  Wakeup("Button 2 Pressed");
}
void btn3_click(void *param)
{
  Log.verboseln("BTN3 Click");
  Wakeup("Button 3 Pressed");
  if (lv_scr_act() == ui_Clock)
  {
    if (NotificationActive())
      notificationdismiss(nullptr);
    // pairBT(random( 0,999999 ));
  }
  else
    screenback();
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
  totimescreen(nullptr);
  twatch->motor_shake(1, 30);
}

void setup()
{
  // setCpuFrequencyMhz(240);

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

  //////////Initalize UI//////////
  LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

  ui_Clock_screen_init();
  ui_Music_screen_init();

  lv_obj_del(ui_Tick_Dots); // Only used For visual purposes in Squareline Studio
  lv_obj_del(ui_Tick_Dashes);

  lv_obj_del(ui_Second_Dash_Include); // Only used to include files
  lv_obj_del(ui_Second_Dot_Include);

  InitTicks(); // Draws the tick marks around the edge

  ui____initial_actions0 = lv_obj_create(NULL);
  lv_disp_load_scr(ui_Clock);

  twatch->backlight_set_value(100);
  // twatch->backlight_gradual_light(255,1000);

#ifdef UPDATE_ELEMENTS
  lv_label_set_text(ui_Now_Playing_Label, "");
#endif

  // InitPercent(); // Battery Percent

  InitUserSettings();

  ApplyTheme(nullptr);
  // lv_timer_handler();

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

  //////////////////////////Fake Notifications///////////
  // FakeNotes();

  ////////////////////////////OTA
  if (useOTA)
  {
    lv_label_set_text(ui_Now_Playing_Label, "WiFi OTA");
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("ESP-Watch");
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("Connection Failed! Rebooting...");
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
      Serial.println("Start updating " + type); })
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
    lv_label_set_text(ui_Now_Playing_Label, WiFi.localIP().toString().c_str());
  }
  ////////////////////////////////////////END OTA

  WriteTime();
  lv_label_set_text(ui_Notification_Amount_Number, "0");
  lv_timer_handler();
  BT_on();
#if defined(CONFIG_BMA423_LATER)
  twatch->bma423_begin(); // This takes 2 seconds
  // twatch->bma423_reset();
#endif
  twatch->hal_auto_update(true, 1);

  twatch->motor_shake(1, 100);
  Log.verboseln("Setup done");
}

void loop()
{
  if (useOTA)
    ArduinoOTA.handle();
  // Log.verboseln("%i%% CPU",100 - lv_timer_get_idle());
  if (!isSleeping()) // If Awake
  {
    // lv_timer_handler(); /* let the GUI do its work */
    // delay(5);
    // delay(lv_timer_handler());

    if (lv_scr_act() == ui_Clock) // Only run this if on the main screen
    {
      WriteTime();
      Powerhandle();
      drawnotificationarc();
    }
    else
    {
      Compass();
      StopwatchHandle();
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

  // this runs every 50ms
  if (BTTimerTriggered)
  {
    BTmsgloop();
    BTTimerTriggered = 0;
  }

  // This stuff runs every X seconds
  if (Timer0Triggered)
  {
    Timer0Triggered = 0;
    StepHandle();
    DrawPower();
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

void ToggleFlashlight(lv_event_t *e)
{
  if (lv_event_get_code(e) == LV_EVENT_SCREEN_UNLOADED)
    twatch->backlight_set_value(GetUserBrightness());

  if ((int)lv_obj_get_style_bg_color(ui_Flashlight, LV_PART_MAIN).full == (int)lv_color_hex(0xFFFFFF).full)
    twatch->backlight_set_value(100);
  else
    twatch->backlight_set_value(GetUserBrightness());
}