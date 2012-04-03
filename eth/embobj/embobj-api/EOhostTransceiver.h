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
#ifndef _EOONEHOSTTRANSCEIVER_H_
#define _EOONEHOSTTRANSCEIVER_H_


/** @file       EOhostTransceiver.h
    @brief      This header file implements public interface to the host transceiver (pc104)
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_ecvrevrebvtr2342r7 Object EOhostTransceiver
    The EOhostTransceiver is a singleton .....
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtransceiver.h"


// - public #define  --------------------------------------------------------------------------------------------------

#warning --> instead of 20 ... can i put 0? answer: seems yes  .. but be carefule w/ eo_ropframe_ROP_NumberOf_quickversion()

#define EOK_HOSTTRANSCEIVER_capacityofpacket                   512
#define EOK_HOSTTRANSCEIVER_capacityofrop                      128
#define EOK_HOSTTRANSCEIVER_capacityofropframeregulars         20
#define EOK_HOSTTRANSCEIVER_capacityofropframeoccasionals      (512-4-4)
#define EOK_HOSTTRANSCEIVER_capacityofropframereplies          20 
#define EOK_HOSTTRANSCEIVER_maxnumberofregularrops             4     

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    const EOconstvector* const  vectorof_endpoint_cfg;
    eOuint16_fp_uint16_t        hashfunction_ep2index;
    eOipv4addr_t                remoteboardipv4addr;
    eOipv4port_t                remoteboardipv4port;
    uint8_t                     tobedefined; // ip, port, numendpoints ... or simply: local ip is retrieved by eeprom, remote ip from
                             // first reception, port is fixed, numendpoints and an constvector of endpoint config
                             // is store in a file containing a variable pointer to const data. 
} eOhosttransceiver_cfg_t;


/** @typedef    typedef struct EOhostTransceiver_hid EOhostTransceiver
    @brief      EOhostTransceiver is an opaque struct. It is used to implement data abstraction for the Parser  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOhostTransceiver_hid EOhostTransceiver;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOhosttransceiver_cfg_t eo_hosttransceiver_cfg_default; // = {NULL, 0, 0, 0};


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOhostTransceiver * eo_hosttransceiver_Initialise(void)
    @brief      Initialise the singleton EOhostTransceiver. 
    @param      cfg         Contains actions to be done on reception or transmission which are specific to the application.
                            If NULL, then  is is issued a info by the EOtheErrorManager.
    @return     A valid and not-NULL pointer to the EOhostTransceiver singleton.
 **/
extern EOhostTransceiver * eo_hosttransceiver_New(const eOhosttransceiver_cfg_t *cfg);


extern EOtransceiver* eo_hosttransceiver_Transceiver(EOhostTransceiver *p);

extern EOnvsCfg* eo_hosttransceiver_NVsCfg(EOhostTransceiver *p);




/** @}            
    end of group eo_ecvrevrebvtr2342r7  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


