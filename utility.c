#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib.h>
#include <string.h>
#include "utility.h"

CLIENT *create_client(char *host, unsigned int prog, unsigned int vers, char *protocol)
{

  CLIENT *client = clnt_create(host, prog, vers, protocol);
  struct timeval TIMEOUT = { 60 * 60 * 24, 0 };
  if(client == NULL)
  {
    clnt_pcreateerror(host);
    exit(1);
  };
  if(strcmp(protocol, "udp") == 0)
    clnt_control(client, CLSET_RETRY_TIMEOUT, (char *)&TIMEOUT);

  clnt_control(client, CLSET_TIMEOUT, (char *)&TIMEOUT);

  return client;
}

void get_local_ip_addresses(GArray *buf)
{
  struct ifaddrs *addrs = (struct ifaddrs *)malloc(sizeof(struct ifaddrs));
  getifaddrs(&addrs);

  for(struct ifaddrs *tmp = addrs; tmp != NULL; tmp = tmp->ifa_next)
  {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
    {
      struct sockaddr_in *p_addr = (struct sockaddr_in *)tmp->ifa_addr;
      char *ip_address = (char *)malloc(sizeof(char) * 17);
      inet_ntop(AF_INET, &(p_addr->sin_addr), ip_address, INET_ADDRSTRLEN);
      g_array_append_val(buf, ip_address);
    }
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