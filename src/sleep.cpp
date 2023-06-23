#include <TWatch_hal.h>

#include "lvgl.h"
#include "ui.h"

#include "time.h"
#include "sleep.h"

extern TWatchClass *twatch;

int Sleeptimeout = 10; // time until watch goes to sleep in seconds
int sleeptimer;
bool Sleeping;
int bl;
String Wakeup_reason;
// bool setCpuFrequencyMhz(uint32_t cpu_freq_mhz);

void Sleephandle()
{
  if (digitalRead(TWATCH_CHARGING) and twatch->power_get_volt() < 4000)
  {
    if (twatch->touch_check())
    {
      Wakeup("Screen Touched");
      sleeptimer = millis();
      Serial.println("Im Touched!");
    }
    if ((millis() - sleeptimer) >= Sleeptimeout * 1000)
      Sleep();
  }
  else
  {
    Ticklesleep();
    if (Sleeping)
      Wakeup("Plugged In");
  }
}

void Wakeup(String Wakeup_reason)
{
  if (Sleeping)
  {
    // setCpuFrequencyMhz(240);
    _ui_screen_change(ui_Clock, LV_SCR_LOAD_ANIM_NONE, 150, 0);
    lv_timer_handler();
    writetime();
    Serial.println("IM AWAKE!");
    sleeptimer = millis();
    Sleeping = 0;
    twatch->backlight_set_value(lv_slider_get_value(ui_Brightness_Slider));
    Serial.println(Wakeup_reason);
  }
  else Ticklesleep();
}

void Sleep()
{
  if (!Sleeping)
  {
    twatch->backlight_set_value(0);
    Serial.println("Go To Sleep");
    Sleeping = 1;
    // setCpuFrequencyMhz(160);
  }
}

void Ticklesleep(){
sleeptimer = millis();
}

bool isSleeping(){
return Sleeping;
}