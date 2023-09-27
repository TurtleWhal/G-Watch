#include "ui.h"
#include "lvgl.h"
#include "screen_management.h"
#include "ClockHandlers.h"
#include "timestuff.h"
#include "ArduinoLog.h"

extern ClockInfo info;

void DefaultClockHandle()
{
    if (lv_scr_act() == ui_Default_Clock)
    {
        lv_img_set_angle(ui_Default_Clock_Hour_Hand, (info.time.hour * 300) + (info.time.minute * 5));
        lv_img_set_angle(ui_Default_Clock_Minute_Hand, (info.time.minute * 60) + (info.time.second));
        lv_img_set_angle(ui_Default_Clock_Second_Hand, (info.time.valsec * 60) + (info.time.valusec * 0.00006));
        ColorTicks(info.time.valsec);

        lv_label_set_text_fmt(ui_Default_Clock_Time, "%i:%i", info.time.hour % 12, info.time.minute);
        
        lv_label_set_text(ui_Default_Clock_Date, info.time.date.c_str());
        lv_label_set_text(ui_Default_Clock_Date_Numerical, info.time.numdate.c_str());

        lv_label_set_text_fmt(ui_Default_Clock_Step_Counter_Text, "%i", info.health.steps);

        lv_label_set_text_fmt(ui_Default_Clock_Battery_Percentage, "%i", info.battery.percentage);

        lv_label_set_text_fmt(ui_Default_Clock_Notification_Amount_Number, "%i", info.notification.count);
    }
}

void SimplisticWatchFaceHandle()
{
    if (lv_scr_act() == ui_SimplisticWatchFace)
    {
        lv_img_set_angle(ui_Simplistic_Hour_Hand, (info.time.hour * 300) + (info.time.minute * 5));
        lv_img_set_angle(ui_Simplistic_Hour_Hand_Shadow, (info.time.hour * 300) + (info.time.minute * 5));

        lv_img_set_angle(ui_Simplistic_Minute_Hand, (info.time.minute * 60) + (info.time.second));
        lv_img_set_angle(ui_Simplistic_Minute_Hand_Shadow, (info.time.minute * 60) + (info.time.second));
    }
}