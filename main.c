#include <stdio.h>
#include <math.h>
#include "simulation.h"
#include "util_func.h"
#include "interface.h"

int main ()
{
	short sSimCase;
	
	char *cFileName;
	
	/* provide a file name to save simulation data into*/
	cFileName = "sim_data.dat";
	
	WelcomeText();
	
	while(1)
	{
		/* simulation initialization */
		SIMSET SimSet = SimInit(SIMTIME);
		
		/* receive user command */
		sSimCase = UserInput();
		
		/* main simulation loop */
		simulation(&SimSet, sSimCase, cFileName);
		
		/* plot data from a file */
		PlotData(cFileName, SimSet.uNbrIter);
		
		/* check if user wants to stop */
		UserStop();
	}	
 	
	//system("pause");	// can also use getch ();
	return 0;
	
} // End: main()


