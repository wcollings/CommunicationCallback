# Module setup
This module is built to be fairly hands-off, from the solver side anyways. Here's how to use it:

- clone the repo
- `make` and `make install` the project. On windows, you'll need `mingw32-make` instead of simply `make`
  - you may need to change some install directories, `INST_INC_DIR` should point to your msys64 folder->usr->include, and `INST_LIB_DIR` to your msys64 folder->usr->lib

In your C code:
- do a `#include <c_comm.h>`
- In the initialization section of your main function, call `comm_setup()`
  - Check the documentation for the specifics of the arguments for this
- In the execution loop, put `comm_log_data(&integrator)` in there somewhere. It handles everything else internally!
- In the cleanup section (after the execution loop), add `comm_cleanup();`

# Communication Protocol
You actually talk with the program with the following sequence:
1. `connect` with SUB socket on selected address
2. `connect` with REQ socket on selected address + 1
3. select signals to subscribe to (don't forget to add *Time* and *done* as well!)
4. Over the REQ socket, send any commands to the solver. Accepted commands are below. If no response is specified, the response will be "OK" if successful
	- *SET VAR VAL*: set a given SCADA input signal *VAR* to *VAL*
	- *CON*: signal that a subscriber is connected. Returns the length of the Header for each message, the Header being the name of the signal being sent
	- *SIM*: start/unpause the simulation
	- *PAUSE*: pause the simulation
5. receive data
	Commands are still able to be sent and processed at any point as the simulation runs as well
	Each message sent over the SUB socket will be of the form: `[(signal name),(data buffer)]`


7. Once the program finishes, the last signal sent will be `done`
