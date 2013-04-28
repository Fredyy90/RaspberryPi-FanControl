/*
 * rpm-interval.h
 *
 *  Created on: 20.04.2013
 *      Author: frede_000
 */

#ifndef RPM_INTERVAL_H_
#define RPM_INTERVAL_H_

extern void setRpmPin ( int pin );
extern void setTicksPerRotation ( int ticks );
extern void setTestInterval ( int ticks );
extern int getRpm( void );


#endif /* RPM_INTERVAL_H_ */
