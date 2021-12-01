#ifndef FRONTEND_H
#define FRONTEND_H

#include "includes.h"
#include "witui.h"

/* global variable so will be set to NULL */
char selected_ssid[100];

static void func(char *name);

static void trim(char *buffer);


static void print_menu_help();


static void make_psk_form(MENU *my_menu, WINDOW *my_menu_win);


static int make_ssid_menu(ssid *wlist, int ssid_count);

#endif
