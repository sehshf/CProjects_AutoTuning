#include "interface.h"
#include "util_func.h"
#include <stdio.h>
#include <stdlib.h>

/**
*  -------------------------------------------------------  *
*  USERINPUT() asks user to choose the simulation case.
*
*  Outputs:
*     sel: simulation case
*     
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
unsigned char UserInput (void)
{
	#define NbrSim   4
	
	int sel;
		
	printf("Select the simulation case:\n   1. Step response\n   2. Already tuned PID\n");
	printf("   3. Manual tuning\n   4. Automatic tuning\n");
	scanf("%i", &sel);
	fflush(stdin);
	
	if (sel < 1 || sel > NbrSim)
	{
		printf("Error: wrong number. Please enter one of the case numbers.\n");
		exit(0);
	}

	return --sel;
	
} // End: UserInput()


/**
*  -------------------------------------------------------  *
*  WELCOMETEXT() shows a welcome text and system transfer
*  function.
*     
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void WelcomeText (void)
{
   #define FRAMETEXT "* * * * * * * * * * * * * * * * * * * * * *"
	#define MIDTEXT   "*                                         *"
	
	puts("This program executes a discrete-time simulation");
	puts("for the following linear dynamical system:\n");
	puts(FRAMETEXT);
	puts(MIDTEXT);
	puts("*                   2 s + 2               *");
	puts("*  G(s) = -----------------------------   *");
	puts("*         s^3 + 0.8 s^2 + 4.2 s + 1.616   *");
	puts(MIDTEXT);
	puts(FRAMETEXT);
	puts("\n");
	
	/*
			* * * * * * * * * * * * * * * * * * * * * *
			*                                         *
			*                   2 s + 2               *
			*  G(s) = -----------------------------   *
			*         s^3 + 0.8 s^2 + 4.2 s + 1.616   *
			*                                         *
			* * * * * * * * * * * * * * * * * * * * * *
	*/
} // End: WelcomeText()


/**
*  -------------------------------------------------------  *
*  USERSTOP() retunrs weather user wants to stop or conti-
*  nue.
*     
*  Author: S. Ehsan Shafiei
*          Jul. 2015
*  -------------------------------------------------------  *
*/
void UserStop(void)
{
	char ch = 0;
	
	puts("Would you like to continue with the program?[y/n]\n");
	
	while (1)
	{
		ch = getch();
		
		if (ch == 'y' || ch == 'Y')
			break;
		else if (ch == 'n' || ch == 'N')
		   exit(0);		
		else
		   puts("Please answer with 'y' or 'n'.\n");
	}
		
} // End: UserStop;



