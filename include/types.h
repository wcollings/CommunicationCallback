#ifndef TYPES_H
#define TYPES_H 1

#include <main.h>

typedef struct{
    realtype t[50000];
    realtype saveval[50000][EL_OUTPUT_SIZE+SP_OUTPUT_SIZE+1 + NUMBER_OF_DIODES];
} SavedValues;

typedef struct
{
    void *IDAMemPtr;
    int retcode;

    realtype t;
    N_Vector yy;
    N_Vector yp;

    realtype rtol;
    realtype atol;
    realtype InitStep;
    int MaxOrd;
    realtype MaxStep;
    int MaxNumSteps;
    int MaxErrTestFails;

    int switches[NUMBER_OF_SWITCHES];
    int diodes[NUMBER_OF_DIODES];
    realtype I_diode[NUMBER_OF_DIODES];
    GHashTable *SW_DIODE_TABLE;
    int root_returned;
    int rootsfound[10];
    externalInputs *ext_inputs;
    externalOutputs *ext_outputs;
    results_output *sp_probes;

    realtype t_end;
    realtype t_sp;
    realtype t_savedata;

    int verbose;
    realtype verbose_start;
    realtype verbose_end;

    int re_init_flag;

} IntegratorType;



#endif
