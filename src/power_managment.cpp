#include "TWatch_hal.h"
#include "lvgl.h"
#include "ui.h"
#include "timestuff.h"
#include "power_managment.h"
#include "screen_management.h"
#include "ArduinoLog.h"
#include "esp_pm.h"
#include "ArduinoJson.h"
#include "BThandle.h"
#include "steps.h"

#define SLEEP_WHEN_CHARGING

extern TWatchClass *twatch;
esp_pm_config_esp32_t pm_config;
extern ClockInfo info;

#define Sleeptimeout 30000 // time until watch goes to sleep in milliseconds
int sleeptimer;
bool Sleeping;
bool dimSleeping;
int prevbrightness = 100;
String Wakeup_reason;
bool charging;
int Brightness = 100;

void Sleephandle()
{
#ifdef SLEEP_WHEN_CHARGING
  if (1)
#else
  if (digitalRead(TWATCH_CHARGING) and twatch->power_get_volt() < 4000) // TWATCH_CHARGING is inverted logic
#endif
  {

    if (!Sleeping)
    {

      if ((millis() - sleeptimer) >= Sleeptimeout)
      {
        Sleep();
      }

      if ((millis() - sleeptimer) >= Sleeptimeout * 0.6)
      {
        if (!dimSleeping)
        {
          prevbrightness = twatch->backlight_get_value();
          if (!prevbrightness)
            prevbrightness = 1;

          twatch->backlight_gradual_light(prevbrightness * 0.3, 500);
          dimSleeping = true;
          twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, true);
        }
      }
      else
      {
        if (dimSleeping)
        {
          twatch->backlight_gradual_light(prevbrightness, 500);
          dimSleeping = false;
          twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, false);
        }
      }

    }
  }
  else
  {
    TickleSleep();
    if (Sleeping)
      Wakeup("Plugged In");
    // Log.verboseln(BMA423_WRIST_WEAR);
  }
}

void Wakeup(String Wakeup_reason)
{
  if (Sleeping)
  {
    // setCpuFrequencyMhz(240);
    //  twatch->hal_auto_update(true, 1);

    UpdateTime();

    UpdatePower();

    StepHandle();

    Log.verboseln("IM AWAKE!");
    //  dad hid this comment here because I'm like that.
    //  A few moments later...
    //  Garrett found this comment because dad didn't go to a different line
    sleeptimer = millis();
    Sleeping = 0;

    // disable the wrist tilt interrupt to not reset timer while awake
    twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, false);

    info.flag.refresh = 1;

    twatch->backlight_gradual_light(prevbrightness, 100);

    Log.verboseln("Wakeup Reason: %s", Wakeup_reason.c_str());
  }
  else
    TickleSleep();

  FullSpeed();
}

void Sleep()
{
  if (!Sleeping)
  {

    twatch->backlight_gradual_light(0, 1000);

    // enable the wrist tilt interrupt
    twatch->bma423_feature_int(BMA423_WRIST_WEAR_INT, true);

    Log.verboseln("Go To Sleep");
    Sleeping = 1;
    SleepSpeed();
    // twatch->hal_auto_update(false, 1);
  }
}

void TickleSleep()
{
  sleeptimer = millis();
}

bool IsSleeping()
{
  return Sleeping;
}

void Powerhandle()
{
  twatch->power_updata(millis(), 1000);
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000) // is charging
  {
    if (!charging) // plugged in
    {
      charging = 1;
      info.battery.ischarging = 1;
      UpdatePower(true);
      info.flag.refresh = 1;

      if (millis() > 4000)
      {
        twatch->motor_shake(1, 30);
        Wakeup("Plugged In");
      }
    }
  }
  else
  {
    if (charging) // unplugged
    {
      charging = 0;
      info.battery.ischarging = 0;
      UpdatePower(true);
      info.flag.refresh = 1;
    }

    if ((twatch->power_get_percent() < 20) and (millis() > 10000))
    { // turn on power saving mode

      Log.verboseln("Power Saving Mode Acitvated");
      BT_off();                                                 // turn off bluetooth
      twatch->backlight_gradual_light(prevbrightness / 2, 500); // lower brightness

      if (twatch->power_get_volt() < 3000) // turn off to protect the battery
      {
        Log.verboseln("Powering off due to low battery");
        PowerOff(nullptr);
      }
    }
  }
}

/*void InitPercent()
{
  if (!digitalRead(TWATCH_CHARGING) || twatch->power_get_volt() > 4000)
    lastpercent = 0;
  else
    lastpercent = 100;
}*/

void UpdatePower(bool force)
{
  static uint8_t lastpercent = 200;
  if (lastpercent != twatch->power_get_percent() or force)
  {

    info.battery.percentage = twatch->power_get_percent();
    info.battery.voltage = twatch->power_get_volt();
    lastpercent = twatch->power_get_percent();

    BTsendpower();
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

void UpdateBrightness(lv_event_t *e)
{
  if (lv_slider_get_value(ui_Brightness_Slider) < 1)
    lv_slider_set_value(ui_Brightness_Slider, 1, LV_ANIM_OFF);
  Brightness = lv_slider_get_value(ui_Brightness_Slider);
  twatch->backlight_set_value(Brightness);
  Log.verboseln("Brightness: %i", Brightness);
  // Log.verboseln(twatch->backlight_get_value);
}

int GetUserBrightness()
{
  return Brightness;
}

void FullSpeed()
{
  pm_config.max_freq_mhz = 240;
  pm_config.min_freq_mhz = 240;
  pm_config.light_sleep_enable = true;
  ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
  setCpuFrequencyMhz(240);
}

void SleepSpeed()
{
  pm_config.max_freq_mhz = 80;
  pm_config.min_freq_mhz = 40;
  pm_config.light_sleep_enable = true;
  ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
  // log_d("custom arduino-esp32 framework detected, enable PM/DFS support, %d/%dMHz %s light sleep (%d)", pm_config.max_freq_mhz, pm_config.min_freq_mhz, lighsleep ? "without" : "with", lighsleep);
  setCpuFrequencyMhz(80);
  // esp_light_sleep_start();
  // esp_light_sleep_start();
}

int GetSleepTimer()
{
  return sleeptimer;
}

void BTsendpower()
{
  /*String buffer;
  StaticJsonDocument<200> batinfo;
  batinfo["t"] = "status";
  batinfo["bat"] = twatch->power_get_percent();
  // batinfo["volt"] = (float)twatch->power_get_volt()/1000;
  batinfo["chg"] = info.battery.ischarging ? 1 : 0;
  serializeJson(batinfo, buffer);
  BTsend(buffer);*/

  BTsendf("{t:\"status\", bat:%i, chg:%i, volt:%f}", twatch->power_get_percent(), info.battery.ischarging ? 1 : 0, twatch->power_get_volt() / 1000);
}