#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__

#include <stdlib.h>
#include <errno.h>
#include <math.h>
typedef int int_t;
typedef unsigned int uint_t;
typedef double realtype;

typedef struct integ{
	double * yy;
	double t;
}integrator_t;

void Integrator_init(integrator_t *Integrator);
void output_fn(integrator_t *Integrator, realtype * save);
void solve(integrator_t *Integrator);
void Integrator_destroy(integrator_t *Integrator);

#endif