## CSE-511 Project 2 Distributed Systems
#### Yuxin Wang / Ding Ding

### Distributed Locks

The distributed locks system implements the Ricart & Agrawala algorithm on top of RPC call systems.

The server is transformed into a multi-threaded server which can handle multiple RPC requests at the same time, making it possible for the lock to hold for a long time.

Since the original RPC system doesn't take multi-threading into their designs, many of the RPC functions are not thread-safe and we used some mutex to guard the functions to make them thread-safe.
The thread-safe version of the RPC functions lie in utility.h and utility.c files.


### Distributed Memory System

### Thread Migrator

The thread migrator uses the Linux's `ucontext` related functions. By copying the context of the original thread and send it to the target server over RPC calls.

Note that for this part the server and client are built together for the server to be aware of the client's code.

Simply run `./psu_thread_test server` to run the server and run `./psu_thread_test <node_ip>` to run the client.

### Build

`CMakeList.txt` are provided, you need `glib`, installed for compilation, where we only used `glib`'s `GArray` class for list implementations in order not to reinvent the wheels.

Run `mkdir build & cd build & cmake.. & make` to perform out-of-source build.
