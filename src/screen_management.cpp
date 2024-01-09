#include "lvgl.h"
#include "ui.h"
#include "Preferences.h"
#include "Functions.h"
#include "screen_management.h"
#include "power_managment.h"
#include "clockhandlers.h"
#include "timestuff.h"
#include "TWatch_hal.h"
#include "ArduinoLog.h"

extern Preferences Storage;

ClockInfo info;

extern TWatchClass *twatch;

// extern int Brightness;

void DefaultClockHandle();
void SimplisticWatchFaceHandle();

int LastTimeScreen = SCREEN_STOPWATCH;
int LastDownScreen = SCREEN_WEATHER;

// extern "C" lv_obj_t *ClockScreen = ui_SimplisticWatchFace;
auto *ClockScreen = &ui_SimplisticWatchFace;

lv_obj_t *Screen;

auto ClockHandler = SimplisticWatchFaceHandle;

// extern "C" lv_obj_t **ClockScreenChange = &ClockScreen;

auto ClockScreenInit = ui_SimplisticWatchFace_screen_init;

lv_obj_t *GetClockScreen()
{
    return Screen;
}

void SetClockScreen(myscreen_t scr)
{
    Serial.print("Set Clock Screen to: ");

    switch (scr)
    {
    case SCREEN_CLOCK_BLOCKY:
        ClockScreenInit = ui_Blocky_Clock_screen_init;
        ClockHandler = BlockyClockHandle;
        ClockScreen = &ui_Blocky_Clock;
        Screen = ui_Blocky_Clock;
        Serial.println("ui_Blocky_Clock");
        break;

    case SCREEN_CLOCK_SIMPLISTIC:
        ClockScreenInit = ui_SimplisticWatchFace_screen_init;
        ClockHandler = SimplisticWatchFaceHandle;
        ClockScreen = &ui_SimplisticWatchFace;
        Screen = ui_SimplisticWatchFace;
        Serial.println("ui_SimplisticWatchFace");
        break;

    case SCREEN_CLOCK_SKELETON:
        ClockScreenInit = ui_SkeletonWatchFace_screen_init;
        ClockHandler = SkeletonWatchFaceHandle;
        ClockScreen = &ui_SkeletonWatchFace;
        Screen = ui_SkeletonWatchFace;
        Serial.println("ui_SkeletonWatchFace");
        break;

    default:
        ClockScreenInit = ui_Default_Clock_screen_init;
        ClockHandler = DefaultClockHandle;
        ClockScreen = &ui_Default_Clock;
        Screen = ui_Default_Clock;
        Serial.println("ui_Default_Clock");
        break;
    }
}

void InitClockScreen()
{
    ClockScreenInit();

    if (ClockScreenInit == ui_Default_Clock_screen_init)
    {
        Serial.println("Init Default clock");
        lv_label_set_text(ui_Default_Clock_Now_Playing_Label, "");

        lv_obj_del(ui_Default_Clock_Tick_Dots); // Only used For visual purposes in Squareline Studio
        lv_obj_del(ui_Default_Clock_Tick_Dashes);

        lv_obj_del(ui_Second_Dash_Include); // Only used to include files
        lv_obj_del(ui_Second_Dot_Include);

        InitTicks(); // Draws the tick marks around the edge
    }
    info.flag.refresh = 1;
}

