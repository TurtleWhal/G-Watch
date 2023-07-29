#include "lvgl.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timers.h"
#include "power_managment.h"

char timertimechar[3];
int stopwatchtime = 0;
int stopwatchtimestarted = 0;
int stopwatchtimestopped = 0;
bool stopwatchmoving = 0;
char stopwatchtimechar[3];
char zerobuffer[3];

int timertime;
bool timermoving;
int timerlasttime;

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
  stopwatchtime = 0;
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
  lv_obj_t *target = lv_event_get_target(e);
  /*if (target == ui_Timer_Hour_Plus_10)
      timertime += 10 * 60 * 60 * 1000;
  else if (target == ui_Timer_Hour_Minus_10)
      timertime -= 10 * 60 * 60 * 1000;
  else if (target == ui_Timer_Hour_Plus_1)
      timertime += 60 * 60 * 1000;
  else if (target == ui_Timer_Hour_Minus_1)
      timertime -= 60 * 60 * 1000;
  else if (target == ui_Timer_Minute_Plus_10)
      timertime += 10 * 60 * 1000;
  else if (target == ui_Timer_Minute_Minus_10)
      timertime -= 10 * 60 * 1000;
  else if (target == ui_Timer_Minute_Plus_1)
      timertime += 60 * 1000;
  else if (target == ui_Timer_Minute_Minus_1)
      timertime -= 60 * 1000;
  else if (target == ui_Timer_Second_Plus_10)
      timertime += 10 * 1000;
  else if (target == ui_Timer_Second_Minus_10)
      timertime -= 10 * 1000;
  else if (target == ui_Timer_Second_Plus_1)
      timertime += 1000;
  else if (target == ui_Timer_Second_Minus_1)
      timertime -= 1000;*/

  timertime = ((lv_roller_get_selected(ui_Timer_Hour_Left_Roller) * 60 * 60 * 10) +
               (lv_roller_get_selected(ui_Timer_Hour_Right_Roller) * 60 * 60) +

               (lv_roller_get_selected(ui_Timer_Minute_Left_Roller) * 60 * 10) +
               (lv_roller_get_selected(ui_Timer_Minute_Right_Roller) * 60) +

               (lv_roller_get_selected(ui_Timer_Second_Left_Roller) * 10) +
               (lv_roller_get_selected(ui_Timer_Second_Right_Roller))) *
              1000;

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
  uint8_t timerseconds = timertime / 1000;
  uint8_t timerminutes = timerseconds / 60;
  uint8_t timerhours = timerminutes / 60;
  timerseconds %= 60;
  timerminutes %= 60;
  timerhours %= 100;

  lv_roller_set_selected(ui_Timer_Second_Right_Roller, timerseconds % 10, LV_ANIM_ON);
  lv_roller_set_selected(ui_Timer_Second_Left_Roller, timerseconds / 10, LV_ANIM_ON);

  lv_roller_set_selected(ui_Timer_Minute_Right_Roller, timerminutes % 10, LV_ANIM_ON);
  lv_roller_set_selected(ui_Timer_Minute_Left_Roller, timerminutes / 10, LV_ANIM_ON);

  lv_roller_set_selected(ui_Timer_Hour_Right_Roller, timerhours % 10, LV_ANIM_ON);
  lv_roller_set_selected(ui_Timer_Hour_Left_Roller, timerhours / 10, LV_ANIM_ON);

  /*if (timerseconds < 10)
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
  lv_label_set_text(ui_Timer_Hours, timertimechar);*/
}

void StopwatchHandle()
{
  if (lv_scr_act() == ui_Stopwatch and stopwatchmoving)
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
  stopwatchtime = (millis() - stopwatchtimestarted);
  uint8_t stopwatchmilliseconds = stopwatchtime / 10;
  uint8_t stopwatchseconds = stopwatchtime / 1000;
  uint8_t stopwatchminutes = stopwatchseconds / 60;
  uint8_t stopwatchhours = stopwatchminutes / 60;
  stopwatchmilliseconds %= 100;
  stopwatchseconds %= 60;
  stopwatchminutes %= 60;
  stopwatchhours %= 99;

  static uint8_t lastmilliseconds;
  static uint8_t lastseconds;
  static uint8_t lastminutes;
  static uint8_t lasthours;

  if (lastmilliseconds != stopwatchmilliseconds)
  {
    lv_label_set_text_fmt(ui_Stopwatch_Milliseconds, "%02d", stopwatchmilliseconds);
    lastmilliseconds = stopwatchmilliseconds;
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
  // lv_label_set_text_fmt(ui_Stopwatch_Time_Long, "%02d:%02d:%02d.%02d", stopwatchhours, stopwatchminutes, stopwatchseconds, stopwatchmilliseconds);
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