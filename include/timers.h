#ifdef __cplusplus
extern "C"
{
#endif

void ToggleStopwatch(lv_event_t * e);
void resetstopwatch(lv_event_t * e);
void ToggleTimer(lv_event_t * e);
void timerhourplus10(lv_event_t * e);
void timerhourplus1(lv_event_t * e);
void timerminuteplus10(lv_event_t * e); 
void timerminuteplus1(lv_event_t * e);
void timersecondplus10(lv_event_t * e); 
void timersecondplus1(lv_event_t * e);
void timerhourminus10(lv_event_t * e);
void timerhourminus1(lv_event_t * e);
void timerminuteminus10(lv_event_t * e);
void timerminuteminus1(lv_event_t * e); 
void timersecondminus10(lv_event_t * e);
void timersecondminus1(lv_event_t * e); 
void istimernegative();
void writetimertime();
void processstopwatch();


#ifdef __cplusplus
} /*extern "C"*/
#endif