/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EORECEIVER_HID_H_
#define _EORECEIVER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOreceiver_hid.h
    @brief      This header file implements hidden interface to a packet object.
    @author     marco.accame@iit.it
    @date       0111/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"
#include "EOropframe.h"
#include "EOrop.h"
#include "EOnvSet.h"
#include "EOagent.h"
#include "EOconfirmationManager.h"
#include "EOproxy.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOreceiver.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define USE_DEBUG_EORECEIVER 

// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    uint32_t    rxinvalidropframes; 
    uint32_t    errorsinsequencenumber; 
    uint32_t    lostreplies;
} EOreceiverDEBUG_t;

/** @struct     EOreceiver_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOreceiver_hid 
{
    EOropframe*                 ropframeinput;
    EOropframe*                 ropframereply;    
    EOrop*                      ropinput;
    EOrop*                      ropreply;
    EOagent*                    agent;
    eOipv4addr_t                ipv4addr;
    eOipv4port_t                ipv4port;
    uint8_t*                    bufferropframereply;
    uint64_t                    rx_seqnum;
    eOabstime_t                 tx_ageofframe;
    eOreceiver_seqnum_error_t   error_seqnumber;
    eOreceiver_invalidframe_error_t error_invalidframe;
    eOreceiver_void_fp_obj_t    on_error_seqnumber;    
    eOreceiver_void_fp_obj_t    on_error_invalidframe;
#if defined(USE_DEBUG_EORECEIVER)      
    EOreceiverDEBUG_t           debug;
#endif    
}; 



// - declaration of extern hidden functions ---------------------------------------------------------------------------



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




