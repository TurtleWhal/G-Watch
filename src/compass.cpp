#include "Arduino.h"
#include "lvgl.h"
#include "ui.h"
#include "TWatch_hal.h"
#include "compass.h"

extern TWatchClass *twatch;

void Compass()
{
  if (ui_Compass != NULL)
  {
    lv_label_set_text_fmt(ui_Compass_Deg, "%i°", twatch->qmc5883l_get_Azimuth());
    // lv_obj_set_x(ui_Compass_N, 0);
    // lv_obj_set_y(ui_Compass_N, -100);
  }
}