#include "TWatch_hal.h"
#include <BluetoothSerial.h>
#include <CST816S.h>

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

#include "Preferences.h"
#include "ArduinoLog.h"

// #include "gptcalc.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TWatchClass *twatch = nullptr;

CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

extern BluetoothSerial SerialBT;
Preferences Storage;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

////////////////////Settings////////////////////
int StepGoal = 6500;         // Step Goal
int NotificationLength = 10; // Amount of time notifications are displayed in seconds
int VibrationStrength = 30;  // Strength of button vibrations
////////////////////////////////////////////////

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
  SerialBT.println(twatch->power_get_percent());
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
  SerialBT.println(twatch->power_get_volt());
  Wakeup("Button 2 Pressed");
}
void btn3_click(void *param)
{
  Log.verboseln("BTN3 Click");
  Wakeup("Button 3 Pressed");
  if (NotificationActive())
  {
    if (lv_scr_act() == ui_Clock)
      notificationdismiss(nullptr);
  }
  else
    buttontoclock();
}
void btn1_held(void *param)
{
  Log.verboseln("BTN1 Held");
  if (lv_scr_act() != ui_Settings)
    tosettingsscreen(nullptr);
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
  totimersscreen(nullptr);
  twatch->motor_shake(1, 30);
}

void setup()
{
  setCpuFrequencyMhz(240);

  Serial.begin(115200); /* prepare for possible serial debug */
  // Log.begin   (LOG_LEVEL_VERBOSE, &Serial);

  twatch = TWatchClass::getWatch();
  twatch->hal_init();
  pinMode(TWATCH_CHARGING, INPUT_PULLUP);

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

  lv_obj_del(ui_Tick_Dots); // Only used For visual purposes in Squareline Studio
  lv_obj_del(ui_Tick_Dashes);

  lv_obj_del(ui_Second_Dash_Include); // Only used to include files
  lv_obj_del(ui_Second_Dot_Include);

  InitTicks();

  ui____initial_actions0 = lv_obj_create(NULL);
  lv_disp_load_scr(ui_Clock);

  ApplyTheme();
  ////////////////////////////////

  twatch->backlight_set_value(100);
  //twatch->backlight_gradual_light(255,1000);
  lv_timer_handler();

#ifdef UPDATE_ELEMENTS
  lv_label_set_text(ui_Now_Playing_Label, "");
#endif

  InitPercent(); // Battery Percent

  Storage.begin("Settings", false);
  if (Storage.isKey("NotifLength"))
    NotificationLength = Storage.getInt("NotifLength");
  if (Storage.isKey("StepGoal"))
    StepGoal = Storage.getInt("StepGoal");

  BT_on();

  hw_timer_t *timer = NULL;
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, Timer0Handle, true);
  timerAlarmWrite(timer, 10000000, true);
  timerAlarmEnable(timer);

  twatch->motor_shake(1, 100);
  Log.verboseln("Setup done");
}

String input = "";
bool Timer0Triggered;