bool IsClockScreen()
{
    //Log.verboseln("%i | %i | %i", (lv_scr_act() == ui_Default_Clock) ? 1 : 0, (Screen == ui_Default_Clock) ? 1 : 0, (Screen == lv_scr_act()) ? 1 : 0);
    if (lv_scr_act() == ui_Default_Clock || lv_scr_act() == ui_Blocky_Clock || lv_scr_act() == ui_SkeletonWatchFace || lv_scr_act() == ui_SimplisticWatchFace)
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

void StopAnim(lv_event_t *e)
{
    lv_anim_del_all();
}

void LoadCalc(lv_event_t *e)
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

void SetKeyboardRound(lv_obj_t *keyboard)
{
    /*Set the relative width of the buttons and other controls*/
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {LV_BTNMATRIX_CTRL_CHECKED | 1, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_BTNMATRIX_CTRL_CHECKED | 2,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  LV_BTNMATRIX_CTRL_HIDDEN | 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, LV_BTNMATRIX_CTRL_HIDDEN | 1,
                                                  LV_BTNMATRIX_CTRL_HIDDEN | 3, 2, 2, 2, 2, 2, 2, 2, LV_BTNMATRIX_CTRL_HIDDEN | 3,
                                                  LV_BTNMATRIX_CTRL_HIDDEN | 1, 2, LV_BTNMATRIX_CTRL_HIDDEN | 1};

    /*Create an Calculator keyboard map*/
    static const char *kb_map_lower[] = {"ABC", "1#", LV_SYMBOL_CLOSE, LV_SYMBOL_NEW_LINE, LV_SYMBOL_BACKSPACE, "\n",
                                         "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
                                         " ", "a", "s", "d", "f", "g", "h", "j", "k", "l", " ", "\n",
                                         " ", "z", "x", "c", "v", "b", "n", "m", " ", "\n",
                                         " ", "", " ", NULL};
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER, kb_map_lower, kb_ctrl);

    /*Create an Calculator keyboard map*/
    static const char *kb_map_upper[] = {"abc", "1#", LV_SYMBOL_CLOSE, LV_SYMBOL_NEW_LINE, LV_SYMBOL_BACKSPACE, "\n",
                                         "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "\n",
                                         " ", "A", "S", "D", "F", "G", "H", "J", "K", "L", " ", "\n",
                                         " ", "Z", "X", "C", "V", "B", "N", "M", " ", "\n",
                                         " ", " ", " ", NULL};
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER, kb_map_upper, kb_ctrl);

    /*Create an Calculator keyboard map*/
    static const char *kb_map_special[] = {"abc", "ABC", LV_SYMBOL_CLOSE, LV_SYMBOL_NEW_LINE, LV_SYMBOL_BACKSPACE, "\n",
                                           "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
                                           " ", "+", "-", "*", "/", "=", ":", ";", "\"", "'", " ", "\n",
                                           " ", "#", "$", "%", "!", "?", "(", ")", " ", "\n",
                                           " ", " ", " ", NULL};
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_SPECIAL, kb_map_special, kb_ctrl);

    // lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);
    lv_keyboard_set_popovers(keyboard, true);

    lv_obj_set_style_bg_color(keyboard, info.theme.color, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(keyboard, lv_color_change_lightness(info.theme.color, 150), LV_PART_ITEMS | LV_STATE_PRESSED);
}

void LoadSettings(lv_event_t *e)
{
    char StepGoalChar[6];
    sprintf(StepGoalChar, "%i", Storage.getUShort("StepGoal"));
    lv_textarea_set_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), StepGoalChar);

    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    lv_textarea_set_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), BTnamechar);
    info.bt.name = BTnamechar;

    char NotificationLengthChar[4];
    sprintf(NotificationLengthChar, "%i", Storage.getUInt("NotifLength"));
    lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

    lv_colorwheel_set_rgb(ui_Theme_Colorwheel, lv_theme_get_color_primary(ui_Settings));
    UpdateTestTheme(nullptr);

    if (Storage.getBool("DarkMode"))
        lv_obj_add_state(ui_Dark_Mode_Setting_Switch, LV_STATE_CHECKED);

    ApplyTheme(nullptr);
}

void SetTimerDefault(lv_event_t *e)
{
    if (lv_event_get_target(e) == ui_Timers)
        LastTimeScreen = SCREEN_TIMER;
    else if (lv_event_get_target(e) == ui_Alarms)
        LastTimeScreen = SCREEN_ALARMS;
    else
        LastTimeScreen = SCREEN_STOPWATCH;
}

