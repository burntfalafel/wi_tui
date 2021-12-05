#include "frontend.h"
#include "wpa_ctrl.h"
#include "util.h"

char selected_ssid[SSID_CHAR_MAX];

void func(char *name)
{	move(20, 0);
	clrtoeol();
	//mvprintw(20, 0, "Item selected is : %s", name);
  strcpy(selected_ssid, name);
}

void
trim(char *buffer)
{
    size_t n = strlen(buffer);
    while (n-- && isspace(buffer[n]))
	buffer[n] = 0;
}

void print_menu_help()
{
attron(COLOR_PAIR(2));
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
  attroff(COLOR_PAIR(2));
}

void make_psk_form(MENU *my_menu, WINDOW *my_menu_win, struct wpa_ctrl *ctrl_conn)
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
  set_field_buffer(field[2], 0, "SSID:");
  set_field_buffer(field[3], 0, "PSK:");

  /* Fill RHS fields */
  set_field_buffer(field[0], 0, selected_ssid);

	
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

  /* remove any exsisting wpa connections */
  char message_status[100];
  char network_id[100];
  wpa_cli_cmd_list_networks(ctrl_conn, message_status);
  get_network_id(selected_ssid, message_status, network_id); 
  if(strlen(network_id))
    wpa_cli_cmd_remove_network(ctrl_conn, message_status, network_id);

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
          /* create a new wpa network */
          wpa_cli_cmd_add_network(ctrl_conn, network_id);
          wpa_cli_cmd_set_ssid(ctrl_conn, message_status, network_id, selected_ssid);
          // assert message_status == "OK"
          char *key_needed = field_buffer(field[1], 0);
          if (key_needed != 0 && (key_needed = strdup(key_needed)) != 0 )
          {
            trim(key_needed);
            exit_form_flag = 1;
            if (strcmp(key_needed, "n") || strcmp(key_needed, "N") || strcmp(key_needed, "0"))
            {
              wpa_cli_cmd_no_psk(ctrl_conn, message_status, network_id);
            }
            else
            {
              wpa_cli_cmd_set_psk(ctrl_conn, message_status, network_id, key_needed);
            }
            free(key_needed);
            break;
          }
          wpa_cli_cmd_enable_network(ctrl_conn, message_status, network_id);
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

	mvprintw(LINES - 4, 0, "If no Wifi password, enter '0'/'n'/'N' in PSK: field");
  print_menu_help();
  post_menu(my_menu);
  wrefresh(my_menu_win);
}


int make_ssid_menu(ssid *wlist, int ssid_count, struct wpa_ctrl *ctrl_conn)
{
  char message[2048];
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
	mvprintw(LINES - 4, 0, "Press F5 to refresh");
  print_menu_help();
  post_menu(my_menu);
	refresh();
  wrefresh(my_menu_win);


  int c = 0;
  int refresh_menu = 0;
  int exit_menu_flag = 0;
	while( (c != KEY_F(1)) && !exit_menu_flag)
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
              /* go up */
			      	menu_driver(my_menu, REQ_UP_ITEM);
			      	break;
            case KEY_F(5):
              /* refresh */
              refresh_menu = 1;
              exit_menu_flag = 1;
              break;
			      case 10: /* Enter */
			      {	
              ITEM *cur;
				      void (*p)(char *);
				      cur = current_item(my_menu);
				      p = item_userptr(cur);
				      p((char *)item_name(cur));
				      pos_menu_cursor(my_menu);
              make_psk_form(my_menu, my_menu_win, ctrl_conn);
				      break;
			      }
            case KEY_F(2):
              wpa_cli_cmd_status(ctrl_conn, message);
              if(get_network_status(message))
                wpa_cli_cmd_disconnect(ctrl_conn, message);
              else
                wpa_cli_cmd_reconnect(ctrl_conn, message);

              break;

			    break;
		      }
          wrefresh(my_menu_win);

	}

  /* free objects from ssid menu */
	free_menu(my_menu);
	for(int i = 0; i < ssid_count; ++i)
		free_item(ssid_items[i]);

  /* save progress to wpa */
  wpa_cli_cmd_save_config(ctrl_conn, message);

  if(refresh_menu)
    return 1;
  return 0;

}
