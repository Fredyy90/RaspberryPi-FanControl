/*
 *  fan-pwm.h
 *
 *  Created on: 22.04.2013
 *      Author: Fredyy90 <frederick.behrends@gmail.com>
 */


#ifndef FAN_PWM_H_
#define FAN_PWM_H_

extern int setPwmPin ( int pin );
extern int setPwmRange ( int min, int max );
extern int setTempRange ( int start, int pwm, int max );

extern int getTemp ( void );
extern int getNewFanSpeed ( int temp );
extern int updateFanPWM ( int pwm );

#endif /* FAN_PWM_H_ */
