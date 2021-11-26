#include "includes.h"

#include "wpa_ctrl.h"
#include "common.h"
#include "witui.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
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
static int wpa_cli_cmd_add_network(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "ADD_NETWORK", message);
}
static int wpa_cli_cmd_set_ssid(struct wpa_ctrl *ctrl, char *message, char *network_id, char *ssid)
{
  char cmd[100];
  sprintf(cmd, "SET_NETWORK %s ssid \"%s\"", network_id, ssid);

  return wpa_ctrl_command(ctrl, cmd, message);
}
static int wpa_cli_cmd_set_psk(struct wpa_ctrl *ctrl, char *message, char *network_id, char *psk)
{
    char cmd[100];
  sprintf(cmd, "SET_NETWORK %s psk \"%s\"", network_id, psk);

  return wpa_ctrl_command(ctrl, cmd, message);
}
static int wpa_cli_cmd_no_psk(struct wpa_ctrl *ctrl, char *message, char *network_id)
{
  char cmd[100];
  sprintf(cmd, "SET_NETWORK %s key_mgmt NONE", network_id);
  
  return wpa_ctrl_command(ctrl, cmd, message);
}
static int wpa_cli_cmd_enable_network(struct wpa_ctrl *ctrl, char *message, char *network_id)
{
  char cmd[100];
  sprintf(cmd, "ENABLE_NETWORK %s", network_id);
  
  return wpa_ctrl_command(ctrl, cmd, message);
}
static int wpa_cli_cmd_save_config(struct wpa_ctrl *ctrl, char *message)
{
  return wpa_ctrl_command(ctrl, "SAVE_CONFIG", message);
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

char selected_ssid[100];
void func(char *name)
{	move(20, 0);
	clrtoeol();
	//mvprintw(20, 0, "Item selected is : %s", name);
  strcpy(selected_ssid, name);
}

void make_psk_form(MENU *my_menu, WINDOW *my_menu_win)
{
  /* Locate form */
  int height = 3;
	int width = 10;
  int starty = (LINES - height) / 2;	/* Calculating for a center placement */
	int startx = (COLS - width) / 2;	/* of the window		*/

  unpost_menu(my_menu);
  wrefresh(my_menu_win);
  /* Initialize the fields */
  FIELD *field[5];
  /* Initialize the fields */
  field[0] = new_field(1, 26, 1 , 10, 0, 0);
  field[1] = new_field(1, 26, 4, 10, 0, 0);
  field[2] = new_field(1, 5, starty-23, 3, 0, 0);
  field[3] = new_field(1, 5, starty-20, 3, 0, 0);
	field[4] = NULL;

	/* Set field options */
  field_opts_off(field[0], O_AUTOSKIP);
	set_field_back(field[0], A_UNDERLINE);
	set_field_back(field[1], A_UNDERLINE); 
  field_opts_off(field[1], O_AUTOSKIP);
  field_opts_off(field[2], O_BLANK | O_EDIT | O_ACTIVE);
  field_opts_off(field[3], O_BLANK | O_EDIT | O_ACTIVE);

  /* Fill LHS menu */
  set_field_buffer(field[2], 0, "KEY?:");
  set_field_buffer(field[3], 0, "PSK:");
	
	/* Create the form and post it */
	FORM *my_form = new_form(field);
	
	/* Calculate the area required for the form */
	int ch = 0, rows, cols;
	scale_form(my_form, &rows, &cols);

	/* Create the window to be associated with the form */
  WINDOW *my_form_win = newwin(rows + 4, cols + 4, starty-20, startx-13);
  keypad(my_form_win, TRUE);

	/* Set main window and sub window */
  set_form_win(my_form, my_form_win);
  set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));

	/* Print a border around the main window and print a title */
  box(my_form_win, 0, 0);
	//print_in_middle(my_form_win, 1, 0, cols + 4, "My Form", COLOR_PAIR(1));
	
	post_form(my_form);
	wrefresh(my_form_win);

	mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
	refresh();

  /* Clear fields */
	/* Loop through to get user requests */
  int exit_form_flag=0;
	while(ch != KEY_F(1) && !exit_form_flag)
	{	
    ch = wgetch(my_form_win);
    switch(ch)
		{	case KEY_DOWN:
				/* Go to next field */
				form_driver(my_form, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(my_form, REQ_PREV_FIELD);
				form_driver(my_form, REQ_END_LINE);
				break;
      case KEY_BACKSPACE:
      case 127:
      case '\b':
        form_driver(my_form, REQ_DEL_PREV);
        break;
      // Delete the char under the cursor
		  case KEY_DC:
			  form_driver(my_form, REQ_DEL_CHAR);
			  break;
      case 10:
        {
        char *key_needed = field_buffer(field[0], 0);
        if (key_needed != 0 && (key_needed = strdup(key_needed)) != 0 )
        {
          trim(key_needed);
          exit_form_flag = 1;
          if (strcmp(key_needed, "y") || strcmp(key_needed, "Y") || strcmp(key_needed, "1"))
            break;
          else
            break;
          free(key_needed);
        }
        }
        break;
			default:
				/* If this is a normal character, it gets */
				/* Printed				  */	
				form_driver(my_form, ch);
				break;
		}
  refresh();
  wrefresh(my_form_win);

	}
  /* Un post form and free the memory */
	unpost_form(my_form);
	free_form(my_form);
  for (int i=0; i<5; i++)
	  free_field(field[i]);
  clear();
  refresh();
  wrefresh(my_form_win);
  delwin(my_form_win);

}

