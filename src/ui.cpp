#include "TWatch_hal.h"
#include <BluetoothSerial.h>
#include <TFT_eSPI.h>
// #include <CST816S.h>

#include <lvgl.h>
#include "ui.h"
#include "ui_helpers.h"

#include "sleep.h"
#include "timestuff.h"

#include "Preferences.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

TWatchClass *twatch = nullptr;

// CST816S touch(26, 25, 33, 32); // sda, scl, rst, irq

BluetoothSerial SerialBT;
Preferences Storage;

// functions
void writetimertime();
void shownotification(bool Store);
void notificationdismiss();
void alarmhandle();
void BThandle();
void istimernegative();
void Powerhandle();
void Sleephandle();
void Compass();
void StepHandle();
void Timer0Handle();

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

typedef struct
{
  bool state;
  bool am;
  uint8_t hour;
  uint8_t minute;
  char *hourtext;
  char *minutetext;
  const char *name;
} Alarm;

Alarm alarms[4];
int selectedalarm = 0;

bool notificationshowing = 0;
int notificationid = 1;
int notificationtime;

int stopwatchtime = 0;
int stopwatchtimestarted = 0;
int stopwatchtimestopped = 0;
bool stopwatchmoving = 0;
char stopwatchtimechar[3];
char zerobuffer[3];

int timertime;
bool timermoving;
int timerlasttime;
char timertimechar[3];

int btlasttime;
bool btconnected;

int songtime;

bool Donotdisturb = 0;
bool BTon = 1;

int lastpercent = 100;

bool charging;

////////////////////Settings////////////////////
int StepGoal;               // Step Goal
int NotificationLength;     // Amount of time notifications are displayed in seconds
int VibrationStrength = 30; // Strength of button vibrations
////////////////////////////////////////////////

lv_obj_t *ui_Notification_Widgets[1];

LV_IMG_DECLARE(ui_img_bluetooth_small_png);

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
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  // uint16_t touchX = 0, touchY = 0;

  // bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );

  if (!twatch->touch_check())
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = twatch->touch_getX();
    data->point.y = twatch->touch_getY();

    Serial.print("Data x ");
    Serial.println(twatch->touch_getX());

    Serial.print("Data y ");
    Serial.println(twatch->touch_getY());
  }
}

void btn1_click(void *param)
{
  Serial.println("BTN1 Click");
  // twatch->motor_shake(1, 60);
  Serial.println(twatch->power_get_percent());
  SerialBT.println(twatch->power_get_percent());
  Wakeup("Button 1 Pressed");
}
void btn2_click(void *param)
{
  Serial.println("BTN2 Click");
  // twatch->motor_shake(1, 60);
  if (lv_scr_act() == ui_Stopwatch)
    ToggleStopwatch(nullptr);
  if (lv_scr_act() == ui_Timers)
    ToggleTimer(nullptr);
  Serial.println(twatch->power_get_volt());
  SerialBT.println(twatch->power_get_volt());
  Wakeup("Button 2 Pressed");
}
void btn3_click(void *param)
{
  Serial.println("BTN3 Click");
  // twatch->motor_shake(1, 60);
  if (lv_scr_act() != ui_Clock)
    _ui_screen_change(ui_Clock, LV_SCR_LOAD_ANIM_NONE, 150, 0);
  Wakeup("Button 3 Pressed");
  if (notificationshowing == 1)
    notificationdismiss(nullptr);
}
void btn1_held(void *param)
{
  Serial.println("BTN1 Held");
  if (lv_scr_act() != ui_Settings)
    _ui_screen_change(ui_Settings, LV_SCR_LOAD_ANIM_NONE, 150, 0);
  Wakeup("Button 1 Held");
}

void btn2_held(void *param)
{
  Serial.println("BTN2 Held");
  Wakeup("Button 2 Held");
  if (lv_scr_act() == ui_Stopwatch)
    resetstopwatch(nullptr);
}

void btn3_held(void *param)
{
  Serial.println("BTN3 Held");
  Wakeup("Button 3 Held");
  _ui_screen_change(ui_Timers, LV_SCR_LOAD_ANIM_NONE, 150, 0);
}

