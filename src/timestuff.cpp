#include "lvgl.h"
#include <TWatch_hal.h>
#include "ui.h"
#include "timestuff.h"
#include "time.h"
#include "ArduinoLog.h"
#include "screen_management.h"

struct tm t_tm;
struct timeval val;
lv_obj_t *tick_index[62];
extern TWatchClass *twatch;

extern ClockInfo info;

// PCF8563_Class *rtc = twatch->rtc_get_instance();

void UpdateTime()
{
    static uint8_t lastsec = 61;
    static uint8_t lastmin = 61;
    static uint8_t lasthour = 61;
    static int lastsecangle;

    info.flag.secondchanged = 0;
    info.flag.minutechanged = 0;
    info.flag.hourchanged = 0;

    //RTC_Date date = twatch->rtc_get_instance()->getDateTime();

    //Log.verboseln("RTC time: %i:%i:%i", date.hour, date.minute, date.second);

    char time[11];

    gettimeofday(&val, NULL);
    getLocalTime(&t_tm);

    info.time.valsec = val.tv_sec % 60;
    info.time.valusec = val.tv_usec;

    info.time.hour = t_tm.tm_hour;
    info.time.hour12 = t_tm.tm_hour;
    if (info.time.hour12 > 12)
        info.time.hour12 -= 12;
    info.time.minute = t_tm.tm_min;
    info.time.second = t_tm.tm_sec;

    if (info.time.second != lastsec)
    {
        Log.verboseln("WriteTime sec");
        lastsec = info.time.second;
        info.flag.secondchanged = 1;

        if (t_tm.tm_min != lastmin)
        {
            Log.verboseln("WriteTime min");
            lastmin = t_tm.tm_min;
            info.flag.minutechanged = 1;

            if (t_tm.tm_hour != lasthour)
            {
                Log.verboseln("WriteTime hour");
                lasthour = t_tm.tm_hour;
                info.flag.hourchanged = 1;
                strftime(time, sizeof(time), "%a %b %e", &t_tm);
                info.time.date = time;
                strftime(time, sizeof(time), "%D", &t_tm);
                info.time.numdate = time;
            }
        }
    }
}

int GetDayOfYear()
{
    getLocalTime(&t_tm);
    return (t_tm.tm_yday);
}

int GetDayOfWeek()
{
    getLocalTime(&t_tm);
    return (t_tm.tm_wday);
}

void InitTicks()
{
    Log.verboseln("InitTicks");
    for (int i = 0; i < 62; i++)
    {
        tick_index[i] = lv_img_create(ui_Default_Clock);

        if (i % 5) // Minor second ticks
        {
            lv_img_set_src(tick_index[i], &ui_img_seconddotstub_png);
            lv_obj_set_y(tick_index[i], -93);
            lv_obj_set_style_img_recolor(tick_index[i], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        else // Major second dashes
        {
            lv_img_set_src(tick_index[i], &ui_img_seconddashstub_png);
            lv_obj_set_y(tick_index[i], -91);

            if (info.theme.darkmode or 1)
                lv_obj_set_style_img_recolor(tick_index[i], lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
            else
                lv_obj_set_style_img_recolor(tick_index[i], lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        lv_obj_set_x(tick_index[i], 0);
        lv_obj_set_align(tick_index[i], LV_ALIGN_CENTER);
        lv_img_set_pivot(tick_index[i], 2, 95);
        lv_img_set_angle(tick_index[i], i * 60);
        lv_obj_set_style_img_recolor_opa(tick_index[i], 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_move_background(tick_index[i]);
    }
    lv_obj_set_style_img_recolor(tick_index[60], lv_color_hex(0xFFD600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(tick_index[61], lv_color_hex(0xFFD600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_swap(tick_index[0], tick_index[60]);
    lv_obj_swap(tick_index[1], tick_index[61]);
}

void ColorTicks(int i)
{

    if (i % 5) // Minor second ticks
    {
        lv_obj_set_x(tick_index[60], 600);
        lv_obj_set_x(tick_index[61], 0);
        lv_img_set_angle(tick_index[61], i * 60);
    }
    else // Major second dashes
    {
        lv_obj_set_x(tick_index[61], 600);
        lv_obj_set_x(tick_index[60], 0);
        lv_img_set_angle(tick_index[60], i * 60);
    }
}

void SetTime(ulong newtime, short timezone)
{
    newtime = newtime + (timezone * 60 * 60);
    gettimeofday(&val, NULL);
    val.tv_sec = newtime;
    val.tv_usec = 0;
    settimeofday(&val, NULL);
    gettimeofday(&val, NULL);
    getLocalTime(&t_tm);
    twatch->rtc_set_time(t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec);
}