#include "lvgl.h"
#include <TWatch_hal.h>
#include "ui.h"
#include "timestuff.h"
#include "time.h"
#include "ArduinoLog.h"

struct tm t_tm;
struct timeval val;
lv_obj_t *tick_index[62];
extern TWatchClass *twatch;

void WriteTime()
{
    static uint8_t lastsec = 61;
    static uint8_t lastmin = 61;
    static uint8_t lasthour = 61;
    static int lastsecangle;

    char time[11];

    gettimeofday(&val, NULL);
    int second = (val.tv_sec % 60);
    int secangle = (val.tv_usec / 100000);

#ifdef UPDATE_ELEMENTS
    if (secangle != lastsecangle)
        lv_img_set_angle(ui_Second_Hand, secangle * 6 + second * 60); // Rounds uSec to 0-12, changes to 0,5,10,15,20.., then adds seconds
    lastsecangle = secangle;
#endif

    if (second != lastsec)
    {
        Log.verboseln("Writetime sec");
        lastsec = second;
        getLocalTime(&t_tm);
#ifdef UPDATE_ELEMENTS
        ColorTicks(second);
        lv_img_set_angle(ui_Minute_Hand, (t_tm.tm_min * 60) + (second));
        //lv_img_set_angle(ui_Simplistic_Minute_Hand, (t_tm.tm_min * 60) + (second));
        //lv_img_set_angle(ui_Simplistic_Minute_Hand_Shadow, (t_tm.tm_min * 60) + (second));
        /*if (lv_scr_act() == ui_Timers){
          lv_roller_set_selected(ui_Timer_Second_Right_Roller, t_tm.tm_sec % 10, LV_ANIM_ON);
          lv_roller_set_selected(ui_Timer_Second_Left_Roller, t_tm.tm_sec / 10 , LV_ANIM_ON);
          }*/
#endif

        if (t_tm.tm_min != lastmin)
        {
            lastmin = t_tm.tm_min;
#ifdef UPDATE_ELEMENTS
            lv_img_set_angle(ui_Hour_Hand, (t_tm.tm_hour * 300) + (t_tm.tm_min * 5));
            //lv_img_set_angle(ui_Simplistic_Hour_Hand, (t_tm.tm_hour * 300) + (t_tm.tm_min * 5));
            //lv_img_set_angle(ui_Simplistic_Hour_Hand_Shadow, (t_tm.tm_hour * 300) + (t_tm.tm_min * 5));

            strftime(time, sizeof(time), "%I:%M", &t_tm);
            lv_label_set_text(ui_Time, time);
#endif

            if (t_tm.tm_hour != lasthour)
            {
                lasthour = t_tm.tm_hour;

                strftime(time, sizeof(time), "%a %b %e", &t_tm);
#ifdef UPDATE_ELEMENTS
                lv_label_set_text(ui_Date, time);
                strftime(time, sizeof(time), "%D", &t_tm);
                lv_label_set_text(ui_Date_Numerical, time);
#endif
            }
        }
    }
}

int GetDay()
{
    getLocalTime(&t_tm);
    return (t_tm.tm_yday);
}

int GetMinute()
{
    getLocalTime(&t_tm);
    return (t_tm.tm_min);
}

int GetHour()
{
    getLocalTime(&t_tm);
    return (t_tm.tm_hour);
}

void InitTicks()
{
    for (int i = 0; i < 62; i++)
    {
        tick_index[i] = lv_img_create(ui_Clock);

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
            lv_obj_set_style_img_recolor(tick_index[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
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