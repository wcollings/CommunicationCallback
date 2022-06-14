#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__

typedef int int_t;
typedef unsigned int uint_t;
typedef double realtype;
struct integrator_t{
	double * yy;
	double t;

};

void output_fn(integrator_t *Integrator, realtype * save);

#endif