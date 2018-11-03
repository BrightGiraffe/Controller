/*
 * pwm.h
 *
 *  Created on: 2017Äê12ÔÂ18ÈÕ
 *      Author: donghao
 */

#ifndef PWM_PWM_H_
#define PWM_PWM_H_

#include "setup.h"
#include "sensor.h"

#define MODULATION_UPPER_THRESHOLD 0.85f
#define MODULATION_LOWER_THRESHOLD -0.85f

#define MODULATION_UPPER_THRESHOLD_UINT 7000u
#define MODULATION_LOWER_THRESHOLD_UINT 500u

#define UNIPOLAR_MODULATION 0
#define UNIPOLAR_MODULATION_2 1

#if UNIPOLAR_MODULATION_2
void PWM_Set_Positive(void) ;
void PWM_Set_Negative(void) ;
#endif

void PWM_Init(int freq);

#endif /* PWM_PWM_H_ */
