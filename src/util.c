#include "util.h"

int ssid_str(char* string, ssid *wlist )
{
  /* parses the *string* list recieved from WPA 
   * and returns a *ssid* list of all WPA names
   */
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
        /* fourth occurance in WPA's list_networks return string */
          for(int j=i; j<(i+100); j++)
          {
              if (string[j]==':')
              {
                /* if ssid in between */
                  strncpy(wlist[message_count].name, string+i+1, j-i-4); /* displacement carefully chosen */
                  wlist[message_count].name[j-i-4] = '\0'; /* null terminate string */
                  break;
              }
              else if (string[j]=='\0')
              {
                /* if ssid last in string */
                  strncpy(wlist[message_count].name, string+i+1, j-i-2);
                  wlist[message_count].name[j-i-2] = '\0';
                  break;
              }
          }
          message_count++;
          itr++;
          tabct=0;
        }
      }
  }
  return message_count;
}

void get_network_id(char *ssid, char *list_networks, char *network_id)
{
  /* gets *network_id* string from a *ssid* string query on 
   * *list_networks* list of networks
   */
  char snum[100];
  sprintf(snum, "\t%s\tany", ssid);
  char *strfound = strstr(list_networks, snum);
  if (strfound)
  {
    if((*(strfound-2) >= '0') && (*(strfound-2) <= '9'))
    {
      /* double digit network_id handling */
      network_id[0] = *(strfound-2);
      network_id[1] = *(strfound-1);
      network_id[2] = '\0';
    }
    else
    {
      /* single digit network_id handling */
      network_id[0] = *(strfound-1);
      network_id[1] = '\0';
    }
  }
}

int get_network_status(char *message)
{
  /* get connection status from WPA's status query 
   */
  char snum[30];
  sprintf(snum, "COMPLETED"); /* connection established */
  char *strfound = strstr(message, snum);
  if (strfound)
  {
    return 1;
  }
  return 0;
}