void setup()
{
  setCpuFrequencyMhz(240);

  Storage.begin("Settings", false);

  Serial.begin(115200); /* prepare for possible serial debug */

  if (Storage.isKey("BTname"))
  {
    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    SerialBT.begin((String)BTnamechar);
    Serial.print("BT Name: ");
    Serial.println(BTnamechar);
  }
  else
    SerialBT.begin("Unnamed Watch"); /*
     char BTnamechar[17];
     Storage.getBytes("BTname", BTnamechar, 17);
     Serial.println(BTnamechar);
     Serial.println(Storage.isKey("BTname"));*/

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

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */

  twatch->touch_init();

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

  alarms[0].state = 0;
  alarms[1].state = 0;
  alarms[2].state = 0;
  alarms[3].state = 0;

  alarms[0].am = 1;
  alarms[1].am = 1;
  alarms[2].am = 1;
  alarms[3].am = 1;

  ui_init();

  twatch->backlight_init();
  twatch->backlight_set_value(100);

  twatch->qmc5883l_init();
  twatch->rtc_init();

  lv_label_set_text(ui_Now_Playing_Label, "");
  
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000)
    lastpercent = 0;

  StepGoal = Storage.getInt("StepGoal");
  char StepGoalChar[6];
  sprintf(StepGoalChar, "%i", StepGoal);
  lv_textarea_set_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), StepGoalChar);

  char BTnamechar[17];
  Storage.getBytes("BTname", BTnamechar, 17);
  lv_textarea_set_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), BTnamechar);

  NotificationLength = Storage.getInt("NotifLength");
  char NotificationLengthChar[4];
  sprintf(NotificationLengthChar, "%i", NotificationLength);
  lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

  // lv_theme_default_init(lv_disp_get_default(), lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_BLUE), true, LV_FONT_DEFAULT);

  hw_timer_t *timer = NULL;
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, Timer0Handle, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  Serial.println("Setup done");
}

String input = "";
bool readStringUntil(String &input, size_t char_limit)
{
  while (SerialBT.available())
  {
    char c = SerialBT.read();
    input += c;
    if (c == 254)
    {
      return true;
    }
    if (input.length() >= char_limit)
    {
      return true;
    }
  }
  return false;
}

char terminatingChar = '\n';
int timestart;

bool Timer0Triggered;

