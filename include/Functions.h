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
void pushnotification(int);
void DrawPower();
//void ApplyTheme();
void popnotification(int);

#ifdef __cplusplus
} /*extern "C"*/
#endif
