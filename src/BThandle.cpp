#include "Arduino.h"
#include "NimBLEDevice.h"
#include "lvgl.h"
#include "ArduinoLog.h"
#include "ui.h"
#include "ui_helpers.h"
#include "TWatch_hal.h"
#include "BThandle.h"
#include "notifications.h"
#include "Preferences.h"
#include "hardware/blectl.h"

extern TWatchClass *twatch;
extern Notification NotificationList[11];
extern Preferences Storage;
extern int notificationid;
lv_obj_t *ui_Notification_Widgets[1];
LV_IMG_DECLARE(ui_img_bluetooth_small_png);
bool BTon;

void ParseGB(char *Message)
{
  lv_label_set_text(ui_Now_Playing_Label, Message);
}

void ToggleBT(lv_event_t *e)
{
  if (BTon)
    BT_off();
  else
    BT_on();
}

void BT_on()
{
  BTon = 1;

  blectl_setup();
  /*
  if (Storage.isKey("BTname"))
  {
    char BTnamechar[17];
    Storage.getBytes("BTname", BTnamechar, 17);
    SerialBT.begin((String)BTnamechar);
    Log.verboseln("BT Name: %s", BTnamechar);
  }
  else
    SerialBT.begin("Unnamed Watch");
    */
}

void BT_off()
{
  BTon = 0;
  //SerialBT.disconnect();
}