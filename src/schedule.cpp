#include "schedule.h"
#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"
#include "timestuff.h"

#define SCHEDULE_ENTRIES 10

int ScheduleTimes[SCHEDULE_ENTRIES][2] = {
    {750, 820},
    {750, 820},
    {750, 820},
    {750, 820},
    {750, 820},
    {1200, 1700},
    {0, 2400},
    {750, 820},
    {750, 820},
    {830, 900}};

String ScheduleNames[SCHEDULE_ENTRIES] = {
    {"Period 1"},
    {"Period 2"},
    {"Period 3"},
    {"Period 4"},
    {"Period 5"},
    {"Period 6"},
    {"Period 7"},
    {"Period 8"},
    {"Period 9"},
    {"Period 10"}};

void DrawSchedule();

void ScheduleHandle()
{
    if (lv_scr_act() == ui_Schedule)
    {
        // DrawSchedule();
    }
}

void DrawSchedule(lv_event_t *e)
{
    int numtime = (GetHour() * 100) + GetMinute();
    int entry;
    String options = "";

    Serial.println(numtime);
    for (int i = 0; i < SCHEDULE_ENTRIES; i++)
    {
        if (ScheduleTimes[i][2] < numtime and ScheduleTimes[i][1] > numtime)
        {
            entry = i;
            Serial.print("Entry : ");
        }
        else
            Serial.print("LoopED: ");
        Serial.print(i);
        Serial.print(" : ");
        Serial.print(ScheduleTimes[i][0]);
        Serial.print(" : ");
        Serial.print(ScheduleTimes[i][1]);
        Serial.print(" : ");
        Serial.println(ScheduleNames[i]);
        options = options + ScheduleNames[i] + "\n";
    }
    // Serial.println(options);
    Serial.println(entry);
    lv_roller_set_options(ui_Schedule_Roller, options.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_Schedule_Roller, entry - 1, LV_ANIM_OFF);

    if (ScheduleTimes[entry][0] > 12)
    lv_label_set_text_fmt(ui_Schedule_Time_Start, "%i:%i", (ScheduleTimes[entry][0] - 1200) / 100, (ScheduleTimes[entry][0] - 1200) % 100);
    else
    lv_label_set_text_fmt(ui_Schedule_Time_End, "%i:%i", (ScheduleTimes[entry][0]) / 100, (ScheduleTimes[entry][0]) % 100);
    
    if (ScheduleTimes[entry][1] > 12)
    lv_label_set_text_fmt(ui_Schedule_Time_Start, "%i:%i", (ScheduleTimes[entry][1] - 1200) / 100, (ScheduleTimes[entry][1] - 1200) % 100);
    else
    lv_label_set_text_fmt(ui_Schedule_Time_End, "%i:%i", (ScheduleTimes[entry][1]) / 100, (ScheduleTimes[entry][1]) % 100);

    lv_bar_set_range(ui_Schedule_Bar, ScheduleTimes[entry][0], ScheduleTimes[entry][1]);
    lv_bar_set_value(ui_Schedule_Bar, numtime, LV_ANIM_OFF);
}