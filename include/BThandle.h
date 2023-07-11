#ifdef __cplusplus
extern "C"
{
#endif

void BThandle();
//bool readStringUntil(String &input, size_t char_limit);
void ToggleBT(lv_event_t *e);
void BT_off();
void BT_on();

#ifdef __cplusplus
} /*extern "C"*/
#endif