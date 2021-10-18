#include "includes.h"

#include "wpa_ctrl.h"
#include "common.h"
//#include "common/version.h"
static struct wpa_ctrl *ctrl_conn;
static void wpa_cli_msg_cb(char *msg, size_t len)
{
  printf("%s\n", msg);
}
static int _wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, int print)
{
        char buf[2048];
        size_t len;
        int ret;

        if (ctrl_conn == NULL) {
                printf("Not connected to wpa_supplicant - command dropped.\n");
                return -1;
        }
        len = sizeof(buf) - 1;
        ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len,
                               wpa_cli_msg_cb);
        if (ret == -2) {
                printf("'%s' command timed out.\n", cmd);
                return -2;
        } else if (ret < 0) {
                printf("'%s' command failed.\n", cmd);
                return -1;
        }
        if (print) {
                buf[len] = '\0';
                printf("%s", buf);
        }
        return 0;
}


static int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd)
{
        return _wpa_ctrl_command(ctrl, cmd, 1);
}
static int wpa_cli_cmd_scan_results(struct wpa_ctrl *ctrl)
{
return wpa_ctrl_command(ctrl, "SCAN_RESULTS");
}

static struct wpa_ctrl *ctrl_conn;
static int hostapd_cli_quit = 0;
static int hostapd_cli_attached = 0;
static const char *ctrl_iface_dir = "/var/run/wpa_supplicant";
static char *ctrl_ifname = NULL;
static int ping_interval = 5;
int main(int argc, char *argv[])
{
  ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    if (!ctrl_conn){
        printf("Could not get ctrl interface!\n");
        return -1;
    }
  printf("%d\n", wpa_cli_cmd_scan_results(ctrl_conn));
  return 0;
}