void loop()
{

  if (!isSleeping())
  {
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    if (lv_scr_act() == ui_Clock) // Only run this if on the main screen
    {
      WriteTime();
    }
    Compass();
  }
  else
    delay(5);

  //alarmhandle();
  BThandle();
  Sleephandle();

  // This stuff runs every X seconds
  if (Timer0Triggered)
  {
    Timer0Triggered = 0;
    Powerhandle();
    StepHandle();
  }

  /*if (digitalRead(TWATCH_CHARGING) == 1 and twatch->power_get_volt() < 3800)
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

  if (notificationshowing == 1)
  {
    lv_arc_set_value(ui_Notification_Timer, ((millis() - notificationtime) / (NotificationLength * 10)) * 10);
    if (notificationtime + (NotificationLength * 1000) < millis())
    {
      NotificationHide_Animation(ui_Notification_Popup, 0);
      notificationshowing = 0;
    }
  }

  if (stopwatchmoving == 1)
  {
    stopwatchtime = (millis() - stopwatchtimestarted);
    // itoa(stopwatchtime,stopwatchtimechar,10);

    unsigned long stopwatchmilliseconds = stopwatchtime / 10;
    unsigned long stopwatchseconds = stopwatchtime / 1000;
    unsigned long stopwatchminutes = stopwatchseconds / 60;
    unsigned long stopwatchhours = stopwatchminutes / 60;
    stopwatchmilliseconds %= 100;
    stopwatchseconds %= 60;
    stopwatchminutes %= 60;
    stopwatchhours %= 24;

    if (stopwatchmilliseconds < 10)
    {
      sprintf(stopwatchtimechar, "0%d", stopwatchmilliseconds);
    }
    else
    {
      itoa(stopwatchmilliseconds, stopwatchtimechar, 10);
    }
    lv_label_set_text(ui_Stopwatch_Milliseconds, stopwatchtimechar);

    if (stopwatchseconds < 10)
    {
      sprintf(stopwatchtimechar, "0%d", stopwatchseconds);
    }
    else
    {
      itoa(stopwatchseconds, stopwatchtimechar, 10);
    }
    lv_label_set_text(ui_Stopwatch_Seconds, stopwatchtimechar);

    if (stopwatchminutes < 10)
    {
      sprintf(stopwatchtimechar, "0%d", stopwatchminutes);
    }
    else
    {
      itoa(stopwatchminutes, stopwatchtimechar, 10);
    }
    lv_label_set_text(ui_Stopwatch_Minutes, stopwatchtimechar);

    if (stopwatchhours < 10)
    {
      sprintf(stopwatchtimechar, "0%d", stopwatchhours);
    }
    else
    {
      itoa(stopwatchhours, stopwatchtimechar, 10);
    }
    lv_label_set_text(ui_Stopwatch_Hours, stopwatchtimechar);
  }

  if (timermoving == 1)
  {
    if (millis() > timerlasttime)
    {
      timertime -= (millis() - timerlasttime);
      timerlasttime = millis();
    }
    writetimertime();
    istimernegative();
  }
}

void setalarm1(lv_event_t *e)
{
  selectedalarm = 0;
  if (alarms[0].am == 1)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm2(lv_event_t *e)
{
  selectedalarm = 1;
  if (alarms[1].am == 1)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm3(lv_event_t *e)
{
  selectedalarm = 2;
  if (alarms[2].am == 1)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm4(lv_event_t *e)
{
  selectedalarm = 3;
  if (alarms[3].am == 1)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}

void setalarm(lv_event_t *e)
{
  lv_label_set_text(ui_Alarm_1_Name, "Alarm Set");
  char temptime[6];
  lv_roller_get_selected_str(ui_Hour_Roller, temptime, 3);
  alarms[selectedalarm].hourtext = temptime;
  if (alarms[selectedalarm].am == 0)
  {
    alarms[selectedalarm].hour = atoi(temptime) + 12;
  }
  else
  {
    alarms[selectedalarm].hour = atoi(temptime);
  }

  lv_roller_get_selected_str(ui_Minute_Roller, temptime, 3);
  alarms[selectedalarm].minutetext = temptime;
  alarms[selectedalarm].minute = atoi(temptime);

  // if (alarms[0].hour<10) alarms[0].minutetext = "0"+temptime;
  if (alarms[selectedalarm].am == 1)
  {
    itoa(alarms[selectedalarm].hour, temptime, 10);
  }
  else
  {
    itoa(alarms[selectedalarm].hour - 12, temptime, 10);
  }

  if (selectedalarm == 0)
    lv_label_set_text(ui_Alarm_1_Hour, temptime);
  if (selectedalarm == 1)
    lv_label_set_text(ui_Alarm_2_Hour, temptime);
  if (selectedalarm == 2)
    lv_label_set_text(ui_Alarm_3_Hour, temptime);
  if (selectedalarm == 3)
    lv_label_set_text(ui_Alarm_4_Hour, temptime);

  if (alarms[selectedalarm].minute < 10)
  {
    sprintf(temptime, "0%d", alarms[selectedalarm].minute);
  }
  else
  {
    itoa(alarms[selectedalarm].minute, temptime, 10);
  }
  if (selectedalarm == 0)
  {
    lv_label_set_text(ui_Alarm_1_Minute, alarms[selectedalarm].minutetext);
    alarms[0].state = 1;
    lv_obj_add_state(ui_Alarm_1_Switch, LV_STATE_CHECKED);
    if (alarms[0].am == 1)
    {
      lv_label_set_text(ui_Alarm_1_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_1_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_1_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[0].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm == 1)
  {
    lv_label_set_text(ui_Alarm_2_Minute, alarms[selectedalarm].minutetext);
    alarms[1].state = 1;
    lv_obj_add_state(ui_Alarm_2_Switch, LV_STATE_CHECKED);
    if (alarms[1].am == 1)
    {
      lv_label_set_text(ui_Alarm_2_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_2_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_2_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[1].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm == 2)
  {
    lv_label_set_text(ui_Alarm_3_Minute, alarms[selectedalarm].minutetext);
    alarms[2].state = 1;
    lv_obj_add_state(ui_Alarm_3_Switch, LV_STATE_CHECKED);
    if (alarms[2].am == 1)
    {
      lv_label_set_text(ui_Alarm_3_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_3_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_3_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[2].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm == 3)
  {
    lv_label_set_text(ui_Alarm_4_Minute, alarms[selectedalarm].minutetext);
    alarms[3].state = 1;
    lv_obj_add_state(ui_Alarm_4_Switch, LV_STATE_CHECKED);
    if (alarms[3].am == 1)
    {
      lv_label_set_text(ui_Alarm_4_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_4_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_4_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[3].name = lv_textarea_get_text(ui_Alarm_Name);
  }
}

void togglealarm1(lv_event_t *e)
{
  if (alarms[0].state == 0)
  {
    alarms[0].state = 1;
  }
  else
  {
    alarms[0].state = 0;
  }
}
void togglealarm2(lv_event_t *e)
{
  if (alarms[1].state == 0)
  {
    alarms[1].state = 1;
  }
  else
  {
    alarms[1].state = 0;
  }
}
void togglealarm3(lv_event_t *e)
{
  if (alarms[2].state == 0)
  {
    alarms[2].state = 1;
  }
  else
  {
    alarms[2].state = 0;
  }
}
void togglealarm4(lv_event_t *e)
{
  if (alarms[3].state == 0)
  {
    alarms[3].state = 1;
  }
  else
  {
    alarms[3].state = 0;
  }
}

void toggleampm(lv_event_t *e)
{
  if (alarms[selectedalarm].am == 1)
  {
    alarms[selectedalarm].am = 0;
  }
  else
  {
    alarms[selectedalarm].am = 1;
  }
}

/*void alarmhandle()
{
  char alarmscreentime[6];
  static uint8_t alarmlastmin;
  if (rtc.minute > alarmlastmin || rtc.minute == 0 & alarmlastmin == 59)
  {
    alarmlastmin = rtc.minute;

    if (alarms[0].state == 1)
    {
      rtc.read();
      if (alarms[0].hour == rtc.hour)
      {
        if (alarms[0].minute == rtc.minute)
        {
          Serial.println("alarm 1 triggered");
          alarms[0].state = 0;
          _ui_state_modify(ui_Alarm_1_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[0].hour) - 12, alarms[0].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[0].hour, alarms[0].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[0].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[1].state == 1)
    {
      rtc.read();
      if (alarms[1].hour == rtc.hour)
      {
        if (alarms[1].minute == rtc.minute)
        {
          Serial.println("alarm 2 triggered");
          alarms[1].state = 0;
          _ui_state_modify(ui_Alarm_2_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[1].hour) - 12, alarms[1].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[1].hour, alarms[1].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[1].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[2].state == 1)
    {
      rtc.read();
      if (alarms[2].hour == rtc.hour)
      {
        if (alarms[2].minute == rtc.minute)
        {
          Serial.println("alarm 3 triggered");
          alarms[2].state = 0;
          _ui_state_modify(ui_Alarm_3_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
          lv_img_set_src(ui_Notification_Image, &ui_img_bell_png);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[2].hour) - 12, alarms[2].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[2].hour, alarms[2].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[2].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[3].state == 1)
    {
      rtc.read();
      if (alarms[3].hour == rtc.hour)
      {
        if (alarms[3].minute == rtc.minute)
        {
          Serial.println("alarm 1 triggered");
          alarms[3].state = 0;
          _ui_state_modify(ui_Alarm_4_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[3].hour) - 12, alarms[3].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[3].hour, alarms[3].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[3].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    Serial.println("Alarms Handled");
  }
}*/

void shownotification(bool Store)
{
  // Create the widget in the Clock screen
  Wakeup("Notification Received");
  lv_obj_set_x(ui_Notification_Popup, 0);
  lv_obj_set_y(ui_Notification_Popup, -160);
  NotificationShow_Animation(ui_Notification_Popup, 0);
  notificationtime = millis();
  notificationshowing = 1;
  if (!Donotdisturb)
    twatch->motor_shake(2, 30);

  // Create the widget in the notifications screen
  if (Store)
  {
    lv_obj_t *NotificationComp = ui_Notification_Widget_create(ui_Notification_Panel);
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TITLE), lv_label_get_text(ui_Notification_Title));
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET_VISIBLE_NOTIFICATION_TEXT), lv_label_get_text(ui_Notification_Text));
    lv_label_set_text(ui_comp_get_child(NotificationComp, UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_SOURCE), lv_label_get_text(ui_Notification_Source));
  }
}

