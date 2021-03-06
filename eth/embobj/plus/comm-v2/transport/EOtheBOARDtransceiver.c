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

#include "EOtheBOARDtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheBOARDtransceiver_hid.h" 


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

static EOnvSet* s_eo_boardtransceiver_nvset_get(const eOboardtransceiver_cfg_t *cfg);
static void s_eo_boardtransceiver_nvset_release(EOtheBOARDtransceiver* p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOtheBOARDtransceiver";
 
static EOtheBOARDtransceiver s_eo_theboardtrans = 
{
    EO_INIT(.transceiver)               NULL,
    EO_INIT(.nvset)                     NULL,
    EO_INIT(.boardnumber)               eo_nv_BRDdummy
};


const eOboardtransceiver_cfg_t eo_boardtransceiver_cfg_default = 
{
    EO_INIT(.nvsetbrdcfg)               NULL,
    EO_INIT(.remotehostipv4addr)        0,
    EO_INIT(.remotehostipv4port)        0,
    EO_INIT(.sizes)                     {0},
    EO_INIT(.mutex_fn_new)              NULL,
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvsetprotection)           eo_nvset_protection_none,
    EO_INIT(.proxycfg)                  NULL,
    EO_INIT(.extfn)                         
    {
        EO_INIT(.onerrorseqnumber)      NULL,
        EO_INIT(.onerrorinvalidframe)   NULL
    }
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOtheBOARDtransceiver * eo_boardtransceiver_Initialise(const eOboardtransceiver_cfg_t *cfg) 
{
    eOtransceiver_cfg_t txrxcfg = eo_transceiver_cfg_default;
    
    if(NULL != s_eo_theboardtrans.transceiver)
    {
        return(&s_eo_theboardtrans);
    }


    if(NULL == cfg)
    {
        cfg = &eo_boardtransceiver_cfg_default;
    }
    
    if(NULL == cfg->nvsetbrdcfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eo_boardtransceiver_Initialise(): NULL nvsetbrdcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    }

    if((0 == cfg->sizes.capacityoftxpacket) || (0 == cfg->sizes.capacityofrop) || (0 == cfg->sizes.capacityofropframeregulars) ||
       (0 == cfg->sizes.capacityofropframeoccasionals) || (0 == cfg->sizes.capacityofropframereplies) || (0 == cfg->sizes.maxnumberofregularrops))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eo_boardtransceiver_Initialise(): a cfg->sizes.field is 0", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    }    
    
    // 1. init the proper transceiver cfg

    s_eo_theboardtrans.nvset = s_eo_boardtransceiver_nvset_get(cfg);
    
    s_eo_theboardtrans.boardnumber = cfg->nvsetbrdcfg->boardnum;
    

    txrxcfg.sizes.capacityoftxpacket            = cfg->sizes.capacityoftxpacket;
    txrxcfg.sizes.capacityofrop                 = cfg->sizes.capacityofrop;
    txrxcfg.sizes.capacityofropframeregulars    = cfg->sizes.capacityofropframeregulars;
    txrxcfg.sizes.capacityofropframeoccasionals = cfg->sizes.capacityofropframeoccasionals;
    txrxcfg.sizes.capacityofropframereplies     = cfg->sizes.capacityofropframereplies;
    txrxcfg.sizes.maxnumberofregularrops        = cfg->sizes.maxnumberofregularrops;
    txrxcfg.remipv4addr                         = cfg->remotehostipv4addr;
    txrxcfg.remipv4port                         = cfg->remotehostipv4port;
    txrxcfg.nvset                               = s_eo_theboardtrans.nvset;
    txrxcfg.confmancfg                          = NULL; // it does not have a confirmation manager cfg->confmancfg;
    txrxcfg.proxycfg                            = cfg->proxycfg;
    txrxcfg.mutex_fn_new                        = cfg->mutex_fn_new;
    txrxcfg.protection                          = cfg->transprotection;
    memcpy(&txrxcfg.extfn, &cfg->extfn, sizeof(eOtransceiver_extfn_t));
    
    s_eo_theboardtrans.transceiver = eo_transceiver_New(&txrxcfg);
    
    
    return(&s_eo_theboardtrans);        
}    

extern void eo_boardtransceiver_DeInitialise(EOtheBOARDtransceiver* p)
{   
    if(NULL == p)
    {
        return;
    }
    
    if(NULL == s_eo_theboardtrans.transceiver)
    {
        return;
    }
    
    // ok. we can delete every object create in _Initialise()
    
    eo_transceiver_Delete(s_eo_theboardtrans.transceiver);
       
    s_eo_boardtransceiver_nvset_release(&s_eo_theboardtrans);
    
    s_eo_theboardtrans.boardnumber  = eo_nv_BRDdummy;
    s_eo_theboardtrans.nvset        = NULL;
    s_eo_theboardtrans.transceiver  = NULL;
    
    
    return;
}
    

extern EOtheBOARDtransceiver * eo_boardtransceiver_GetHandle(void) 
{
    if(NULL != s_eo_theboardtrans.transceiver)
    {
        return(&s_eo_theboardtrans);
    }
    else
    {    
        return(NULL);
    }
}

extern EOtransceiver * eo_boardtransceiver_GetTransceiver(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.transceiver);
    }
    else
    {    
        return(NULL);
    }     
}

extern EOnvSet * eo_boardtransceiver_GetNVset(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.nvset);
    }
    else
    {    
        return(NULL);
    }        
}

extern eOnvBRD_t eo_boardtransceiver_GetBoardNumber(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.boardnumber);
    }
    else
    {    
        return(eo_nv_BRDdummy);
    }          
}






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// extern EOnvSet * eo_boardtransceiver_hid_GetNVset(void)
// {
//     return(s_eo_theboardtrans.nvset);
// }

// extern eOnvBRD_t eo_boardtransceiver_hid_GetBoardNumber(void)
// {
//     return(s_eo_theboardtrans.boardnumber);
// }

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static EOnvSet* s_eo_boardtransceiver_nvset_get(const eOboardtransceiver_cfg_t *cfg)
{
    if(NULL != s_eo_theboardtrans.nvset)
    {   // if i call it more than once ... then i return the configuration but allocate and init only once 
        return(s_eo_theboardtrans.nvset);
    }

    EOnvSet* nvset = eo_nvset_New(cfg->nvsetprotection, cfg->mutex_fn_new);
    eo_nvset_InitBRD_LoadEPs(nvset, eo_nvset_ownership_local, EO_COMMON_IPV4ADDR_LOCALHOST, (eOnvset_BRDcfg_t*)cfg->nvsetbrdcfg, eobool_true);           
    return(nvset);
}


static void s_eo_boardtransceiver_nvset_release(EOtheBOARDtransceiver* p)
{
    if((NULL == p) || (NULL == p->nvset))
    {   // if i call it more than once ... 
        return;
    }

    // _delete also unit the brd etc.            
    eo_nvset_Delete(p->nvset);   
    p->nvset = NULL;  
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




