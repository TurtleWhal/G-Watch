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
        } TimeInfo;
        TimeInfo time;

        typedef struct
        {
            uint16_t steps;
            uint16_t stegoal;
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
        } MusicInfo;
        MusicInfo music;

        typedef struct
        {
            int Temp = 0;
            int High = 0;
            int Low = 0;
            int Humidity = 0;
            int Precip = 0;
            int UV = 0;
            int Code = 0;
            lv_img_dsc_t Img = ui_img_mostly_cloudy_png;
            String Type = "Not Updated";
            int Wind = 0;
            int Winddir = 0;
            String Loc = "";
        } WeatherInfo;
        WeatherInfo weather;

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

#ifdef __cplusplus
} /*extern "C"*/
#endif