static void make_ssid_menu(ssid *wlist, int ssid_count)
{
  start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
  ITEM **ssid_items = (ITEM **)calloc(ssid_count + 1, sizeof(ITEM *));

  for(int i = 0; i < ssid_count; ++i)
	{       
    ssid_items[i] = new_item(wlist[i].name, NULL);
		/* Set the user pointer */
		set_item_userptr(ssid_items[i], func);
	}
  ssid_items[ssid_count] = (ITEM *)NULL;

  /* Locate menu */
  int height = 3;
	int width = 10;
	int starty = (LINES - height) / 2;	/* Calculating for a center placement */
	int startx = (COLS - width) / 2;	/* of the window		*/
	/* Create menu */
	MENU *my_menu = new_menu((ITEM **)ssid_items);

  /* Create the window to be associated with the menu */
  WINDOW *my_menu_win = newwin(100, 100, starty-20, startx);

  /* Set main window and sub window */
  set_menu_win(my_menu, my_menu_win);
  set_menu_sub(my_menu, derwin(my_menu_win, 50, 50, 3, 1));
  /* Post the menu */
  attron(COLOR_PAIR(2));
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
  attroff(COLOR_PAIR(2));
	post_menu(my_menu);
	refresh();
  wrefresh(my_menu_win);


  int c = 0;
	while( c != KEY_F(1) )
	{       
    c = getch();
    switch(c)
	        {	
            case 106: 
            case KEY_DOWN:
				      menu_driver(my_menu, REQ_DOWN_ITEM);
				      break;
            case 107:
			      case KEY_UP:
			      	menu_driver(my_menu, REQ_UP_ITEM);
			      	break;
			      case 10: /* Enter */
			      {	ITEM *cur;
				      void (*p)(char *);
				      cur = current_item(my_menu);
				      p = item_userptr(cur);
				      p((char *)item_name(cur));
				      pos_menu_cursor(my_menu);
              make_psk_form(my_menu, my_menu_win);
				      break;
			      }

			    break;
		      }
          wrefresh(my_menu_win);

	}

  /* free objects from ssid menu */
	free_menu(my_menu);
	for(int i = 0; i < ssid_count; ++i)
		free_item(ssid_items[i]);

}

static struct wpa_ctrl *ctrl_conn;
static int hostapd_cli_quit = 0;
static int hostapd_cli_attached = 0;
static const char *ctrl_iface_dir = "/var/run/wpa_supplicant/wlp3s0";
static char *ctrl_ifname = NULL;
static int ping_interval = 5;

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

static void
trim(char *buffer)
{
    size_t n = strlen(buffer);
    while (n-- && isspace(buffer[n]))
	buffer[n] = 0;
}

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
  make_ssid_menu(wlist, ssid_count);

	endwin();


  return 0;
}


