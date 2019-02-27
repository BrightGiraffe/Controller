/** Inverter main program
 *  From Lab209 Prof. YongQiang Ye
 *  by Donghao@NUAA
 *  2017.12.15
 */

#include "main.h"

float rc_output = 0.0 ;
float error_rc_input ;
float error_pi_input ;
float phase = 0.0f ;
float pi_output = 0.0f ;

float control_modulation = 0.0f ;
float error = 0.0f, ge_output = 0.0f, gi_output = 0.0f, ig_reference = 0.0f ;
int ref_count = 0 ;

pll_sogi_s s_pll_sogi_gird ;

filter_s ge, gi ;

float bufSum[400];
int indexSum ;
float dcSum = 0.0f ;

enum InverterOutputState g_inv_state = InitialState ;

void Delay(void){
    unsigned int i, j = 0 ;
    for(i = 0; i < 65535 ; i ++){
        for(j = 0 ; j < 100; j ++ ){
            asm(" RPT #1 || NOP");
        }
    }
}

/***********************************************************************************************************************
                                                         * main
 **********************************************************************************************************************/
int main(void)
{
/***************************************************************************************************
                                    * System Initialization
 **************************************************************************************************/
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    Delay();
#if USE_PWM     //  Initialize Gpio for PWM

    PWM_Init(FREQUENCY_SWITCHING) ;
    IER|=M_INT3;
    PieCtrlRegs.PIEIER3.bit.INTx1=1;
    PieCtrlRegs.PIEIER3.bit.INTx2=1;
    EINT;
    ERTM;
#endif

    RelayGpioInit();
    KeyGpioInit();

    AD7606_Init();
    InitXintf();    // InitXintf16Gpio() is called inside.

    DAC8554Init();

/***************************************************************************************************
                                  * Controller and PLL Initialization
 **************************************************************************************************/


    control_modulation = 0.0 ;
    g_inv_state = InitialState ;
    //TS_PLL_TWENTY_KHZ
    init_pll_sogi(&s_pll_sogi_gird, KP_PLL, KI_PLL, K_GAIN_SOGI_PLL, TS_TWENTY_KHZ) ; // InitPLL();
    // TODO
    filter_init(&ge, num_ge, den_ge, 5) ;
    filter_init(&gi, num_gi, den_gi, 9) ;

    for(indexSum = 0 ; indexSum < 400 ; indexSum ++ ){
        bufSum[indexSum] = 0.0f ;
    }
    indexSum = 0 ;
/***************************************************************************************************
                                             * Main Loop
 **************************************************************************************************/
    for(;;)
    {
        if(flag_timer2_updated)
        {
            SCOPE_PU ;
            flag_timer2_updated = 0 ;

            if( ErrorDetected() ){
                Shutdown_PWM_RELAY();
                g_inv_state = ErrorEncountered ;
            }else{
                // Do nothing or pull down the IO for the scope to see
                // SCOPE_PD ;
            }

            if(g_inv_state == ErrorEncountered ){
                //SCOPE_PU ;
                Shutdown_PWM_RELAY() ;
            }else{
                //SCOPE_PD ;
            }

/*********************Data Storage************************/
// TODO : store data to external RAM

/*************PLL and Phase angle Generation**************/
            phase = calc_pll_sogi(&s_pll_sogi_gird, MeasureBuf[CH_AC_VOLTAGE] ) ; // 2.2us ;

            ig_reference = CURRENT_REF * sinf(phase) ;
            // ig_reference = CURRENT_REF * sinf(ref_count * 0.015707963) ;
            ref_count ++ ;
            ref_count %= 400 ;

/*****************Control Algorithm***********************/
            // close loop enabled
            if(g_inv_state == CurrentControlled){
                dcSum += MeasureBuf[CH_GRID_CURRENT];
                dcSum -= bufSum[indexSum] ;
                bufSum[indexSum] = MeasureBuf[CH_GRID_CURRENT] ;
                indexSum ++ ;
                indexSum %= 400 ;

                error = ig_reference - MeasureBuf[CH_GRID_CURRENT] ;
                ge_output = filter_calc(&ge, error);
                gi_output = filter_calc(&gi, MeasureBuf[CH_GRID_CURRENT] - dcSum * 0.0025);

                control_modulation = ge_output + gi_output
                        + MeasureBuf[CH_AC_VOLTAGE] * K_FEEDFORWARD; // PCC voltage feedforward
            }else if(g_inv_state != ErrorEncountered){
                //calc_wdvrc(p_wdvrc, phase, 0, 0) ;
            }

            // KEY
            if(KEY_CLOSELOOP_CONTROL_ENABLED && (g_inv_state != ErrorEncountered )){
                // Ensure connecting to the grid when the grid voltage is low ;
                if( fabs(phase - 1.02) < 0.03f){
                    g_inv_state = CurrentControlledPreparation ;
                }
            }

            if(KEY_CLOSELOOP_CONTROL_DISABLED){
                g_inv_state = ErrorEncountered ;
                PWM_DIS ;
                RELAY_1_OPEN ;
                RELAY_2_OPEN ;
            }

            if(g_inv_state == CurrentControlledPreparation){
                PWM_EN ;
                RELAY_1_CLOSEUP ;
                RELAY_2_CLOSEUP ;

                // near the zero crossing point
                if( phase < 0.1){
                    g_inv_state = CurrentControlled ;
                }
                else if( fabs(phase - 3.1415926) < 0.1 ){
                    g_inv_state = CurrentControlled ;
                }
                // when the current is larger than 0.1A
                else if( fabs(MeasureBuf[CH_GRID_CURRENT]) > 0.15){
                    g_inv_state = CurrentControlled ;
                }
            }

/***************End Control Algorithm*********************/

#if USE_DAC

#if !USE_DAC_SPI
//            if(debug_scope_1[debug_scope_count] > 1.0 ){
//                debug_scope_1[debug_scope_count] = 1.0 ;
//            }else if(debug_scope_1[debug_scope_count] < -1.0){
//                debug_scope_1[debug_scope_count] = -1.0 ;
//            }
            if(g_inv_state != ErrorEncountered){
                switch(g_inv_state){
                case CurrentControlled:
                    StoreVoltage(0, 2.5 + 0.2 * ig_reference , ADDR_DAC8554, 1);
                    //StoreVoltage(0, 2.5 + 0.2 * pid_ig.reference , ADDR_DAC8554, 1);
                    break ;
                default :
                    StoreVoltage(0, 2.5 + 0.2 * ig_reference , ADDR_DAC8554, 1);
                }
                //StoreVoltage(0, 2 + 0.5 * rc_output , ADDR_DAC8554, 1);
            // StoreVoltage(1,  2 + 2 * debug_scope_1[debug_scope_count] , ADDR_DAC8554, 1);
            }
#else
            // Approximately 2.6us
            DAC8554SpiSetVoltage(MeasureBuf[CH_DC_BUS] * 0.01 , CH_DC_BUS, 0);
            DAC8554SpiSetVoltage(MeasureBuf[CH_AC_VOLTAGE] * 0.005 + 2.5 ,CH_AC_VOLTAGE,0);
            DAC8554SpiSetVoltage(MeasureBuf[CH_GRID_CURRENT] * 0.1 + 2.5 ,CH_GRID_CURRENT,0);
            DAC8554SpiSetVoltage(MeasureBuf[CH_CAP_CURRENT] * 0.1 + 2.5 ,CH_CAP_CURRENT,1);

#endif

#endif
            SCOPE_PD ;
            }
    }
}
