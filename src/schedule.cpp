#include "schedule.h"
#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"
#include "timestuff.h"
#include "screen_management.h"

extern ClockInfo info;

struct Schedule
{
    // Number of Schedule times
    uint8_t Entries;
    // [Start Time], [End Time]
    uint16_t Times[16][2];
    // [Event Name], [Parenthesis enclosed optional modifier]
    String Names[16][2];
};

Schedule ODay = {.Entries = 5, .Times = {{750, 930}, {940, 1015}, {1030, 1210}, {1210, 1245}, {1255, 1445}}, .Names = {{"Band", "T121"}, {"HiHo", "209"}, {"Algebra II", "307"}, {"Lunch", ""}, {"French", "229"}}};

Schedule EDay = {.Entries = 5, .Times = {{750, 930}, {940, 1015}, {1030, 1210}, {1210, 1245}, {1255, 1445}}, .Names = {{"Robotics", "180"}, {"HiHo", "209"}, {"English", "351"}, {"Lunch", ""}, {"Biology", "313"}}};

Schedule ADay = {.Entries = 7, .Times = {{750, 825}, {830, 905}, {925, 1010}, {1020, 1055}, {1105, 1130}, {1140, 1215}, {1220, 1255}}, .Names = {{"Band", "T121"}, {"Robotics", "180"}, {"Algebra II", "307"}, {"English", "351"}, {"Lunch", ""}, {"French", "229"}, {"Biology", "313"}}};
Schedule None;
//               Schedule for sun,   mon,   tue,   wed,   thu,   fri,   sat
Schedule *WeeklySchedule[] = {&None, &ODay, &EDay, &ADay, &ODay, &EDay, &None};

Schedule CurrentSchedule;

uint16_t StartTime = 0;
uint16_t EndTime = 2400;
uint8_t entry = UINT8_MAX;
String options = "";
bool passing;
uint16_t numtime;

bool HasScheduleEvent()
{
    if (StartTime == UINT16_MAX)
        return 0;
    return 1;
}

void ScheduleHandle()
{
    CurrentSchedule = *WeeklySchedule[GetDayOfWeek()];
    StartTime = 0;
    EndTime = 2400;
    entry = UINT8_MAX;
    options = "";
    passing = 0;
    numtime = 0;

    numtime = (info.time.hour * 100) + info.time.minute;
    ////Serial.print("hour: ");
    ////Serial.println(info.time.hour);
    ////Serial.print("minute: ");
    ////Serial.println(info.time.minute);
    numtime = (info.time.hour * 100) + info.time.minute;
    // Serial.print("hour: ");
    // Serial.println(GetHour());
    // Serial.print("minute: ");
    // Serial.println(GetMinute());

    ////Serial.print("Time: ");
    ////Serial.println(numtime);

    for (int i = 0; i < CurrentSchedule.Entries; i++)
    {
        if (CurrentSchedule.Times[i][0] <= numtime and CurrentSchedule.Times[i][1] > numtime)
        {
            entry = i;
            passing = 0;
            ////Serial.print("Entry : ");
            ////Serial.println(entry);
        }
        /*else
           //Serial.print("LoopED: ");
        //Serial.print(i);
        //Serial.print(" : ");
        //Serial.print(CurrentSchedule.Times[i][0]);
        //Serial.print(" : ");
        //Serial.print(CurrentSchedule.Times[i][1]);
        //Serial.print(" : ");
        //Serial.println(CurrentSchedule.Names[i]);*/

        if (CurrentSchedule.Names[i][1] != "")
            options = options + CurrentSchedule.Names[i][0] + " (" + CurrentSchedule.Names[i][1] + ")" + "\n";
        else
            options = options + CurrentSchedule.Names[i][0] + "\n";
    }

    if (entry == UINT8_MAX)
    {
        for (int i = 0; i < CurrentSchedule.Entries - 1; i++)
        {
            if (CurrentSchedule.Times[i][1] <= numtime and CurrentSchedule.Times[i + 1][0] >= numtime)
            {
                entry = i;
                passing = 1;
                ////Serial.print("Passed: ");
                ////Serial.println(i);
            }
            /*else
                //Serial.print("LoppEd: ");
            //Serial.print(i);
            //Serial.print(" : ");
            //Serial.print(CurrentSchedule.Times[i][1]);
            //Serial.print(" : ");
            //Serial.println(CurrentSchedule.Times[i + 1][0]);*/
        }
    }
    // //Serial.println(options);
    ////Serial.println(entry);
    if (entry != UINT8_MAX)
    {

        if (!passing)
        {
            int temptime = CurrentSchedule.Times[entry][0];

            if (temptime > 1259)
                temptime -= 1200;
            // Serial.println(temptime);
            StartTime = temptime;

            temptime = CurrentSchedule.Times[entry][1];

            if (temptime > 1259)
                temptime -= 1200;
            // Serial.println(temptime);
            EndTime = temptime;
        }
        else
        {
            int temptime = CurrentSchedule.Times[entry][1];

            if (temptime > 1259)
                temptime -= 1200;
            // Serial.println(temptime);

            StartTime = temptime;

            temptime = CurrentSchedule.Times[entry + 1][0];

            if (temptime > 1259)
                temptime -= 1200;
            // Serial.println(temptime);
            EndTime = temptime;
        }

        info.schedule.currenttime = numtime;
        info.schedule.starttime = StartTime;
        info.schedule.endtime = EndTime;
    }
    else
    {
        StartTime = UINT16_MAX;
        info.schedule.currenttime = NULL;
        info.schedule.starttime = NULL;
        info.schedule.endtime = NULL;
    }

    // Serial.print("StartTime = ");
    // Serial.println(StartTime);

    // Serial.print("EndTime = ");
    // Serial.println(EndTime);

    // Serial.print("Entry = ");
    // Serial.println(entry);

    if (lv_scr_act() == ui_Schedule)
    {
        DrawSchedule(nullptr);
    }

    // lv_arc_set_range(ui_Default_Clock_Arc_Right, StartTime, EndTime);
    // lv_arc_set_value(ui_Default_Clock_Arc_Right, numtime);
}

