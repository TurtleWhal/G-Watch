#include "lvgl.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timers.h"
#include "power_managment.h"

int stopwatchtimems = 0;
int stopwatchtimestarted = 0;
int stopwatchtimestopped = 0;
bool stopwatchmoving = 0;
char stopwatchtimechar[3];
char zerobuffer[3];

int timertime;
bool timermoving;
int timerlasttime;
char timertimechar[3];

int vibrate;

extern TWatchClass *twatch;

void DrawStopwatch();

void Vibrate();
void VibrateStart(int);

void ToggleStopwatch(lv_event_t *e)
{
  if (!stopwatchmoving)
  {
    stopwatchmoving = 1;
    if (!stopwatchtimestarted)
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
  stopwatchtimems = 0;
  lv_label_set_text(ui_Stopwatch_Milliseconds, "00");
  lv_label_set_text(ui_Stopwatch_Seconds, "00");
  lv_label_set_text(ui_Stopwatch_Minutes, "00");
  lv_label_set_text(ui_Stopwatch_Hours, "00");
  // DrawStopwatch();
  // lv_label_set_text(ui_Stopwatch_Time_Long, "00:00:00.00");
  twatch->motor_shake(1, 50);
}

void ToggleTimer(lv_event_t *e)
{
  if (!timermoving)
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

void InitTimer(lv_event_t *e)
{
  writetimertime();
  if (timermoving)
  {
    lv_obj_set_y(ui_Timer_Play_Pause_Image, 34);
  }
}

void timeradjust(lv_event_t *e)
{
  timertime = ((lv_roller_get_selected(ui_Timer_Hour_Left_Roller) * 60 * 60 * 10) +
               (lv_roller_get_selected(ui_Timer_Hour_Right_Roller) * 60 * 60) +

               (lv_roller_get_selected(ui_Timer_Minute_Left_Roller) * 60 * 10) +
               (lv_roller_get_selected(ui_Timer_Minute_Right_Roller) * 60) +

               (lv_roller_get_selected(ui_Timer_Second_Left_Roller) * 10) +
               (lv_roller_get_selected(ui_Timer_Second_Right_Roller))) *
                  1000 +
              500;

  Serial.println(timertime);

  istimernegative();
  writetimertime();
}

void istimernegative()
{
  if (timertime <= 0)
  {
    timertime = 0;
    if (timermoving)
    {
      timermoving = 0;
      // ToggleTimer(nullptr);
      _ui_screen_change(&ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Alarm_Going_Off_screen_init);
      lv_label_set_text(ui_Alarm_Going_Off_Time, "00:00:00");
      lv_label_set_text(ui_Alarm_Going_Off_Name, "Timer");
      Wakeup("Timer");
      VibrateStart(100); // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    }
  }
}

void writetimertime()
{
  if (ui_Timers != NULL)
  {
    uint temptime = (timertime / 1000);
    lv_roller_set_selected(ui_Timer_Second_Left_Roller, (temptime % 60) / 10, LV_ANIM_ON);
    lv_roller_set_selected(ui_Timer_Second_Right_Roller, (temptime % 60) % 10, LV_ANIM_ON);
    temptime = temptime / 60;
    lv_roller_set_selected(ui_Timer_Minute_Left_Roller, (temptime % 60) / 10, LV_ANIM_ON);
    lv_roller_set_selected(ui_Timer_Minute_Right_Roller, (temptime % 60) % 10, LV_ANIM_ON);
    temptime = temptime / 60;
    lv_roller_set_selected(ui_Timer_Hour_Left_Roller, (temptime % 60) / 10, LV_ANIM_ON);
    lv_roller_set_selected(ui_Timer_Hour_Right_Roller, (temptime % 60) % 10, LV_ANIM_ON);
  }
}

void TimersHandle()
{
  if (ui_Stopwatch != NULL and stopwatchmoving)
  {
    DrawStopwatch();
  }

  if (timermoving)
  {
    static int lasttime = millis();
    if (millis() > timerlasttime)
    {
      timertime -= (millis() - timerlasttime);
      timerlasttime = millis();
    }
    if (lasttime < millis() - 500)
    {
      Serial.println(timertime);
      writetimertime();
      istimernegative();
      lasttime = millis();
    }
  }
}

void DrawStopwatch()
{
  stopwatchtimems = (millis() - stopwatchtimestarted);
  
  int stopwatchhundredths = stopwatchtimems / 10;
  uint16_t stopwatchseconds = stopwatchtimems / 1000;
  uint8_t stopwatchminutes = stopwatchseconds / 60;
  uint8_t stopwatchhours = stopwatchminutes / 60;

  stopwatchhundredths %= 100;
  stopwatchseconds %= 60;
  stopwatchminutes %= 60;
  stopwatchhours %= 99;

  static uint8_t lastmilliseconds;
  static uint8_t lastseconds;
  static uint8_t lastminutes;
  static uint8_t lasthours;

  if (lastmilliseconds != stopwatchhundredths)
  {
    lv_label_set_text_fmt(ui_Stopwatch_Milliseconds, "%02d", stopwatchhundredths);
    lastmilliseconds = stopwatchhundredths;
  }

  if (lastseconds != stopwatchseconds)
  {
    lv_label_set_text_fmt(ui_Stopwatch_Seconds, "%02d", stopwatchseconds);
    lastseconds = stopwatchseconds;
  }

  if (lastminutes != stopwatchminutes)
  {
    lv_label_set_text_fmt(ui_Stopwatch_Minutes, "%02d", stopwatchminutes);
    lastminutes = stopwatchminutes;
  }

  if (lasthours != stopwatchhours)
  {
    lv_label_set_text_fmt(ui_Stopwatch_Hours, "%02d", stopwatchhours);
    lasthours = stopwatchhours;
  }
  // lv_label_set_text_fmt(ui_Stopwatch_Time_Long, "%02d:%02d:%02d.%02d", stopwatchhours, stopwatchminutes, stopwatchseconds, stopwatchhundredths);
}

void VibrateHandle()
{
  static int lasttime;
  if (lasttime < millis() - 4000)
  {
    lasttime = millis();
    Vibrate();
  }
}

void Vibrate()
{
  if (vibrate)
  {
    twatch->motor_shake(2, vibrate);
    delay(800);
    twatch->motor_shake(2, vibrate);
  }
}

void VibrateStart(int Strength)
{
  vibrate = Strength;
}

void VibrateStop(lv_event_t *e)
{
  vibrate = 0;
}