#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include "utility.h"


bool get_local_ip(char *buf)
{
  struct ifaddrs *addrs = (struct ifaddrs *)malloc(sizeof(struct ifaddrs));
  getifaddrs(&addrs);
  struct ifaddrs * tmp = addrs;
  buf[0] = '\0';

  while (tmp != NULL)
  {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
    {
      struct sockaddr_in *p_addr = (struct sockaddr_in *)tmp->ifa_addr;
      if((p_addr->sin_addr.s_addr & 0x0000FFFF) == 0x0000A8C0)
      {
        char *ip = inet_ntoa(p_addr->sin_addr);
        strncpy(buf, ip, strlen(ip));
        break;
      }
    }

    tmp = tmp->ifa_next;
  }

  freeifaddrs(addrs);
}