void DrawSchedule(lv_event_t *e)
{
    lv_label_set_text_fmt(ui_Schedule_Time_Start, "%i:%02i", StartTime / 100, StartTime % 100);
    lv_label_set_text_fmt(ui_Schedule_Time_End, "%i:%02i", EndTime / 100, EndTime % 100);

    lv_bar_set_range(ui_Schedule_Bar, StartTime, EndTime);
    lv_bar_set_value(ui_Schedule_Bar, numtime, LV_ANIM_ON);

    lv_roller_set_options(ui_Schedule_Roller, options.c_str(), LV_ROLLER_MODE_NORMAL);

    if (!passing)
    {
        if (entry != UINT8_MAX)
        {
            lv_roller_set_selected(ui_Schedule_Roller, entry, LV_ANIM_OFF);
#ifdef SCHEDULE_SHOWINFO
            lv_label_set_text(ui_Schedule_Name, (CurrentSchedule.Names[entry][0] + " (" + CurrentSchedule.Names[entry][1] + ")").c_str());
#else
            lv_label_set_text(ui_Schedule_Name, (CurrentSchedule.Names[entry][0]).c_str());
#endif
        }
    }
    else
    {
        lv_roller_set_selected(ui_Schedule_Roller, entry + 1, LV_ANIM_OFF);
        lv_label_set_text(ui_Schedule_Name, "Passing Period");
    }
    if (entry == UINT8_MAX)
    {
        info.schedule.currenttime = NULL;
        info.schedule.starttime = NULL;
        info.schedule.endtime = NULL;

        lv_roller_set_options(ui_Schedule_Roller, "----------", LV_ROLLER_MODE_NORMAL);
        lv_label_set_text(ui_Schedule_Name, "Nothing Scheduled");
        lv_label_set_text(ui_Schedule_Time_Start, "N/A");
        lv_label_set_text(ui_Schedule_Time_End, "N/A");
    }
    else
    {
        info.schedule.currenttime = numtime;
        info.schedule.starttime = StartTime;
        info.schedule.endtime = EndTime;
    }
}