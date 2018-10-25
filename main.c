/** Inverter main program
 *  From Lab209 Prof. YongQiang Ye
 *  by Donghao@NUAA
 *  2017.12.15
 */

#include "main.h"

float rc_output = 0.0 ;
float error_rc_input ;
float error_pi_input ;
float debug_scope_1[400] ;
Uint16 debug_scope_count = 0 ;

float control_modulation = 0.0f ;

pll_sogi_s s_pll_sogi_gird ;
pidStruct pid_ig ;

dvrc_struct wdvrc ;
dvrc_struct * p_wdvrc = & wdvrc ;

filter_s low_pass_filter ;
filter_s * p_low_pass_filter = & low_pass_filter ;

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

#if USE_PWM     //  Initialize Gpio for PWM
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    PWM_Init(FREQUENCY_SWITCHING) ;
    // Setup for Interrupts.
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
    enum InverterOutputState g_inv_state = InitialState ;

    float phase = 0.0f ;
    float pi_output = 0.0f ;

    init_pll_sogi(&s_pll_sogi_gird, KP_PLL, KI_PLL, TS_PLL_TWENTY_KHZ) ; // InitPLL();

    filter_init(p_low_pass_filter, num_filter, den_filter, order_filter ) ;

    init_wdvrc(p_wdvrc, p_low_pass_filter, RC_Q_COEFF, RC_K_RC, RC_LEAD_STEPS) ;

    Init_pidStruct(&pid_ig, C_CTRL_KP, C_CTRL_KI ) ;

/***************************************************************************************************
                                             * Main Loop
 **************************************************************************************************/
    for(;;)
    {
        if(flag_timer2_updated)
        {
            SCOPE_PU ;
            flag_timer2_updated = 0 ;
            debug_scope_count ++ ;
            debug_scope_count %= 400 ;
            debug_scope_1[debug_scope_count] =  MeasureBuf[CH_DC_BUS] ;

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

            pid_ig.reference = CURRENT_REF * sinf(phase) ;

/*****************Control Algorithm***********************/

            // close loop enabled
            if(g_inv_state == CurrentControlled){
                error_rc_input = pid_ig.reference - MeasureBuf[CH_GRID_CURRENT] ;
                // SCOPE_PU ;
                // Real repetitive controller
                // rc_output = calc_wdvrc(p_wdvrc, phase, error_rc_input, 1);
                // Simulate repetitive controller
                rc_output = calc_wdvrc(p_wdvrc, phase, 0.001 * sinf(phase), 1);
                // SCOPE_PD ;
                //debug_scope_1[debug_scope_count] = rc_output ;
                // rc_output = 0.0 ;
                error_pi_input = error_rc_input + rc_output ;
                pi_output = Calc_pidStruct(&pid_ig, error_pi_input) ;

                // Read time damping is used to reduce delay ;
                control_modulation = pi_output // PI controller
                        + MeasureBuf[CH_AC_VOLTAGE] * K_FEEDFORWARD; // PCC voltage feedforward
                // rc_output = 0.0 ;

            }else if(g_inv_state != ErrorEncountered){
                calc_wdvrc(p_wdvrc, phase, 0, 0) ;
            }

            // KEY
            if(KEY_CLOSELOOP_CONTROL_ENABLED && (g_inv_state != ErrorEncountered )){
                // Ensure connecting to the grid when the grid voltage is low ;
                if( fabs(phase - 1.02) < 0.02f){
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
                if( phase < 0.06){
                    g_inv_state = CurrentControlled ;
                }
                else if( fabs(phase - 3.1415926) < 0.05 ){
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
                StoreVoltage(0, 2 + sinf(phase) , ADDR_DAC8554, 1);
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
