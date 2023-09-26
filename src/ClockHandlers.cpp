#include "ui.h"
#include "lvgl.h"
#include "screen_management.h"
#include "ClockHandlers.h"

extern ClockInfo info;

void DefaultClockHandle()
{
    if (lv_scr_act() == ui_Default_Clock)
    {
        lv_img_set_angle(ui_Default_Clock_Hour_Hand, (info.time.hour * 300) + (info.time.minute * 5));
        lv_img_set_angle(ui_Default_Clock_Minute_Hand, (info.time.minute * 60) + (info.time.second));
        lv_img_set_angle(ui_Default_Clock_Minute_Hand, (info.time.valsec * 60) + (info.time.valusec * 0.00006));
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