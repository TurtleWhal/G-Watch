#include "screen_management.h"
#include "lvgl.h"
#include "ui.h"
#include "ui_events.h"

void toscreen(lv_obj_t *screento)
{
    lv_obj_t *prevscreen = lv_scr_act();
    _ui_screen_change(screento, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0);
    //if (lv_scr_act() != prevscreen)
    lv_obj_del(prevscreen);
}

void toappsscreen(lv_event_t *e)
{
    ui_Apps_screen_init();
    toscreen(ui_Apps);
    //_ui_screen_change(ui_Apps, LV_SCR_LOAD_ANIM_MOVE_LEFT, 150, 0);
}