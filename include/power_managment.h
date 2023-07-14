#ifdef __cplusplus
extern "C"
{
#endif

  void Wakeup(String);
  void Sleep();
  void Sleephandle();
  void Ticklesleep();
  bool isSleeping();
  void Powerhandle();
  void DrawPower();
  void PowerOff(lv_event_t *e);
  void UpdateBrightness(lv_event_t *e);
  void InitPercent();

#ifdef __cplusplus
} /*extern "C"*/
#endif
