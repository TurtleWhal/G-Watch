#include "Arduino.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        typedef struct
        {
            // 24 hour hour
            uint8_t hour;
            // 12 hour hour
            uint8_t hour12;
            uint8_t minute;
            uint8_t second;
            // seconds lined up with valusec
            uint8_t valsec;
            // microseconds since second changed (0 - 1,000,000)
            int valusec;
            // Formatted Mon Jan 1
            String date;
            // Formatted M/DD/YY
            String numdate;
        } TimeInfo;
        TimeInfo time;

        typedef struct
        {
            bool secondchanged;
            bool minutechanged;
            bool hourchanged;
            bool refresh = 1;
        } FlagInfo;
        FlagInfo flag;

        typedef struct
        {
            uint16_t steps;
            uint16_t stepgoal;
        } HealthInfo;
        HealthInfo health;

        typedef struct
        {
            uint16_t starttime;
            uint16_t endtime;
            uint16_t currenttime;
        } ScheduleInfo;
        ScheduleInfo schedule;

        typedef struct
        {
            uint16_t degrees;
            // NNE, N, SSW
            char heading[3];
        } CompassInfo;
        CompassInfo compass;

        // Bluetooth
        typedef struct
        {
            String name;
            bool ison;
            bool isconnected = 1;
        } BluetoothInfo;
        BluetoothInfo bt;

        typedef struct
        {
            int position;
            int length;
            String song;
            String artist;
            String album;
            bool isplaying;
            // song recognized by pixel phones
            String nowplaying;
            uint8_t nowplayingtimer;
        } MusicInfo;
        MusicInfo music;

        typedef struct
        {
            int temp = 0;
            int high = 0;
            int low = 0;
            int humidity = 0;
            int precip = 0;
            // UV index
            double uv = 0;
            int code = 0;
            lv_img_dsc_t img = ui_img_mostly_cloudy_png;
            String type = "Not Updated";
            // Wind Speed
            int wind = 0;
            // Wind Direction
            int winddir = 0;
            String location = "";
        } WeatherInfo;
        WeatherInfo weather;

        typedef struct
        {
            // Last received notification title
            String lasttitle;
            // Last received notification text
            String lasttext;
            // Last received notification image
            const void *lastimg;
            // Number of notifications
            uint8_t count;
            bool donotdisturb;
        } NotificationInfo;
        NotificationInfo notification;

        typedef struct
        {
            uint8_t percentage;
            uint8_t voltage;
            bool ischarging;
        } PowerInfo;
        PowerInfo battery;

        typedef struct
        {
            lv_color16_t color;
            bool darkmode;
        } ThemeInfo;
        ThemeInfo theme;

        typedef struct
        {
            String ip;
            bool useOTA;
        } OTAInfo;
        OTAInfo OTA;

    } ClockInfo;

    enum
    {
        WEATHER_SCREEN,
        MUSIC_SCREEN
    };

    enum
    {
        STOPWATCH_SCREEN,
        TIMER_SCREEN,
        ALARMS_SCREEN
    };

    void ButtonToClock();
    lv_obj_t *GetClockScreen();
    void SetDownScreen(int Screen);
    bool IsClockScreen();
    void ScreenHandleHandle();
    void SetClockScreen(lv_obj_t *screen);
    void InitClockScreen();
    void DispLoadClockScreen();

#ifdef __cplusplus
} /*extern "C"*/
#endif
