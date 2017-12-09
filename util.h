#ifndef _UTIL_H_
#define _UTIL_H_

void get_local_ipaddr(char ip[IP_LEN]);

unsigned long hash(unsigned char *str);

dir_t* search_dir(pageid_t page, dir_t *dir);

void print_dir(dir_t *dir);

int insert_dir(dir_entry_t entry, dir_t *dir);

dsm_t* search_dsm(pageid_t page, dsm_t *dsm);

void print_dsm(dsm_t *dsm);

void insert_dsm(pageid_t page, char where[IP_LEN], dsm_t *dsm);

int get_server_id(char host[IP_LEN]);

void get_name_from_addr(dsm_entry_t *dest, void *addr, dsm_t *dsm);
#endif
