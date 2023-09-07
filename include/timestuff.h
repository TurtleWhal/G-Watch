#ifdef __cplusplus
extern "C" {
#endif

void WriteTime();
int GetDay();
int GetMinute();
int GetHour();
void InitTicks();
void ColorTicks(int);
void SetTime(ulong, short);

#ifdef __cplusplus
} /*extern "C"*/
#endif