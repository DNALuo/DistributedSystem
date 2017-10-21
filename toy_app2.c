// Sample Application 2
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

void baz()
{
  print_hostname();
  printf("Baz:Hello.....\n");
  psu_thread_migrate(REMOTE_IP_ADDR);
  print_hostname();
  printf("Baz:.....World\n");
}

void bar()
{
  int var1=100;
  int var2 = var1+200;
  print_hostname();
  printf("%d, %d\n", var1, var2);
  printf("Bar:Hello.....\n");
  baz();
  print_hostname();
  printf("%d, %d\n", var1, var2);
  printf("Bar:.....World\n");
}

void* foo(void* arg)
{
  int var1=142, var2=252;
  print_hostname();
  printf("%d, %d\n", var1, var2);
  printf("Foo:Hello.....\n");
  bar();
  print_hostname();
  printf("%d, %d\n", var1, var2);
  printf("Foo:.....World\n");
  printf("World in Foo\n");
  return NULL;
}

int main(int argc, char* argv[])
{
  if(argc < 2) {
    printf("Usage: toy_app2 <ip_addr_to_migrate_to>\n");
    exit(1);
  }
  strcpy(REMOTE_IP_ADDR, argv[1]);
  psu_thread_info_t t_info;
  psu_thread_create(&t_info, foo, NULL);
}
