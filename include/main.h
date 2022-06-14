#ifndef MAIN_H
#define MAIN_H 1

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <glib.h>

/* Include Sundials header files */

#include <ida/ida.h>                       /* prototypes for IDA fcts., consts.    */
#include <nvector/nvector_serial.h>        /* access to serial N_Vector            */
#include <sunmatrix/sunmatrix_sparse.h>    /* access to sparse SUNMatrix           */
#include <sunlinsol/sunlinsol_klu.h>       /* access to KLU linear solver          */
#include <sundials/sundials_types.h>       /* defs. of realtype, sunindextype      */
#include <sundials/sundials_math.h>        /* defs. of SUNRabs, SUNRexp, etc.      */

/* Include exported C code of signal processing components */

#include <c_code.h>

#if defined(SUNDIALS_EXTENDED_PRECISION)
#define GSYM "Lg"
#define ESYM "Le"
#define FSYM "Lf"
#else
#define GSYM "g"
#define ESYM "e"
#define FSYM "f"
#endif




#define ZERO RCONST(0.0)
#define ONE  RCONST(1.0)
#define PI          RCONST(3.1415926535898)   /* pi */
#define SQRT2       RCONST(1.41421356237)     /* root 2 */
#define SQRT3       RCONST(1.7320508075688772)     /* root 3 */
#define rad_inv     RCONST(0.0174532925199)   /* pi/180 */

/* Simulation Global Constants */
#define SIM_TIME RCONST(0.5)
#define MAX_SIM_STEP RCONST(0.0001)
#define NEQ 19

/* SP Global Constants */
#define SP_OUTPUT_SIZE 1
#define SP_EXE_RATE RCONST(0.0002)


#define EL_OUTPUT_SIZE 12
#define DATA_SAVE_RATE 1e-05

/* Constants related to switching devices */
#define NUMBER_OF_SWITCHES 2

#define NUMBER_OF_DIODES 10
#define DIODE_TURN_ON_THRESHOLD RCONST(7e-7)
#define DIODE_TURN_OFF_THRESHOLD 0

/* Diode State Estimator parameters */
#define NUM_DS_CORR_ATTEMPTS 5
#define MAX_DEGREE 10



#define MAX_ABSOLUTE_TOLERANCE 100000
#define ROUNDOFF_ERROR 1e-10

#define MaxAllowedRelTol 0.01
#define MaxAllowedNumSteps 10000
#define MaxAllowedErrTestFails 1000

#endif
