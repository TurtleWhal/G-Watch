#include "./TWatch_hal.h"

#if defined(CONFIG_TWATCH_HAS_POWER)
/* Quantity comparison meter */
/*                             0%   10%    20%   30%  40%    50%   60%   70%   80%   90%  100% */
// static const float Qcm[11] = {3000, 3450, 3680, 3740, 3770, 3790, 3820, 3920, 3980, 4060, 4120};

static const float Qcm[2][11]= 
{
  {2671, 3079, 3148, 3211, 3282, 3335, 3379, 3441, 3522, 3611, 3740}, //discharge
  {4017, 4020, 4036, 4039, 4043, 4043, 4060, 4073, 4083, 4097, 4200} //charge  
};

void TWatchClass::power_init() {
  pinMode(TWATCH_PWR_ON, OUTPUT);
  pinMode(TWATCH_BAT_ADC, INPUT);
  digitalWrite(TWATCH_PWR_ON, HIGH);
}

void TWatchClass::power_updata(uint32_t millis, uint32_t time_ms) {
  static uint32_t Millis;
  if (millis - Millis > time_ms) {
    if (_power_cnt < POWER_UPDATA_CYCLE) {
      _pow_temp[_power_cnt] = analogRead(TWATCH_BAT_ADC);
      _power_cnt++;
    } else {
      tw_bubble_sort(_pow_temp, POWER_UPDATA_CYCLE);

      for (uint8_t i = 2; i < (POWER_UPDATA_CYCLE - 4); i++) {
        _power_sub += _pow_temp[i];
      }
      _power_sub = _power_sub / (POWER_UPDATA_CYCLE - 4);

      // if (_pow_temp[POWER_UPDATA_CYCLE - 1] - _power_sub > 5)
      // {
      //     _ischarge = true;
      // }
      // else if (_power_sub - _pow_temp[0] > 5)
      // {
      //     _ischarge = false;
      // }
      // else
      // {
      //     _ischarge = false;
      // }
      _pow_cur_vol = ((_power_sub * 3300 * 2) / 4096) + 200;
      _power_sub = 0;
      _power_cnt = 0;
    }
    Millis = millis;
  }
}
float TWatchClass::power_get_percent() {

  /*float Qcm[11] = {2671, 3079, 3148, 3211, 3282, 3335, 3379, 3441, 3522, 3611, 3740};
  if (!digitalRead(TWATCH_CHARGING) || power_get_volt() > 4000) {
    float Qcm[11] = {4017, 4020, 4036, 4039, 4043, 4043, 4060, 4073, 4083, 4097, 4140};
  } 
  */
  
  char charging = 0;
  if (!digitalRead(TWATCH_CHARGING) || power_get_volt() > 4000)
    charging = 1;
    
  int8_t i = 10;
  while (_pow_cur_vol <= Qcm[charging][i]) {
    if (i == 0)
      break;
    else
      i--;
  }
  float vol_section = Qcm[charging][i + 1] - Qcm[charging][i];
  float decade = i * 10.0;
  _pow_percent = constrain(decade + 10.0 * ((_pow_cur_vol - Qcm[charging][i]) / vol_section), 0.0, 100.0);

  return _pow_percent;
}
float TWatchClass::power_get_volt() {
  return constrain(_pow_cur_vol, 0, 4200);
}
#endif