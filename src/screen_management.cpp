#include "lvgl.h"
#include "ui.h"
#include "Preferences.h"
#include "Functions.h"
#include "screen_management.h"

extern Preferences Storage;

extern int Brightness;

int LastTimeScreen = 1;

void toappsscreen(lv_event_t *e)
{
    ui_Apps_screen_init();
    ApplyTheme();
    lv_scr_load_anim(ui_Apps, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 0);
}

void appsscreenback(lv_event_t *e)
{
    lv_anim_del_all();
    ApplyTheme();
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
}

void notificationscreenback(lv_event_t *e)
{
    ApplyTheme();
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
    ApplyTheme();
}

void timersscreenback(lv_event_t *e)
{
    ApplyTheme();
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
}

void tosettingsscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Settings_screen_init();

    ApplyTheme();

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
        // Serial.println("Not Clock");
    }
    else
    {
        lv_scr_load_anim(ui_Settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
        // Serial.println("Clock");
    }
}

void tocompassscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Compass_screen_init();
    ApplyTheme();
    lv_scr_load_anim(ui_Compass, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
}

void tocalculatorscreen(lv_event_t *e)
{
    // lv_anim_del_all();
    ui_Calculator_screen_init();
    // lv_Calculator_Keyboard_Create();

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

    ApplyTheme();

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
    ApplyTheme();
    lv_scr_load_anim(ui_Set_Alarm, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
}

void setalarmscreenback(lv_event_t *e)
{
    ui_Alarms_screen_init();
    ApplyTheme();
    lv_scr_load_anim(ui_Set_Alarm, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
}

void generictoclock(lv_event_t *e)
{
    if (lv_scr_act() != ui_Clock)
    {
        ApplyTheme();
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
        }
    }
}

void buttontoclock()
{
    if (lv_scr_act() != ui_Clock)
    {
        lv_anim_del_all();
        ApplyTheme();
        lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
    }
}