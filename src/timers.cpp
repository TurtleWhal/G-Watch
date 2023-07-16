#include "lvgl.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timers.h"

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

extern TWatchClass *twatch;

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
  lv_label_set_text(ui_Stopwatch_Milliseconds, "00");
  lv_label_set_text(ui_Stopwatch_Seconds, "00");
  lv_label_set_text(ui_Stopwatch_Minutes, "00");
  lv_label_set_text(ui_Stopwatch_Hours, "00");
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

void timeradjust(lv_event_t *e)
{
lv_obj_t * target = lv_event_get_target(e);
if (target == ui_Timer_Hour_Plus_10)
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
    timertime -= 1000;

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
      lv_label_set_text(ui_Alarm_Going_Off_Time, "00:00:00");
      lv_label_set_text(ui_Alarm_Going_Off_Name, "Timer");
      //_ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
      lv_scr_load_anim(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
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
  timerhours %= 100;

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

void processstopwatch()
{
  if (stopwatchmoving)
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

  if (timermoving)
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