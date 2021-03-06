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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "stdio.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOnv_hid.h"
#include "EOrop_hid.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOhostTransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOhostTransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static EOnvSet* s_eo_hosttransceiver_nvset_get(const eOhosttransceiver_cfg_t *cfg);

static void s_eo_hosttransceiver_nvset_release(EOhostTransceiver *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOhostTransceiver";
 

const eOhosttransceiver_cfg_t eo_hosttransceiver_cfg_default = 
{
    EO_INIT(.nvsetbrdcfg)               NULL,
    EO_INIT(.remoteboardipv4addr)       EO_COMMON_IPV4ADDR(10, 0, 1, 1), 
    EO_INIT(.remoteboardipv4port)       12345,
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityoftxpacket)                EOK_HOSTTRANSCEIVER_capacityoftxpacket,
        EO_INIT(.capacityofrop)                     EOK_HOSTTRANSCEIVER_capacityofrop,
        EO_INIT(.capacityofropframeregulars)        EOK_HOSTTRANSCEIVER_capacityofropframeregulars,
        EO_INIT(.capacityofropframeoccasionals)     EOK_HOSTTRANSCEIVER_capacityofropframeoccasionals,
        EO_INIT(.capacityofropframereplies)         EOK_HOSTTRANSCEIVER_capacityofropframereplies,
        EO_INIT(.maxnumberofregularrops)            EOK_HOSTTRANSCEIVER_maxnumberofregularrops       
    },    
    EO_INIT(.mutex_fn_new)              NULL,
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvsetprotection)           eo_nvset_protection_none,
    EO_INIT(.confmancfg)                NULL,
    EO_INIT(.extfn)                         
    {
        EO_INIT(.onerrorseqnumber)      NULL,
        EO_INIT(.onerrorinvalidframe)   NULL
    }    
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOhostTransceiver * eo_hosttransceiver_New(const eOhosttransceiver_cfg_t *cfg) 
{
    EOhostTransceiver* retptr = NULL;
    eOtransceiver_cfg_t txrxcfg = eo_transceiver_cfg_default;

    if(NULL == cfg)
    {
        cfg = &eo_hosttransceiver_cfg_default;
    }
    
    if(NULL == cfg->nvsetbrdcfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eo_hosttransceiver_New(): NULL nvsetbrdcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    }  
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOhostTransceiver), 1);

    // 1. init the proper transceiver cfg

    retptr->nvset = s_eo_hosttransceiver_nvset_get(cfg);
    

    txrxcfg.sizes.capacityoftxpacket            = cfg->sizes.capacityoftxpacket;
    txrxcfg.sizes.capacityofrop                 = cfg->sizes.capacityofrop;
    txrxcfg.sizes.capacityofropframeregulars    = cfg->sizes.capacityofropframeregulars;
    txrxcfg.sizes.capacityofropframeoccasionals = cfg->sizes.capacityofropframeoccasionals;
    txrxcfg.sizes.capacityofropframereplies     = cfg->sizes.capacityofropframereplies;
    txrxcfg.sizes.maxnumberofregularrops        = cfg->sizes.maxnumberofregularrops;
    txrxcfg.remipv4addr                         = cfg->remoteboardipv4addr;
    txrxcfg.remipv4port                         = cfg->remoteboardipv4port;
    txrxcfg.nvset                               = retptr->nvset; 
    txrxcfg.confmancfg                          = cfg->confmancfg;
    txrxcfg.proxycfg                            = NULL; // the host does not have a proxy
    txrxcfg.mutex_fn_new                        = cfg->mutex_fn_new;
    txrxcfg.protection                          = cfg->transprotection;
    memcpy(&txrxcfg.extfn, &cfg->extfn, sizeof(eOtransceiver_extfn_t));

    
    
    retptr->transceiver = eo_transceiver_New(&txrxcfg);
    
    retptr->ipaddressofboard = cfg->remoteboardipv4addr;
    
    eo_nvset_BRD_Get(retptr->nvset, &retptr->boardnumber);
    
    return(retptr);        
}    


extern void eo_hosttransceiver_Delete(EOhostTransceiver *p) 
{    
    if(NULL == p)
    {
        return;
    }
    
    if(NULL == p->transceiver)
    {   // protection vs multiple calls of _Delete()
        return;
    }
    
    
    s_eo_hosttransceiver_nvset_release(p);
    
    eo_transceiver_Delete(p->transceiver);
   
    memset(p, 0, sizeof(EOhostTransceiver));
    eo_mempool_Delete(eo_mempool_GetHandle(), p);    
    return;
}    


extern EOtransceiver* eo_hosttransceiver_GetTransceiver(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->transceiver);
}


extern EOnvSet * eo_hosttransceiver_GetNVset(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->nvset);
}


extern eOnvBRD_t eo_hosttransceiver_GetBoardNumber(EOhostTransceiver* p)
{
    if(NULL == p)
    {
        return(eo_nv_BRDdummy);
    }
    
    return(p->boardnumber);    
}

extern eOipv4addr_t eo_hosttransceiver_GetRemoteIP(EOhostTransceiver* p)
{
    if(NULL == p)
    {
        return(eo_nv_IPdummy);
    }
    
    return(p->ipaddressofboard);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static EOnvSet* s_eo_hosttransceiver_nvset_get(const eOhosttransceiver_cfg_t *cfg)
{
    EOnvSet* nvset = eo_nvset_New(cfg->nvsetprotection, cfg->mutex_fn_new);    
    eo_nvset_InitBRD_LoadEPs(nvset, eo_nvset_ownership_remote, cfg->remoteboardipv4addr, (eOnvset_BRDcfg_t*)cfg->nvsetbrdcfg, eobool_true);   
    return(nvset);
}
 
  

static void s_eo_hosttransceiver_nvset_release(EOhostTransceiver *p)
{
    if((NULL == p) || (NULL == p->nvset))
    {
        return;
    }
    
    // _delete also unit the brd etc.            
    eo_nvset_Delete(p->nvset);   
    p->nvset = NULL;  
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




