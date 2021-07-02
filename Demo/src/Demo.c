/***********************************************************************
*
*  FILE        : Demo.c
*  DATE        : 2021-07-02
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "r_smc_entry.h"

void main(void);

void main(void)
{
	PORTD.PDR.BIT.B6 = 1;
	PORTD.PDR.BIT.B7 = 1;

	R_Config_DA1_Start();
	R_Config_TMR0_TMR1_Start();

	while(1)
	{
	}

}
