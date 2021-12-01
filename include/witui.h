#ifndef WITUI_H
#define WITUI_H
/* Number of SSIDs possible in GUI */
#define SSID_NUM 10

/* Path to wpa_supplcant conf file */
#define WPA_SUPPLICANT_PATH "/etc/wpa_supplicant/wpa_supplicant.conf"

typedef struct ssid
{
    char name[100];
} ssid;
#endif
