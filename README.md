# Module setup
This module is built to be fairly hands-off, from the solver side anyways. Here's how to use it:

- clone the repo
- `make` and `make install` the project. On windows, you'll need `mingw32-make` instead of simply `make`
  - you may need to change some install directories, `INST_INC_DIR` should point to your msys64 folder->usr->include, and `INST_LIB_DIR` to your msys64 folder->usr->lib

In your Julia code:
- do a `#include <c_comm.h>`
- In the initialization section of your main function, call `comm_setup()`
  - Check the documentation for the specifics of the arguments for this
- In the execution loop, put `comm_log_data(&integrator)` in there somewhere. It handles everything else internally!
- In the cleanup section (after the execution loop), add `comm_cleanup();`

# Communication Protocol
You actually talk with the program with the following sequence:
1. `connect` with SUB socket on selected address
2. `connect` with REQ socket on selected address + 1
3. select signals to subscribe to (don't forget to add *t* and *done* as well!)
4. send test string over REQ socket, to establish that a connection has been made
	 - just send a blank string or a single space or something small like that
5. receive a reply on REQ socket
	 - 2x64 bit integers: `[length of signal name string, size of data buffer]`
	 - at this point, the REQ socket can be freed
6. receive data
	binary stream: `[(signal name),(data buffer)]`

7. Once the program finishes, the last signal sent will be `done`
