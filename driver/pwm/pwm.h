/*
 * pwm.h
 *
 *  Created on: 2017��12��18��
 *      Author: donghao
 */

#ifndef PWM_PWM_H_
#define PWM_PWM_H_

#include "setup.h"
#include "sensor.h"

#define MODULATION_UPPER_THRESHOLD 0.85f
#define MODULATION_LOWER_THRESHOLD -0.85f

#define MODULATION_UPPER_THRESHOLD_UINT 7350u
#define MODULATION_LOWER_THRESHOLD_UINT 150u

#define UNIPOLAR_MODULATION 1

void PWM_Init(int freq);

#endif /* PWM_PWM_H_ */
