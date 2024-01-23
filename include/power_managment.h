#ifdef __cplusplus
extern "C"
{
#endif

  void Wakeup(String);
  void Sleep();
  void Sleephandle();
  void TickleSleep();
  bool IsSleeping();
  void Powerhandle();
  void UpdatePower(bool = false);
  void PowerOff(lv_event_t *e);
  void UpdateBrightness(lv_event_t *e);
  int GetUserBrightness();
  void FullSpeed();
  void SleepSpeed();
  int GetSleepTimer();
  void BTsendpower();
  void ActivatePowerSaving();
  void DeactivatePowerSaving();

#ifdef __cplusplus
} /*extern "C"*/
#endif
