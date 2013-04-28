//============================================================================
// Name        : Hello.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "fan-pwm.h"

void signal_callback_handler(int signum);
int setup( void );
int main();


/*
 *********************************************************************************
 * signal_callback_handler():
 * handle signals to turn off fan before ending the programm
 *********************************************************************************
 */
void signal_callback_handler(int signum)
{
   updateFanPWM ( 0 );
   printf("Fan stopped, exiting.. \n",signum);
   exit ( signum );
}


/*
 *********************************************************************************
 * setup():
 * Setup modules, with the values from config.h and set signal handlers
 *********************************************************************************
 */
int setup( void )
{

	/*
	if( setPwmPin ( pin ) != 0 ){
		printf("Setting PWM-pin failed!");
		exit ( EXIT_FAILURE );
	}
	if( setPwmRange ( min, max ) != 0 ){
		printf("Setting PWM-range failed!");
		exit ( EXIT_FAILURE );
	}
	if( setTempRange ( start, pwm, max ) != 0 ){
		printf("Setting temprange failed!");
		exit ( EXIT_FAILURE );
	}
	if( setRpmPin ( pin ) != 0 ){
		printf("Setting RPM-pin failed!");
		exit ( EXIT_FAILURE );
	}
	if( setTicksPerRotation ( ticks ) != 0 ){
		printf("Ticks per rotation has to be divisible by 2!");
		exit ( EXIT_FAILURE );
	}
	if( setTestInterval ( ticks ) != 0 ){
		printf("Setting TestInterval failed.");
		exit ( EXIT_FAILURE );
	}
	*/
	if( signal(SIGINT,  signal_callback_handler) == SIG_ERR ||
		signal(SIGTERM, signal_callback_handler) == SIG_ERR ||
		signal(SIGQUIT, signal_callback_handler) == SIG_ERR
	) {
		printf("Binding signal handlers failed!");
		exit ( EXIT_FAILURE );
	}

	return ( 0 );

}


/*
 *********************************************************************************
 * main():
 * The main loop of this program
 *********************************************************************************
 */
int main()
{

	if( getuid() != 0 )
	{
		printf("You must be root to run this app\n");
		exit ( EXIT_FAILURE );
	}

	if( setup() != 0 )
	{
		printf("There is an error in your configuration\n");
		exit ( EXIT_FAILURE );
	}

	int i    = 0,
		temp = 0,
		pwm  = 0,
		rpm  = 0;

	while( 1 )
	{

		temp = getTemp();
		pwm  = getNewFanSpeed( temp );
		rpm  = updateFanPWM( pwm );

		printf("RUN: %i\tTEMP: %0.2f°C\tPWM: %i\tRPM: %i\n", i, (float) temp/1000, pwm, rpm);

		sleep(5);

		i++;
	}

	exit ( EXIT_SUCCESS );

}
