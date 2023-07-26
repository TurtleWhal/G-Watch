#include "steps.h"
#include "Preferences.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "timestuff.h"
#include "notifications.h"

extern Preferences Storage;
extern TWatchClass *twatch;
int Steps;

void StepHandle()
{
  static int StepDay;
  static int LastSteps = -1;
  static int StepOffset = -1;

  if (StepOffset == -1)
  {
    if (Storage.getUInt("StepDay") == GetDay())
      StepOffset = Storage.getUInt("Steps");
    else
      StepOffset = 0;
  }

  int GetStep = twatch->bma423_get_step();

  if (LastSteps != GetStep)
  {
    LastSteps = GetStep;
    Steps = GetStep + StepOffset;
#ifdef UPDATE_ELEMENTS
    lv_label_set_text_fmt(ui_Step_Counter_Text, "%i", Steps);
    lv_arc_set_value(ui_Arc_Right, ((float)Steps / Storage.getUInt("StepGoal")) * 250);
#endif

    if (Steps >= Storage.getUInt("StepGoal") and !Storage.getBool("StepReach"))
    {
      // #ifdef UPDATE_ELEMENTS
      //       lv_label_set_text(ui_Notification_Title, "Step Goal Reached!");
      //       lv_label_set_text_fmt(ui_Notification_Text, "You have reached your step goal of %i Steps!", Storage.getUInt("StepGoal"));
      // #endif
      char *StepNotif;
      sprintf(StepNotif, "You have reached your step goal of %i Steps!", Storage.getUInt("StepGoal"));
      shownotification("Step Goal Reached!", StepNotif, "local.stephandle", 0, 0);
      Storage.putBool("StepReach", 1);
    }

    Storage.putUInt("Steps", Steps);
    StepDay = GetDay();
    Storage.putUInt("StepsDay", StepDay);
  }

  if (Storage.getUInt("StepDay") != GetDay())
  {
    Storage.putUInt("StepDay", GetDay());
    Storage.putBool("StepReach", 0);
    StepOffset = 0;
    Storage.putUInt("Steps", 0);
    twatch->bma423_step_reset();
  }
}

int getSteps()
{
  return Steps;
}