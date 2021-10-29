#include "includes.h"

#include "wpa_ctrl.h"
#include "common.h"
//#include "common/version.h"
//

typedef struct ssid
{
    char name[100];
} ssid;

static struct wpa_ctrl *ctrl_conn;
static void wpa_cli_msg_cb(char *msg, size_t len)
{
  printf("%s\n", msg);
}
static int _wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char *message)
{
        size_t len;
        int ret;

        if (ctrl_conn == NULL) {
                fprintf(stderr, "Not connected to wpa_supplicant - command dropped.\n");
                return -1;
        }
        len = 2047; // message looses all size info: sizeof(message) - 1
        ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), message, &len,
                               wpa_cli_msg_cb);
        if (ret == -2) {
                fprintf(stderr, "'%s' command timed out.\n", cmd);
                return -2;
        } else if (ret < 0) {
                fprintf(stderr, "'%s' command failed.\n", cmd);
                return -1;
        }
        message[len] = '\0';
        return 0;
}


static int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char *message)
{
        return _wpa_ctrl_command(ctrl, cmd, message);
}

static int wpa_cli_cmd_scan(struct wpa_ctrl *ctrl, char *message)
{
         return wpa_ctrl_command(ctrl, "SCAN", message);
}
static int wpa_cli_cmd_scan_results(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "SCAN_RESULTS", message);
}
static int ssid_str(char* string, ssid *wlist )
{
int tabct=0;
int itr = 0;
int message_count=0;
for (int i = 0; i< strlen(string); i++ )
{
    if (string[i]=='\t')
    {
        ++tabct;
    if (tabct==4)
    {
        for(int j=i; j<(i+100); j++)
        {
            if (string[j]==':')
            {
                strncpy(wlist[message_count].name, string+i, j-i);
                wlist[message_count].name[j-i-2] = '\0';
                break;
            }
            else if (string[j]=='\0')
            {
                strncpy(wlist[message_count].name, string+i, j-i);
                wlist[message_count].name[j-i] = '\0';
            }
        }
        //int len = strlen(wlist[message_count].name);
        //printf("%s\n",wlist[message_count].name);
        message_count++;
        itr++;
        tabct=0;
    }
    }
}
return message_count;
}

static struct wpa_ctrl *ctrl_conn;
static int hostapd_cli_quit = 0;
static int hostapd_cli_attached = 0;
static const char *ctrl_iface_dir = "/var/run/wpa_supplicant/wlp3s0";
static char *ctrl_ifname = NULL;
static int ping_interval = 5;
int main(int argc, char *argv[])
{
  char message[2048];
  ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    if (!ctrl_conn){
        printf("Could not get ctrl interface!\n");
        return -1;
    }
  printf("%d\n", wpa_cli_cmd_scan(ctrl_conn, message));
  printf("%d\n", wpa_cli_cmd_scan_results(ctrl_conn, message));
  wpa_ctrl_close(ctrl_conn);
  ssid wlist[10];
  int ssid_count = ssid_str(message, wlist);
  for (int i=0; i<ssid_count; i++)
    printf("%s\n", wlist[i].name);

  return 0;
}
