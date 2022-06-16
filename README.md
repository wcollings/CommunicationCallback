# Module setup
This module is built to be fairly hands-off, from the Julia side anyways. Here's how to use it:

Open a Julia console, type `]add https://github.com/wcollings/CommunicationCallback.git`, if this is the first time,
or if it's already installed and you want to update it, just type `]up CommunicationCallback`

In your Julia code:
- do a `using CommunicationCallback`
- In the callback initialization section of your code, add a communication callback variable. This is initializied by the function: 
`comm(sig::Vector{String}|String, BufferSize::Int,Addr:Int,send_time_signals::Bool,translation_function::Function)`
	- Check the documentation (hover over the function name) to see the specifics of each one.
- Then just add it to the list of callbacks!

# Communication Protocol
You actually talk with the program with the following sequence:
1. `connect` with SUB socket on selected address
2. `connect` with REQ socket on selected address + 1
3. select signals to subscribe to (don't forget *t* for time and *done* as mentioned at the end!)
4. send test string over REQ socket, to establish that a connection has been made
	 - just send a blank string or a single space or something small like that
5. receive a reply on REQ socket
	 - 2x64 bit integers: `[length of signal name string, size of data buffer]`
	 - at this point, the REQ socket can be freed
6. receive data
	binary stream: `[(signal name),(data buffer)]`

7. Once the program finishes, the last signal sent will be `done`
