/*
 *  fan-pwm.c
 *
 *  Created on: 22.04.2013
 *      Author: Fredyy90 <frederick.behrends@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
//#include "../wiringPi/wiringPi/wiringPi.h"
#include "fan-rpm.h"


/*
 *********************************************************************************
 * Setup variables and default values
 *********************************************************************************
 */
static volatile int PWM_PIN    = 1;
static volatile int PWM_MIN    = 40;
static volatile int PWM_MAX    = 100;
static volatile int TEMP_ZERO  = 35000;
static volatile int TEMP_PWM   = 40000;
static volatile int TEMP_MAX   = 60000;
static volatile int TEMP_CACHE = 0;
static volatile int INIT_PWM   = 0;

int setPwmPin ( int pin );
int setPwmRange ( int min, int max );
int setTempRange ( int start, int pwm, int max );
int getTemp ( void );
int setupWiringPiPWM ( void );
int getNewFanSpeed ( int temp );
int setPWMValue ( int pwm );
int updateFanPWM ( int pwm );


/*
 *********************************************************************************
 * setPwmPin():
 *********************************************************************************
 */
int setPwmPin ( int pin )
{
	if(pin != 1){
		return ( 1 );
	}else{
		PWM_PIN = pin;
		return ( 0 );
	}
}


/*
 *********************************************************************************
 * setPwmRange():
 * Set min and max Speed for your Fan. Going lower then ~40% could end in a
 * stopped fan.
 * 	Scale: 0-1024
 * 	Default values:
 * 		min:  550 (~53% of maximum Fanspeed)
 * 		max: 1024 (100% of maximum Fanspeed)
 *********************************************************************************
 */
int setPwmRange ( int min, int max )
{
	if( min >= 0   && min <= 1024 &&
		max >= min && max <= 1024
	){
		PWM_MIN = min;
		PWM_MAX = max;
		return ( 0 );
	}else{
		return ( 1 );
	}
}


/*
 *********************************************************************************
 * setTempRange():
 * Set the CPU temperature values, when the fan should start, and the range, in
 * which to calculate linear PWM values to keep the fan as silent as possible.
 * 	Scale: 0-85°C
 * 	Default values:
 * 		start:  35 (if temp lower than 35°C turn off the fan)
 * 		pwm  :  40 (if temp lower than 45°C run fan on minimum speed)
 * 		max  :  60 (if temp lower than 60°C scale fanspeed dynamic,
 * 					if temp higher than 60°C run at maximum speed)
 *********************************************************************************
 */
int setTempRange ( int start, int pwm, int max )
{

	if( start >= 0     && start <= 85 &&
		pwm   >= start && pwm   <= 85 &&
		max   >= pwm   && max   <= 85
	){
		TEMP_ZERO = start * 1000;
		TEMP_PWM  = pwm * 1000;
		TEMP_MAX  = max * 1000;
		return ( 0 );
	}else{
		return ( 1 );
	}

}


/*
 *********************************************************************************
 * getTemp():
 * Get current CPU temperature
 *********************************************************************************
 */
int getTemp( void )
{
	FILE* tempfile;
	tempfile = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
	fscanf (tempfile, "%d", &TEMP_CACHE);
	fclose (tempfile);
	return (TEMP_CACHE);
}


/*
 *********************************************************************************
 * setupWiringPiPWM():
 * Setup wiringPi and PWM Pin
 *********************************************************************************
 */
int setupWiringPiPWM( void )
{
    pullUpDnControl (PWM_PIN, PUD_UP);
	
	if(PWM_PIN != 0){
		softPwmCreate(PWM_PIN, 100, 100);
	}else{
		pinMode (PWM_PIN, PWM_OUTPUT);
	}

    return (0);
}


/*
 *********************************************************************************
 * getNewFanSpeed():
 * Calculate new fanspeed as PWM value, based on CPU temperature
 *********************************************************************************
 */
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
		return (PWM_MIN);
	}
	else
	{
		float m = ( PWM_MAX - PWM_MIN ) / ( (TEMP_MAX/1000) - (TEMP_PWM/1000) );
		float b = PWM_MIN - m * (TEMP_PWM/1000);
		//printf( "m: %f \t b: %f", m, b);
		return ((int) ( m * (float)(temp/1000) + b));
	}

	return (0);
}


/*
 *********************************************************************************
 * setPWMValue():
 * Update PWM Speed
 *********************************************************************************
 */
int setPWMValue( int pwm )
{

	if(PWM_PIN != 0){
		softPwmWrite (PWM_PIN, pwm);
	}else{
		pwmWrite (PWM_PIN, (int)(pwm * 1024/100));
	}

    return (0);
}


/*
 *********************************************************************************
 * updateFanPWM():
 * Set PWM duty cycle to updated value
 *********************************************************************************
 */
int updateFanPWM( int pwm )
{
	// check if wiringPi loaded correctly
    if (INIT_PWM == 0 && setupWiringPiPWM() != 0)
    {
		fprintf (stderr, "RPM setup Failed\n") ;
		exit (EXIT_FAILURE) ;
    }

    INIT_PWM = 1;

	if (pwm < 0 || pwm > PWM_MAX)
	{
		fprintf (stderr, "Invalid PWM Value! Invalid Value: %i, MIN: %d, MAX: %d\n", pwm, 0, PWM_MAX);
		return (-1);
	}
	else if (pwm == 0)
	{
		setPWMValue (0);
		return (0);
	}
	else
	{
		setPWMValue (100);
		int rpm = getRpm();
		setPWMValue (pwm);
		return (rpm);
	}
}
