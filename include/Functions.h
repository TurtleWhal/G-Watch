#ifdef __cplusplus
extern "C"
{
#endif

void writetimertime();
//void notificationdismiss();
void alarmhandle();
void BThandle();
void istimernegative();
void Powerhandle();
void Sleephandle();
void Compass();
void StepHandle();
void Timer0Handle();
void BTTimerHandle();
void StepGraphHandle();
void pushnotification(int);
void DrawPower();
//void ApplyTheme();
void popnotification(int);
void CenterNotif_Animation(lv_obj_t *TargetObject, int delay);

#ifdef __cplusplus
} /*extern "C"*/
#endif
