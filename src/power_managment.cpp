#include <TWatch_hal.h>
#include "lvgl.h"
#include "ui.h"
#include "timestuff.h"
#include "power_managment.h"
#include "screen_management.h"
#include "ArduinoLog.h"

extern TWatchClass *twatch;

// #define SleepWhenPluggedIn
#define Sleeptimeout 10000 // time until watch goes to sleep in mS
int sleeptimer;
bool Sleeping;
int prevbrightness = 100;
String Wakeup_reason;
bool charging;
int lastpercent;
int Brightness = 100;
// bool setCpuFrequencyMhz(uint32_t cpu_freq_mhz);

void Sleephandle()
{
#ifdef SleepWhenPluggedIn
  if (1)
#else
  if (digitalRead(TWATCH_CHARGING) and twatch->power_get_volt() < 4000) // TWATCH_CHARGING is inverted logic
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
    // lv_timer_handler();
    WriteTime();
    Log.verboseln("IM AWAKE!");
    // dad hid this comment here because I'm like that.
    // A few moments later...
    // Garrett found this comment because dad didn't go to a different line
    sleeptimer = millis();
    Sleeping = 0;
    twatch->backlight_set_value(prevbrightness);
    // twatch->backlight_gradual_light(prevbrightness, 1000);
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

void Powerhandle()
{
  twatch->power_updata(millis(), 1000);
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000)
  {
    if (!charging)
    {
      charging = 1;
      DrawPower();
    }
  }
  else
  {
    if (charging)
    {
      charging = 0;
      DrawPower();
    }
  }
}

void InitPercent()
{
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000)
    lastpercent = 0;
  else
    lastpercent = 100;
}

void DrawPower()
{
  char percentchar[] = "179&";
  sprintf(percentchar, "%.0f%%", (twatch->power_get_percent()));
#ifdef UPDATE_ELEMENTS
  lv_label_set_text(ui_Battery_Percentage, percentchar);
  lv_arc_set_value(ui_Arc_Battery, (twatch->power_get_percent() / 100) * 250);
#endif
  lastpercent = twatch->power_get_percent();

#ifdef UPDATE_ELEMENTS
  if (charging)
    lv_obj_set_style_text_color(ui_Battery_Percentage, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  else
    lv_obj_set_style_text_color(ui_Battery_Percentage, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
#endif
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

void UpdateBrightness(lv_event_t *e)
{
  if (lv_slider_get_value(ui_Brightness_Slider) < 1)
    lv_slider_set_value(ui_Brightness_Slider, 1, LV_ANIM_OFF);
  Brightness = lv_slider_get_value(ui_Brightness_Slider);
  twatch->backlight_set_value(Brightness);
  Log.verboseln("Brightness: %i", Brightness);
  // Serial.println(twatch->backlight_get_value);
}
