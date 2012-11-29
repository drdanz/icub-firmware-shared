
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_HID_H_
#define _EOEMSCONTROLLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOemsController_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOspeedmeter.h"
#include "EOaxisController.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 

// - #define used with hidden struct ----------------------------------------------------------------------------------

#define MAX_JOINTS 4

// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum 
{
    EMS_ALL_OK  = 0,
    EMS_PRUDENT = 1,
    EMS_ALARM   = 2
} EMSdefcon_t;

/*  @struct     EOemsController_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOemsController_hid 
{
    emsBoardType_t boardType;

    uint8_t n_joints;
    
    EMSdefcon_t defcon;
    
    EOmotors         *motors;
    EOspeedmeter     *enc_speedometer[MAX_JOINTS];
    EOaxisController *axis_controller[MAX_JOINTS];
   
    //float torque_meas[MAX_MOTORS];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




