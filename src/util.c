#include "util.h"

int ssid_str(char* string, ssid *wlist )
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
void get_network_id(char *ssid, char *list_networks, char *network_id)
{
  char snum[100];
  sprintf(snum, "\t%s\tany", ssid);
  char *strfound = strstr(list_networks, ssid);
  if (strfound)
  {
    if((*(strfound-3) >= '0') && (*(strfound-3) <= '9'))
    {
      network_id[0] = *(strfound-3);
      network_id[1] = *(strfound-2);
      network_id[2] = '\0';
    }
    else
    {
      network_id[0] = *(strfound-2);
      network_id[1] = '\0';
    }
  }
}
