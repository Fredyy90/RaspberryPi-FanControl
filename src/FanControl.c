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

void signal_callback_handler(int signum)
{
   printf("\nCaught signal %d\n",signum);
   updateFanPWM(0);
   exit(signum);
}

int main()
{

	signal(SIGINT, signal_callback_handler);
	signal(SIGTERM, signal_callback_handler);
	signal(SIGKILL, signal_callback_handler);

	int i = 0;
	while(1 /*i < 10*/)
	{

		printf("RUN: %i\t", i);

		int temp = getTemp();
		printf("TEMP: %0.2f°C\t", (float) temp/1000);

		int pwm = getNewFanSpeed( temp );
		//int pwm = 1024;
		printf("PWM: %i\t", pwm);

		int rpm = updateFanPWM(pwm);
		printf("RPM: %i\t", rpm);

		printf("\n");

		sleep(5000);

		i++;
	}

	return ( EXIT_SUCCESS );

}