void SetDeafaultDown(lv_event_t *e)
{
    if (lv_scr_act() == ui_Weather)
        LastDownScreen = SCREEN_WEATHER;

    else if (lv_scr_act() == ui_Music)
        LastDownScreen = SCREEN_MUSIC;

    else if (lv_scr_act() == ui_Health)
        LastDownScreen = SCREEN_HEALTH;
}

void ClockRight(lv_event_t *e)
{
    if (lv_scr_act() != ui_Timers and lv_scr_act() != ui_Stopwatch and lv_scr_act() != ui_Alarms)
        switch (LastTimeScreen)
        {
        case SCREEN_TIMER:
            _ui_screen_change(&ui_Timers, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Timers_screen_init);
            break;
        case SCREEN_STOPWATCH:
            _ui_screen_change(&ui_Stopwatch, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Stopwatch_screen_init);
            break;
        case SCREEN_ALARMS:
            _ui_screen_change(&ui_Alarms, LV_SCR_LOAD_ANIM_OVER_LEFT, 150, 0, &ui_Alarms_screen_init);
            break;
        }
}

void ClockDown(lv_event_t *e)
{
    switch (LastDownScreen)
    {
    case SCREEN_MUSIC:
        _ui_screen_change(&ui_Music, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Music_screen_init);
        break;

    case SCREEN_HEALTH:
        _ui_screen_change(&ui_Health, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Health_screen_init);
        break;

    default:
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, &ui_Weather_screen_init);
        break;
    }
}

void ClockUpwards(lv_event_t *e)
{
    _ui_screen_change(&ui_Notifications, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 150, 0, &ui_Notifications_screen_init);
}

void ClockLeft(lv_event_t *e)
{
    _ui_screen_change(&ui_Apps, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, &ui_Apps_screen_init);
}

void GenericToClock(lv_event_t *e)
{
    if (!IsClockScreen)
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

void ButtonToClock()
{
    if (!IsClockScreen)
    {
        lv_anim_del_all();
        ApplyTheme(nullptr);
        lv_scr_load_anim((lv_obj_t *)GetClockScreen, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
    }
}

void DispLoadClockScreen()
{
    // Not atcually using disp load because it dosen't work and it just calls ui screen change
    _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, nullptr);
}

void ToggleFlashlight(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_SCREEN_UNLOADED)
        twatch->backlight_set_value(GetUserBrightness());

    if ((int)lv_obj_get_style_bg_color(ui_Flashlight, LV_PART_MAIN).full == (int)lv_color_hex(0xFFFFFF).full)
        twatch->backlight_set_value(100);
    else
        twatch->backlight_set_value(GetUserBrightness());
}

void ScreenBack(lv_event_t *e)
{
    if (lv_scr_act() == ui_Apps)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, InitClockScreen);
    }

    else if (lv_scr_act() == ui_Calculator or lv_scr_act() == ui_Compass or lv_scr_act() == ui_Settings or lv_scr_act() == ui_Flashlight)
    {
        _ui_screen_change(&ui_Apps, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Apps_screen_init);
    }

    else if (lv_scr_act() == ui_Notifications)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, InitClockScreen);
    }

    else if (lv_scr_act() == ui_Music or lv_scr_act() == ui_Weather or lv_scr_act() == ui_Health)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, InitClockScreen);
    }

    else if (lv_scr_act() == ui_Timers or lv_scr_act() == ui_Stopwatch or lv_scr_act() == ui_Alarms)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, InitClockScreen);
    }

    else if (lv_scr_act() == ui_Alarm_Going_Off or lv_scr_act() == ui_Schedule)
    {
        _ui_screen_change(ClockScreen, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, InitClockScreen);
    }

    else if (lv_scr_act() == ui_Set_Alarm)
    {
        _ui_screen_change(&ui_Alarms, LV_SCR_LOAD_ANIM_FADE_OUT, 150, 0, NULL);
    }
}