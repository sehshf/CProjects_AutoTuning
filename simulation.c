#include "simulation.h"
#include "control_system.h"
#include "data_treatment.h"
#include "util_func.h"

/**
*  -------------------------------------------------------  *
*  SYS2NDORDER() represents a second order linear dynamic- 
*  al system described by the following transfer function:
*
*                   2 s + 2
*  G(s) = -----------------------------
*         s^3 + 0.8 s^2 + 4.2 s + 1.616
*
*  It is discretized with an appropriate sampling time.
*
*  Inputs:
*     sUin: plant input
*
*  Outputs:
*     sYout: plant output
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
short Sys2ndOrder (unsigned char bReset, short sUin)
{
	float fY;
	
	short sYout; 
	
	static float fY1Old, fY2Old, fY3Old, fU1Old, fU2Old, fU3Old;
	
	/* reset static variables for the next simulation. */
	if (bReset)
	{	fY1Old = 0;
		fY2Old = 0;
		fY3Old = 0;
		fU1Old = 0;
		fU2Old = 0;
		fU3Old = 0;
	}
	
	/* calculate the output
	The transfer function has been discretized by sampling time of 0.1s */
		 
	fY = 2.8821 * fY1Old - 2.8068 * fY2Old + 0.9231 * fY3Old
 	   + 0.0100 * fU1Old + 0.0010 * fU2Old - 0.0091 * fU3Old;
	  
   /* update inputs */
   fU3Old = fU2Old;
   fU2Old = fU1Old;
   fU1Old = (float)sUin / PREC;
   fU1Old = sat(fU1Old, UMIN, UMAX);
   
   /* update inputs */
   fY3Old = fY2Old;
   fY2Old = fY1Old;
   fY1Old = fY;
	
	sYout = fY * PREC;
	
   return sYout;
   
} // End: Sys2ndOrder()


/**
*  -------------------------------------------------------  *
*  SIMINIT() initializes the simulation parameters.
*
*  Inputs:
*     sTsim: simulation time
*
*  Outputs:
*     SimSet: simulation setting in an structure
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
SIMSET SimInit (short sTsim)
{
	SIMSET SimSet;
	
	SimSet.fTs = SAMPLINGTIME;
	SimSet.uNbrIter = sTsim / SimSet.fTs;
	
	return SimSet;
} // End: SimInit()


/**
*  -------------------------------------------------------  *
*  SIMULATION() simulates a discrete-time dynamical syste-
*  m.
*
*  Inputs:
*     *SimSet    : structure of the simulation settings.
*     cTuneMethod: if tuning required
*     cFileName  : name of the file to save data into
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void simulation (SIMSET *SimSet, short sSimCase, const char *cFileName)
{
	/* simulation setting */
	double time;
	
	unsigned i;
	
	short  sSysIn = 0, sSysOut = 0; 
	
	PIDSET PID;
	
	unsigned char bTuned = FALSE; 
	
	unsigned char bReset = TRUE;
	
	/* set-point */
	short sSetpoint;
	
	if (sSimCase != STEP)
		sSetpoint = GetSetpoint();
	
	/* open a file to save data */
	FILE *DataFile;
	DataFile = fopen(cFileName, "w");
	
	/* main simulation loop */
	for (i = 0; i < SimSet->uNbrIter; i++)
	{
		time = i * SimSet->fTs;
		
		/* system response */
		sSysOut = Sys2ndOrder(bReset, sSysIn);
		
		if (!bTuned)
		{
			switch (sSimCase)
			{
			   case STEP:
			   	/* step input */
					sSysIn = step(time);
					break;
					
			   case TUNED:
			   	/* an already tuned PID is used */
					TunedPID(&PID);
					bTuned = TRUE;
					break;
					
			   case MANUAL:
			   	/* PID gains is entered manually */
			   	SetPIDParam(&PID);
			   	bTuned = TRUE;
					break;
					
		   	case AUTO:
		   		/* controller automatic tuning */
		   		sSysIn = AutoTune(&bTuned, &PID, time, sSetpoint, sSysOut, SimSet->fTs);
					break;
		   }
		}
 		else
		{
			if (i < 2)
				bReset = TRUE;
			else
			   bReset = FALSE;
			   
			sSysIn = PIDCtrl (bReset, &PID, sSetpoint, sSysOut, SimSet->fTs);
		}
		
		bReset = FALSE;
		/* save data into the file */
		SaveData(DataFile, time, sSysIn, sSysOut);
	}
	
	/* close the data file */
 	fclose(DataFile);	
	 	
} // End: simulation()

