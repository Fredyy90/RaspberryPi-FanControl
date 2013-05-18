/*
 *  fan-rpm.c
 *
 *  Created on: 20.04.2013
 *      Author: Fredyy90 <frederick.behrends@gmail.com>
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <wiringPi.h>

/*
 *********************************************************************************
 * Setup variables and default values
 *********************************************************************************
 */
static volatile int INTERRUPT_COUNT    = 0;
static volatile int RPM_PIN            = 0;
static volatile int RPM_PIN_GPIO       = 0;
static volatile int TEST_INTERVAL      = 25;
static volatile int TICKS_PER_ROTATION = 2;
static volatile int TICKS              = 0;
static volatile int INIT_RPM		   = 0;

int 	setRpmPin ( int pin );
int 	setTicksPerRotation ( int ticks );
int 	setTestInterval ( int ticks );
void 	interruptCounter (void);
double 	time_diff(struct timeval x , struct timeval y);
int 	setupWiringPiRPM( void );
void 	pauseInterruptHandler(int pause);
int 	getRpm( void );

/*
 *********************************************************************************
 * setRpmPin():
 *********************************************************************************
 */
int setRpmPin ( int pin )
{
	RPM_PIN = pin;
	return ( 0 );
}


/*
 *********************************************************************************
 * setTicksPerRotation():
 *********************************************************************************
 */
int setTicksPerRotation ( int ticksPerRotation )
{
	if(ticksPerRotation % 2 != 0){
		return ( 1 );
	}else{
		TICKS_PER_ROTATION = ticksPerRotation;
		return ( 0 );
	}
}


/*
 *********************************************************************************
 * setTestInterval():
 *********************************************************************************
 */
int setTestInterval ( int testInterval )
{
	TEST_INTERVAL = testInterval;
	return ( 0 );
}


/*
 *********************************************************************************
 * interruptCounter():
 * Count up the Interrupts
 *********************************************************************************
 */
void interruptCounter (void)
{
	INTERRUPT_COUNT++;
}


/*
 *********************************************************************************
 * time_diff():
 * Calculate time difference in ms between 2 timeval
 *********************************************************************************
 */
double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000 + (double)x.tv_usec/1000;
    y_ms = (double)y.tv_sec*1000 + (double)y.tv_usec/1000;

    diff = (double)y_ms - (double)x_ms;

    return (diff);
}


/*
 *********************************************************************************
 * setupWiringPi():
 * Setup wiringPi, RPM Pin and InterruptHandler
 *********************************************************************************
 */
int setupWiringPiRPM( void )
{
	RPM_PIN_GPIO = wpiPinToGpio(RPM_PIN);

    pullUpDnControl (RPM_PIN, PUD_UP);
    if ( wiringPiISR (RPM_PIN, INT_EDGE_RISING, &interruptCounter) != 0 )
    {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
        return (-1) ;
    }

    return (0) ;

}


/*
 *********************************************************************************
 * pauseInterruptHandler():
 * Pause and restart the InterruptHandler to save Performance, when not active
 * measuring.
 *********************************************************************************
 */
void pauseInterruptHandler(int pause)
{

	char cmd[50];
    if ( pause == 1 )
    {
    	sprintf (cmd, "/usr/local/bin/gpio edge %d none", RPM_PIN_GPIO);
    	system (cmd);
    }
    else
    {
    	sprintf (cmd, "/usr/local/bin/gpio edge %d rising", RPM_PIN_GPIO);
    	system (cmd);
    }

}


/*
 *********************************************************************************
 * getRpm():
 * get current Fan rpm
 *********************************************************************************
 */
int getRpm( void )
{

    if (INIT_RPM == 0 && setupWiringPiRPM() != 0)
    {
		fprintf (stderr, "RPM setup Failed\n") ;
		exit (EXIT_FAILURE) ;
    }
    else
    {
    	pauseInterruptHandler(0);
    }

    INIT_RPM = 1;

	double maxTimePerTick = (60 * 1000) / (TICKS_PER_ROTATION * 500);
    double timeLimit      = (double)TEST_INTERVAL * maxTimePerTick ;
    long rpm              = 0 ;
    struct timeval before, after;

    INTERRUPT_COUNT = 0;
    gettimeofday(&before , NULL);

    while (INTERRUPT_COUNT < TEST_INTERVAL)
    {
    	gettimeofday(&after , NULL);
        if( time_diff(before, after) > timeLimit )
        {
        	pauseInterruptHandler(1);
        	return( -1 );
        }
        delay(75);
    }

    TICKS = INTERRUPT_COUNT;
    gettimeofday(&after , NULL);

	pauseInterruptHandler(1);

    rpm = ( TICKS / TICKS_PER_ROTATION ) * ( (1000 * 60) / time_diff(before, after));

    return( (int)rpm );
}
