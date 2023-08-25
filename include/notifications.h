#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  String Title;
  String Text;
  String Source;
  int id;
  const void *img;
} Notification;

void InitNotif();
void shownotification(String, String, String, int);
void pushnotification(int);
void popnotification(int);
void deleteNotification(lv_event_t *e);
void notificationdismiss(lv_event_t *e);
void drawnotifications(lv_event_t *e);
void drawnotificationarc();
void ToggleDoNotDisturb(lv_event_t *e);
bool NotificationActive();
void FakeNotes();
void notificationhide(bool = 1);

#ifdef __cplusplus
} /*extern "C"*/
#endif