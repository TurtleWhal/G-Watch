#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        typedef struct
        {
            uint8_t hours;
            uint8_t minutes;
            uint8_t seconds;
            uint8_t valsec;
            int valusec;
            String date;
            String numdate;
        } TimeInfo;
        TimeInfo time;

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
            char heading[3];
        } CompassInfo;
        CompassInfo compass;

        typedef struct
        {
            String name;
            bool status;
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
            String nowplaying;
        } MusicInfo;
        MusicInfo music;

        typedef struct
        {
            int temp = 0;
            int high = 0;
            int low = 0;
            int humidity = 0;
            int precip = 0;
            int uv = 0;
            int code = 0;
            lv_img_dsc_t img = ui_img_mostly_cloudy_png;
            String type = "Not Updated";
            int wind = 0;
            int winddir = 0;
            String location = "";
        } WeatherInfo;
        WeatherInfo weather;

        typedef struct
        {
            String lasttitle;
            String lasttext;
            const void *lastimg;
            uint8_t count;
        } NotificationInfo;
        NotificationInfo notification;

        typedef struct
        {
            uint8_t percentage;
            uint8_t voltage;
            bool ischarging;
        } PowerInfo;
        PowerInfo battery;

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

    void buttontoclock();
    lv_obj_t *GetClockScreen();
    void SetDownScreen(int Screen);
    bool isClockScreen();
    void ScreenHandleHandle();
    void SetClockScreen(lv_obj_t *screen);
    void InitClockScreen();

#ifdef __cplusplus
} /*extern "C"*/
#endif
