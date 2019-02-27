/*
 * control_para.c
 *
 *  Created on: 2018��10��11��
 *      Author: donghao
 */

#include "control_para.h"

const float num_ge[6] = {63.5324749336048      ,   -165.058509528854      ,    160.980255884043     ,    -73.9086964860023      ,    16.2950118341303 , -1.50182021620002};
const float den_ge[6] = { 1     ,    -2.22507124567522     ,     1.80900411465748    ,    -0.723613053538164     ,    0.151915879215545  ,-0.0122356946596432};

const float num_gi[10] = {0      ,    -3.1776786060705      ,    25.3519171606937    ,     -63.2529388889476     ,     72.4550323542472    ,  -41.5868829509874      ,    10.7953432723197    ,    -0.198537823951556    ,    -0.449903290440236    ,      0.06314008324708};
const float den_gi[10] = {1   ,       -3.7837157951766      ,    6.18811409586531      ,   -5.80693836435801     ,     3.47743608641536   ,  -1.3876531780718      ,   0.370422546860972    ,   -0.063781269560913   ,    0.00639796108563532    , -0.000282083059946979};

/*
 *
 *
 * const float num_ge[6] = {80.5795087118232       ,  -212.572214750902     ,      210.04937956655      ,   -97.5695398325299     ,     21.7413583035998   , -1.90725847365252};
const float den_ge[6] = {1   ,      -2.11157911053213     ,     1.60296053112161     ,   -0.595136753748852       ,  0.112303690969715  ,   -0.00854835781034358};

const float num_gi[10] = { 0    ,      17.8833630774641       ,  -58.6511323588281      ,     71.254805454267     ,    -36.3892826923091 ,2.61845621456233   ,      5.18435994734666    ,     -2.24539016639837    ,     0.364678337120256    ,   -0.0202998614579943};
const float den_gi[10] = {  1      ,   -3.67022366003352       ,    5.8051766144774       ,   -5.2539203399831        ,  3.02300743042215    ,  -1.15380237588017       ,  0.293432907599068    ,   -0.0480856011936061     ,  0.00461209937958517    ,  -0.00019707478780232};
 *
 * numuez=[ 63.5324749336048      ,   -165.058509528854      ,    160.980255884043     ,    -73.9086964860023      ,    16.2950118341303 , -1.50182021620002]
denuez=[ 1     ,    -2.22507124567522     ,     1.80900411465748    ,    -0.723613053538164     ,    0.151915879215545  ,-0.0122356946596432]
numi2z=[  0      ,    -3.1776786060705      ,    25.3519171606937    ,     -63.2529388889476     ,     72.4550323542472    ,  -41.5868829509874      ,    10.7953432723197    ,    -0.198537823951556    ,    -0.449903290440236    ,      0.06314008324708]
deni2z=[ 1   ,       -3.7837157951766      ,    6.18811409586531      ,   -5.80693836435801     ,     3.47743608641536   ,  -1.3876531780718      ,   0.370422546860972    ,   -0.063781269560913   ,    0.00639796108563532    , -0.000282083059946979]
 */
