#include "integrator.h"
void Integrator_init(integrator_t *Integrator)
{
	Integrator->yy=malloc(sizeof(double)*3);
}

void output_fn(integrator_t *Integrator, realtype * save)
{
	save[0]=Integrator->t;
	save[1]=Integrator->yy[0]+Integrator->yy[1];
	save[2]=Integrator->yy[1]+Integrator->yy[2];
	save[3]=Integrator->yy[1]+Integrator->t;
	save[4]=Integrator->yy[0]-Integrator->t;
}

void solve(integrator_t *Integrator)
{
	Integrator->yy[0]=rand()*M_PI;
	Integrator->yy[1]=rand()*M_PI;
	Integrator->yy[2]=rand()*M_PI;
	Integrator->t++;
}
void Integrator_destroy(integrator_t *Integrator)
{
	free(Integrator->yy);
}