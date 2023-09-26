#include "lvgl.h"
#include "ui.h"
#include "Preferences.h"
#include "Functions.h"
#include "screen_management.h"
#include "power_managment.h"
#include "ClockHandlers.h"

extern Preferences Storage;

ClockInfo info;

// extern int Brightness;

void DefaultClockHandle();
void SimplisticWatchFaceHandle();

int LastTimeScreen = STOPWATCH_SCREEN;
int LastDownScreen = WEATHER_SCREEN;

// extern "C" lv_obj_t *ClockScreen = ui_SimplisticWatchFace;
auto *ClockScreen = &ui_SimplisticWatchFace;

lv_obj_t *Screen = ui_SimplisticWatchFace;

auto ClockHandler = SimplisticWatchFaceHandle;

// extern "C" lv_obj_t **ClockScreenChange = &ClockScreen;

auto ClockScreenInit = ui_SimplisticWatchFace_screen_init;

lv_obj_t *GetClockScreen()
{
    return Screen;
}

void SetClockScreen(lv_obj_t *screen)
{

    Serial.print("Set Clock Screen to: ");
    Screen = screen;
    ClockScreen = &screen;

    if (Screen == ui_Default_Clock)
    {
        ClockScreenInit = ui_Default_Clock_screen_init;
        ClockHandler = DefaultClockHandle;
        ClockScreen = &ui_SimplisticWatchFace;
        Serial.println("ui_Default_Clock");
    }
    else if (Screen == ui_SimplisticWatchFace)
    {
        ClockScreenInit = ui_SimplisticWatchFace_screen_init;
        ClockHandler = SimplisticWatchFaceHandle;
        ClockScreen = &ui_SimplisticWatchFace;
        Serial.println("ui_SimplisticWatchFace");
    }
}

void InitClockScreen()
{
    ClockScreenInit();
    // ui_SimplisticWatchFace_screen_init();
}

bool isClockScreen()
{
    if (lv_scr_act() == Screen)
        return 1;
    else
        return 0;
}

void ScreenHandleHandle()
{
    ClockHandler();
}

void SetDownScreen(int Screen)
{
    LastDownScreen = Screen;
}

void stopanim(lv_event_t *e)
{
    lv_anim_del_all();
}

void loadcalc(lv_event_t *e)
{
    /*Create an Calculator keyboard map*/
    static const char *kb_map[] = {"C", LV_SYMBOL_BACKSPACE, "\n",
                                   "7", "8", "9", "/", "\n",
                                   "4", "5", "6", "x", "\n",
                                   "1", "2", "3", "-", "\n",
                                   ".", "0", "=", "+", NULL};

    /*Set the relative width of the buttons and other controls*/
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {LV_BTNMATRIX_CTRL_CHECKED | 2, 2,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1};

    lv_keyboard_set_map(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1);
}

void loadsettings(lv_event_t *e)
{
    char StepGoalChar[6];
    sprintf(StepGoalChar, "%i", Storage.getUInt("StepGoal"));
    lv_textarea_set_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), StepGoalChar);

    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    lv_textarea_set_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), BTnamechar);
    info.bt.name = BTnamechar;

    char NotificationLengthChar[4];
    sprintf(NotificationLengthChar, "%i", Storage.getUInt("NotifLength"));
    lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

    lv_slider_set_value(ui_Brightness_Slider, GetUserBrightness(), LV_ANIM_OFF);

    lv_colorwheel_set_rgb(ui_Theme_Colorwheel, lv_theme_get_color_primary(ui_Settings));
    UpdateTestTheme(nullptr);

    if (Storage.getBool("DarkMode"))
        lv_obj_add_state(ui_Dark_Mode_Setting_Switch, LV_STATE_CHECKED);

    ApplyTheme(nullptr);
}

void settimerdefault(lv_event_t *e)
{
    if (lv_event_get_target(e) == ui_Timers)
        LastTimeScreen = TIMER_SCREEN;
    else if (lv_event_get_target(e) == ui_Alarms)
        LastTimeScreen = ALARMS_SCREEN;
    else
        LastTimeScreen = STOPWATCH_SCREEN;
}

void SetWeatherDeafault(lv_event_t *e)
{
    LastDownScreen = WEATHER_SCREEN;
}

void SetMusicDeafault(lv_event_t *e)
{
    LastDownScreen = MUSIC_SCREEN;
}

void ClockRight(lv_event_t *e)
{
    if (lv_scr_act() != ui_Timers and lv_scr_act() != ui_Stopwatch and lv_scr_act() != ui_Alarms)
        switch (LastTimeScreen)
        {
        case TIMER_SCREEN:
            _ui_screen_change(&ui_Timers, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Timers_screen_init);
            break;
        case STOPWATCH_SCREEN:
            _ui_screen_change(&ui_Stopwatch, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Stopwatch_screen_init);
            break;
        case ALARMS_SCREEN:
            _ui_screen_change(&ui_Alarms, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Alarms_screen_init);
            break;
        }
}

void ClockDown(lv_event_t *e)
{
    if (LastDownScreen == MUSIC_SCREEN)
        _ui_screen_change(&ui_Music, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Music_screen_init);
    else
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Weather_screen_init);
}

void ClockUpwards(lv_event_t *e)
{
    _ui_screen_change(&ui_Notifications, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 150, 0, &ui_Notifications_screen_init);
}

void ClockLeft(lv_event_t *e)
{
    _ui_screen_change(&ui_Apps, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, &ui_Apps_screen_init);
}

void generictoclock(lv_event_t *e)
{
    if (!isClockScreen)
    {
        ApplyTheme(nullptr);
        lv_anim_del_all();
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_LEFT:
            lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
            break;
        case LV_DIR_RIGHT:
            lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
            break;
        case LV_DIR_TOP:
            lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, 1);
            break;
        case LV_DIR_BOTTOM:
            lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
            break;
        default:
            lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
            break;
        }
    }
}

void buttontoclock()
{
    if (!isClockScreen)
    {
        lv_anim_del_all();
        ApplyTheme(nullptr);
        lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
    }
}

void screenback(lv_event_t *e)
{
    if (lv_scr_act() == ui_Apps)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, ClockScreenInit);
    }

    else if (lv_scr_act() == ui_Calculator or lv_scr_act() == ui_Compass or lv_scr_act() == ui_Settings or lv_scr_act() == ui_Flashlight)
    {
        _ui_screen_change(&ui_Apps, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Apps_screen_init);
    }

    else if (lv_scr_act() == ui_Notifications)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, ClockScreenInit);
    }

    else if (lv_scr_act() == ui_Music or lv_scr_act() == ui_Weather)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, ClockScreenInit);
    }

    else if (lv_scr_act() == ui_Timers or lv_scr_act() == ui_Stopwatch or lv_scr_act() == ui_Alarms)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, ClockScreenInit);
    }

    else if (lv_scr_act() == ui_Alarm_Going_Off or lv_scr_act() == ui_Schedule)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, ClockScreenInit);
    }

    else if (lv_scr_act() == ui_Set_Alarm)
    {
        _ui_screen_change(&ui_Alarms, LV_SCR_LOAD_ANIM_FADE_OUT, 150, 0, NULL);
    }

    else if (lv_scr_act() == ui_Health)
    {
        _ui_screen_change(&ui_Notifications, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, NULL);
    }
}