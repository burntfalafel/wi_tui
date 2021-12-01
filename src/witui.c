#include "includes.h"

#include "common.h"
#include "frontend.h"
#include "util.h"
#include "wpa_ctrl.h"
#include "witui.h"

//static struct wpa_ctrl *ctrl_conn;
static int hostapd_cli_quit = 0;
static int hostapd_cli_attached = 0;
static const char *ctrl_iface_dir = "/var/run/wpa_supplicant/wlp3s0";
static char *ctrl_ifname = NULL;
static int ping_interval = 5;

int main(int argc, char *argv[])
{
  /* setup & scan wpa signals */
  /* 
  char message[2048];
  ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    if (!ctrl_conn){
        printf("Could not get ctrl interface!\n");
        return -1;
    }
  printf("%d\n", wpa_cli_cmd_scan(ctrl_conn, message));
  printf("%d\n", wpa_cli_cmd_scan_results(ctrl_conn, message));
  wpa_ctrl_close(ctrl_conn);
  */
  /* end of wpa signals */

  /* parse signal names */
  /*
  ssid wlist[SSID_NUM];
  int ssid_count = ssid_str(message, wlist);
  for (int i=0; i<ssid_count; i++)
    printf("%s\n", wlist[i].name);
    */

char *FIND_SSID = "Halo2";
char pp[3];
char message[2048];
get_network_id(FIND_SSID, message, pp);

  /* initialize curses menu */
  ssid wlist[10];
  int ssid_count = 10;
  for(int i=0; i<ssid_count; i++)
  {
    char snum[10];
    sprintf(snum, "Wifi-%d", i);
    strcpy(wlist[i].name, snum );
  }

  initscr();
  while(make_ssid_menu(wlist, ssid_count))
  {
    strcpy(wlist[0].name,"TEST");
  }

	endwin();


  return 0;
}
