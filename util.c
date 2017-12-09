#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "psu_dsm_msg.h"
#include "psu_dsm_msg_svc.h"

unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void get_local_ipaddr(char ip[IP_LEN])
{
	int fd;	
	struct ifreq ifr;
     
	char iface[] = "eth0";
     
   	fd = socket(AF_INET, SOCK_DGRAM, 0);
   
   	ifr.ifr_addr.sa_family = AF_INET;
         
   	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
                  
	ioctl(fd, SIOCGIFADDR, &ifr);
                       
    	close(fd);
    
     	strncpy(ip, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), IP_LEN);
 }



// search if page is inside local dir
// return 1 if found, 0 if not found
dir_t*
search_dir(pageid_t page, dir_t *dir)
{
	dir_t *p = dir;
	while(p != NULL){
		if(strncmp(p->entry.page.name, page.name, NAME_LEN) == 0){
			return p;
		}
		p = p->next;
      	}
	return NULL;
}


void
print_dir(dir_t *dir)
{
	dir_t *p = dir;
	printf("dir = \n");
	while(p != NULL){
		printf("host %s\n", p->entry.page.name);
		p = p->next;
      	}
}





// insert a page into local dir
// return 1 if success, 0 if fail
int
insert_dir(dir_entry_t entry, dir_t *dir)
{
	printf("insert_dir called.\n");
	dir_t *p = dir;
	dir_t *pp = dir;
	while(p != NULL){
		pp = p;
		p = p->next;	
      	}

	dir_t *new = malloc(sizeof(dir_t));
	if(new == NULL){
		perror("malloc");
		exit(1);
	}
	
	strncpy(new->entry.page.name, entry.page.name, NAME_LEN);
	new->entry.page.size = entry.page.size;
	int i;
	for(i = 0; i < MAX_SERV; i++){
		new->entry.pbits[i] = entry.pbits[i];
	}
	new->entry.mode = entry.mode;

	new->next = NULL;

	if(pp == NULL)
		dir = new;
	else
		pp->next = new;
}


dsm_t*
search_dsm(pageid_t page, dsm_t *dsm)
{
	dsm_t *p = dsm;
	while(p != NULL){
		if(strncmp(p->entry.page.name, page.name, NAME_LEN) == 0){
			return p;
		}
		p = p->next;
      	}
	return NULL;
}

void
print_dsm(dsm_t *dsm)
{
	dsm_t *p = dsm;
	printf("dsm = \n");
	while(p != NULL){
		printf("host %s\n", p->entry.page.name);
		p = p->next;
      	}
}

// insert a page into local dir
// return 1 if success, 0 if fail
void
insert_dsm(pageid_t page, char where[IP_LEN], dsm_t* dsm)
{
	printf("insert_dir called.\n");
	dsm_t *p = dsm;
	dsm_t *pp = dsm;
	while(p != NULL){
		pp = p;
		p = p->next;	
      	}

	dsm_t *new = malloc(sizeof(dsm_t));
	if(new == NULL){
		perror("malloc");
		exit(1);
	}
	
	strncpy(new->entry.page.name, page.name, NAME_LEN);
	new->entry.page.size = page.size;
	strncpy(new->entry.where, where, IP_LEN);
	
	if(pp == NULL){
		dsm = new;
	}
	else{
		pp->next = new;
	}
}


int
get_server_id(char host[IP_LEN])
{
	int i;
	for(i = 0; i < MAX_SERV; i++){
		if(strncmp(server[i], host, IP_LEN) == 0)
		return i;
	} 
	return -1;
}

void get_name_from_addr(dsm_entry_t *dentry, void *addr, dsm_t *dsm)
{
	dsm_t *p = dsm;

	while(p != NULL){
		if(addr >= p->entry.addr && addr < p->entry.addr + PAGE_SIZE)
		strncpy(dentry->page.name, p->entry.page.name, NAME_LEN);
		dentry->page.size = p->entry.page.size;
		dentry->addr = p->entry.addr;
		strncpy(dentry->where, p->entry.where, IP_LEN);
	}

}
