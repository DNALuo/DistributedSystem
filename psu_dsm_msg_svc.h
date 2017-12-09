#ifndef __PSU_DSM_MSG_SVC_
#define __PSU_DSM_MSG_SVC_

#define MAX_SERV 40
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

typedef char serverid_t[IP_LEN];
extern unsigned int nserver;
extern serverid_t server[MAX_SERV];

struct dir_entry_t{
	pageid_t page;
	mode_t pbits[MAX_SERV];
	mode_t mode;
	pthread_mutex_t lock;
};

typedef struct dir_entry_t dir_entry_t;

struct dir_t{
	dir_entry_t entry;
	struct dir_t *next;
};

typedef struct dir_t dir_t;

extern dir_t *dir;


struct dsm_entry_t{
	pageid_t page;
	void * addr;
	serverid_t where;
};

typedef struct dsm_entry_t dsm_entry_t;

struct dsm_t{
	dsm_entry_t entry;
	struct dsm_t *next;
};

typedef struct dsm_t dsm_t;

extern dsm_t *dsm;
extern dsm_t *cdsm;

#endif
