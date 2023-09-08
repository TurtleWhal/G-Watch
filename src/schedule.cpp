#include "schedule.h"
#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"
#include "timestuff.h"

/*#define SCHEDULE_ENTRIES 10

int ScheduleTimes[SCHEDULE_ENTRIES][2] = {
    {750, 820},
    {750, 820},
    {750, 2115},
    {2150, 2200},
    {750, 820},
    {1200, 1700},
    {845, 1955},
    {2000, 2007},
    {2008, 2010},
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
    {"Period 10"}};*/

#define SCHEDULE_ENTRIES 7

int ScheduleTimes[SCHEDULE_ENTRIES][2] = {
    {750, 840},
    {850, 940},
    {1000, 1050},
    {1110, 1200},
    {1200, 1245},
    {1245, 1335},
    {1345, 1435}};

String ScheduleNames[SCHEDULE_ENTRIES] = {
    {"Band (T121)"},
    {"Robotics (180)"},
    {"Algebra (307)"},
    {"English (351)"},
    {"Lunch"},
    {"French (229)"},
    {"Biology (344)"}};

// void DrawSchedule();

void ScheduleHandle()
{
    if (lv_scr_act() == ui_Schedule)
    {
        DrawSchedule(nullptr);
    }
}

void DrawSchedule(lv_event_t *e)
{
    int numtime = (GetHour() * 100) + GetMinute();
    bool passing;
    Serial.println(GetHour());
    Serial.println(GetMinute());
    uint8_t entry = UINT8_MAX;
    String options = "";

    Serial.println(numtime);
    for (int i = 0; i < SCHEDULE_ENTRIES; i++)
    {
        if (ScheduleTimes[i][0] <= numtime and ScheduleTimes[i][1] >= numtime)
        {
            entry = i;
            passing = 1;
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
    Serial.println(entry);
    if (entry == UINT8_MAX)
    {
        for (int i = 0; i < SCHEDULE_ENTRIES - 1; i++)
        {
            if (ScheduleTimes[i][1] <= numtime and ScheduleTimes[i + 1][0] >= numtime)
            {
                entry = i;
                passing = 1;
                Serial.print("Passed: ");
            }
            else
                Serial.print("LoppEd: ");
            Serial.print(i);
            Serial.print(" : ");
            Serial.print(ScheduleTimes[i][1]);
            Serial.print(" : ");
            Serial.println(ScheduleTimes[i + 1][0]);
        }
    }
    // Serial.println(options);
    Serial.println(entry);
    if (entry != UINT8_MAX)
    {
        lv_roller_set_options(ui_Schedule_Roller, options.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(ui_Schedule_Roller, entry - 1, LV_ANIM_OFF);

        if (!passing)
        {
            int temptime = ScheduleTimes[entry][0];

            if (temptime > 1259)
                temptime -= 1200;
            Serial.println(temptime);

            lv_label_set_text_fmt(ui_Schedule_Time_Start, "%i:%02i", temptime / 100, temptime % 100);

            temptime = ScheduleTimes[entry][1];

            if (temptime > 1259)
                temptime -= 1200;
            Serial.println(temptime);

            lv_label_set_text_fmt(ui_Schedule_Time_End, "%i:%02i", temptime / 100, temptime % 100);

            lv_bar_set_range(ui_Schedule_Bar, ScheduleTimes[entry][0], ScheduleTimes[entry][1]);
            lv_bar_set_value(ui_Schedule_Bar, numtime, LV_ANIM_OFF);

            lv_label_set_text(ui_Schedule_Name, (ScheduleNames[entry]).c_str());
        }
        else
        {
            int temptime = ScheduleTimes[entry][1];

            if (temptime > 1259)
                temptime -= 1200;
            Serial.println(temptime);
            lv_label_set_text_fmt(ui_Schedule_Time_Start, "%i:%02i", temptime / 100, temptime % 100);

            temptime = ScheduleTimes[entry + 1][0];

            if (temptime > 1259)
                temptime -= 1200;
            Serial.println(temptime);

            lv_label_set_text_fmt(ui_Schedule_Time_End, "%i:%02i", temptime / 100, temptime % 100);

            lv_bar_set_range(ui_Schedule_Bar, ScheduleTimes[entry][1], ScheduleTimes[entry + 1][0]);
            lv_bar_set_value(ui_Schedule_Bar, numtime, LV_ANIM_OFF);

            lv_label_set_text(ui_Schedule_Name, "Passing Period");
        }
    }
    else
    {
        lv_label_set_text(ui_Schedule_Name, "Nothing Scheduled");
        lv_bar_set_value(ui_Schedule_Bar, 0, LV_ANIM_OFF);
        lv_roller_set_options(ui_Schedule_Roller, "----------", LV_ROLLER_MODE_NORMAL);
        lv_label_set_text(ui_Schedule_Time_Start, "N/A");
        lv_label_set_text(ui_Schedule_Time_End, "N/A");
    }
}