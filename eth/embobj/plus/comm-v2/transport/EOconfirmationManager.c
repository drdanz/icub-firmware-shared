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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOconfirmationManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOconfirmationManager_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EO_TAILOR_CODE_FOR_ARM)
    #define EONV_DONT_USE_EOV_MUTEX_FUNCTIONS
#endif


#if defined(EONV_DONT_USE_EOV_MUTEX_FUNCTIONS)
    #define eov_mutex_Take(a, b)   
    #define eov_mutex_Release(a)
#endif

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

static void s_eo_confman_default_rop_conf_requested(eOipv4addr_t toipaddr, eOropdescriptor_t* ropdes);
static void s_eo_confman_default_rop_conf_received(eOipv4addr_t fromipaddr, eOropdescriptor_t* ropdes);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOconfirmationManager";

const eOconfman_cfg_t eOconfman_cfg_default = 
{
    EO_INIT(.mode)                          eoconfman_mode_disabled,
    EO_INIT(.maxnumberofconfreqrops)        16,
    EO_INIT(.mutex_fn_new)                  NULL,
    EO_INIT(.on_rop_conf_requested)         s_eo_confman_default_rop_conf_requested, 
    EO_INIT(.on_rop_conf_received)          s_eo_confman_default_rop_conf_received
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOconfirmationManager* eo_confman_New(const eOconfman_cfg_t *cfg)
{
    EOconfirmationManager *retptr = NULL;   

    if(NULL == cfg)
    {    
        cfg = &eOconfman_cfg_default;
    }
    
    if(eoconfman_mode_disabled == cfg->mode)
    {
        return(NULL);
    }
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOconfirmationManager), 1);
    
    memcpy(&retptr->config, cfg, sizeof(eOconfman_cfg_t));
    
    retptr->confrequests = (0 == cfg->maxnumberofconfreqrops) ? (NULL) : (eo_vector_New(sizeof(eOropdescriptor_t), cfg->maxnumberofconfreqrops, NULL, 0, NULL, NULL));

    retptr->mtx = (NULL == cfg->mutex_fn_new) ? (NULL) : (cfg->mutex_fn_new());
    
    return(retptr);
}


extern void eo_confman_Delete(EOconfirmationManager *p)
{
    if(NULL == p)
    {    
        return;
    }
    
    if(NULL != p->mtx)
    {
        eov_mutex_Delete(p->mtx);
    }
    
    if(NULL != p->confrequests)
    {
        eo_vector_Delete(p->confrequests);
    }
   
    memset(p, 0, sizeof(EOconfirmationManager));
    eo_mempool_Delete(eo_mempool_GetHandle(), p);
    return;   
}


extern eOresult_t eo_confman_ConfirmationRequests_Process(EOconfirmationManager *p, eOipv4addr_t toipaddr)
{
    if(NULL == p)
    {
        return(eores_NOK_generic);  
    }
    
    eov_mutex_Take(p->mtx, eok_reltimeINFINITE);

    if(0 != eo_vector_Size(p->confrequests))
    {
        uint16_t size = eo_vector_Size(p->confrequests);
        uint16_t i=0;
        for(i=0; i<size; i++)
        {
            eOropdescriptor_t *ropdes = (eOropdescriptor_t*) eo_vector_At(p->confrequests, i);
            eo_confman_Confirmation_Requested(p, toipaddr, ropdes);            
        }       
        eo_vector_Clear(p->confrequests);   // remove the conf requests
    }
    
    eov_mutex_Release(p->mtx);
    
    return(eores_OK);    
}


extern eOresult_t eo_confman_ConfirmationRequest_Insert(EOconfirmationManager *p, eOropdescriptor_t* ropdesc)
{   
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_generic);  
    }
    
    // if conf request is flagged on
    if(1 == ropdesc->control.rqstconf)
    {
        if(NULL != p->confrequests)
        { 
            eov_mutex_Take(p->mtx, eok_reltimeINFINITE);
            
            if(eobool_false == eo_vector_Full(p->confrequests))
            {
                eo_vector_PushBack(p->confrequests, ropdesc);
            }
            else
            {
                eov_mutex_Release(p->mtx);
                return(eores_NOK_generic); 
            }
            
            eov_mutex_Release(p->mtx);
        }        
    }
    
    return(eores_OK);
}


extern eOresult_t eo_confman_Confirmation_Requested(EOconfirmationManager *p, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdes)
{
    if((NULL == p) || (NULL == ropdes))
    {
        return(eores_NOK_generic);  
    }

    if(1 == ropdes->control.rqstconf)
    {
        if(NULL !=  p->config.on_rop_conf_requested)
        {
            p->config.on_rop_conf_requested(toipaddr, ropdes);
        }
        return(eores_OK);
    }

    return(eores_NOK_generic);   
}



extern eOresult_t eo_confman_Confirmation_Received(EOconfirmationManager *p, eOipv4addr_t fromipaddr, eOropdescriptor_t* ropdes)
{
    eOropconfinfo_t confinfo = eo_ropconf_none;

    if((NULL == p) || (NULL == ropdes))
    {
        return(eores_NOK_generic);  
    }
    
    confinfo = (eOropconfinfo_t)ropdes->control.confinfo;

    if(eo_ropconf_none != confinfo)
    {
        // received a confirmation ack/nak: execute the callback
        if(NULL != p->config.on_rop_conf_received)
        {
            p->config.on_rop_conf_received(fromipaddr, ropdes);
        }

        return(eores_OK); 
    } 

    return(eores_NOK_generic);      
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_confman_default_rop_conf_requested(eOipv4addr_t toipaddr, eOropdescriptor_t* ropdes)
{
    // do nothing
}

static void s_eo_confman_default_rop_conf_received(eOipv4addr_t fromipaddr, eOropdescriptor_t* ropdes)
{
    // do nothing
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




