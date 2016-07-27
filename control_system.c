#include <stdio.h>
#include <math.h>

#include "control_system.h"
#include "simulation.h"
#include "util_func.h"


/**
*  -------------------------------------------------------  *
*  PIDCTRL() is the PID controller.
*
*  Inputs:
*     *PID: pointer to a PID structure
*     sR  : setpoint (reference)
*     sY  : plant output
*     fTs : sampling time
*
*  Outputs:
*     sU: controll command
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
short PIDCtrl (
		   unsigned char  bReset,
		   const PIDSET *PID, 
			short sR, 
			short sY, 
			float fTs
			)
{
	float fError, fDY, fY, fV;
	
	float fP, fI, fD;
	
	short sU;
	
	float fKd1, fKd2, fKi, fKt, fTt;
	
	static float fIOld, fDOld;
	
	static short sYOld;
	
	/* reset static variables for the next simulation */
	if (bReset)
	{
	   fIOld = 0;
		fDOld = 0;
		sYOld = 0;
	}
	
	/* floating point error signal */
   fError = (float)(sR - sY) / PREC;

   /* proportional part */
   fP = PID->K * fError;
   
   /* derivative params */
   fKd1 = PID->Td / (PID->Td + PID->N * fTs);
   fKd2 = fKd1 * PID->K * PID->N;
		
   /* integral part */
   fI = fIOld;
   
	/* derivative parts */
   fDY = (float)(sY - sYOld) / PREC;	// floating point output difference
   fD  = fKd1 * fDOld - fKd2 * fDY;

   /* paralle PID */
   fV = fP + fI + fD;
   sU = fV * PREC;
   
   /* saturation filter */
   sU = sat(sU, UMIN * PREC, UMAX * PREC);
   
   /* integrator params */
   fKi = PID->K * fTs / (PID->Ti);					// integrator gain
   
   /* anti-windup params */
   if (PID->Td > 0.1 * PID->Ti)
   	fTt = sqrt((double)(PID->Ti * PID->Td));	// anti-windup time
  	else
  		 fTt = 0.3 * PID->Ti;							// anti-windup time
   
	fKt = fTs / fTt;										// anti-windup gain

   /* updates */
   fIOld = fI + fKi * fError + fKt * ((float)sU / PREC - fV);	// integrator update including anti-windup
   sYOld = sY;
   fDOld = fD;
   
	return sU;
   
} // End: PIDCtrl()


/**
*  -------------------------------------------------------  *
*  AUTOTUNE() automatically tunes a PID controller gains 
*  using a relay feedback method. A biased relay is used 
*  where the bias value is adjusted automatically.
*
*  Inputs:
*     *PID: pointer to a PID structure
*     sR  : setpoint (reference)
*     sY  : plant output
*     fTs : sampling time
*
*  Outputs:
*     sU: controll command
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
short AutoTune (
		   unsigned char *bTuned, 
			PIDSET *PID, 
			float fTime, 
			short sR, 
			short sY, 
			float fTs
			)
{
   float fTimeOld, fError, fDeltaU, fDeltaError, fKu, fPu, fUn;
   
   static char ErrorSignOld;
   
   static unsigned char bOscillation = 0;
   
   static float fTup, fTdown, fErrorOld, fErrorMax, fUb = 0.5 * (UMIN + UMAX);
   
   static short sPerCount, sUOld = UMAX * PREC;
	
	short sU;
   
   #define DELTAERROR   2    // error hysteresis bound in percent
	#define PERIODDIF    10   // difference between half perios oscillations
   
   /* hysteresis bound for error */
   fDeltaError = (float)abs(sR) * DELTAERROR / PREC / 100;
   
   /* regulation error */
	fError = (float)(sR - sY)/ PREC;

   /* bias limits */
   fUb 	  = sat(fUb, UMIN, UMAX);
   fDeltaU = min((UMAX - fUb), (fUb - UMIN));
   
   /* relay actions */
	if ((fError < -fDeltaError) && (sUOld > fUb * PREC))
   {
   	fTup = fTime - fTimeOld;	// update the time of high relay
   	fTimeOld = fTime;
   	
   	sU = (fUb - fDeltaU) * PREC;
   	sUOld = sU;
   }
   else if ((fError > fDeltaError) && (sUOld < fUb * PREC))
   {
   	fTdown = fTime - fTimeOld;	// update the time of high relay
   	fTimeOld = fTime;
   	
		sU = (fUb + fDeltaU) * PREC;
   	sUOld = sU;
   	
   	/* ckeck if the half periods differ more than 10% */
   	if (fabs(fTup - fTdown) * 100 / (fTup + fTdown) > PERIODDIF)
   	{
   		bOscillation = 0;
      	
			/* bias value in hysteresis relay in case of biased relay.
			   fUb has to be normalized (e.g. to [0 100]); otherwise it
			   cannot be updated if initialized to zero. It can happen
			   in case of a symmetric input limits. */
			fUn = (fUb - UMIN) * 100 / (UMAX - UMIN);	// normalized to [0 100]
         fUn = fUn * (1 + (fTup - fTdown) / 2 / (fTup + fTdown));
         fUb = fUn * (UMAX - UMIN) / 100 + UMIN;			
		}
		else
		{
			bOscillation = 1;   // oscillation starts at critical freq
         sPerCount ++;		  // number of oscillation half-periods
      	
      	/* check if there are "enough" oscillations 
			*  to conclude the tuning  */
		   if (sPerCount > 5)
		   {
		   	*bTuned = TRUE;

				/* tune the PID gains (Ziegler-Nichols) */
		   	fKu = 4 * fDeltaU / (pi * sqrt((double)(fErrorMax * fErrorMax - fDeltaError * fDeltaError)));
		   	fPu = fTup + fTdown;
		   	
		   	PID->K  = 0.6   * fKu;   // proportional gain
			   PID->Ti = 0.5   * fPu;   // integration time
			   PID->Td = 0.125 * fPu;	 // derivative time
			   PID->N  = 100;
			}			   
		}
	}
	else
	{
		sU = sUOld;
	}
   
   /* calculate the maximum amplitude of variation */
   if (bOscillation)
   {
      if (fabs(fError) > fabs(fErrorOld))
         fErrorMax = fabs(fError);
      
      fErrorOld = fError;
   }
	
	/* reset static variables for the next simulation */
	if (*bTuned)
	{
		bOscillation = 0;
   	fErrorMax    = 0;
		fUb          = 0.5 * (UMIN + UMAX);
   	sPerCount    = 0;
		sUOld        = UMAX * PREC;
	}
	
	return sU;

} // End: AutoTune()


