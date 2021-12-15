#ifndef UTIL_H
#define UTIL_H
#include "includes.h"
#include "witui.h"

int ssid_str(char* string, ssid* wlist );
void get_network_id(char* ssid, char* list_networks, char* network_id);
int get_network_status(char* message);

#endif
