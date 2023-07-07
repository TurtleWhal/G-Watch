#include "screen_management.h"
#include "lvgl.h"
#include "ui.h"
#include "ui_events.h"
#include "Preferences.h"

extern Preferences Storage;

extern int Brightness;

void lv_Calculator_Keyboard_Create(void)
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

    /*Create a keyboard and add the new map as USER_1 mode*/

    //lv_keyboard_set_map(ui_Keyboard2, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    //lv_keyboard_set_mode(ui_Keyboard2, LV_KEYBOARD_MODE_USER_1);

    lv_obj_t *ui_Calculator_Keyboard = lv_keyboard_create(ui_Calculator);

    lv_keyboard_set_map(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(ui_Calculator_Keyboard, LV_KEYBOARD_MODE_USER_1);
    lv_obj_set_width(ui_Calculator_Keyboard, 165);
    lv_obj_set_height(ui_Calculator_Keyboard, 175);
    lv_obj_set_y(ui_Calculator_Keyboard, 0);
    lv_obj_set_style_radius(ui_Calculator_Keyboard, 20, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(ui_Calculator_Keyboard, lv_color_hex(0xFF7D00), LV_PART_ITEMS | LV_STATE_CHECKED);
    // lv_obj_set_style_text_font(ui_Calculator_Keyboard, &ui_font_Comfortaa_16, LV_PART_ITEMS);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t *ta;
    ta = lv_textarea_create(ui_Calculator);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 25);
    lv_obj_set_size(ta, 165, 35);
    lv_obj_add_state(ta, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ta, 20, LV_PART_MAIN);

    lv_keyboard_set_textarea(ui_Calculator_Keyboard, ta);
}

void toappsscreen(lv_event_t *e)
{
    ui_Apps_screen_init();
    lv_scr_load_anim(ui_Apps, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 0);
}

void appsscreenback(lv_event_t *e)
{
    lv_anim_del_all();
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 1);
}

void notificationscreenback(lv_event_t *e)
{
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 150, 0, 1);
}

void totimersscreen(lv_event_t *e)
{
    ui_Timers_screen_init();
    lv_scr_load_anim(ui_Timers, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0, 0);
}

void timersscreenback(lv_event_t *e)
{
    lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 150, 0, 1);
}

void tosettingsscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Settings_screen_init();

    char StepGoalChar[6];
    sprintf(StepGoalChar, "%i", Storage.getInt("StepGoal"));
    lv_textarea_set_text(lv_obj_get_child(ui_Step_goal_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), StepGoalChar);

    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    lv_textarea_set_text(lv_obj_get_child(ui_BTname_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), BTnamechar);

    char NotificationLengthChar[4];
    sprintf(NotificationLengthChar, "%i", Storage.getInt("NotifLength"));
    lv_textarea_set_text(lv_obj_get_child(ui_Notification_Time_Setting_Panel, UI_COMP_SETTING_PANEL_SETTING_LABEL), NotificationLengthChar);

    lv_slider_set_value(ui_Brightness_Slider, Brightness, LV_ANIM_OFF);

    lv_scr_load_anim(ui_Settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
}

void tocompassscreen(lv_event_t *e)
{
    lv_anim_del_all();
    ui_Compass_screen_init();
    lv_scr_load_anim(ui_Compass, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
}

void tocalculatorscreen(lv_event_t *e)
{
    //lv_anim_del_all();
    //ui_Calculator_screen_init();
    //lv_Calculator_Keyboard_Create();
    lv_scr_load_anim(ui_Calculator, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 0);
}

void generictoclock()
{
    if (lv_scr_act() != ui_Clock)
    {
        lv_anim_del_all();
        lv_scr_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, 1);
    }
}