module CommunicationCallback

using DiffEqCallbacks
using ZMQ
export comm, pub_setup
size=10000
#dv=Vector{Float64}(undef,2*size) # data vector
dv= Nothing
ind=1
soc = Nothing
signal = Nothing
send_t = true
func = Nothing

function cbfun(u::Vector{Float64},t,int)
	global send_t
	global soc
	global dv
	global ind
	global func
	res=func(u,t, int)
	res=u
	for i=1:length(u)
		dv[i+1,ind]=res[i]
	end
	dv[1,ind]=t
	ind+=1
	if ind == size+1
		s_len=length(signal[1])
		tn="t"*" "^(s_len-1)
		msg=append(Vector{UInt8}(tn),dv[1])
		send(soc,Message(msg))
		for i=1:length(signal)
			msg=append(Vector{UInt8}(signal[i]),dv[i])
			send(soc,Message(msg))
		end
		ind=1
	end
end




function cbfun(u::Float64,t::Float64,int)
	global dv
	global ind
	global size
	global send_t
	global signal
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
function comm(sig,buffSize::Int, addr::Int64, send_t=true, f=Nothing)
	# setup the server
	full_addr="tcp://*:$addr"
	socMaster=Socket(PUB)
	bind(socMaster,full_addr)
	println("Pub established")
	
	# setup the syncronizer
	full_addr2="tcp://*:$(addr+1)"
	syncservice=Socket(REP)
	bind(syncservice,full_addr2)
	println("Reporter established")
	subs=0
	println("listening...")
	while subs == 0
		msg=recv(syncservice,String)
		subs+=1
		report=[length(sig[1]),buffSize]
		send(syncservice,report)
	end
	
	if typeof(sig) == Vector{String}
		global dv=Array{Float64}(undef,length(sig)+1,buffSize)
	elseif send_t==true
		global dv=Vector{Float64}(undef, 2*size)
	else
		global dv=Vector{Float64}(undef, size)
	end
	print("dv=")
	print(typeof(dv))
	print('\n')
	global soc=socMaster
	global size=buffSize
	global send_t=send_t
	global signal=sig
	global func=f
	FunctionCallingCallback(cbfun,func_everystep=true)
end
function stop()
		@async send(soc, "done")
end
Base.atexit(stop)
end # module
