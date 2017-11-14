// Sample Application 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "psu_thread.h"

char REMOTE_IP_ADDR[255];

void print_hostname()
{
  char buffer[100];
  int ret;
  if ((ret = gethostname(buffer, sizeof(buffer))) == -1) {
    perror("gethostname");
    exit(1);
  }
  printf("Hostname: %s\n", buffer);
}

void* foo(void* arg)
{
  print_hostname();
  printf("Foo:Hello.....\n");
  psu_thread_migrate(REMOTE_IP_ADDR);
  print_hostname();
  printf("Foo:.....World\n");
  return NULL;
}

int main(int argc, char* argv[])
{
  if(argc < 2) {
    printf("Usage: toy_app1 <ip_addr_to_migrate_to>\n");
    exit(1);
  }
  printf("Creating thread.\n");
  strcpy(REMOTE_IP_ADDR, argv[1]);
  psu_thread_info_t t_info;
  psu_thread_create(&t_info, foo, NULL);
  printf("Thread created.\n");
}