/**
*  -------------------------------------------------------  *
*  TUNEDPID() assigns already "tuned" PID gains.
*
*  Inputs:
*     *PID: pointer to a PID structure
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void TunedPID (PIDSET *PID)
{
	PID->K  = 1;	// 0.38
	PID->Ti = 5;	// 1.40
	PID->Td = 1;	//0.35
	PID->N  = 100;
	
} // End: TunedPID()


/**
*  -------------------------------------------------------  *
*  SETPIDPARAMS() gets the PID params from the user.
*
*  Inputs:
*     *PID: pointer to a PID structure
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void SetPIDParam (PIDSET *PID)
{
	float a[3];
	
	const char *text[4];
	text[0] = "Enter PID gain K = ";
	text[1] = "Enter integration time Ti = ";
	text[2] = "Enter the gain derivative time Td = ";
	
	int i;
	
	printf("Enter the parameters for the following PID implementation:\n");
	printf("PID = K ( 1 + 1 / (s Ti) + s Td / (1 + s Td / N) )\n\n");
	
	for (i = 0; i < 3; i++)
	{
		puts(text[i]);
		scanf("%f", &a[i]);
		if (i == 1 && a[i] < 0.001)
			printf("Warning: integration time is too small!\n");
		fflush(stdin);
	}
	
	PID->K  = a[0];
	PID->Ti = max(a[1], eps);	// avoiding zero devision
	PID->Td = max(a[2], 0  );
	PID->N  = 100;
		
} // End: SetPIDParam()


/**
*  -------------------------------------------------------  *
*  STEP() applies step input to a system.
*
*  Inputs:
*     fT: current time instant
*
*  Output:
*     sStep: step input command
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
short step(float fT)
{
	short sU;
	static float fStepAmp, fStepDelay;
	static unsigned char bRead = TRUE;
	
	if (bRead)
	{
		printf("Enter step size:\n");
		scanf("%f", &fStepAmp);
		fflush(stdin);
		
		if (fStepAmp < UMIN || fStepAmp > UMAX)
			printf("Warning: system input is limited to [%2.2f %2.2f]!\n", (float)UMIN, (float)UMAX);
			
		printf("Enter step delay [sec]:\n");
		scanf("%f", &fStepDelay);
		fflush(stdin);
		
		if (fStepDelay < 0)
		{
			fStepDelay = 0;
			puts("Warning: a minimum step delay equals to zero has been assined.\n");
		}
		else if (fStepDelay > SIMTIME - SAMPLINGTIME)
		{
			fStepDelay = SIMTIME - 3 * SAMPLINGTIME;
			puts("Warning: step delay is larger than the simulation time.\n");	
		}
		
		bRead = FALSE;
	}
	
	if (fT > fStepDelay)
		sU = fStepAmp * PREC;
	else
		sU = 0 * PREC;		
	
	/* reset bRead value for the next simulation */
	if (fT >= SIMTIME - SAMPLINGTIME)
		bRead = TRUE;
	
	return sU;
	
} // End: step()


/**
*  -------------------------------------------------------  *
*  GETSETPOINT() gets a set-popint value from user.
*
*  Output:
*     sSetp: set-point value
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
short GetSetpoint (void)
{
	float fSetp;
	
	short sSetp;
	
	printf("Enter a set-point value:\n");
	scanf("%f", &fSetp);
	
	sSetp = fSetp * PREC;
	
	return sSetp;
	
} // End: GetSetpoint


