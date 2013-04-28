/*
 *  rpm-interval.c:
 *  Wait for Interrupt test program - ISR method
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <wiringPi.h>
//#include "../wiringPi/wiringPi/wiringPi.h"

void setRpmPin ( int pin );
void setTicksPerRotation ( int ticks );
void setTestInterval ( int ticks );

int setupWiringPiRPM( void );
int getRpm( void );

double time_diff(struct timeval x , struct timeval y);


// globalCounter:
//  Global variable to count interrupts
//  Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter    = 0;
static volatile int rpmPin           = 0;
static volatile int rpmPinGPIO       = 0;
static volatile int testInterval     = 25;
static volatile int ticksPerRotation = 2;
static volatile int ticks            = 0;
static volatile int initRPM			 = 0;
char cmd[50];

/*
 * setRpmPin:
 *********************************************************************************
 */

void setRpmPin ( int pin )
{
	rpmPin = pin;
}

/*
 * setTicksPerRotation:
 *********************************************************************************
 */

void setTicksPerRotation ( int ticks )
{
	ticksPerRotation = ticks;
}

/*
 * setTestInterval:
 *********************************************************************************
 */

void setTestInterval ( int ticks )
{
	testInterval = ticks;
}

/*
 * interruptCounter:
 *********************************************************************************
 */

void interruptCounter (void)
{
	//printf("%d - ", ++globalCounter);
	++globalCounter;
}

/*
 * time_diff:
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
 * setupWiringPi
 *********************************************************************************
 */


int setupWiringPiRPM( void )
{

    if ( wiringPiSetup() != 0 )
    {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
        return (-1);
    }

	rpmPinGPIO = wpiPinToGpio(rpmPin);

    pullUpDnControl (rpmPin, PUD_UP);
    if ( wiringPiISR (rpmPin, INT_EDGE_RISING, &interruptCounter) != 0 )
    {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
        return (-1) ;
    }

    return (0) ;

}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int getRpm( void )
{

    if (initRPM == 0 && setupWiringPiRPM() != 0)
    {
      fprintf (stderr, "RPM setup Failed\n") ;
      exit (EXIT_FAILURE) ;
    }else{
    	sprintf (cmd, "/usr/local/bin/gpio edge %d rising", rpmPinGPIO);
    	system (cmd);
    }

    initRPM = 1;

	double maxTimePerTick = (60 * 1000) / (ticksPerRotation * 500);
    double timeLimit      = (double)testInterval * maxTimePerTick ;
    long rpm              = 0 ;
    struct timeval before, after;

    globalCounter =  0;

    gettimeofday(&before , NULL);

    while (globalCounter < testInterval)
    {
    	gettimeofday(&after , NULL);
        if( time_diff(before, after) > timeLimit )
        {
        	//fprintf (stderr, "Counting to %d Ticks took longer then %.2fms. Abort after %.2fms\n", testInterval, timeLimit, time_diff(before, after));
            return( -1 );
        }
        delay(75);
    }

    ticks = globalCounter;
    gettimeofday(&after , NULL);

	sprintf (cmd, "/usr/local/bin/gpio edge %d none", rpmPinGPIO);
	system (cmd);
	//system (sprintf("/usr/local/bin/gpio edge %d none", rpmPinGPIO));

    rpm = ( ticks / ticksPerRotation ) * ( (1000 * 60) / time_diff(before, after));

    //fprintf (stderr, "ticks: %d  ticksPerRotation: %dms. time_diff: %.2fms\n", ticks, ticksPerRotation, time_diff(before, after));

    return( (int)rpm );
}
