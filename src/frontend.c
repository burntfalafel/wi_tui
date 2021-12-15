#include "frontend.h"
#include "wpa_ctrl.h"
#include "util.h"
#include <curses.h>
#include <form.h>

char selected_ssid[SSID_CHAR_MAX];

void
func(char* name) {
    /* function callback to shared ssid name between menu and form */
    move(20, 0);
    clrtoeol();
    //mvprintw(20, 0, "Item selected is : %s", name);
    strcpy(selected_ssid, name);
}

void
trim(char* buffer) {
    /* get rid of any trailing spaces in PSK key entered */
    size_t n = strlen(buffer);
    while (n-- && isspace(buffer[n]))
    { buffer[n] = 0; }
}

void
print_menu_help() {
    /* print common menu help entries used by menu and form */
    attron(COLOR_PAIR(2));
    mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
    attroff(COLOR_PAIR(2));
}

void
print_ssid_help() {
    attron(COLOR_PAIR(1));
    mvprintw(LINES - 5, 0, "Press F5 to refresh");
    mvprintw(LINES - 4, 0, "Press F2 to switch off/on Wifi device");
    attroff(COLOR_PAIR(1));
    print_menu_help();
}

void
make_psk_form(MENU* ssid_menu, WINDOW* ssid_menu_win, struct wpa_ctrl* ctrl_conn) {
    /* Locate form */
    int height = 3;
    int width = 10;
    int starty = (LINES - height) / 2;    /* Calculating for a center placement */
    int startx = (COLS - width) / 2;    /* of the window        */

    unpost_menu(ssid_menu);
    wrefresh(ssid_menu_win);
    /* Initialize the fields */
    FIELD* field[5];
    field[0] = new_field(1, 26, 1, 10, 0, 0);
    field[1] = new_field(1, 26, 4, 10, 0, 0);
    field[2] = new_field(1, 5, starty - 23, 3, 0, 0);
    field[3] = new_field(1, 5, starty - 20, 3, 0, 0);
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
    FORM* psk_form = new_form(field);

    /* Calculate the area required for the form */
    int ch = 0, rows, cols;
    scale_form(psk_form, &rows, &cols);

    /* Create the window to be associated with the form */
    WINDOW* psk_form_win = newwin(rows + 4, cols + 4, starty - 20, startx - 13);
    keypad(psk_form_win, TRUE);

    /* Set main window and sub window */
    set_form_win(psk_form, psk_form_win);
    set_form_sub(psk_form, derwin(psk_form_win, rows, cols, 2, 2));

    /* Print a border around the main window and print a title */
    box(psk_form_win, 0, 0);

    post_form(psk_form);
    wrefresh(psk_form_win);

    attron(COLOR_PAIR(1));
    mvprintw(LINES - 4, 0, "If no Wifi password, enter '0'/'n'/'N' in PSK: field");
    attroff(COLOR_PAIR(1));
    print_menu_help();
    refresh();

    /* create any local variables to be shared among wpa_ctrl method calls */
    char message_status[100];
    char network_id[100];

    /* Loop through to get user requests */
    int exit_form_flag = 0;
    while (ch != KEY_F(1) && !exit_form_flag) {
        if (ch == ERR)
        { continue; }
        ch = wgetch(psk_form_win);
        switch (ch) {
            case KEY_DOWN:
                /* Go to next field */
                form_driver(psk_form, REQ_NEXT_FIELD);
                /* Go to the end of the present buffer */
                /* Leaves nicely at the last character */
                form_driver(psk_form, REQ_END_LINE);
                break;
            case KEY_UP:
                /* Go to previous field */
                form_driver(psk_form, REQ_PREV_FIELD);
                form_driver(psk_form, REQ_END_LINE);
                break;
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                /* Backspace the char under the cursor */
                form_driver(psk_form, REQ_DEL_PREV);
                break;
            case KEY_DC:
                /* Delete the char under the cursor */
                form_driver(psk_form, REQ_DEL_CHAR);
                break;
            case 10:
                /* Enter */
                form_driver(psk_form, REQ_VALIDATION);
                /* remove any exsisting wpa connections */
                wpa_cli_cmd_list_networks(ctrl_conn, message_status);
                get_network_id(selected_ssid, message_status, network_id);
                if (strlen(network_id))
                { wpa_cli_cmd_remove_network(ctrl_conn, message_status, network_id); }
                /* create a new wpa network */
                wpa_cli_cmd_add_network(ctrl_conn, network_id);
                wpa_cli_cmd_set_ssid(ctrl_conn, message_status, network_id, selected_ssid);
                // assert message_status == "OK"
                char* key_needed = field_buffer(field[1], 0);
                if (key_needed != 0 && (key_needed = strdup(key_needed)) != 0 ) {
                    trim(key_needed);
                    if (!strcmp(key_needed, "n") || !strcmp(key_needed, "N") || !strcmp(key_needed, "0")) {
                        wpa_cli_cmd_no_psk(ctrl_conn, message_status, network_id);
                    } else {
                        wpa_cli_cmd_set_psk(ctrl_conn, message_status, network_id, key_needed);
                    }
                    free(key_needed);
                    wpa_cli_cmd_enable_network(ctrl_conn, message_status, network_id);
                    exit_form_flag = 1;
                }
                break;
            default:
                /* If this is a normal character, it gets */
                /* Printed                */
                form_driver(psk_form, ch);
                break;
        }
        refresh();
        wrefresh(psk_form_win);

    }
    /* Un post form and free the memory */
    unpost_form(psk_form);
    free_form(psk_form);
    for (int i = 0; i < 5; i++)
    { free_field(field[i]); }
    clear();
    refresh();
    wrefresh(psk_form_win);
    delwin(psk_form_win);

    print_ssid_help();
    post_menu(ssid_menu);
    wrefresh(ssid_menu_win);
}


