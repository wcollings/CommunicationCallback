#include "c_comm.h"
#include "integrator.h"
#include <errno.h>

int get_output_names(char ** names);
void delay();
int main()
{
	integrator_t integ;
	Integrator_init(&integ);
	char ** name_buff;
	int num_outpts=get_output_names(name_buff);
	comm_setup(name_buff,num_outpts,10000,5556,output_fn);
	for (int i=0; i < 100000; ++i)
	{
		solve(&integ);
		comm_log_data(&integ);
		delay();
	}
	comm_cleanup();
	Integrator_destroy(&integ);
	free(name_buff);
}

/**
 * @brief Get the array of strings containing variable names 
 * 
 * @param names the buffer to store the names to
 * @return (int) the number of names recorded
 */
int get_output_names(char ** names)
{
	names[0]=strdup("Vout");
	names[1]=strdup("Vin");
	names[2]=strdup("Iin");
	names[3]=strdup("loooongName");
	return 4;
}

void delay()
{
	int k=0;
	for (int i=0; i < 1000; ++i)
		//for (int j=0; j < 1000; ++j)
			k++;
}