#ifdef __cplusplus
extern "C"
{
#endif

void BTHandle();
void ParseGB(char *);
void BTmsgloop();
void BTsend(String);
//bool readStringUntil(String &input, size_t char_limit);
//void ToggleBT(lv_event_t *e);
void BT_off();
void BT_on();
bool isBTconnected();
void onBTConnect();
void pairBT(int);

#define BTtermchar "\x1e" // 0x1e is "record seperator" in ascii and is used to seperate messages

#ifdef __cplusplus
} /*extern "C"*/
#endif