int
make_ssid_menu(ssid* wlist, int ssid_count, struct wpa_ctrl* ctrl_conn) {
    char message[2048];
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    ITEM** ssid_items = (ITEM**)calloc(ssid_count + 1, sizeof(ITEM*));

    for (int i = 0; i < ssid_count; ++i) {
        ssid_items[i] = new_item(wlist[i].name, NULL);
        /* Set the user pointer */
        set_item_userptr(ssid_items[i], func);
    }
    ssid_items[ssid_count] = (ITEM*)NULL;

    /* Locate menu */
    int height = 3;
    int width = 10;
    int starty = (LINES - height) / 2;  /* Calculating for a center placement */
    int startx = (COLS - width) / 2;    /* of the window        */
    /* Create menu */
    MENU* ssid_menu = new_menu((ITEM**)ssid_items);

    /* Create the window to be associated with the menu */
    WINDOW* ssid_menu_win = newwin(100, 100, starty - 20, startx);

    /* Set main window and sub window */
    set_menu_win(ssid_menu, ssid_menu_win);
    set_menu_sub(ssid_menu, derwin(ssid_menu_win, 50, 50, 3, 1));
    /* Post the menu */
    print_ssid_help();
    post_menu(ssid_menu);
    refresh();
    wrefresh(ssid_menu_win);

    int c = 0;
    int refresh_menu = 0;
    int exit_menu_flag = 0;
    while ( (c != KEY_F(1)) && !exit_menu_flag) {
        c = getch();
        switch (c) {
            case 106:
            case KEY_DOWN:
                /* go down */
                menu_driver(ssid_menu, REQ_DOWN_ITEM);
                break;
            case 107:
            case KEY_UP:
                /* go up */
                menu_driver(ssid_menu, REQ_UP_ITEM);
                break;
            case KEY_F(5):
                /* refresh */
                refresh_menu = 1;
                exit_menu_flag = 1;
                break;
            case 10:
                /* Enter */
            {
                ITEM* cur;
                void (*p)(char*);
                cur = current_item(ssid_menu);
                p = item_userptr(cur);
                p((char*)item_name(cur));
                pos_menu_cursor(ssid_menu);
                make_psk_form(ssid_menu, ssid_menu_win, ctrl_conn);
                /* save progress to wpa */
                wpa_cli_cmd_save_config(ctrl_conn, message);
                break;
            }
            case KEY_F(2):
                /* toggle connection */
                wpa_cli_cmd_status(ctrl_conn, message);
                if (get_network_status(message))
                { wpa_cli_cmd_disconnect(ctrl_conn, message); }
                else
                { wpa_cli_cmd_reconnect(ctrl_conn, message); }

                break;

                break;
        }
        wrefresh(ssid_menu_win);

    }

    /* free objects from ssid menu */
    free_menu(ssid_menu);
    for (int i = 0; i < ssid_count; ++i)
    { free_item(ssid_items[i]); }

    if (refresh_menu)
    { return 1; } /* refresh ssid menu */
    return 0; /* exit ssid menu */

}
