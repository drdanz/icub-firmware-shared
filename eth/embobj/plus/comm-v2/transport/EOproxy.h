/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOPROXY_H_
#define _EOPROXY_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOproxy.h
    @brief      This header file implements public interface to the parser singleton used for communication protocol
    @author     marco.accame@iit.it
    @date       12/10/2013
**/

/** @defgroup eo_proxy Object EOproxy
    The EOproxy is an object used to forward requests to a different device, typically on CAN.
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"
#include "EOVmutex.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOProxy_hid EOproxy
    @brief      EOproxy is an opaque struct. It is used to implement data abstraction for the proxy  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOproxy_hid EOproxy;

typedef enum 
{
    eoproxy_mode_disabled   = 0,
    eoproxy_mode_enabled    = 1   
} eOproxymode_t;

typedef struct
{
    eOproxymode_t                       mode;
    uint16_t                            capacityoflistofropdes;
    eOreltime_t                         replyroptimeout;            // no timeout if eok_reltimeINFINITE 
    eov_mutex_fn_mutexderived_new       mutex_fn_new;
    void*                               transceiver; // points to a EOtransceiver
} eOproxy_cfg_t;


typedef struct
{
    uint8_t     p08_1;
    uint8_t     p08_2;
    uint16_t    p16_3;
    uint32_t    p32_4;
} eOproxy_params_t; //EO_VERIFYsizeof(eOproxy_params_t, 8); 
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOproxy_cfg_t eo_proxy_cfg_default;


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOproxy* eo_proxy_New(const eOproxy_cfg_t *cfg)
    @brief      creates a EOproxy. 
    @param      cfg         the configuration.
    @return     A valid and not-NULL pointer to the EOproxy object. NULL only if cfg->mode is eoproxy_mode_disabled
 **/
extern EOproxy* eo_proxy_New(const eOproxy_cfg_t *cfg);


/** @fn         extern void eo_proxy_Delete(EOproxy *p)
    @brief      deletes the object.
    @param      p           the object.
 **/
extern void eo_proxy_Delete(EOproxy *p);

/** @fn         extern eOresult_t eo_proxy_ROP_Forward(EOproxy *p, EOrop* rop, EOrop* ropout)
    @brief      asks the proxy to forward a rop. that is done by calling the update() function of the
                relevant netvar. if the rop is of kind ask<>, then the ropdescriptor and the netvar are stored
                inside an internal list and are assigned a timeout. if within the timeout the reply arrives, the user
                calls eo_proxy_ReplyROP_Load() and the say<> is automatically loaded in the transceiver.
    @param      p           the object.
    @param      rop         the rop to forward.
    @param      ropout      the possible rop to send back (a nak for instance or a ack).
    @return     eores_NOK_nullpointer if any argument is NULL, eores_NOK_generic if the netvar canot be proxied,
                or eores_OK on success.    
 **/
extern eOresult_t eo_proxy_ROP_Forward(EOproxy *p, EOrop* rop, EOrop* ropout);


/** @fn         extern eOproxy_params_t * eo_proxy_Params_Get(EOproxy *p, eOnvID32_t id32)
    @brief      asks the proxy to retrieve the pointer to the internal struct param associated to the id32.
                if there is no match for a given id32, then the result is NULL.
                The user can retrieve the param struct and modify it according to the needs.
                For instance, if one wants to receive two can frames before issuying the eo_proxy_ReplyROP_Load(),
                then it is possible to do that. inside the UPDT callback retrieve the param, assign value 2 to param->p08_1 
                and value 0 to param->p08_2 and tehn sending two can queries. at reception of each can reply, the param is 
                retrieved and the param->p08_2 is incremented. If equal to param->p08_1 then it is called  eo_proxy_ReplyROP_Load().              
    @param      p           the object.
    @param      rop         the rop to forward.
    @param      ropout      the possible rop to send back (a nak for instance or a ack).
    @return     eores_NOK_nullpointer if any argument is NULL, eores_NOK_generic if the netvar canot be proxied,
                or eores_OK on success.    
 **/
extern eOproxy_params_t * eo_proxy_Params_Get(EOproxy *p, eOnvID32_t id32); 

/** @fn         extern eOresult_t eo_proxy_ReplyROP_Load(EOproxy *p, eOnvID32_t id32, void *data)
    @brief      tells the proxy that a reply has arrived. the ropdescriptor to use is searched inside the object using 
                two keys: the id32 and the signature. If signature has value EOK_uint32dummy then it is not used.
    @param      p           the object.
    @param      id32        the id of the variable.
    @param      signature   the signature of the rop. if EOK_uint32dummy the signature is not used for teh search 
    @param      data        the data to be inserted inside the reply rop. if not NULL data is copied into nv(id32).
                            if NULL, then the reply rop uses what in nv(id32). 
    @return     eores_NOK_nullpointer if any argument is NULL, eores_NOK_generic if the netvar canot be proxied,
                or eores_OK on success.    
 **/
extern eOresult_t eo_proxy_ReplyROP_Load(EOproxy *p, eOnvID32_t id32, void *data);


/** @fn         extern eOresult_t eo_proxy_Tick(EOproxy *p)
    @brief      it must be called now and then to update the internal list of rop-descriptors waiting to be sent back.
                it internally retrieves the current time and removes ropdescriptors which has passed beyond timeout.
    @param      p           the object.
    @return     eores_NOK_nullpointer if any argument is NULL, eores_NOK_generic if the netvar canot be proxied,
                or eores_OK on success.    
 **/
extern eOresult_t eo_proxy_Tick(EOproxy *p);





/** @}            
    end of group eo_proxy  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



