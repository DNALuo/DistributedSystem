#pragma once
// this function will be called to initialize the lock manager.
// information about the nodes on which the lock manager is specified.
void psu_init_lock_mgr(char** nodes, int num_nodes);

// synchronous function call to acquire a lock. Returns when it has acquired the lock.
void psu_acquire_lock(int lock_number);

// synchronous function call to release a lock. Returns when lock release is successful.
void psu_release_lock(int lock_number);
