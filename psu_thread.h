#pragma once

// This is the structure used to store the thread information.
// It can contain details about the stack, current context of execution, shared memory locations etc.
typedef struct psu_thread_info psu_thread_info_t
{

};

// Like pthread_create() function, this interface creates and starts execution of a new thread on current machine,
// returns 0 if successful.
int psu_thread_create(psu_thread_info_t* t_info, void* (*start_routine)(void*), void* args);

// This function is called within an executing thread to migrate to a remote machine specified by node. When called,
// the thread manager should initiate the migration,
// and resume execution on the remote machine. Returns 0 if successful.
int psu_thread_migrate(char* node);