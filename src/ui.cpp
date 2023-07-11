#include "TWatch_hal.h"
#include <BluetoothSerial.h>
#include <TFT_eSPI.h>
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

#include "Preferences.h"
#include "ArduinoLog.h"

//#include "gptcalc.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

TWatchClass *twatch = nullptr;

CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

extern BluetoothSerial SerialBT;
Preferences Storage;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

lv_color_t ThemeColor = lv_palette_darken(LV_PALETTE_AMBER, 4);

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
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, false);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
if (touch.available()) {
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
}

void btn2_held(void *param)
{
  Log.verboseln("BTN2 Held");
  Wakeup("Button 2 Held");
  if (lv_scr_act() == ui_Stopwatch)
    resetstopwatch(nullptr);
}

void btn3_held(void *param)
{
  Log.verboseln("BTN3 Held");
  Wakeup("Button 3 Held");
  totimersscreen(nullptr);
}

void setup()
{
  setCpuFrequencyMhz(240);

  Storage.begin("Settings", false);


  Serial.begin(115200); /* prepare for possible serial debug */
  Log.begin   (LOG_LEVEL_VERBOSE, &Serial);

  BT_on();
  

  if (Storage.isKey("NotifLength"))
    NotificationLength = Storage.getInt("NotifLength");
  if (Storage.isKey("StepGoal"))
    StepGoal = Storage.getInt("StepGoal");

  // lv_obj_del(ui_Notification_Widget2);

  twatch = TWatchClass::getWatch();

  twatch->hal_auto_update(true, 1);

  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_CLICK, btn1_click);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_CLICK, btn2_click);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_CLICK, btn3_click);
  twatch->button_bind_event(TWATCH_BTN_1, BUTTON_LONG_PRESS_START, btn1_held);
  twatch->button_bind_event(TWATCH_BTN_2, BUTTON_LONG_PRESS_START, btn2_held);
  twatch->button_bind_event(TWATCH_BTN_3, BUTTON_LONG_PRESS_START, btn3_held);

  twatch->power_init();

  pinMode(TWATCH_CHARGING, INPUT_PULLUP);

  Log.verboseln("Hello Arduino! V%i.%i.%i", lv_version_major(), lv_version_minor(), lv_version_patch());
  Log.verboseln("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */

  touch.begin();
  //touch.sleep();

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
  // ui_init();

  LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

  // lv_palette_t *myorange = lv_palette_darken(LV_PALETTE_AMBER, 4);

  // lv_disp_set_theme(dispp, theme);

  ui_Clock_screen_init();

  ui____initial_actions0 = lv_obj_create(NULL);
  lv_disp_load_scr(ui_Clock);

  ApplyTheme();
  ////////////////////////////////

  twatch->backlight_init();
  twatch->backlight_set_value(100);

  twatch->qmc5883l_init();
  twatch->rtc_init();

  lv_label_set_text(ui_Now_Playing_Label, "");

  InitPercent();

  // lv_theme_default_init(lv_disp_get_default(), lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_BLUE), true, LV_FONT_DEFAULT);

  hw_timer_t *timer = NULL;
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, Timer0Handle, true);
  timerAlarmWrite(timer, 10000000, true);
  timerAlarmEnable(timer);

  twatch->motor_shake(1, 100);
  Log.verboseln("Setup done");
}

String input = "";

//char terminatingChar = '\n';
//int timestart;

bool Timer0Triggered;

void loop()
{
  //Log.verboseln("%i%% CPU",100 - lv_timer_get_idle());
  if (!isSleeping())
  {
    //lv_timer_handler(); /* let the GUI do its work */
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

void Compass()
{
  if (lv_scr_act() == ui_Compass)
  {
    // Serial.println(mag.getAzimuth());
    char deg[6];
    sprintf(deg, "%i°", twatch->qmc5883l_get_Azimuth());
    lv_label_set_text(ui_Compass_Deg, deg);
    // lv_obj_set_x(ui_Compass_N, 0);
    // lv_obj_set_y(ui_Compass_N, -100);
  }
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
    lv_label_set_text(ui_Step_Counter_Text, StepChar);
    lv_arc_set_value(ui_Arc_Right, ((float)Steps / StepGoal) * 250);

    if (Steps >= StepGoal and !Storage.getBool("StepReach"))
    {
      lv_label_set_text(ui_Notification_Title, "Step Goal Reached!");
      char StepGoalText[50];
      sprintf(StepGoalText, "You have reached your step goal of %i Steps!", StepGoal);
      lv_label_set_text(ui_Notification_Text, StepGoalText);
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

void UpdateTestTheme(lv_event_t *e)
{
  lv_obj_set_style_bg_color(ui_Theme_Apply_Button, lv_colorwheel_get_rgb(ui_Theme_Colorwheel), LV_PART_MAIN);
  // char color[32];
  // sprintf(color, "Hex: #%i", lv_color_hex(lv_colorwheel_get_hsv(ui_Theme_Colorwheel)));
  // sprintf(color, "Hex: #%X%X%X", lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.red, lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.green_h, lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.blue);
  /*Serial.print(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.red);
  Serial.print(", ");
  Serial.print(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.green_l);
  Serial.print(", ");
  Serial.println(lv_colorwheel_get_rgb(ui_Theme_Colorwheel).ch.blue);*/
  // lv_label_set_text(ui_Theme_Hex_Label, color);
  lv_label_set_text(ui_Theme_Hex_Label, "Hex Code");
}

void ToggleTheme(lv_event_t *e)
{
  static bool themeexpanded;
  if (themeexpanded)
  {
    ThemeSettingShrink_Animation(ui_Theme_Setting_Panel, 0);
    lv_img_set_angle(ui_Theme_Expand_Arrow, 1800);
    themeexpanded = 0;
    lv_obj_add_flag(ui_Theme_Colorwheel, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    ThemeSettingExpand_Animation(ui_Theme_Setting_Panel, 0);
    lv_img_set_angle(ui_Theme_Expand_Arrow, 0);
    themeexpanded = 1;
    lv_obj_clear_flag(ui_Theme_Colorwheel, LV_OBJ_FLAG_HIDDEN);
  }
}

void ApplyTheme()
{
  lv_disp_t *dispp = lv_disp_get_default();
  if (Storage.isKey("Theme"))
  {
    lv_color16_t color;
    color.full = Storage.getInt("Theme");
    lv_theme_t *theme = lv_theme_default_init(dispp, color, lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ThemeColor = color;
  }
  else
  {
    lv_theme_t *theme = lv_theme_default_init(dispp, ThemeColor, lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
  }

  //////////Apply colors to unthemed items//////////

  // Clock Screen
    lv_obj_set_style_img_recolor(ui_Minute_Hand, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Steps_Image, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Second_Dot, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Step_Counter_Text, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Date_Numerical, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Notification_Timer, ThemeColor, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Notification_Image_Panel, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Compass Screen
  if (ui_Compass != NULL)
  {
    lv_obj_set_style_text_color(ui_Compass_Direction, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Compass_N, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_Compass_Arrow, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  // Calculator Screen
  if (ui_Calculator != NULL)
  {
    lv_obj_set_style_bg_color(ui_Calculator_Keyboard, ThemeColor, LV_PART_ITEMS | LV_STATE_CHECKED);
  }

  // Alarm Set Screen
  if (ui_Set_Alarm != NULL)
  {
    lv_obj_set_style_text_color(ui_AM, ThemeColor, LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}