void notificationdismiss(lv_event_t *e)
{
  lv_obj_set_x(ui_Notification_Popup, 0);
  lv_obj_set_y(ui_Notification_Popup, -60);
  NotificationDismiss_Animation(ui_Notification_Popup, 0);
  notificationtime = 0;
  notificationshowing = 0;
}

void DeleteNotification(lv_event_t *e)
{
  // lv_obj_del(ui_comp_get_child(lv_event_get_target(e), UI_COMP_NOTIFICATION_WIDGET_NOTIFICATION_WIDGET));
  // lv_obj_del(lv_event_get_current_target(e));
  // lv_group_del(lv_obj_get_group(lv_event_get_target(e)));
  //_lv_obj_destruct(lv_event_get_target(e));
  //_ui_flag_modify(lv_event_get_target(e), LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}

void ToggleStopwatch(lv_event_t *e)
{
  if (stopwatchmoving == 0)
  {
    stopwatchmoving = 1;
    if (stopwatchtimestarted == 0)
    {
      stopwatchtimestarted = millis();
    }
    else
    {
      stopwatchtimestarted = stopwatchtimestarted + (millis() - stopwatchtimestopped);
    }
    PlayToPause_Animation(ui_Stopwatch_Play_Pause_Image, 0);
  }
  else
  {
    stopwatchmoving = 0;
    stopwatchtimestopped = millis();
    PauseToPlay_Animation(ui_Stopwatch_Play_Pause_Image, 0);
  }
  twatch->motor_shake(1, 50);
}

void resetstopwatch(lv_event_t *e)
{
  stopwatchmoving = 0;
  PauseToPlay_Animation(ui_Stopwatch_Play_Pause_Image, 0);

  stopwatchtimestarted = 0;
  lv_label_set_text(ui_Stopwatch_Milliseconds, "00");
  lv_label_set_text(ui_Stopwatch_Seconds, "00");
  lv_label_set_text(ui_Stopwatch_Minutes, "00");
  lv_label_set_text(ui_Stopwatch_Hours, "00");
  twatch->motor_shake(1, 50);
}

void ToggleTimer(lv_event_t *e)
{
  if (timermoving == 0)
  {
    timermoving = 1;
    PlayToPause_Animation(ui_Timer_Play_Pause_Image, 0);
    timerlasttime = millis();
  }
  else
  {
    timermoving = 0;
    PauseToPlay_Animation(ui_Timer_Play_Pause_Image, 0);
  }
  twatch->motor_shake(1, 50);
}

void timerhourplus10(lv_event_t *e)
{
  timertime += 36000000;
  writetimertime();
}
void timerhourplus1(lv_event_t *e)
{
  timertime += 3600000;
  writetimertime();
}
void timerminuteplus10(lv_event_t *e)
{
  timertime += 600000;
  writetimertime();
}
void timerminuteplus1(lv_event_t *e)
{
  timertime += 60000;
  writetimertime();
}
void timersecondplus10(lv_event_t *e)
{
  timertime += 10000;
  writetimertime();
}
void timersecondplus1(lv_event_t *e)
{
  timertime += 1000;
  writetimertime();
}
void timerhourminus10(lv_event_t *e)
{
  timertime -= 36000000;
  istimernegative();
  writetimertime();
}
void timerhourminus1(lv_event_t *e)
{
  timertime -= 3600000;
  istimernegative();
  writetimertime();
}
void timerminuteminus10(lv_event_t *e)
{
  timertime -= 600000;
  istimernegative();
  writetimertime();
}
void timerminuteminus1(lv_event_t *e)
{
  timertime -= 60000;
  istimernegative();
  writetimertime();
}
void timersecondminus10(lv_event_t *e)
{
  timertime -= 10000;
  istimernegative();
  writetimertime();
}
void timersecondminus1(lv_event_t *e)
{
  timertime -= 1000;
  istimernegative();
  writetimertime();
}

void istimernegative()
{
  if (timertime <= 0)
  {
    timertime = 0;
    if (timermoving == 1)
    {
      lv_label_set_text(ui_Alarm_Going_Off_Time, "00:00:00");
      lv_label_set_text(ui_Alarm_Going_Off_Name, "Timer");
      _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
      timermoving = 0;
      ToggleTimer(nullptr);
      // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    }
  }
}

void writetimertime()
{
  unsigned long timerseconds = timertime / 1000;
  unsigned long timerminutes = timerseconds / 60;
  unsigned long timerhours = timerminutes / 60;
  timerseconds %= 60;
  timerminutes %= 60;
  timerhours %= 24;

  if (timerseconds < 10)
  {
    sprintf(timertimechar, "0%d", timerseconds);
  }
  else
  {
    itoa(timerseconds, timertimechar, 10);
  }
  lv_label_set_text(ui_Timer_Seconds, timertimechar);

  if (timerminutes < 10)
  {
    sprintf(timertimechar, "0%d", timerminutes);
  }
  else
  {
    itoa(timerminutes, timertimechar, 10);
  }
  lv_label_set_text(ui_Timer_Minutes, timertimechar);

  if (timerhours < 10)
  {
    sprintf(timertimechar, "0%d", timerhours);
  }
  else
  {
    itoa(timerhours, timertimechar, 10);
  }
  lv_label_set_text(ui_Timer_Hours, timertimechar);
}

void BThandle()
{
  /*    if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }*/

  static String input = "";
  static bool ping = 0;
  static bool waitingforping = 1;
  if (readStringUntil(input, 240))
  { // read until find newline or have read 20 chars
    if (input.lastIndexOf(254) >= 0)
    { // could also use check  if (input[input.length()-1] == terminatingChar) {
      // Serial.print(F(" got a line of input '")); Serial.print(input); Serial.println("'");
      if (input.charAt(0) == 1)
      {
        Serial.print("Getting Time From Phone: ");
        Serial.print((int)(input.charAt(1)));
        Serial.print("h ");
        Serial.print((int)(input.charAt(2)));
        Serial.print("m ");
        Serial.print((int)(input.charAt(3)));
        Serial.print("s ");
        Serial.print((int)(input.charAt(4)));
        Serial.print("month ");
        Serial.print((int)(input.charAt(5)));
        Serial.print("day ");
        Serial.print((int)((input.charAt(6) + 2000)));
        Serial.println("year");
        // rtc.adjust(input.charAt(1), input.charAt(2), input.charAt(3), input.charAt(6) + 2000, input.charAt(4), input.charAt(5));
        twatch->rtc_set_time(input.charAt(6) + 2000, input.charAt(4), input.charAt(5), input.charAt(1), input.charAt(2), input.charAt(3));
      }
      if (input.charAt(0) == 2)
      {
        Serial.print("Notification Title: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Title, input.c_str());
      }
      if (input.charAt(0) == 3)
      {
        Serial.print("Notification Text: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Text, input.c_str());
      }
      if (input.charAt(0) == 4)
      {
        Serial.print("Notification Source: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        lv_label_set_text(ui_Notification_Source, input.c_str());
        shownotification(1);
      }
      if (input.charAt(0) == 5)
      {
        Serial.print("Now Playing: ");
        input.remove(0, 1);
        input.remove(input.length() - 1, 1);
        Serial.println(input);
        char song[64];
        sprintf(song, "♪ %s ♪", input.c_str());
        lv_label_set_text(ui_Now_Playing_Label, song);
        songtime = millis();
      }

      if (input.charAt(0) == 7)
      {
        // static char componentname[25];
        // sprintf(componentname, "ui_Generic_Notification%d", notificationid);
        // lv_obj_t * ui_Notification_Widgets[1];
        ui_Notification_Widgets[0] = ui_Notification_Widget_create(ui_Notification_Panel);
        lv_obj_set_x(ui_Notification_Widgets[0], 0);
        lv_obj_set_y(ui_Notification_Widgets[0], (((notificationid - 1) * 60) - 50));
        notificationid += 1;
      }
    }
    else
    {
      Serial.print(F(" reached limit without newline '"));
      Serial.print(input);
      Serial.println("'");
    }
    input = ""; // clear after processing for next line
  }

  if (songtime < (millis() - 70000))
  {
    songtime = millis();
    lv_label_set_text(ui_Now_Playing_Label, "");
  }

  if (SerialBT.connected())
  {
    lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_bluetooth_small_png);
  }
  else
  {
    lv_img_set_src(ui_Bluetooth_Indicator, &ui_img_no_bluetooth_small_png);
  }
}

void Powerhandle()
{
  twatch->power_updata(millis(), 1000);
  if ((!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000) and charging == 0)
    lastpercent = 0;
  if ((digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() < 4000) and charging == 1)
    lastpercent = 100;
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000)
  {
    lv_obj_set_style_text_color(ui_Battery_Percentage, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    charging = 1;
  }
  else
  {
    lv_obj_set_style_text_color(ui_Battery_Percentage, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    charging = 0;
  }

  if ((lastpercent > twatch->power_get_percent() and charging == 0) or (lastpercent < twatch->power_get_percent() and charging == 1))
  {
    char percentchar[] = "179&";
    sprintf(percentchar, "%.0f%%", (twatch->power_get_percent()));
    lv_label_set_text(ui_Battery_Percentage, percentchar);
    lv_arc_set_value(ui_Arc_Battery, (twatch->power_get_percent() / 100) * 250);
    lastpercent = twatch->power_get_percent();
  }
}

void PowerOff(lv_event_t *e)
{

  twatch->backlight_set_value(0);
  digitalWrite(TWATCH_PWR_ON, LOW);
  esp_deep_sleep_start();

  /*twatch->bma423_feature(BMA423_WRIST_WEAR | BMA423_SINGLE_TAP | BMA423_DOUBLE_TAP | BMA423_STEP_CNTR, true);
  twatch->bma423_feature(BMA423_WRIST_WEAR_INT | BMA423_STEP_CNTR_INT | BMA423_SINGLE_TAP_INT | BMA423_DOUBLE_TAP_INT, true);
  twatch->bma423_acc_feature(true);
  twatch->bma423_feature_int(BMA423_SINGLE_TAP_INT | BMA423_DOUBLE_TAP_INT | BMA423_WRIST_WEAR_INT, true);

  twatch->hal_sleep(false);*/
}

void Brightness(lv_event_t *e)
{
  if (lv_slider_get_value(ui_Brightness_Slider) < 1)
    lv_slider_set_value(ui_Brightness_Slider, 1, LV_ANIM_OFF);
  short int Brightness = lv_slider_get_value(ui_Brightness_Slider);
  twatch->backlight_set_value(Brightness);
  Serial.println(Brightness);
  // Serial.println(twatch->backlight_get_value);
}

void ToggleDoNotDisturb(lv_event_t *e)
{
  /*if (!Donotdisturb)
  {
    Donotdisturb = 1;
    // lv_obj_set_style_bg_color(ui_Do_Not_Disturb_Button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  if (Donotdisturb)
  {
    Donotdisturb = 0;
    // lv_obj_set_style_bg_color(ui_Do_Not_Disturb_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
  }*/

  Donotdisturb = !Donotdisturb;
  // erial.println(Donotdisturb);
}

void ToggleBT(lv_event_t *e)
{
  if (!BTon)
  {
    BTon = 1;
    SerialBT.begin("Garrett's Watch"); // Bluetooth device name
  }
  if (BTon)
  {
    BTon = 0;
    SerialBT.disconnect();
  }
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
  Serial.println("Timer0");
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
    {
      Storage.putInt("StepDay", GetDay());
      Storage.putBool("StepReach", 0);
    }
  }

  int GetStep = twatch->bma423_get_step();

  if (LastSteps != GetStep)
  {
    LastSteps = GetStep;
    Steps = GetStep + StepOffset;
    sprintf(StepChar, "%i Steps", Steps);
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
  Storage.putInt("StepGoal", StepGoal);

  NotificationLength = atoi(lv_textarea_get_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));
  Storage.putInt("NotifLength", NotificationLength);

  Storage.putBytes("BTname", lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)), 17);
  // Serial.println(lv_textarea_get_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL)));
}