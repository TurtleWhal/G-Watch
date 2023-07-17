#include "lvgl.h"
#include "ui.h"
#include "Preferences.h"
#include "Functions.h"
#include "screen_management.h"
#include "power_managment.h"

extern Preferences Storage;

//extern int Brightness;

int LastTimeScreen = 2;

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

    char NotificationLengthChar[4];
    sprintf(NotificationLengthChar, "%i", Storage.getUInt("NotifLength"));
    lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

    lv_slider_set_value(ui_Brightness_Slider, GetUserBrightness(), LV_ANIM_OFF);

    lv_colorwheel_set_rgb(ui_Theme_Colorwheel, lv_theme_get_color_primary(ui_Clock));
    UpdateTestTheme(nullptr);
}

void settimerdefault(lv_event_t *e)
{
    if (lv_event_get_target(e) == ui_Timers)
    LastTimeScreen = 1;
    else if (lv_event_get_target(e) == ui_Alarms)
    LastTimeScreen = 3;
    else
    LastTimeScreen = 2;
}

void totimescreen(lv_event_t *e)
{
    switch (LastTimeScreen)
    {
    case 1:
        _ui_screen_change(&ui_Timers, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, &ui_Timers_screen_init);
        break;
    case 2:
        _ui_screen_change(&ui_Stopwatch, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, &ui_Stopwatch_screen_init);
        break;
    case 3:
        _ui_screen_change(&ui_Alarms, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, &ui_Alarms_screen_init);
        break;
    }
}

/*
void toappsscreen(lv_event_t *e)
{
    ui_Apps_screen_init();
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Apps, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 0);
}

void appsscreenback(lv_event_t *e)
{
    lv_anim_del_all();
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
}

void notificationscreenback(lv_event_t *e)
{
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
}

void totimersscreen(lv_event_t *e)
{
    switch (LastTimeScreen)
    {
    case 1:
        ui_Timers_screen_init();
        lv_scr_load_anim(ui_Timers, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 0);
        break;
    case 2:
        ui_Stopwatch_screen_init();
        lv_scr_load_anim(ui_Stopwatch, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 0);
        break;
    case 3:
        ui_Alarms_screen_init();
        lv_scr_load_anim(ui_Alarms, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 0);
        break;
    }
    ApplyTheme(nullptr);
}

void timersscreenback(lv_event_t *e)
{
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
}

void tosettingsscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Settings_screen_init();

    ApplyTheme(nullptr);

    char StepGoalChar[6];
    sprintf(StepGoalChar, "%i", Storage.getUInt("StepGoal"));
    lv_textarea_set_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), StepGoalChar);

    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    lv_textarea_set_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), BTnamechar);

    char NotificationLengthChar[4];
    sprintf(NotificationLengthChar, "%i", Storage.getUInt("NotifLength"));
    lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

    lv_slider_set_value(ui_Brightness_Slider, Brightness, LV_ANIM_OFF);

    lv_colorwheel_set_rgb(ui_Theme_Colorwheel, lv_theme_get_color_primary(ui_Clock));
    UpdateTestTheme(nullptr);

    if (lv_scr_act() != ui_Clock)
    {
        lv_scr_load_anim(ui_Settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
        // Log.verboseln("Not Clock");
    }
    else
    {
        lv_scr_load_anim(ui_Settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
        // Log.verboseln("Clock");
    }
}

void tocompassscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Compass_screen_init();
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Compass, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
}

void tocalculatorscreen(lv_event_t *e)
{
    // lv_anim_del_all();
    ui_Calculator_screen_init();
    // lv_Calculator_Keyboard_Create();

    //Create an Calculator keyboard map
    static const char *kb_map[] = {"C", LV_SYMBOL_BACKSPACE, "\n",
                                   "7", "8", "9", "/", "\n",
                                   "4", "5", "6", "x", "\n",
                                   "1", "2", "3", "-", "\n",
                                   ".", "0", "=", "+", NULL};

    //Set the relative width of the buttons and other controls
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {LV_BTNMATRIX_CTRL_CHECKED | 2, 2,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
                                                  1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1};

    lv_keyboard_set_map(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1);

    ApplyTheme(nullptr);

    lv_scr_load_anim(ui_Calculator, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
}

void timermoveup(lv_event_t *e)
{
    ui_Stopwatch_screen_init();
    lv_scr_load_anim(ui_Stopwatch, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, 1);
    LastTimeScreen = 2;
}

void timermovedown(lv_event_t *e)
{
    ui_Alarms_screen_init();
    lv_scr_load_anim(ui_Alarms, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
    LastTimeScreen = 3;
}

void stopwatchmoveup(lv_event_t *e)
{
    ui_Alarms_screen_init();
    lv_scr_load_anim(ui_Alarms, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, 1);
    LastTimeScreen = 3;
}

void stopwatchmovedown(lv_event_t *e)
{
    ui_Timers_screen_init();
    lv_scr_load_anim(ui_Timers, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
    LastTimeScreen = 1;
}

void alarmsmoveup(lv_event_t *e)
{
    ui_Timers_screen_init();
    lv_scr_load_anim(ui_Timers, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, 1);
    LastTimeScreen = 1;
}

void alarmsmovedown(lv_event_t *e)
{
    ui_Stopwatch_screen_init();
    lv_scr_load_anim(ui_Stopwatch, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
    LastTimeScreen = 2;
}

void tosetalarm(lv_event_t *e)
{
    ui_Set_Alarm_screen_init();
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Set_Alarm, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
}

void setalarmscreenback(lv_event_t *e)
{
    ui_Alarms_screen_init();
    ApplyTheme(nullptr);
    lv_scr_load_anim(ui_Set_Alarm, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
}*/

void generictoclock(lv_event_t *e)
{
    if (lv_scr_act() != ui_Clock)
    {
        ApplyTheme(nullptr);
        lv_anim_del_all();
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_LEFT:
            lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
            break;
        case LV_DIR_RIGHT:
            lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
            break;
        case LV_DIR_TOP:
            lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, 1);
            break;
        case LV_DIR_BOTTOM:
            lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
            break;
        default:
            lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
            break;
        }
    }
}

void buttontoclock()
{
    if (lv_scr_act() != ui_Clock)
    {
        lv_anim_del_all();
        ApplyTheme(nullptr);
        lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
    }
}

void screenback()
{
    if (lv_scr_act() == ui_Apps)
    {
    _ui_screen_change( &ui_Clock, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, NULL);
    }

    else if (lv_scr_act() == ui_Calculator or lv_scr_act() == ui_Compass or lv_scr_act() == ui_Settings or lv_scr_act() == ui_Flashlight)
    {
    _ui_screen_change( &ui_Apps, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, &ui_Apps_screen_init);
    }

    else if (lv_scr_act() == ui_Notifications)
    {
    _ui_screen_change( &ui_Clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 150, 0, NULL);
    }

    else if (lv_scr_act() == ui_Timers or lv_scr_act() == ui_Stopwatch or lv_scr_act() == ui_Alarms)
    {
    _ui_screen_change( &ui_Clock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, NULL);
    }
    
    else if (lv_scr_act() == ui_Alarm_Going_Off)
    {
    _ui_screen_change( &ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, NULL);
    }

    else if (lv_scr_act() == ui_Set_Alarm)
    {
    _ui_screen_change( &ui_Alarms, LV_SCR_LOAD_ANIM_FADE_OUT, 150, 0, NULL);
    }

}