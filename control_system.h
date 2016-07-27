#ifndef __CONTROL_SYSTEM_H__
#define __CONTROL_SYSTEM_H__


// PID gains for ideal form implementation
typedef struct tagPIDSET
{
	float K ;		// PID gain
	float Ti;		// integration time
	float Td;		// derivative time
	short N ;		// derivative filter factor	
} PIDSET;

short step(float fT);

short PIDCtrl (unsigned char bReset, const PIDSET *PID, short sR, short sY, float fTs);

short AutoTune (unsigned char *bTuned, PIDSET *PID, float fTime, short sR, short sY, float fTs);

void TunedPID(PIDSET *PID);

void SetPIDParam (PIDSET *PID);

short GetSetpoint (void);

#endif // __CONTROL_SYSTEM_H__
