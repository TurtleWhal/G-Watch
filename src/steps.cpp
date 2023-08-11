#include "steps.h"
#include "Preferences.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timestuff.h"
#include "notifications.h"
#include "ArduinoJson.h"
#include "BThandle.h"
#include "ArduinoLog.h"

void BTsendSteps();

extern Preferences Storage;
extern TWatchClass *twatch;
int Steps;

void StepHandle()
{
  static uint16_t StepDay;
  static uint16_t LastSteps = UINT16_MAX;
  static uint16_t StepOffset = UINT16_MAX;
  int GetStep = twatch->bma423_get_step();

  Log.verboseln("BMA423: %i", GetStep);
  Log.verboseln("Storage: %i", Storage.getUShort("Steps"));

  if (StepOffset == UINT16_MAX)
  {
    //twatch->bma423_reset();
    if (Storage.getUShort("StepDay") == GetDay())
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

    DrawSteps();

    if (Steps >= Storage.getUShort("StepGoal") and !Storage.getBool("StepReach"))
    {
      // #ifdef UPDATE_ELEMENTS
      //       lv_label_set_text(ui_Notification_Title, "Step Goal Reached!");
      //       lv_label_set_text_fmt(ui_Notification_Text, "You have reached your step goal of %i Steps!", Storage.getUInt("StepGoal"));
      // #endif
      char StepNotif[] = "You have reached your step goal of 4294967295 Steps!";
      sprintf(StepNotif, "You have reached your step goal of %i Steps!", Storage.getUShort("StepGoal"));
      shownotification("Step Goal Reached!", StepNotif, "local.stephandle", 0, 0);
      Storage.putBool("StepReach", 1);
    }

    Storage.putUShort("Steps", Steps);
    StepDay = GetDay();
    Storage.putUShort("StepsDay", StepDay);

    BTsendSteps();
  }

  if (Storage.getUShort("StepDay") != GetDay())
  {
    Storage.putUShort("StepDay", GetDay());
    Storage.putBool("StepReach", 0);
    StepOffset = 0;
    Storage.putUShort("Steps", 0);
    twatch->bma423_step_reset();
  }
}

int getSteps()
{
  return Steps;
}

void DrawSteps()
{
#ifdef UPDATE_ELEMENTS
  lv_label_set_text_fmt(ui_Step_Counter_Text, "%i", Steps);
  lv_arc_set_value(ui_Arc_Right, ((float)Steps / Storage.getUShort("StepGoal")) * 250);
#endif
}

void BTsendSteps()
{
  static int laststep = getSteps();
  String buffer;
  // t:"act", hrm:int, stp:int, time:int
  StaticJsonDocument<200> actinfo;
  actinfo["t"] = "act";
  // actinfo["stp"] = Storage.getInt("Steps");
  actinfo["stp"] = getSteps() - laststep;
  laststep = getSteps();
  serializeJson(actinfo, buffer);
  BTsend(buffer);
}