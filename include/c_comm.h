#ifndef _C_COMM_H
#define _C_COMM_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <types.h>


struct data_buff{
	int index;
	int num_signals;
	int size;
	int sig_name_len;
	void * soc;
	int (*map)(IntegratorType *, realtype *);
	char ** sigs;
	double ** data;
};

extern struct data_buff buff;
extern void * ctx;
extern struct sig_names names;

#define ROUND_TO_EIGHT(x) (int) 8*ceil(log(x)/log(8))
/**
 * 
 * @brief Module initialization function for communication module
 * 
 * @param sigs an array of strings containing the output variables
 * @param num_sigs the size of the array of strings
 * @param buffSize the desired size of the output buffer
 * @param addr the address for the PUB socket to work on
 * @param f the mapping function between Integrator's results and the output variables
 */
void comm_setup(int buffSize,int addr, int (*f)(IntegratorType *, realtype *));

/**
 * @brief the intermediary save data function. Records data until the buffer is full, then transmits said data
 * 
 * @param Integrator the solver being used
 */
void comm_log_data(IntegratorType * Integrator);

/**
 * @brief deconstructor for the comm module
 * 
 * @param context 
 */
void comm_cleanup(void);

/**
 * @brief Sends the data across ZMQ socket buff.soc (a PUB socket), with the pattern:
 * [(signal name),(signal buffer)] as one continuous char array
 * 
 */
void transmit_data(void);

/**
 * @brief For better transmitting, each output variable string should be the same length. This takes a string and pads it to the
 * length it needs to be for that to be true
 * 
 * @param str the string to be padded
 * @param final_len the desired length of the string
 */
void pad_str(char str[], int final_len);

#endif