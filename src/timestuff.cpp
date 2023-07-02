
#include "lvgl.h"
#include <TWatch_hal.h>
#include "ui.h"
#include "timestuff.h"
#include "time.h"

static uint8_t lastsec = 61;
static uint8_t lastmin = 61;
static uint8_t lasthour = 61;

void WriteTime()
{

    char time[11];
    struct tm t_tm;
    struct timeval val;
    gettimeofday(&val, NULL);
    
    char second = val.tv_sec % 60;

    lv_img_set_angle(ui_Second_Hand, (val.tv_usec * 0.00006) + second * 60);
    lv_img_set_angle(ui_Second_Dot, second * 60);

    if (val.tv_sec != lastsec)
    {
        lastsec = val.tv_sec;
        getLocalTime(&t_tm);
        lv_img_set_angle(ui_Minute_Hand, (t_tm.tm_min * 60) + (t_tm.tm_sec));

        if (t_tm.tm_min != lastmin)
        {
            lastmin = t_tm.tm_min;
            lv_img_set_angle(ui_Hour_Hand, (t_tm.tm_hour * 300) + (t_tm.tm_min * 5));

            strftime(time, sizeof(time), "%I:%M", &t_tm);
            lv_label_set_text(ui_Time, time);

            if (t_tm.tm_hour != lasthour)
            {
                lasthour = t_tm.tm_hour;

                strftime(time, sizeof(time), "%a %b %e", &t_tm);
                lv_label_set_text(ui_Date, time);

                strftime(time, sizeof(time), "%D", &t_tm);
                lv_label_set_text(ui_Date_Numerical, time);
            }
        }
    } 
}

int GetDay()
{
    struct tm t_tm;
    getLocalTime(&t_tm);
    return (t_tm.tm_yday);
}