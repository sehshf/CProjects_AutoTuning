#include <string.h>
#include <stdlib.h>

#include "util_func.h"
#include "data_treatment.h"
#include "gnuplot_i.h"



/**
*  -------------------------------------------------------  *
*  SAVEDATA() saves input-output data in an specific form-
*  -at:
*     DATA: {time input output}
*
*  Inputs:
*     *file: pointer to an openned file
*     time : current time instant
*     sIn  : input value
*     sOut : output value
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void SaveData(
	     FILE *file, 
		  double time, 
		  short sIn, 
		  short sOut
		  )
{
	double lfIn, lfOut;
	
	lfIn  = (double)sIn  / PREC;
	lfOut = (double)sOut / PREC;
	
	fprintf(file, "%2.2f %2.2f %2.2f\n", time, lfIn, lfOut);
	fflush(stdout);
} // End: SaveData()


/**
*  -------------------------------------------------------  *
*  READIODATA() reads input-output data from a file.
*     DATA: {time input output}
*
*  Inputs:
*     *file  : pointer to an openned file
*     uLength: length of a culomn
*     
*  Outputs:
*     DataSet: a DATASET structure
*
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
DATASET ReadIOData (FILE *file, unsigned uLength)
{
	unsigned count = 0;
	DATASET DataSet;
	
	/* assign dynamic size arrays. */
	DataSet.Time   = malloc(sizeof(double) * uLength);
	DataSet.Input  = malloc(sizeof(double) * uLength);
	DataSet.Output = malloc(sizeof(double) * uLength);
	
	/* check if memory are allocated successfully */
	if (DataSet.Time == NULL || DataSet.Input == NULL || DataSet.Output == NULL)
		puts("Error: memory allocaion failed!\n");
	
	/* check for file opening. */
	if (!file)
	{
		perror("Error opening file");		
	}
	
	/* read data from the file. */
	while (!feof(file) && (count < uLength))	// check for the end of file
	{
		fscanf(file, "%lf %lf %lf", &(DataSet.Time[count]), &(DataSet.Input[count]), &(DataSet.Output[count]));
		fflush(stdin);		
		count++;
	}
	
	DataSet.Length = uLength;
	
	return DataSet;
} // End: ReadIOData()


/**
*  -------------------------------------------------------  *
*  PLOTDATA() plots input-output data.
*     DATA: {time input output}
*
*  Inputs:
*     cFileName: name of the file to read data from
*     uLength  : length of a culomn
*     
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void	PlotData(const char *cFileName, unsigned uLength)
{
	DATASET IOData;
	
	FILE *DataFile;
	
	DataFile = fopen(cFileName, "r");
	IOData = ReadIOData(DataFile, uLength);
	fclose(DataFile);
	
//	gnuplot_ctrl *h;
//	h = gnuplot_init();
	
	fflush(stdin);
	gnuplot_plot_once("Input Data", "lines", "tim [sec]", "Input",   IOData.Time, IOData.Input, IOData.Length);
	
	fflush(stdin);	
	gnuplot_plot_once("Output Data", "lines", "tim [sec]", "Output", IOData.Time, IOData.Output, IOData.Length);

//	gnuplot_close(h);
	fflush(stdout);
	
	/* release the allocated memory. */
	free(IOData.Time)  ;
	free(IOData.Input) ;
	free(IOData.Output);
	
} // End: PlotData()
	
	

unsigned long fsize(FILE *file)
{
	unsigned long uLen;
	
   fseek(file, 0, SEEK_END);
	uLen = (unsigned long)ftell(file);
	fseek(file, 0, SEEK_SET);
	
	return uLen;
}	// End: fsize()
