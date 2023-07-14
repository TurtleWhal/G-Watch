#include "lvgl.h"
#include "ui.h"
#include "alarms.h"
#include "Arduino.h"

typedef struct
{
  bool state;
  bool am=1;
  uint8_t hour;
  uint8_t minute;
  char *hourtext;
  char *minutetext;
  const char *name;
} Alarm;

Alarm alarms[4];
int selectedalarm = 0;

void setalarm1(lv_event_t *e)
{
  selectedalarm = 0;
  if (alarms[0].am)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm2(lv_event_t *e)
{
  selectedalarm = 1;
  if (alarms[1].am)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm3(lv_event_t *e)
{
  selectedalarm = 2;
  if (alarms[2].am)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}
void setalarm4(lv_event_t *e)
{
  selectedalarm = 3;
  if (alarms[3].am)
  {
    _ui_state_modify(ui_AMPM_Label, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
  }
  else
  {
    lv_obj_add_state(ui_AMPM_Label, LV_STATE_CHECKED);
  }
}

void setalarm(lv_event_t *e)
{
  lv_label_set_text(ui_Alarm_1_Name, "Alarm Set");
  char temptime[6];
  lv_roller_get_selected_str(ui_Hour_Roller, temptime, 3);
  alarms[selectedalarm].hourtext = temptime;
  if (!alarms[selectedalarm].am)
  {
    alarms[selectedalarm].hour = atoi(temptime) + 12;
  }
  else
  {
    alarms[selectedalarm].hour = atoi(temptime);
  }

  lv_roller_get_selected_str(ui_Minute_Roller, temptime, 3);
  alarms[selectedalarm].minutetext = temptime;
  alarms[selectedalarm].minute = atoi(temptime);

  // if (alarms[0].hour<10) alarms[0].minutetext = "0"+temptime;
  if (alarms[selectedalarm].am)
  {
    itoa(alarms[selectedalarm].hour, temptime, 10);
  }
  else
  {
    itoa(alarms[selectedalarm].hour - 12, temptime, 10);
  }

  if (selectedalarm == 0)
    lv_label_set_text(ui_Alarm_1_Hour, temptime);
  if (selectedalarm)
    lv_label_set_text(ui_Alarm_2_Hour, temptime);
  if (selectedalarm == 2)
    lv_label_set_text(ui_Alarm_3_Hour, temptime);
  if (selectedalarm == 3)
    lv_label_set_text(ui_Alarm_4_Hour, temptime);

  if (alarms[selectedalarm].minute < 10)
  {
    sprintf(temptime, "0%d", alarms[selectedalarm].minute);
  }
  else
  {
    itoa(alarms[selectedalarm].minute, temptime, 10);
  }
  if (selectedalarm == 0)
  {
    lv_label_set_text(ui_Alarm_1_Minute, alarms[selectedalarm].minutetext);
    alarms[0].state = 1;
    lv_obj_add_state(ui_Alarm_1_Switch, LV_STATE_CHECKED);
    if (alarms[0].am)
    {
      lv_label_set_text(ui_Alarm_1_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_1_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_1_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[0].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm)
  {
    lv_label_set_text(ui_Alarm_2_Minute, alarms[selectedalarm].minutetext);
    alarms[1].state = 1;
    lv_obj_add_state(ui_Alarm_2_Switch, LV_STATE_CHECKED);
    if (alarms[1].am)
    {
      lv_label_set_text(ui_Alarm_2_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_2_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_2_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[1].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm == 2)
  {
    lv_label_set_text(ui_Alarm_3_Minute, alarms[selectedalarm].minutetext);
    alarms[2].state = 1;
    lv_obj_add_state(ui_Alarm_3_Switch, LV_STATE_CHECKED);
    if (alarms[2].am)
    {
      lv_label_set_text(ui_Alarm_3_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_3_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_3_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[2].name = lv_textarea_get_text(ui_Alarm_Name);
  }
  if (selectedalarm == 3)
  {
    lv_label_set_text(ui_Alarm_4_Minute, alarms[selectedalarm].minutetext);
    alarms[3].state = 1;
    lv_obj_add_state(ui_Alarm_4_Switch, LV_STATE_CHECKED);
    if (alarms[3].am)
    {
      lv_label_set_text(ui_Alarm_4_AMPM, "AM");
    }
    else
    {
      lv_label_set_text(ui_Alarm_4_AMPM, "PM");
    }
    lv_label_set_text(ui_Alarm_4_Name, lv_textarea_get_text(ui_Alarm_Name));
    alarms[3].name = lv_textarea_get_text(ui_Alarm_Name);
  }
}

void togglealarm1(lv_event_t *e)
{
  if (!alarms[0].state)
  {
    alarms[0].state = 1;
  }
  else
  {
    alarms[0].state = 0;
  }
}
void togglealarm2(lv_event_t *e)
{
  if (!alarms[1].state)
  {
    alarms[1].state = 1;
  }
  else
  {
    alarms[1].state = 0;
  }
}
void togglealarm3(lv_event_t *e)
{
  if (!alarms[2].state)
  {
    alarms[2].state = 1;
  }
  else
  {
    alarms[2].state = 0;
  }
}
void togglealarm4(lv_event_t *e)
{
  if (!alarms[3].state)
  {
    alarms[3].state = 1;
  }
  else
  {
    alarms[3].state = 0;
  }
}

void toggleampm(lv_event_t *e)
{
  if (alarms[selectedalarm].am)
  {
    alarms[selectedalarm].am = 0;
  }
  else
  {
    alarms[selectedalarm].am = 1;
  }
}

/*void alarmhandle()
{
  char alarmscreentime[6];
  static uint8_t alarmlastmin;
  if (rtc.minute > alarmlastmin || rtc.minute == 0 & alarmlastmin == 59)
  {
    alarmlastmin = rtc.minute;

    if (alarms[0].state)
    {
      rtc.read();
      if (alarms[0].hour == rtc.hour)
      {
        if (alarms[0].minute == rtc.minute)
        {
          Serial.println("alarm 1 triggered");
          alarms[0].state = 0;
          _ui_state_modify(ui_Alarm_1_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[0].hour) - 12, alarms[0].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[0].hour, alarms[0].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[0].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[1].state)
    {
      rtc.read();
      if (alarms[1].hour == rtc.hour)
      {
        if (alarms[1].minute == rtc.minute)
        {
          Serial.println("alarm 2 triggered");
          alarms[1].state = 0;
          _ui_state_modify(ui_Alarm_2_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[1].hour) - 12, alarms[1].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[1].hour, alarms[1].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[1].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[2].state)
    {
      rtc.read();
      if (alarms[2].hour == rtc.hour)
      {
        if (alarms[2].minute == rtc.minute)
        {
          Serial.println("alarm 3 triggered");
          alarms[2].state = 0;
          _ui_state_modify(ui_Alarm_3_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
          lv_img_set_src(ui_Notification_Image, &ui_img_bell_png);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[2].hour) - 12, alarms[2].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[2].hour, alarms[2].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[2].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    if (alarms[3].state)
    {
      rtc.read();
      if (alarms[3].hour == rtc.hour)
      {
        if (alarms[3].minute == rtc.minute)
        {
          Serial.println("alarm 1 triggered");
          alarms[3].state = 0;
          _ui_state_modify(ui_Alarm_4_Switch, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);

          if (alarms[0].am == 0)
          {
            sprintf(alarmscreentime, "%i:%i", (alarms[3].hour) - 12, alarms[3].minute);
          }
          else
          {
            sprintf(alarmscreentime, "%i:%i", alarms[3].hour, alarms[3].minute);
          }
          lv_label_set_text(ui_Alarm_Going_Off_Time, alarmscreentime);
          lv_label_set_text(ui_Alarm_Going_Off_Name, alarms[3].name);
          _ui_screen_change(ui_Alarm_Going_Off, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0);
          // VIBRATION MOTOR GO BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        }
      }
    }
    Serial.println("Alarms Handled");
  }
}*/
