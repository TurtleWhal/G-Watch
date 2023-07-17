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
    // Serial.println(mag.getAzimuth());
    char deg[6];
    sprintf(deg, "%i°", twatch->qmc5883l_get_Azimuth());
    lv_label_set_text(ui_Compass_Deg, deg);
    // lv_obj_set_x(ui_Compass_N, 0);
    // lv_obj_set_y(ui_Compass_N, -100);
  }
}