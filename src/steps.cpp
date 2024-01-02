#include "steps.h"
#include "Preferences.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timestuff.h"
#include "notifications.h"
#include "ArduinoJson.h"
#include "BThandle.h"
#include "ArduinoLog.h"
#include "schedule.h"
#include "screen_management.h"

void BTSendSteps();

extern Preferences Storage;
extern TWatchClass *twatch;
ushort Steps;
extern ClockInfo info;

lv_coord_t StepArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void StepHandle()
{
  static uint16_t StepDay;
  static uint16_t LastSteps = UINT16_MAX;
  static uint16_t StepOffset = UINT16_MAX;
  int GetStep = twatch->bma423_get_step();

  // Log.verboseln("BMA423: %i", GetStep);
  // Log.verboseln("Storage: %i", Storage.getUShort("Steps"));

  if (StepOffset == UINT16_MAX)
  {
    // twatch->bma423_reset();
    if (Storage.getUShort("StepDay") == GetDayOfYear())
    {
      StepOffset = Storage.getUShort("Steps");
    }
    else
      StepOffset = 0;
  }

  if (LastSteps != GetStep)
  {
    LastSteps = GetStep;
    Steps = GetStep + StepOffset;

    if (Steps >= Storage.getUShort("StepGoal") and !Storage.getBool("StepReach"))
    {
      // #ifdef UPDATE_ELEMENTS
      //       lv_label_set_text(ui_Notification_Title, "Step Goal Reached!");
      //       lv_label_set_text_fmt(ui_Notification_Text, "You have reached your step goal of %i Steps!", Storage.getUInt("StepGoal"));
      // #endif
      char StepNotif[] = "You have reached your step goal of 4294967295 Steps!";
      sprintf(StepNotif, "You have reached your step goal of %i Steps!", Storage.getUShort("StepGoal"));
      ShowNotification("Step Goal Reached!", StepNotif, "local.stephandle", 0);
      Storage.putBool("StepReach", 1);
    }

    Storage.putUShort("Steps", Steps);
    StepDay = GetDayOfYear();
    Storage.putUShort("StepsDay", StepDay);

    info.health.steps = Steps;

    BTSendSteps();
  }

  if (Storage.getUShort("StepDay") != GetDayOfYear())
  {
    Storage.putUShort("StepDay", GetDayOfYear());
    Storage.putBool("StepReach", 0);
    StepOffset = 0;
    Storage.putUShort("Steps", 0);
    twatch->bma423_step_reset();
  }
}

void BTSendSteps()
{
  static int laststep = info.health.steps;
  /*String buffer;
  // t:"act", hrm:int, stp:int, time:int
  StaticJsonDocument<200> actinfo;
  actinfo["t"] = "act";
  actinfo["stp"] = info.health.steps - laststep;
  laststep = info.health.steps;
  serializeJson(actinfo, buffer);
  BTsend(buffer);
  StepArray[24] = info.health.steps;*/

  BTsendf("{t:\"act\", stp:%i}", info.health.steps - laststep);
}

void InitStepsScreen(lv_event_t *e)
{
  Storage.getBytes("StepArray", &StepArray, sizeof(StepArray));
  ushort TempGoal = Storage.getUShort("StepGoal");

  lv_label_set_text_fmt(ui_Steps_Info, "Steps: %i\nGoal: %i", info.health.steps, TempGoal);
  lv_label_set_text_fmt(ui_Reset_Storage_Label, "Storage\n%i", Storage.getUShort("Steps"));
  lv_label_set_text_fmt(ui_Reset_Counter_Label, "Counter\n%i", twatch->bma423_get_step());
  lv_bar_set_range(ui_Steps_Bar, 0, TempGoal);
  lv_bar_set_value(ui_Steps_Bar, info.health.steps, LV_ANIM_OFF);
  lv_chart_set_range(ui_Steps_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, TempGoal);
  WriteStepGraph();
}

void ResetStorage(lv_event_t *e)
{
  Storage.putUShort("Steps", 0);
}

void ResetCounter(lv_event_t *e)
{
  twatch->bma423_step_reset();
}

void WriteStepGraph()
{
  lv_chart_set_ext_y_array(ui_Steps_Chart, lv_chart_get_series_next(ui_Steps_Chart, NULL), StepArray);
}

void AdvanceStepArray()
{
  StepArray[24] = info.health.steps;
  for (int i = 0; i < 24; i++)
  {
    StepArray[i] = StepArray[i + 1];
  }
  StepArray[24] = info.health.steps;

  Storage.putBytes("StepArray", &StepArray, sizeof(StepArray));
}