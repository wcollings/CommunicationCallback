#ifndef _C_COMM_H
#define _C_COMM_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>

#define STR_BUFF_SIZE (int)80

struct data_buff{
	int index;
	int num_signals;
	int size;
	void * soc;
	void (*map)(void *, double *);
	char ** sigs;
	double ** data;
};

extern struct data_buff buff;
extern void * ctx;

void comm_setup(char * sigs[], int num_sigs, int buffSize,int addr, void (*f)(void *, double *));
void comm_log_data(double * u, int num_ele, double t, void * Integrator);
void comm_cleanup(void * context);
void transmit_data(void);
void pad_str(char str[], int final_len);

#endif