void loop()
{
  // Log.verboseln("%i%% CPU",100 - lv_timer_get_idle());
  if (!isSleeping())
  {
    // lv_timer_handler(); /* let the GUI do its work */
    // delay(5);
    delay(lv_timer_handler());

    if (lv_scr_act() == ui_Clock) // Only run this if on the main screen
    {
      WriteTime();
      Powerhandle();
    }
    Compass();
    CalcHandle();
    // twatch->backlight_updata(millis(), 10);
  }
  else
  {
    if (touch.available()) // Normally handled by lv_timer_handler
      Wakeup("Screen Touched");
    delay(100);
  }
  // alarmhandle();
  BThandle();
  Sleephandle();

  // This stuff runs every X seconds
  if (Timer0Triggered)
  {
    Timer0Triggered = 0;
    StepHandle();
    DrawPower();
  }

  /*if (digitalRead(TWATCH_CHARGING) and twatch->power_get_volt() < 3800)
    Sleephandle();*/
  /* Serial.println(digitalRead(TWATCH_CHARGING));
   Serial.println(twatch->power_get_volt());*/

  // Serial.println(twatch->rtc_get_time());
  // Serial.println(twatch->rtc_get_date());

  /*static uint32_t lastTime;
  if (millis() - lastTime >= 6000)
  {*/

  /*int mils = 2000;

      twatch->backlight_set_value(100);
      delay(mils);
      //SerialBT.print("100%: = ");
      //SerialBT.print("100%:");
      SerialBT.print(twatch->power_get_volt());
      SerialBT.print(",");

      twatch->backlight_set_value(75);
      delay(mils);
      //SerialBT.print("75%: = ");
      //SerialBT.print("75%:");
      SerialBT.print(twatch->power_get_volt());
      SerialBT.print(",");

      twatch->backlight_set_value(50);
      delay(mils);
      //SerialBT.print("50%: = ");
      //SerialBT.print("50%:");
      SerialBT.print(twatch->power_get_volt());
      SerialBT.print(",");

      twatch->backlight_set_value(25);
      delay(mils);
      //SerialBT.print("25%: = ");
      //SerialBT.print("25%:");
      SerialBT.print(twatch->power_get_volt());
      SerialBT.print(",");

      twatch->backlight_set_value(0);
      delay(mils);
      //SerialBT.print("0%: = ");
      //SerialBT.print("0%:");
      SerialBT.println(twatch->power_get_volt());*/

  /*lastTime = millis();
  // 3750 = battery full
}*/

  // lv_obj_set_style_img_recolor(ui_Colorset_Tick_Dots, lv_color_hex(0xEE3C39), LV_PART_MAIN | LV_STATE_DEFAULT);

  drawnotificationarc();
  processstopwatch();
}

void Timer0Handle()
{
  Log.verboseln("Timer 0 Fired");
  Timer0Triggered = 1;
}

void StepHandle()
{
  int Steps;
  static int StepDay;
  static int LastSteps = -1;
  static int StepOffset = -1;
  char StepChar[12];

  if (StepOffset == -1)
  {
    if (Storage.getInt("StepDay") == GetDay())
      StepOffset = Storage.getInt("Steps");
    else
      StepOffset = 0;
  }

  int GetStep = twatch->bma423_get_step();

  if (LastSteps != GetStep)
  {
    LastSteps = GetStep;
    Steps = GetStep + StepOffset;
    sprintf(StepChar, "%i", Steps);
#ifdef UPDATE_ELEMENTS
    lv_label_set_text(ui_Step_Counter_Text, StepChar);
    lv_arc_set_value(ui_Arc_Right, ((float)Steps / StepGoal) * 250);
#endif

    if (Steps >= StepGoal and !Storage.getBool("StepReach"))
    {
#ifdef UPDATE_ELEMENTS
      lv_label_set_text(ui_Notification_Title, "Step Goal Reached!");
      char StepGoalText[50];
      sprintf(StepGoalText, "You have reached your step goal of %i Steps!", StepGoal);
      lv_label_set_text(ui_Notification_Text, StepGoalText);
#endif
      shownotification(0);
      Storage.putBool("StepReach", 1);
    }

    Storage.putInt("Steps", Steps);
    StepDay = GetDay();
    Storage.putInt("StepsDay", StepDay);
  }

  if (Storage.getInt("StepDay") != GetDay())
  {
    Storage.putInt("StepDay", GetDay());
    Storage.putBool("StepReach", 0);
    StepOffset = 0;
    Storage.putInt("Steps", 0);
    twatch->bma423_step_reset();
  }
}

void UpdateSettings(lv_event_t *e)
{
  StepGoal = atoi(lv_textarea_get_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));
  if (StepGoal != Storage.getInt("StepGoal"))
  {
    Storage.putInt("StepGoal", StepGoal);
    Storage.putBool("StepReach", 0);
  }

  NotificationLength = atoi(lv_textarea_get_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));
  Storage.putInt("NotifLength", NotificationLength);

  Storage.putBytes("BTname", lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)), 17);
  // Serial.println(lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));

  Storage.putInt("Theme", lv_colorwheel_get_rgb(ui_Theme_Colorwheel).full);
  ApplyTheme();
}
