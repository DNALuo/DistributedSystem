#pragma once
#include <rpc/rpc.h>

void get_local_ip_addresses(GArray *buf);
u_quad_t get_mac_address();
CLIENT *create_client(char *host, unsigned int prog, unsigned int vers, char *protocol);