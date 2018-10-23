/*********************************************************************
/*  Author      :   Donghao@Lab209_NUAA
/*  Date        :   2016.10.10
/*  Function    :   Move the program from flash to ram after power on 
                    and before the _c_init00 prior to main function.
                    So the execution speed will improve a lot.
                    This cmd file is particularly for DSP2833x series
                    MCU.
/*  History     :   (Revision written below please.)

**********************************************************************/
MEMORY
{
    PAGE 0 :
        /* SARAM                    */
        SARAML0123  : origin = 0x008000, length = 0x004000     /* L2 - L3 SARAM combined */

        /* FLASH and ROM            */
        BEGIN_FLASH : origin = 0x3F7FF6, length = 0x000002     /* part of FLASHA    */
        RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM  */

        FLASH0      : origin = 0x300000, length = 0x03FF80     /* On-chip FLASH for PAGE0 */
        OTP         : origin = 0x380400, length = 0x000400     /* OTP */
        PASSWDS     : origin = 0x33FFF8, length = 0x000008     /* Part of FLASH Sector A.  CSM password locations. */

        IQTABLES    : origin = 0x3FE000, length = 0x000B50     /* Part of Boot ROM */
        IQTABLES2   : origin = 0x3FEB50, length = 0x00008C     /* Part of Boot ROM */
        FPUTABLES   : origin = 0x3FEBDC, length = 0x0006A0     /* Part of Boot ROM */
        CSM_RSVD    : origin = 0x33FF80, length = 0x000076     /* Part of FLASH Sector A.  Reserved when CSM is in use. */
        
    PAGE 1 :
        /* SARAM                     */
        SARAMM01        : origin = 0x000000, length = 0x000800
        L4SARAM         : origin = 0x00C000, length = 0x001000        /* L4 SARAM, DMA accessible, 1 WS prog access */
        L5SARAM         : origin = 0x00D000, length = 0x001000        /* L5 SARAM, DMA accessible, 1 WS prog access */
        L6SARAM         : origin = 0x00E000, length = 0x001000        /* L6 SARAM, DMA accessible, 1 WS prog access */
        L7SARAM         : origin = 0x00F000, length = 0x001000        /* L7 SARAM, DMA accessible, 1 WS prog access */
}


SECTIONS
{
    /* For program-transferring : */
    /* Allocated and run in Flash Memory */
    codestart       : > BEGIN_FLASH,    PAGE = 0
    wddisable       : > FLASH0,   PAGE = 0
    copysections    : > FLASH0,   PAGE = 0

    /** Allocate program areas: **/

/*    vectors             : > VECTORS     PAGE = 0*/


    /* Initialized Sections of PAGE0 */
    .reset              : > RESET       PAGE = 0, TYPE = DSECT
    .const  :   LOAD = FLASH0, PAGE = 0     
                RUN = L4SARAM , PAGE = 1      
                LOAD_START(_const_loadstart) 
                RUN_START(_const_runstart)   
                SIZE(_const_size)            
    .econst :   LOAD = FLASH0, PAGE = 0      
                RUN = L4SARAM , PAGE = 1       
                LOAD_START(_econst_loadstart) 
                RUN_START(_econst_runstart)   
                SIZE(_econst_size)            

    .cinit  :   LOAD = FLASH0, PAGE = 0     
                RUN = SARAML0123, PAGE = 0        
                LOAD_START(_cinit_loadstart) 
                RUN_START(_cinit_runstart)   
                SIZE(_cinit_size)            

    .text    :   LOAD = FLASH0, PAGE = 0
                RUN = SARAML0123, PAGE = 0
                LOAD_START(_text_loadstart)
                RUN_START(_text_runstart)
                SIZE(_text_size)

    .switch :   LOAD = FLASH0, PAGE = 0      
                RUN = SARAML0123, PAGE = 0         
                LOAD_START(_switch_loadstart) 
                RUN_START(_switch_runstart)   
                SIZE(_switch_size)            

    .pinit   :  LOAD = FLASH0, PAGE = 0      
                RUN = SARAML0123, PAGE = 0        
                LOAD_START(_pinit_loadstart) 
                RUN_START(_pinit_runstart)   
                SIZE(_pinit_size) 

    /* Uninitialized Sections of PAGE1 */
    /* Allocate data areas: Uninitialized, Do Not Have To transfer*/
    /* Allocated Directly in RAM */
    .stack              : > SARAMM01     PAGE = 1 /* Uninitialized Section */
    .bss                : > L5SARAM     PAGE = 1 /* Uninitialized Section */
    .ebss               : > L5SARAM     PAGE = 1 /* Uninitialized Section */
    .sysmem             : > L5SARAM     PAGE = 1 /* Uninitialized Section */

    /* Allocate IQ math areas: Have to be transferred to RAM */
    IQmath              : > FLASH0 , PAGE = 0

    /* IQmath Tables in ROM: Do not have to be transferred */                                                                                                 
    IQmathTables        : > IQTABLES , PAGE = 0

}
