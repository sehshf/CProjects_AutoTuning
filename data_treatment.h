#ifndef __DATATREATMENT_H__
#define __DATATREATMENT_H__

#include <stdio.h>

#include "simulation.h"

typedef struct tagDataSet {
	double 	*Time  ;
	double 	*Input ;
	double 	*Output;
	unsigned Length ;
} DATASET;

void SaveData(FILE *file, double time, short sIn, short sOut);

DATASET ReadIOData (FILE *file, unsigned uLength);

void	PlotData(const char *cFileName, unsigned uLength);

unsigned long fsize(FILE *file);

#endif	// __DATATREATMENT_H__
