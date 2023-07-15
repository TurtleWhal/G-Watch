#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  String Title;
  String Text;
  String Source;
  //lv_obj_t *NotificationComp;
} Notification;

void shownotification(bool);
void pushnotification(int);
void popnotification(int);
void deleteNotification(lv_event_t *e);
void notificationdismiss(lv_event_t *e);
void drawnotifications(lv_event_t *e);
void drawnotificationarc();
void ToggleDoNotDisturb(lv_event_t *e);
bool NotificationActive();
void FakeNotes();

#ifdef __cplusplus
} /*extern "C"*/
#endif