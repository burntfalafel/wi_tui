#ifndef FRONTEND_H_
#define FRONTEND_H_

#include "includes.h"
#include "witui.h"

void func(char *name);

void trim(char *buffer);


struct wpa_ctrl;

void print_menu_help();

void make_psk_form(MENU *my_menu, WINDOW *my_menu_win, struct wpa_ctrl *ctrl);

int make_ssid_menu(ssid *wlist, int ssid_count, struct wpa_ctrl *ctrl);

#endif
