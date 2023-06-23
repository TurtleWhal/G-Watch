
#include "lvgl.h"

#include <TWatch_hal.h>
#include "ui.h"

#include "time.h"

extern TWatchClass *twatch;
extern PCF8563_Class *PCF;

int millisoffset = 0;
static uint8_t lastmin = 0;
static uint8_t lasthour = 0;

void writetime()
{
    PCF = twatch->rtc_get_instance();
    RTC_Date rtc = PCF->getDateTime();

    lv_img_set_angle(ui_Second_Hand, (((millis() - millisoffset) % 60000) * 0.06));
    // lv_img_set_angle(ui_Second_Hand, (rtc.second * 60) + (millis() % 1000) * 0.06);
    lv_img_set_angle(ui_Minute_Hand, (rtc.minute * 60) + (rtc.second));
    lv_img_set_angle(ui_Hour_Hand, (rtc.hour * 300) + (rtc.minute * 5));
    lv_img_set_angle(ui_Second_Dot, rtc.second * 60);

    if (rtc.minute > lastmin || lastmin == 59 & rtc.minute == 0)
    {
        millisoffset = (millis() % 60000);
        lastmin = rtc.minute;
    }

    if (rtc.hour > lasthour || lasthour == 59 & rtc.hour == 0)
    {
        lv_label_set_text(ui_Date, PCF->formatDateTime(PCF_TIMEFORMAT_DOM_MM_DD));
        lasthour = rtc.hour;
    }

    char time[6];
    if (rtc.hour > 12)
        snprintf(time, sizeof(time), "%02d:%02d", rtc.hour - 12, rtc.minute);
    else
        snprintf(time, sizeof(time), "%02d:%02d", rtc.hour, rtc.minute);
    lv_label_set_text(ui_Time, time);

    /*
      /*char date[11];
      rtc.getTime("%a %b %e").toCharArray(date, 11);
      lv_label_set_text(ui_Date, date);*/

    char numdate[10];
    snprintf(numdate, sizeof(numdate), "%02d/%02d/%02d", rtc.month, rtc.day, rtc.year - 2000);
    lv_label_set_text(ui_Date_Numerical, numdate);
}