module CommunicationCallback
using TimerOutputs

using DiffEqCallbacks
using ZMQ
export comm, pub_setup, soc
size=10000
#dv=Vector{Float64}(undef,2*size) # data vector
dv= Nothing
ind=1
soc = Nothing
signal = Nothing
send_t = true
func = Nothing
round_to_eight = (x) -> convert(Int,8*ceil(log(x)/log(8)))
last_time= Nothing
curr_time=Nothing

function cbfun(u::Vector{Float64},t,int)
	global send_t
	global soc
	global dv
	global ind
	global func
	global curr_time
	global last_time
	curr_time=time_ns()
	res=u
	try
		res=func(u,t, int)
	catch
		stacktrace(catch_backtrace())
	end
	#println("output_fn executed fine")
	for i=1:length(u)
		dv[i+1,ind]=res[i]
	end
	dv[1,ind]=t
	ind+=1
	if ind == size+1
		ind=1
		transmit()
	end
	last_time=time()
end


function transmit()
	if (curr_time-last_time > 100000)
		global size /= 1.25
	end
	s_len=length(signal[1])
	tn="t"*" "^(s_len-1)
	msg=Vector{UInt8}(tn)
	append!(msg,reinterpret(UInt8, dv[1,:]))
	send(soc,Message(msg))
	for i=2:length(signal)
		try
			msg=Vector{UInt8}(signal[i-1])
			append!(msg,reinterpret(UInt8, dv[i,:]))
			#msg=output(signal[i-1],dv[i,:])
			send(soc,Message(msg))
		catch
			stacktrace(catch_backtrace())
		end
	end
end

function cbfun(u::Float64,t::Float64,int)
	global dv
	global ind
	global size
	global send_t
	global signal
	println("In bad function")
	dv[ind]=u
	if send_t==true
		dv[ind+size]=t
	end
	ind+=1
	if ind==size+1 # if the data vector is full
		if send_t==true
			s_len=length(signal)
			tn="t"*" "^(s_len-1)
			msg=append(Vector{UInt8}(tn),dv[ind:end])
			send(soc,Message(msg))
		end
		msg=append(Vector{UInt8}(signal),dv[1:size])
		send(soc,Message(msg))

		ind=1
	end
end
"""
	# Arguments
	-`sig::Vector{String}|String`: the signal or list of signals that you will be sending
	-`buffSize::Int`: the size of the internal buffer(s)
	-`addr::Int`: the port on which to send the publisher results
	-`send_t::Bool`: whether or not you're sending the *time* signal (or any independend variable) as well as the dependend variables
	-`f::Function`: the mapping function between results and true output, if it is needed
"""
function comm(sig,buffSize::Int, addr::Int64, send_t=true, f=Nothing)
	# setup the server
	full_addr="tcp://*:$addr"
	socMaster=Socket(PUB)
	bind(socMaster,full_addr)
	#println("Pub established")

	# Make all the names the same length, for ease of sending
	max_len=0
	for i=1:length(sig)
		if length(sig[i]) > max_len
			max_len=length(sig[i])
		end
	end
	for i=1:length(sig)
		l=round_to_eight(max_len)-length(sig[i])
		sig[i]*=" "^l
	end
	# setup the syncronizer
	full_addr2="tcp://*:$(addr+1)"
	syncservice=Socket(REP)
	bind(syncservice,full_addr2)
	println("Reporter established")
	subs=0
	report=[length(sig[1]),buffSize]
	#println("listening...")
	while subs == 0
		msg=recv(syncservice,String)
		msg1=Message(report)
		send(syncservice,msg1)
		subs+=1
	end
	#println("all subs connected")
	
	if typeof(sig) == Vector{String}
		global dv=Array{Float64}(undef,length(sig)+1,buffSize)
	elseif send_t==true
		global dv=Vector{Float64}(undef, 2*size)
	else
		global dv=Vector{Float64}(undef, size)
	end
	global soc=socMaster
	global size=buffSize
	global send_t=send_t
	global signal=sig
	global func=f
	global sync = syncservice
	@timeit "CommunicationCallback" FunctionCallingCallback(cbfun,func_everystep=true)
end

function __init__()
	atexit() do 
		send(soc, "done")
		print("done")
	end
end
end # module
