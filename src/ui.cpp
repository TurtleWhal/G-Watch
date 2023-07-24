#include "TWatch_hal.h"
#include <BluetoothSerial.h>
#include <CST816S.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <lvgl.h>
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

// #include "gptcalc.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TWatchClass *twatch = nullptr;

CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

// extern BluetoothSerial SerialBT;
extern Preferences Storage;

bool useOTA;
extern bool BTon;
String input = "";
bool Timer0Triggered;
bool BTTimerTriggered;

extern NimBLECharacteristic *pGadgetbridgeTXCharacteristic;

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

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  TFT_eSPI *tft = twatch->tft_get_instance();
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft->startWrite();
  tft->setAddrWindow(area->x1, area->y1, w, h);
  tft->pushColors((uint16_t *)&color_p->full, w * h, true);
  tft->endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch.available())
  {
    data->state = LV_INDEV_STATE_PR;
    /*Set the coordinates*/
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;
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
  Log.verboseln("BTN1 Click. Power Percent: %i", (int)twatch->power_get_percent());
  // twatch->motor_shake(1, 60);
  /*
   if (BTon)
     SerialBT.println(twatch->power_get_percent());
     */
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
  /*
  if (BTon)
    SerialBT.println(twatch->power_get_volt());
    */
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
    resetstopwatch(nullptr);
  twatch->motor_shake(1, 30);
}

void btn3_held(void *param)
{
  Log.verboseln("BTN3 Held");
  Wakeup("Button 3 Held");
  totimescreen(nullptr);
  twatch->motor_shake(1, 30);
  gadgetbridge_send_msg("{\t\":\"notify\",\"id\":1654906063,\"src\":\"K-9 Mail\",\"title\":\"foo\",\"body\":\"bar 23\"}");
  gadgetbridge_send_loop_msg("{\"t\":\"notify\",\"id\":1654906064,\"src\":\"K-9 Mail\",\"title\":\"foo\",\"body\":\"bar 23\"}");
  // gadgetbridge_send_msg( "{\"t\":\"notify\",\"id\":1654906064,\"src\":\"K-9 Mail\",\"title\":\"foo\",\"body\":\"bar 23\"}" );
}

void setup()
{
  setCpuFrequencyMhz(240);

  Serial.begin(115200); /* prepare for possible serial debug */
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  twatch = TWatchClass::getWatch();
  twatch->hal_init();
  pinMode(TWATCH_CHARGING, INPUT_PULLUP);

  if (!digitalRead(TWATCH_BTN_2)) // Enable OTA if B2 is held at boot
    useOTA = 1;

  twatch->hal_auto_update(true, 1);

  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_CLICK, btn1_click);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_CLICK, btn2_click);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_CLICK, btn3_click);
  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_LONG_PRESS_START, btn1_held);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_LONG_PRESS_START, btn2_held);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_LONG_PRESS_START, btn3_held);

  touch.begin();

  Log.verboseln("Hello Arduino! V%i.%i.%i", lv_version_major(), lv_version_minor(), lv_version_patch());
  Log.verboseln("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

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

  InitTicks();

  ui____initial_actions0 = lv_obj_create(NULL);
  lv_disp_load_scr(ui_Clock);

  twatch->backlight_set_value(100);
  // twatch->backlight_gradual_light(255,1000);

#ifdef UPDATE_ELEMENTS
  lv_label_set_text(ui_Now_Playing_Label, "");
#endif

  InitPercent(); // Battery Percent

  InitUserSettings();

  ApplyTheme(nullptr);
  lv_timer_handler();

  hw_timer_t *timer = NULL;
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, Timer0Handle, true);
  timerAlarmWrite(timer, 10000 * 1000, true); // 10 seconds
  timerAlarmEnable(timer);

  // BTmsgloop 50ms timer
  hw_timer_t *timer2 = NULL;
  timer2 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer2, BTTimerHandle, true);
  timerAlarmWrite(timer2, 1000 * 1000, true);
  timerAlarmEnable(timer2);

  //////////////////////////Fake Notifications///////////
  FakeNotes();

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
  else
    BT_on();

  ////////////////////////////////////////END OTA

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
    delay(lv_timer_handler());

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

    StaticJsonDocument<200> batinfo;
    batinfo["t"] = "status";
    batinfo["bat"] = twatch->power_get_percent();
    //batinfo["volt"] = (float)twatch->power_get_volt()/1000;
    batinfo["chg"] = isCharging() ? 1:0;
    String buffer;
    serializeJson(batinfo, buffer);
    BTsend(buffer);
    // gadgetbridge_send_msg("\r\n{t:\"status\", bat:%d}\r\n", 79);
    // gadgetbridge_send_msg("\r\n{t:\"status\", bat:%d}\r\n", 79);
    // const char *buffer = "{t:\"status\", bat:0}";
    // sprintf((char *)buffer, "\r\n{t:\"status\", bat:%d}\r\n", 79);
    //  const unsigned char *TempMsg = "\r\n{t:\"status\", bat:79}\r\n";
    // pGadgetbridgeTXCharacteristic->setValue(*buffer, 24);
    // pGadgetbridgeTXCharacteristic->setValue(&3, 1);

    /*StaticJsonDocument<200> Sendjson;
    Sendjson["t"] = "status";
    Sendjson["bat"] = 72;
    String TempJson;
    serializeJson(Sendjson, TempJson);
    TempJson = "\r\n{t:\"status\", bat:0}\r\n";
    // TempJson = "\0\r\n" + TempJson + "\r\n";
    // sprintf((char *)TempJson.c_str(), "", TempJson.c_str());
    lv_label_set_text(ui_Now_Playing_Label, TempJson.c_str());

    // unsigned char buf[26] = "\0\r\n{t:\"status\", bat:";
    // unsigned char buf2[26] = "98}\r\n";
    unsigned char buf[26] = "\0\r\n{\"t\":\"status\", \"b";
    unsigned char buf2[26] = "at\":96}\r\n";

    // gadgetbridge_send_chunk(buf, 21);

    gadgetbridge_send_chunk(buf, 20);
    delay(50);
    gadgetbridge_send_chunk(buf2, 9);*/

    /*static bool temp = true;
    if (temp)
    {
      // buffer = "\r\n{t:\"status\", bat:7";
      //buf = "\r\n{t:\"status\", bat:\0";
      gadgetbridge_send_chunk(buf, 21);
      temp = !temp;
    }
    else
    {
      // buffer = "9}\r\n";
      //buf[26] = "0}\r\n";
      gadgetbridge_send_chunk(buf2, 2);
      temp = !temp;
    }*/

    // gadgetbridge_send_chunk((unsigned char *)buffer, 25);
    //  gadgetbridge_send_loop_msg("\r\n{t:\"status\", bat:%d}\r\n", 79);

    // pGadgetbridgeTXCharacteristic->notify();
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