#include "./TWatch_hal.h"

int16_t Value;

void TWatchClass::backlight_init() {
  pinMode(TWATCH_TFT_BL, OUTPUT);
  ledcSetup(0, 2000, 14);
  ledcAttachPin(TWATCH_TFT_BL, 0);
  ledcWrite(0, 0);
}
/*Enter Value 0 - 100*/
void TWatchClass::backlight_set_value(int16_t val) {

  if (val < 100) {
    ledcWrite(0, (uint32_t)((val * val * 0.8192) + 0.5));
    Value = val;
  } else {
    ledcWrite(0, 0x1FFF);
    Value = 100;
  }

  Serial.printf("Backlight Set to %i/%i\n", (uint32_t)((val * val * 0.8192) + 0.5), 8192);
}

int16_t TWatchClass::backlight_get_value() {
  return (Value);
}

void TWatchClass::backlight_gradual_light(int16_t val, uint32_t ms) {
  int32_t temp1;
  _old_Backlight = backlight_get_value();
  temp1 = (val - _old_Backlight);
  _endtime = millis() + ms;
  _k = (float)((temp1 * 1.0) / ms);
  DEBUGF("Backlight_Gradually Val:%d,ms:%d,k:%f\n", val, ms, _k);
  _adjust = true;
}

void TWatchClass::backlight_updata(uint32_t millis, uint32_t time_ms) {
  static uint32_t Millis;
  static uint8_t count;
  if (_adjust) {
    if (millis - Millis > time_ms) {
      count++;
      backlight_set_value((uint16_t)((_k * count) + _old_Backlight));
      if (Millis > _endtime) {
        _adjust = false;
        count = 0;
      }
      Millis = millis;
    }
  }
}
