#include "c_comm.h"

struct data_buff buff;
void * ctx;
void comm_setup(char * sigs[], int num_sigs, int buffSize,int addr, void (*f)(void *, double *))
{
	// buffer initialization
	buff.data = (double**) malloc(sizeof(double*)*(num_sigs+1));
	for (int i=0; i < num_sigs+1; ++i) buff.data[i]=(double *) malloc(sizeof(double)*buffSize);
	buff.sigs = (char**) malloc(sizeof(char*)*(num_sigs+1));
	
	//copy across our signal names to the buffer
	buff.sigs[0]=(char *) malloc(sizeof(char)*STR_BUFF_SIZE);
	strcpy(buff.sigs[0],"t");
	for (int i=1; i < num_sigs+1; ++i)
	{
		buff.sigs[i]=(char *) malloc(sizeof(char)*STR_BUFF_SIZE);
		strcpy(buff.sigs[i],sigs[i-1]);
	}

	buff.num_signals=num_sigs;
	buff.size=buffSize;

	// padding the signal names so that each one is the same length. Helps the other end parse messages correctly
	buff.index=0;
	buff.map=f;
	int longest_str=0;
	for (int i=0; i < num_sigs; ++i)
		if (strlen(sigs[i]) > longest_str)
			longest_str=strlen(sigs[i]);
	
	//Python expects any strings sent over ZMQ to be in multiples of 8 bytes, so we just round this up to the nearest multiple of 8
	longest_str=ROUND_TO_EIGHT(longest_str);
	buff.sig_name_len=longest_str;
	for (int i=0; i < num_sigs; ++i)
		pad_str(buff.sigs[i],longest_str);

	// and add the time signal
	char temp[STR_BUFF_SIZE]="t";
	pad_str(temp, longest_str);
	strcpy(buff.sigs[0],temp);
	
	//first contact
	ctx=zmq_ctx_new();
	buff.soc =zmq_socket(ctx,ZMQ_PUB);
	char pub_addr[STR_BUFF_SIZE];
	sprintf(pub_addr,"tcp://*:%d",addr);
	int rc=zmq_bind(buff.soc,pub_addr);
	assert(rc==0); //make sure our Publisher actually connected

	//this is our temporary listener/subscription manager
	void *syncservice = zmq_socket(ctx, ZMQ_REP);
	char sync_addr[STR_BUFF_SIZE];
	sprintf(sync_addr,"tcp://*:%d",addr+1);
	rc = zmq_bind(syncservice, sync_addr);
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

void comm_log_data(void * Integrator)
{
	double * results;
	results=(double *) malloc(sizeof(double)*buff.num_signals+1);
	buff.map(Integrator, results);
	int num_ele=buff.num_signals+1;
	for (int i=0; i < num_ele; ++i)
		buff.data[i][buff.index]=results[i];
	buff.index++;
	if (buff.index==buff.size)
	{
		buff.index=0;
		transmit_data();
	}
}

void comm_cleanup()
{
	zmq_send(buff.soc,"done",4,0);
	zmq_send(buff.soc,"done",4,0);
	zmq_send(buff.soc,"done",4,0);
	printf("DONE sent");
	free(buff.data);
	free(buff.sigs);
	zmq_close(buff.soc);
	zmq_ctx_destroy(ctx);
}

void transmit_data(void)
{
	//the ultimate size of our message
	size_t buff_sz=sizeof(double)*buff.size;
	size_t sz=buff_sz + sizeof(char[buff.sig_name_len]);

	//our message payload
	struct DATA_s{
		char name[buff.sig_name_len-1];
		double data[buff.size];
	};
	//a shorthand to be able to reinterpret the payload as a bytestring
	union MSG{
		struct DATA_s data;
		char *ptr;
	}msg;
	//load up the payload with our time signal
	strncpy(msg.data.name,buff.sigs[0], buff.sig_name_len);
	memcpy(msg.data.data, buff.data[0], buff_sz);

	printf("Sending packets...\n");
	//pack it into a ZMQ structure
	zmq_msg_t zmsg;
	zmq_msg_init_size(&zmsg,sz);
	memcpy(zmq_msg_data(&zmsg),&msg.ptr, sz);
	//...and send!
	int res=zmq_send(buff.soc,&msg.ptr,sz, 0);
	// and repeat for all the rest of the signals
	for (int i = 1; i < buff.num_signals+1; ++i)
	{
		strncpy(msg.data.name,buff.sigs[i], buff.sig_name_len);
		memcpy(msg.data.data, buff.data[i], buff_sz);
		//memcpy(zmq_msg_data(&zmsg),&msg.ptr, sz);
		res=zmq_send(buff.soc,&msg.ptr,sz, 0);
	}
}

void pad_str(char str[], int final_len)
{
	char * fmt= (char*) malloc(8);
	sprintf(fmt,"%%-%ds",final_len);
	char temp[80];
	sprintf(temp,fmt,str);
	strcpy(str,temp);
	free(fmt);
}