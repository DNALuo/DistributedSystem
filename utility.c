#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include "utility.h"

void get_local_ip_addresses(GArray *buf)
{
  struct ifaddrs *addrs = (struct ifaddrs *)malloc(sizeof(struct ifaddrs));
  getifaddrs(&addrs);
  struct ifaddrs *tmp = addrs;

  while (tmp != NULL)
  {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
    {
      struct sockaddr_in *p_addr = (struct sockaddr_in *)tmp->ifa_addr;
      char *ip_address = (char *)malloc(sizeof(char) * 17);
      g_array_append_val(buf, ip_address);
      char *ip = inet_ntoa(p_addr->sin_addr);
      strncpy(ip_address, ip, strlen(ip));
    }

    tmp = tmp->ifa_next;
  }
  freeifaddrs(addrs);
}

bool compare_ip(char *ip1, char *ip2)
{
  struct in_addr *ip1_addr = (struct in_addr *)malloc(sizeof(struct in_addr));
  struct in_addr *ip2_addr = (struct in_addr *)malloc(sizeof(struct in_addr));

  inet_aton(ip1, ip1_addr);
  inet_aton(ip2, ip2_addr);

  bool result = ip1_addr->s_addr < ip2_addr->s_addr;

  free(ip1_addr);
  free(ip2_addr);

  return result;
}