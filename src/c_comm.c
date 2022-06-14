#include "c_comm.h"

struct data_buff buff;
void * context;

void comm_setup(char * sigs[], int num_sigs, int buffSize,int addr, void (*f)(void *, double *))
{
	// buffer initialization
	buff.data = (double**) malloc(sizeof(double*)*(num_sigs+1));
	for (int i=0; i < num_sigs+1; ++i) buff.data[i]=(double *) malloc(sizeof(double)*buffSize);
	buff.sigs = (char**) malloc(sizeof(char*)*(num_sigs+1));
	for (int i=0; i < num_sigs; ++i)
	{
		buff.sigs[i]=(char *) malloc(sizeof(char)*STR_BUFF_SIZE);
		strcpy(buff.sigs[i],sigs[i]);
	}
	buff.sigs[num_sigs]=(char *) malloc(sizeof(char)*STR_BUFF_SIZE);
	strcpy(buff.sigs[num_sigs],"t");

	buff.num_signals=num_sigs;
	buff.size=buffSize;

	// padding the signal names so that each one is the same length. Helps the other end parse messages correctly
	buff.index=0;
	buff.map=f;
	int longest_str=0;
	for (int i=0; i < num_sigs; ++i)
		if (strlen(sigs[i]) > longest_str)
			longest_str=strlen(sigs[i]);
	
	for (int i=0; i < num_sigs; ++i)
	{
		pad_str(sigs[i],longest_str);
		strcpy(buff.sigs[i],sigs[i]);
	}

	// and add the time signal
	char temp[STR_BUFF_SIZE]="t";
	pad_str(temp, longest_str);
	strcpy(buff.sigs[num_sigs],temp);
	
	//first contact
	ctx=zmq_ctx_new();
	buff.soc =zmq_socket(ctx,ZMQ_PUB);
	char addr1[STR_BUFF_SIZE];
	sprintf(addr1,"tcp://*:%d",addr);
	int rc=zmq_bind(buff.soc,addr1);
	assert(rc==0); //make sure our Publisher actually connected

	//this is our temporary listener/subscription manager
	void *syncservice = zmq_socket(ctx, ZMQ_REP);
	char addr2[STR_BUFF_SIZE];
	sprintf(addr2,"tcp://*:%d",addr+1);
	rc = zmq_bind(syncservice, addr2);
	assert(rc==0); //make sure our listener actually connected

	// we should only get a blank string as our signal, so 10 chars should be plenty
	char buffer[10]; 
	// our first report is [length of signal names, size of data buffer]
	int64_t len_report[2];
	len_report[0]=longest_str;
	len_report[1]=buffSize;
	// now we wait for a signal
	zmq_recv(syncservice,buffer, 10,0);
	//once it's recieved, send out our first report and exit
	zmq_send(syncservice,len_report,sizeof(len_report),0);
	zmq_close(syncservice);
}

void comm_log_data(double * u, int num_ele, double t, void * Integrator)
{
	double * results;
	if (buff.map != NULL)
	{
		results=(double *) malloc(sizeof(double)*buff.num_signals);
		buff.map(Integrator, results);
		num_ele=buff.num_signals;
	}
	else
		results=u;
	for (int i=0; i < num_ele; ++i)
		buff.data[i][buff.index]=results[i];
	buff.data[buff.num_signals][buff.index]=t;
	buff.index++;
	if (buff.index==buff.size)
		transmit_data();
	
}

void comm_cleanup()
{
	zmq_close(buff.soc);
	zmq_ctx_destroy(ctx);
}

void transmit_data(void)
{
	size_t sz=sizeof(buff.data[buff.num_signals]) + sizeof(buff.sigs[buff.num_signals]);
	struct DATA_s{
		char * name;
		double * data;
	};
	union MSG{
		struct DATA_s data;
		char * ptr;
	};
	union MSG msg;
	msg.ptr=malloc(sz);
	strcpy(msg.data.name,buff.sigs[buff.num_signals]);
	memcpy(msg.data.data, buff.data[buff.num_signals], sz);
	zmq_msg_t zmsg;
	zmq_msg_init_size(&zmsg,sz);
	memcpy(zmq_msg_data(&zmsg),msg.ptr, sz);
	zmq_msg_send(&zmsg,buff.soc, 0);
	for (int i = 0; i < buff.num_signals; ++i)
	{
		strcpy(msg.data.name,buff.sigs[i]);
		memcpy(msg.data.data, buff.data[i], sz);
		memcpy(zmq_msg_data(&zmsg),msg.ptr, sz);
		zmq_msg_send(&zmsg,buff.soc, 0);
	}
}

void pad_str(char str[], int final_len)
{
	char * fmt= (char*) malloc(8);
	sprintf(fmt,"%%-%ds",final_len);
	char temp[80];
	sprintf(temp,fmt,str);
	strcpy(str,temp);
}
