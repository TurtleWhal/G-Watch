#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"

#include "calculator.h"

void CalcHandle()
{
    if (lv_scr_act() == ui_Calculator)
    {
        const char *calcbutton = lv_btnmatrix_get_btn_text(ui_Calculator_Keyboard, lv_btnmatrix_get_selected_btn(ui_Calculator_Keyboard));
        if (calcbutton == "C")
        {
            lv_textarea_set_text(ui_Calculator_textarea, "");
            Serial.println((int)calcbutton);
        }
    }
}