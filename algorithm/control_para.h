/*
 * control_para.h
 *
 *  Created on: 2018Äê3ÔÂ12ÈÕ
 *      Author: donghao
 */

#ifndef CONTROLLER_CONTROL_PARA_H_
#define CONTROLLER_CONTROL_PARA_H_

/***************************************************************************************************/
// #define VOLTAGE_REF 80.0f
#define CURRENT_REF 10.0f
// #define FREQ_G 50.0f

#define KP_PLL 0.2
#define KI_PLL 0.6
#define K_GAIN_SOGI_PLL 1.4142135623730950488016887242097

// #define TS_PLL_TEN_KHZ 1e-4
#define TS_TWENTY_KHZ 5e-5

#define K_FEEDFORWARD 1.0

extern const float num_ge[6] ;
extern const float den_ge[6] ;
extern const float num_gi[10] ;
extern const float den_gi[10] ;


#endif /* CONTROLLER_CONTROL_PARA_H_ */
