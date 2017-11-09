#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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


#ifdef AF_LINK
#include <net/if_dl.h>
uint8_t *get_ptr(struct ifaddrs *ifaptr)
{
  if (ifaptr->ifa_addr->sa_family == AF_LINK)
    return (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
  else
    return NULL;
}
#endif
#ifdef AF_PACKET
#include <netpacket/packet.h>
uint8_t *get_ptr(struct ifaddrs *ifaptr)
{
    if (ifaptr->ifa_addr->sa_family == AF_PACKET)
        return (unsigned char *)((struct sockaddr_ll*)ifaptr->ifa_addr)->sll_addr;
    else
      return NULL;
}
#endif

u_quad_t get_mac_address()
{
  struct ifaddrs *ifap;
  u_quad_t result = 0;
  uint8_t *ptr = NULL;

  if (getifaddrs(&ifap) == 0)
  {
    for (struct ifaddrs *ifaptr = ifap; ifaptr != NULL; ifaptr = ifaptr->ifa_next)
    {
      ptr = get_ptr(ifaptr);

      if(ptr != NULL)
      {
        for (int i = 5; i >= 0; --i)
          result |= (u_quad_t) *ptr++ << (CHAR_BIT * i);
        if(result != 0)
          break;
      }
    }
    freeifaddrs(ifap);
  }
  return result;
}