#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#define SIMTIME 		  100   // simulation time in sec
#define SAMPLINGTIME   0.1   // simulation time in sec

#define UMIN			 -3     // minimum input limit
#define UMAX           3     // maximum input limit

enum SimCase
{
	STEP,		// 0
	TUNED,	// 1
	MANUAL,	// 2
	AUTO		// 3
};


short Sys2ndOredr (unsigned char bReset, short u);

typedef struct tagSimSet {
   float      fTs;		// sampling time
   unsigned   uNbrIter;	// number of iteration in the simulation loop	
} SIMSET;

SIMSET SimInit (short sTsim);

void simulation (SIMSET *SimSet, short sSimCase, const char *cFileName);

#endif // __SIMULATION_H__
