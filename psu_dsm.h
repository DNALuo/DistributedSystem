#ifndef _PSU_DSM_H_
#define _PSU_DSM_H_

typedef void* psu_dsm_ptr_t;

//void psu_dsm_init(void);

psu_dsm_ptr_t psu_dsm_malloc(char* name, int size);

void psu_dsm_free(char* name);

#endif
