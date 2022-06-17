#include <c_comm.h>

struct data_buff buff;
void * ctx;
void comm_setup(int buffSize,int addr, int (*f)(IntegratorType *, realtype *))
{
	int i=0;
	buff.num_signals=names.num_names;
	buff.sigs=names.names;
	//buff.sigs=malloc(names->num_names);
	//for (i=0; i < names->num_names; ++i)
		//buff.sigs[i]=malloc(STR_BUFF_SIZE);
	//copy across our signal names to the buffer
	// buffer initialization
	buff.data = (double**) malloc(sizeof(double*)*(buff.num_signals));
	for (int i=0; i < buff.num_signals; ++i) buff.data[i]=(double *) malloc(sizeof(double)*buffSize);

	buff.size=buffSize;

	// padding the signal names so that each one is the same length. Helps the other end parse messages correctly
	buff.index=0;
	buff.map=f;
	int longest_str=0;
	for (i=0; i < buff.num_signals; ++i)
	{
		printf("name=%s\n",buff.sigs[i]);
		if (strlen(buff.sigs[i]) > longest_str)
			longest_str=strlen(buff.sigs[i]);
	}
	//Python expects any strings sent over ZMQ to be in multiples of 8 bytes, so we just round this up to the nearest multiple of 8
	longest_str=ROUND_TO_EIGHT(longest_str);
	buff.sig_name_len=longest_str;

	
	//first contact
	ctx=zmq_ctx_new();
	buff.soc =zmq_socket(ctx,ZMQ_PUB);
	char pub_addr[STR_BUFF_SIZE];
	sprintf(pub_addr,"tcp://*:%d",addr);
	int rc=zmq_bind(buff.soc,pub_addr);
	if (rc==0)
		printf("Couldn't connect to pub service!\n");
	assert(rc==0); //make sure our Publisher actually connected

	//this is our temporary listener/subscription manager
	void *syncservice = zmq_socket(ctx, ZMQ_REP);
	char sync_addr[STR_BUFF_SIZE];
	sprintf(sync_addr,"tcp://*:%d",addr+1);
	rc = zmq_bind(syncservice, sync_addr);
	if (rc==0)
		printf("Couldn't connect to sync service!\n");
	assert(rc==0); //make sure our listener actually connected

	printf("Connected and waiting...\n");
	// we should only get a blank string as our signal, so 10 chars should be plenty
	char buffer[10]; 
	// our first report is [length of signal names, size of data buffer]
	int64_t len_report[2];
	len_report[0]=longest_str;
	len_report[1]=buffSize;
	// now we wait for a signal
	zmq_recv(syncservice,buffer, 10,0);
	printf("Recieved!");
	//once it's recieved, send out our first report and exit
	zmq_send(syncservice,len_report,sizeof(len_report),0);
	zmq_close(syncservice);
}

void comm_log_data(IntegratorType * Integrator)
{
	double * results;
	results=(double *) malloc(sizeof(double)*buff.num_signals);
	buff.map(Integrator, results);
	int num_ele=buff.num_signals;
	for (int i=0; i < num_ele; ++i)
		buff.data[i][buff.index]=results[i];
	buff.index++;
	if (buff.index==buff.size)
	{
		buff.index=0;
		transmit_data();
	}
	free(results);
}

void comm_cleanup()
{
	zmq_send(buff.soc,"done",4,0);
	printf("DONE sent");
	free(buff.data);
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

	// and repeat for all the rest of the signals
	for (int i = 0; i < buff.num_signals; ++i)
	{
		sprintf(msg.data.name,"%-*s",buff.sig_name_len,buff.sigs[i]);
		memcpy(msg.data.data, buff.data[i], buff_sz);
		zmq_send(buff.soc,&msg.ptr,sz, 0);
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
