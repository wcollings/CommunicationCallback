#include "c_comm.h"
#include "integrator.h"

int main()
{
	integrator_t integ;
	char ** sigs = (char**)malloc(sizeof(char*)*4);
	for (int i=0; i < 4; ++i) sigs[i]=(char*) malloc(sizeof(char)*STR_BUFF_SIZE);
	//malloc_2d_str(sigs,4,STR_BUFF_SIZE);
	strcpy(sigs[0],"one");
	strcpy(sigs[1],"two");
	strcpy(sigs[2],"three");
	strcpy(sigs[3],"loooooooooooong");

	comm_setup(sigs,4,100,5555,&get_mapping);
	for (int i=0; i < 1000000; ++i)
	{
		
	}
	comm_cleanup();
}