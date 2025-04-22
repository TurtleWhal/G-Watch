#ifdef __cplusplus
extern "C"
{
#endif

    void UpdateTime();
    int GetDayOfYear();
    int GetDayOfWeek();
    void InitTicks();
    void ColorTicks(int);
    void SetTime(ulong, short);
    void SyncTime();

#ifdef __cplusplus
} /*extern "C"*/
#endif