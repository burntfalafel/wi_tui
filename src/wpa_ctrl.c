#include "wpa_ctrl.h"

void wpa_cli_msg_cb(char *msg, size_t len)
{
  printf("%s\n", msg);
}
int _wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char *message)
{
        size_t len;
        int ret;

        if (ctrl == NULL) {
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


int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char *message)
{
        return _wpa_ctrl_command(ctrl, cmd, message);
}

int wpa_cli_cmd_scan(struct wpa_ctrl *ctrl, char *message)
{
         return wpa_ctrl_command(ctrl, "SCAN", message);
}
int wpa_cli_cmd_scan_results(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "SCAN_RESULTS", message);
}
int wpa_cli_cmd_list_networks(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "LIST_NETWORKS", message);
}
int wpa_cli_cmd_add_network(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "ADD_NETWORK", message);
}
int wpa_cli_cmd_remove_network(struct wpa_ctrl *ctrl, char *message, char *network_id)
{
  char cmd[100];
  sprintf(cmd, "REMOVE_NETWORK %s", network_id);

  return wpa_ctrl_command(ctrl, cmd, message);
}
int wpa_cli_cmd_set_ssid(struct wpa_ctrl *ctrl, char *message, char *network_id, char *ssid)
{
  char cmd[100];
  sprintf(cmd, "SET_NETWORK %s ssid \"%s\"", network_id, ssid);

  return wpa_ctrl_command(ctrl, cmd, message);
}
int wpa_cli_cmd_set_psk(struct wpa_ctrl *ctrl, char *message, char *network_id, char *psk)
{
    char cmd[100];
  sprintf(cmd, "SET_NETWORK %s psk \"%s\"", network_id, psk);

  return wpa_ctrl_command(ctrl, cmd, message);
}
int wpa_cli_cmd_no_psk(struct wpa_ctrl *ctrl, char *message, char *network_id)
{
  char cmd[100];
  sprintf(cmd, "SET_NETWORK %s key_mgmt NONE", network_id);
  
  return wpa_ctrl_command(ctrl, cmd, message);
}
int wpa_cli_cmd_enable_network(struct wpa_ctrl *ctrl, char *message, char *network_id)
{
  char cmd[100];
  sprintf(cmd, "ENABLE_NETWORK %s", network_id);
  
  return wpa_ctrl_command(ctrl, cmd, message);
}
int wpa_cli_cmd_save_config(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "SAVE_CONFIG", message);
}
int wpa_cli_cmd_disconnect(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "DISCONNECT", message);
}
int wpa_cli_cmd_reconnect(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "RECONNECT", message);
}
int wpa_cli_comd_status(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "STATUS", message);
}
