#ifdef __cplusplus
extern "C"
{
#endif

void WriteTimerTime();
void alarmhandle();
void BThandle();
void IsTimerNegative();
void Powerhandle();
void Sleephandle();
void Compass();
void StepHandle();
void Timer0Handle();
void BTTimerHandle();
void StepGraphHandle();
void PushNotification(int);
void UpdatePower();
void PopNotification(int);
void CenterNotif_Animation(lv_obj_t *TargetObject, int delay);

#ifdef __cplusplus
} /*extern "C"*/
#endif
