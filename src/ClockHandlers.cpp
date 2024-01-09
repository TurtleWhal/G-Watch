#include "ui.h"
#include "lvgl.h"
#include "screen_management.h"
#include "ClockHandlers.h"
#include "timestuff.h"
#include "ArduinoLog.h"

#include <cstdarg>

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

            if (info.bt.isconnected)
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
                lv_arc_set_value(ui_Default_Clock_Arc_Battery, info.battery.percentage);
                if (info.battery.ischarging)
                    lv_obj_set_style_text_color(ui_Default_Clock_Battery_Percentage, lv_color_hex(0x00FF00), LV_PART_MAIN);
                else
                    lv_obj_set_style_text_color(ui_Default_Clock_Battery_Percentage, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

                if (info.schedule.currenttime)
                {
                    lv_arc_set_range(ui_Default_Clock_Arc_Right, info.schedule.starttime, info.schedule.endtime);
                    lv_arc_set_value(ui_Default_Clock_Arc_Right, info.schedule.currenttime);
                    lv_obj_set_style_arc_color(ui_Default_Clock_Arc_Right, lv_color_hex(0x00DFFF), LV_PART_INDICATOR);
                }
                else
                {
                    lv_obj_remove_local_style_prop(ui_Default_Clock_Arc_Right, LV_STYLE_ARC_COLOR, LV_PART_INDICATOR);
                    lv_arc_set_range(ui_Default_Clock_Arc_Right, 0, info.health.stepgoal);
                    lv_arc_set_value(ui_Default_Clock_Arc_Right, info.health.steps);
                }

                if (info.flag.hourchanged or info.flag.refresh)
                {
                    lv_label_set_text(ui_Default_Clock_Date, info.time.date.c_str());
                    lv_label_set_text(ui_Default_Clock_Date_Numerical, info.time.numdate.c_str());
                }
            }

            if (info.music.nowplayingtimer <= 80)
            {
                if (info.OTA.useOTA)
                    lv_label_set_text_fmt(ui_Default_Clock_Now_Playing_Label, "OTA IP: %s", info.OTA.ip);

                if (info.music.nowplayingtimer < 2)
                    lv_label_set_text(ui_Default_Clock_Now_Playing_Label, info.music.nowplaying.c_str());
                info.music.nowplayingtimer++;
            }
            else if (info.music.nowplayingtimer >= 80)
            {
                lv_label_set_text(ui_Default_Clock_Now_Playing_Label, "");
                if (info.OTA.useOTA)
                    lv_label_set_text_fmt(ui_Default_Clock_Now_Playing_Label, "OTA IP: %s", info.OTA.ip);
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
        if (info.flag.secondchanged or info.flag.refresh)
        {
            lv_img_set_angle(ui_Simplistic_Hour_Hand, (info.time.hour * 300) + (info.time.minute * 5));
            lv_img_set_angle(ui_Simplistic_Hour_Hand_Shadow, (info.time.hour * 300) + (info.time.minute * 5));

            if (info.flag.minutechanged or info.flag.refresh)
            {
                lv_img_set_angle(ui_Simplistic_Minute_Hand, (info.time.minute * 60) + (info.time.second));
                lv_img_set_angle(ui_Simplistic_Minute_Hand_Shadow, (info.time.minute * 60) + (info.time.second));

                if (info.flag.hourchanged or info.flag.refresh)
                {
                }
            }
        }
    }
}

void SkeletonWatchFaceHandle()
{
    if (lv_scr_act() == ui_SkeletonWatchFace)
    {

        if (info.flag.secondchanged or info.flag.refresh)
        {
            lv_img_set_angle(ui_Skeleton_Watch_Face_Minute_Hand, (info.time.hour * 300) + (info.time.minute * 5));
            lv_img_set_angle(ui_Skeleton_Watch_Face_Minute_Hand_Shadow, (info.time.hour * 300) + (info.time.minute * 5));

            if (info.flag.minutechanged or info.flag.refresh)
            {
                lv_img_set_angle(ui_Skeleton_Watch_Face_Hour_Hand, (info.time.minute * 60) + (info.time.second));
                lv_img_set_angle(ui_Skeleton_Watch_Face_Hour_Hand_Shadow, (info.time.minute * 60) + (info.time.second));

                if (info.flag.hourchanged or info.flag.refresh)
                {
                }
            }
        }
    }
}

void WriteBlockyText(const char *text, uint8_t arg_count, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, arg_count);

    for (uint8_t i = 0; i < arg_count; i++)
    {
        lv_label_set_text(va_arg(arg_ptr, lv_obj_t *), text);
    }
}

void BlockyClockHandle()
{
    if (lv_scr_act() == ui_Blocky_Clock)
    {

        if (info.flag.secondchanged or info.flag.refresh)
        {

            if (info.flag.minutechanged or info.flag.refresh)
            {
                char timechar[] = "12:00";
                sprintf(timechar, "%i:%02i", info.time.hour12, info.time.minute);
                WriteBlockyText(timechar, 6,
                                ui_Blocky_Clock_Clock_Layer_1,
                                ui_Blocky_Clock_Clock_Layer_2,
                                ui_Blocky_Clock_Clock_Layer_3,
                                ui_Blocky_Clock_Clock_Layer_4,
                                ui_Blocky_Clock_Clock_Layer_5,
                                ui_Blocky_Clock_Clock_Layer_6);

                WriteBlockyText(info.time.numdate.c_str(), 2, ui_Blocky_Clock_Date_Layer_1, ui_Blocky_Clock_Date_Layer_2);

                lv_bar_set_value(ui_Blocky_Clock_Battery_Bar, info.battery.percentage, LV_ANIM_OFF);
                if (info.battery.ischarging)
                {
                    lv_obj_set_style_bg_color(ui_Blocky_Clock_Battery_Bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
                }
                else
                {
                    lv_obj_set_style_bg_color(ui_Blocky_Clock_Battery_Bar, info.theme.color, LV_PART_INDICATOR);
                }

                if (info.flag.hourchanged or info.flag.refresh)
                {

                    info.flag.refresh = 0;
                }
            }
        }
    }
}