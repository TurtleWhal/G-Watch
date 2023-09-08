#ifdef __cplusplus
extern "C"
{
#endif

enum {
    WEATHER_SCREEN,
    MUSIC_SCREEN
};

enum {
    STOPWATCH_SCREEN,
    TIMER_SCREEN,
    ALARMS_SCREEN
};

void buttontoclock();
lv_obj_t *GetClock();
void SetDownScreen(int Screen);
bool isClockScreen();

#ifdef __cplusplus
} /*extern "C"*/
#endif
