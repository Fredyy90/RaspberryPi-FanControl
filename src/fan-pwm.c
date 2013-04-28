/*
 * fan-pwm.c
 *
 *  Created on: 22.04.2013
 *      Author: frede_000
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
//#include "../wiringPi/wiringPi/wiringPi.h"
#include "fan-rpm.h"

/*#include <unistd.h>
#include <sys/stat.h>*/

int getTemp( void );
int setupWiringPiPWM( void );
int getNewFanSpeed( int temp );
int updateFanPWM( int pwm );

//-------INIT-------//
int PWM_PIN = 1;
//------------------//
static volatile int PWM_ZERO = 550;
static volatile int PWM_MAX = 1024;
//------------------//
//int TEMP_ZERO = 45000;
//int TEMP_PWM = 50000;
static volatile int TEMP_ZERO = 35000;
static volatile int TEMP_PWM  = 40000;
static volatile int TEMP_MAX  = 60000;
//---- getTemp -----//
FILE* tempfile;
static volatile int tempCache = 0;
static volatile int initPWM = 0;
//------------------//

int getTemp( void )
{
	// Read Temp
	tempfile = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
	fscanf (tempfile, "%d", &tempCache);
	fclose (tempfile);
	return (tempCache);
}

int setupWiringPiPWM( void )
{

    if ( wiringPiSetup() != 0 )
    {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
        return (-1);
    }

    pullUpDnControl (PWM_PIN, PUD_UP);
    pinMode (PWM_PIN, PWM_OUTPUT);

    return (0);
}

int getNewFanSpeed( int temp )
{
	if ( temp < TEMP_ZERO )
	{
		return (0);
	}
	else if ( temp > TEMP_MAX )
	{
		return (PWM_MAX);
	}
	else if ( temp < TEMP_PWM )
	{
		return (PWM_ZERO);
	}
	else
	{
		float m = ( PWM_MAX - PWM_ZERO ) / ( (TEMP_MAX/1000) - (TEMP_PWM/1000) );
		float b = PWM_ZERO - m * (TEMP_PWM/1000);
		//printf( "m: %f \t b: %f", m, b);
		return ((int) ( m * (float)(temp/1000) + b));
	}

	return (0);
}

int updateFanPWM( int pwm )
{
	// check if wiringPi loaded correctly
    if (initPWM == 0 && setupWiringPiPWM() != 0)
    {
		fprintf (stderr, "RPM setup Failed\n") ;
		exit (EXIT_FAILURE) ;
    }

    initPWM = 1;

	if (pwm < 0 || pwm > PWM_MAX)
	{
		fprintf (stderr, "Invalid PWM Value! Invalid Value: %i, MIN: %d, MAX: %d\n", pwm, 0, PWM_MAX);
		return (-1);
	}
	else if (pwm == 0)
	{
		pwmWrite (PWM_PIN, pwm);
		return (0);
	}
	else
	{
		pwmWrite (PWM_PIN, PWM_MAX);
		int rpm = getRpm();
		pwmWrite (PWM_PIN, pwm);
		return (rpm);
	}
}
