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
