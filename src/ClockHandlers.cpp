#include "ui.h"
#include "lvgl.h"
#include "screen_management.h"
#include "ClockHandlers.h"
#include "timestuff.h"
#include "ArduinoLog.h"

extern ClockInfo info;

LV_IMG_DECLARE(ui_img_bluetooth_small_png);

/*
void [ScreenName]Handle()
{
    if (lv_scr_act() == [screen])
    {

        if (info.flag.secondchanged or info.flag.refresh)
        {

            if (info.flag.minutechanged or info.flag.refresh)
            {

                if (info.flag.hourchanged or info.flag.refresh)
                {

                }
            }
        }
    }
}
*/

void DefaultClockHandle()
{
    if (lv_scr_act() == ui_Default_Clock)
    {
        lv_img_set_angle(ui_Default_Clock_Second_Hand, (info.time.valsec * 60) + (info.time.valusec * 0.00006));

        if (info.flag.secondchanged or info.flag.refresh)
        {
            ColorTicks(info.time.valsec);
            lv_img_set_angle(ui_Default_Clock_Minute_Hand, (info.time.minute * 60) + (info.time.second));
            lv_label_set_text_fmt(ui_Default_Clock_Notification_Amount_Number, "%i", info.notification.count);

            if (info.bt.status)
            {
                if (lv_img_get_src(ui_Default_Clock_Bluetooth_Indicator) != &ui_img_bluetooth_small_png)
                    lv_img_set_src(ui_Default_Clock_Bluetooth_Indicator, &ui_img_bluetooth_small_png);
            }
            else
            {
                if (lv_img_get_src(ui_Default_Clock_Bluetooth_Indicator) != &ui_img_no_bluetooth_small_png)
                    lv_img_set_src(ui_Default_Clock_Bluetooth_Indicator, &ui_img_no_bluetooth_small_png);
            }

            if (info.flag.minutechanged or info.flag.refresh)
            {
                lv_label_set_text_fmt(ui_Default_Clock_Time, "%02i:%02i", info.time.hour12, info.time.minute);
                lv_img_set_angle(ui_Default_Clock_Hour_Hand, (info.time.hour * 300) + (info.time.minute * 5));

                lv_label_set_text_fmt(ui_Default_Clock_Step_Counter_Text, "%i", info.health.steps);
                lv_label_set_text_fmt(ui_Default_Clock_Battery_Percentage, "%i%%", info.battery.percentage);

                if (info.flag.hourchanged or info.flag.refresh)
                {
                    lv_label_set_text(ui_Default_Clock_Date, info.time.date.c_str());
                    lv_label_set_text(ui_Default_Clock_Date_Numerical, info.time.numdate.c_str());
                }
            }
        }
        if (info.flag.refresh)
            info.flag.refresh = 0;
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