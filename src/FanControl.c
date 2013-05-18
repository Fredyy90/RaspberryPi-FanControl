/*
 *  FanControl.c
 *
 *  Created on: 20.04.2013
 *      Author: Fredyy90 <frederick.behrends@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <wiringPi.h>
//#include <mcp23017.h>

#include "options.h"
#include "fan-pwm.h"
#include "fan-rpm.h"


volatile sig_atomic_t g_eflag = 0;
volatile sig_atomic_t g_hupflag = 1;


static void signal_handler(int sig);
void setup_signal_handler( void );
int setup( void );
int writeFiles(int pwm, int rpm);
int main();


/*
 *********************************************************************************
 * signal_handler():
 * handle signals to end the loop or reload config
 *********************************************************************************
 */
static void signal_handler(int sig)
{
    switch(sig)
    {
    case SIGHUP:
        g_hupflag = 1;
        break;
    case SIGINT:
    case SIGTERM:
        g_eflag = 1;
        break;
    }
}


/*
 *********************************************************************************
 * setup_signal_handler():
 * Setup signal handlers
 *********************************************************************************
 */
void setup_signal_handler( void )
{

	if( signal(SIGINT,  signal_handler) == SIG_ERR ||
		signal(SIGTERM, signal_handler) == SIG_ERR ||
		signal(SIGQUIT, signal_handler) == SIG_ERR ||
		signal(SIGPIPE, SIG_IGN) == SIG_ERR
	) {
		printf("Binding signal handlers failed!\n");
		exit ( EXIT_FAILURE );
	}
}


/*
 *********************************************************************************
 * setup():
 * Setup modules, with the values from the config
 *********************************************************************************
 */
int setup( void )
{

    if ( wiringPiSetup() != 0 )
    {
        printf ("Unable to setup wiringPi\n") ;
        return (-1);
    }
	//mcp23017Setup (I2C_PINBASE, I2C_ADDRESS) ;

	if( setPwmPin(DEFAULT_PWM_PIN) != 0 ){
		printf("Setting PWM-pin failed!\n");
		exit ( EXIT_FAILURE );
	}
	if( setPwmRange ( PWM_RANGE_MIN, PWM_RANGE_MAX ) != 0 ){
		printf("Setting PWM-range failed!\n");
		exit ( EXIT_FAILURE );
	}
	if( setTempRange ( TEMP_RANGE_START, TEMP_RANGE_MIN, TEMP_RANGE_MAX ) != 0 ){
		printf("Setting temprange failed!\n");
		exit ( EXIT_FAILURE );
	}
	if( setRpmPin ( DEFAULT_RPM_PIN ) != 0 ){
		printf("Setting RPM-pin failed!\n");
		exit ( EXIT_FAILURE );
	}
	if( setTicksPerRotation ( TICKS_PER_ROTATION ) != 0 ){
		printf("Ticks per rotation has to be divisible by 2!\n");
		exit ( EXIT_FAILURE );
	}
	if( setTestInterval ( MAX_TEST_INTERVAL ) != 0 ){
		printf("Setting TestInterval failed.\n");
		exit ( EXIT_FAILURE );
	}

	return ( 0 );

}


/*
 *********************************************************************************
 * writeFiles(pwm, rpm):
 * write files to be read by other programs
 *********************************************************************************
 */
int outfile;
int writeFiles(int pwm, int rpm)
{

	char tmp[12]={0x0};

	mkdir("/dev/fan", S_IRWXU);

	outfile = open("/dev/fan/rpm", O_WRONLY|O_CREAT|O_TRUNC, 0766);	/*create the new file */
	sprintf(tmp,"%d\n", rpm);
	write(outfile, tmp, sizeof(tmp)); 								/*write the rpm to the file */
	close(outfile); 										/*close the file */

	outfile = open("/dev/fan/pwm", O_WRONLY|O_CREAT|O_TRUNC, 0766);	/*create the new file */
	sprintf(tmp,"%d\n", pwm);
	write(outfile, tmp, sizeof(tmp)); 								/*write the rpm to the file */
	close(outfile); 										/*close the file */

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

	setup_signal_handler();

	int i    = 0,
		temp = 0,
		pwm  = 0,
		rpm  = 0;

	while (!g_eflag)
	{

        if(g_hupflag)
        {

            g_hupflag = 0;
            if( setup() != 0 )
        	{
        		printf("There is an error in your configuration\n");
        	}

        }

		temp = getTemp();
		pwm  = getNewFanSpeed( temp );
		rpm  = updateFanPWM( pwm );

		writeFiles(pwm, rpm);

		printf("RUN: %i\tTEMP: %0.2f°C\tPWM: %i\tRPM: %i\n", i, (float) temp/1000, pwm, rpm);

		sleep(5);

		i++;

	}

	setPWMValue ( 0 );
	writeFiles  (0, 0);

	exit ( EXIT_SUCCESS );

}
