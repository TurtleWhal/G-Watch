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

void ShowNotification(String, String, String, int);
void PushNotification(int);
void PopNotification(int);
void DeleteNotification(lv_event_t *e);
void NotificationDismiss(lv_event_t *e);
void DrawNotifications(lv_event_t *e);
void NotificationHandle();
void ToggleDoNotDisturb(lv_event_t *e);
bool NotificationActive();
void AddFakeNotifications();
void NotificationHide(bool = 1);

#ifdef __cplusplus
} /*extern "C"*/
#endif