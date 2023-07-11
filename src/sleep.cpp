#include <TWatch_hal.h>
#include "lvgl.h"
#include "ui.h"
#include "timestuff.h"
#include "sleep.h"
#include "screen_management.h"
#include "ArduinoLog.h"

extern TWatchClass *twatch;

//#define SleepWhenPluggedIn
#define Sleeptimeout 10000 // time until watch goes to sleep in mS
int sleeptimer;
bool Sleeping;
int prevbrightness = 100;
String Wakeup_reason;
// bool setCpuFrequencyMhz(uint32_t cpu_freq_mhz);

void Sleephandle()
{
  #ifdef SleepWhenPluggedIn
    if (1)
  #else
    if (digitalRead(TWATCH_CHARGING) and twatch->power_get_volt() < 4000) //TWATCH_CHARGING is inverted logic
  #endif
  {  
    /*
    twatch->bma423_feature_int(BMA423_WRIST_WEAR, 1);
    if (BMA423_WRIST_WEAR)
    {
      Wakeup("Shook");
      sleeptimer = millis();
      Serial.println("Im Shooken!");
    }
    */
    if ((millis() - sleeptimer) >= Sleeptimeout)
      Sleep();
  }
  else
  {
    Ticklesleep();
    if (Sleeping)
      Wakeup("Plugged In");
    // Serial.println(BMA423_WRIST_WEAR);
  }
}

void Wakeup(String Wakeup_reason)
{
  if (Sleeping)
  {
    // setCpuFrequencyMhz(240);
    //_ui_screen_change(ui_Clock, LV_SCR_LOAD_ANIM_NONE, 150, 0);
    generictoclock(nullptr);
    //lv_timer_handler();
    WriteTime();
    Log.verboseln("IM AWAKE!");
    // dad hid this comment here because I'm like that.
    // A few moments later...
    // Garrett found this comment because dad didn't go to a different line
    sleeptimer = millis();
    Sleeping = 0;
    twatch->backlight_set_value(prevbrightness);
    //twatch->backlight_gradual_light(prevbrightness, 1000);
    Log.verboseln("Wakeup Reason: %s", Wakeup_reason);
  }
  else
    Ticklesleep();
}

void Sleep()
{
  if (!Sleeping)
  {
    prevbrightness = twatch->backlight_get_value();
    twatch->backlight_set_value(0);
    Serial.println("Go To Sleep");
    Sleeping = 1;
    // setCpuFrequencyMhz(160);
  }
}

void Ticklesleep()
{
  sleeptimer = millis();
}

bool isSleeping()
{
  return Sleeping;
}