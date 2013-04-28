/*
 * fan-pwm.h
 *
 *  Created on: 22.04.2013
 *      Author: frede_000
 */

#ifndef FAN_PWM_H_
#define FAN_PWM_H_

extern int getTemp( void );
extern int getNewFanSpeed( int temp );
extern int updateFanPWM( int pwm );

#endif /* FAN_PWM_H_ */
