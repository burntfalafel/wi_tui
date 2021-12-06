#ifndef WITUI_H
#define WITUI_H
/* Number of SSIDs possible in GUI */
#define SSID_NUM 15

/* Selected char string SSID */
#define SSID_CHAR_MAX 100

/* Path to wpa_supplcant conf file */
#define WPA_SUPPLICANT_PATH "/etc/wpa_supplicant/wpa_supplicant.conf"

typedef struct ssid
{
    char name[SSID_CHAR_MAX];
} ssid;


static const char *ctrl_iface_dir = "/var/run/wpa_supplicant/wlp3s0";

#endif
