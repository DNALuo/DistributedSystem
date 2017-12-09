## CSE-511 Project 2 Distributed Systems
#### Yuxin Wang / Ding Ding

### Distributed Locks

The distributed locks system implements the Ricart & Agrawala algorithm on top of RPC call systems.

The server is transformed into a multi-threaded server which can handle multiple RPC requests at the same time, making it possible for the lock to hold for a long time.

Since the original RPC system doesn't take multi-threading into their designs, many of the RPC functions are not thread-safe and we used some mutex to guard the functions to make them thread-safe.
The thread-safe version of the RPC functions lie in utility.h and utility.c files.

The test program is to create 3 threads acquiring lock No.1 (2 threads) and lock No.2(1 thread), it needs to provide nodes information on commandline argument. Like
`./psu_dist_lock_mgr_test <node_ip_1> <node_ip_2> ...`


### Distributed Memory System

On each machine, server and client share memory at regions created by `shmget()`.

Each server maintains a directory, as well as a list of shared memory currently on this server.

On `psu_dsm_malloc()` call, the client sends a msg to local server.  The local server contacts all servers (including itself) to find who's directory has the name, then it sends a request to that server to request the page. Upon receiving the request, the name server checks its directory, fetch the page from the right server, sends out update massage (either to invalidate the page or change to read only mode) to all servers that has the page, update its directory, then return the page to the requesting server.

The shared memory is protected by `mprotect()`. If a shared memory is accessed without permission, a SIGSEGV signal is generated. The signal is caught by a handler which will check the faulting operation (read or write) and faulting address, then send a request to local server to locate the name(dir) server, then send a request to fetch the page and put it in the right mode.

The `psu_dsm_free()` will send a request local server to find the name(dir) server, then ask the name server to instruct all servers with this page  to invalidate it and detach shared memory.

For the user, `psu_dsm_init()` has to be called first to register the signal handler.

### Thread Migrator

The thread migrator uses the Linux's `ucontext` related functions. By copying the context of the original thread and send it to the target server over RPC calls.

Note that for this part the server and client are built together for the server to be aware of the client's code.

Simply run `./psu_thread_test server` to run the server and run `./psu_thread_test <node_ip>` to run the client.

### Build

`CMakeList.txt` is provided, you need `glib`, installed for compilation, where we only used `glib`'s `GArray` class for list implementations in order not to reinvent the wheels.

Run `mkdir build & cd build & cmake.. & make` to perform out-of-source build.

The included `Makefile` is auto-generated by CMake, it anything wrong happens, use cmake to build instead.
