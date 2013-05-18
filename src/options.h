/*
 *  options.h
 *
 *  Created on: 28.04.2013
 *      Author: Fredyy90 <frederick.behrends@gmail.com>
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_



/*
 *********************************************************************************
 *  Pins used for PWM and RPM signal, use wiringPi numbering:
 *  	http://wiringpi.com/pins/
 *
 * 	Default values:
 * 		DEFAULT_PWM_PIN: 1 (I recommend you to use Pin 1 since it has
 *                                                               hardware support)
 * 		DEFAULT_RPM_PIN: 0 (Choose the pin, that you like best)
 *********************************************************************************
 */
int DEFAULT_PWM_PIN = 1;
int DEFAULT_RPM_PIN = 0;


/*
 *********************************************************************************
 * PWM_RANGE_*:
 * Set min and max Speed for your Fan. Going lower then ~40% could end in a
 * stopped fan.
 *
 * 	Scale: 0-1024
 *
 * 	Default values:
 * 		PWM_RANGE_MIN:  40 (~53% of maximum Fanspeed)
 * 		PWM_RANGE_MAX: 100 (100% of maximum Fanspeed)
 *********************************************************************************
 */
int PWM_RANGE_MIN    = 40;
int PWM_RANGE_MAX    = 100;


/*
 *********************************************************************************
 * TEMP_RANGE_*:
 * Set the CPU temperature values, when the fan should start, and the range, in
 * which to calculate linear PWM values to keep the fan as silent as possible.
 *
 * 	Scale: 0-85
 *
 * 	Default values:
 * 		TEMP_RANGE_START:  35 (if temp lower than 35°C turn off the fan)
 * 		TEMP_RANGE_MIN  :  40 (if temp lower than 45°C run fan on minimum speed)
 * 		TEMP_RANGE_MAX  :  60 (if temp lower than 60°C scale fanspeed dynamic,
 * 				               if temp higher than 60°C run at maximum speed)
 *********************************************************************************
 */
int TEMP_RANGE_START = 35;
int TEMP_RANGE_MIN   = 40;
int TEMP_RANGE_MAX   = 60;


/*
 *********************************************************************************
 * TICKS_PER_ROTATION:
 * Count of Interrupts send by your fan per full rotation, take a look at the
 * datasheet to find the correct value.
 *********************************************************************************
 */
int TICKS_PER_ROTATION = 2;


/*
 *********************************************************************************
 * MAX_TEST_INTERVAL:
 * Minimum count of interrupts to wait befire calculating RPM, 25 seems to be a
 * good value.
 *********************************************************************************
 */
int MAX_TEST_INTERVAL = 25;


#endif /* OPTIONS_H_ */
