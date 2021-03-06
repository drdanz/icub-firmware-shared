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
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOdeque_hid.h"
#include "EOVmutex_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOfifo.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOfifo_hid.h" 


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOfifo";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOfifo * eo_fifo_New(eOsizeitem_t item_size, eOsizecntnr_t capacity,
                            eOres_fp_voidp_uint32_t item_init, uint32_t init_arg, 
                            eOres_fp_voidp_voidp_t item_copy, eOres_fp_voidp_t item_clear,
                            EOVmutexDerived *mutex) 
{
    EOfifo *retptr = NULL; 
    
    // i get memory for a mutexfifo. it will never be null
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOfifo), 1);

    eo_errman_Assert(eo_errman_GetHandle(), (0 != item_size), "eo_fifo_New(): 0 item_size", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (0 != capacity), "eo_fifo_New(): 0 capacity", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    
    // now i fill the mutexfifo with a deque 
    retptr->dek = eo_deque_New(item_size, capacity, item_init, init_arg, item_copy, item_clear);

    // now i copy the passed mutex into mutexfifo. beware for future use, ... it may be NULL
    retptr->mutex = mutex;
 
    // ok, done
    return(retptr);
}

extern void eo_fifo_Delete(EOfifo * fifo)
{
    if(NULL == fifo) 
    {   // invalid fifo
        return;    
    }   
    
    if(NULL == fifo->dek)
    {
        return;
    }
    
    if(NULL != fifo->mutex)    
    {
        eov_mutex_Take(fifo->mutex, eok_reltimeINFINITE);
    }

    eo_fifo_Clear(fifo, eok_reltimeINFINITE);
    
    eo_mempool_Delete(eo_mempool_GetHandle(), fifo->dek);
    fifo->dek = NULL;

    if(NULL != fifo->mutex)    
    {
        //eov_mutex_Release(fifo->mutex);
        // however, if someone is waiting for this mutex, then there is a crash ... so, maybe better not to release
    }

    
    memset(fifo, 0, sizeof(EOfifo));    
    eo_mempool_Delete(eo_mempool_GetHandle(), fifo);
    return;    
}


extern eOresult_t eo_fifo_Capacity(EOfifo *fifo, eOsizecntnr_t *capacity, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;
    

    if(NULL == fifo) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        *capacity = eo_deque_Capacity(fifo->dek);
        res = eores_OK;    
    }
    else 
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            *capacity = eo_deque_Capacity(fifo->dek);
            eov_mutex_Release(fifo->mutex);
            res = eores_OK;
        }
        else 
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }


    return(res);
}


extern eOresult_t eo_fifo_Size(EOfifo *fifo, eOsizecntnr_t *size, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;

    if(NULL == fifo) 
    {
        *size = 0;
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        *size = eo_deque_Size(fifo->dek);
        res = eores_OK;    
    }
    else 
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            *size = eo_deque_Size(fifo->dek);
            eov_mutex_Release(fifo->mutex);
            res = eores_OK;
        }
        else 
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }
    
    return(res);        
}


extern eOresult_t eo_fifo_Put(EOfifo *fifo, void *pitem, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;
    

    if(NULL == fifo) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        if(eobool_false == eo_deque_Full(fifo->dek))
        {
            eo_deque_PushBack(fifo->dek, pitem);
            res = eores_OK;
        }
        else 
        {
            // queue is full
            res = eores_NOK_busy;
        }
    
    }
    else 
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            // try to put item inside
            if(eobool_false == eo_deque_Full(fifo->dek))
            {
                eo_deque_PushBack(fifo->dek, pitem);
                res = eores_OK;
            }
            else 
            {
                // queue is full
                res = eores_NOK_busy;
            }        

            // release it
            eov_mutex_Release(fifo->mutex);

        }
        else 
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }
    
    return(res);    
}


extern eOresult_t eo_fifo_Get(EOfifo *fifo, const void **ppitem, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == fifo) || (NULL == ppitem)) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        *ppitem = eo_deque_Front(fifo->dek);
        if(NULL != *ppitem) 
        {
            res = eores_OK;
        }
        else 
        {
             res = eores_NOK_nodata; // empty
        }    
    }
    else 
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            *ppitem = eo_deque_Front(fifo->dek);
            eov_mutex_Release(fifo->mutex);
            if(NULL != *ppitem) 
            {
                res = eores_OK;
            }
            else 
            {
                 res = eores_NOK_nodata; // empty
            }    
        }
        else 
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }
    
    return(res);    
}


extern eOresult_t eo_fifo_Rem(EOfifo *fifo, eOreltime_t tout) {

    eOresult_t res = eores_NOK_generic;
    

    if(NULL == fifo) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        eo_deque_PopFront(fifo->dek);
        res = eores_OK;    
    }
    else {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            eo_deque_PopFront(fifo->dek);
            eov_mutex_Release(fifo->mutex);
            res = eores_OK;
        }
        else {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }

    
    return(res);    
    
}


extern eOresult_t eo_fifo_GetRem(EOfifo *fifo, void *pitem, eOreltime_t tout)
{

    eOresult_t res = eores_NOK_generic;
    void *dekitem;

    if((NULL == fifo) || (NULL == pitem)) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
    	dekitem = eo_deque_Front(fifo->dek);
		if(NULL != dekitem)
        {
            res = eores_OK;
			//memcpy(pitem, dekitem, fifo->dek->item_size);

            if(NULL != fifo->dek->item_copy_fn) 
            {
                fifo->dek->item_copy_fn(pitem, dekitem);
            }
            else
            {
                memcpy(pitem, dekitem, fifo->dek->item_size);
            }

        	//eo_deque_PopFront(fifo->dek);
            eo_deque_hid_QuickPopFront(fifo->dek); // can safely use this one as we know that there is an item inside
        }
        else 
        {
             res = eores_NOK_nodata; // empty
        }  
    }
    else
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
    	    dekitem = eo_deque_Front(fifo->dek);
			if(NULL != dekitem)
	        {
	            res = eores_OK;
				//memcpy(pitem, dekitem, fifo->dek->item_size);

                if(NULL != fifo->dek->item_copy_fn) 
                {
                    fifo->dek->item_copy_fn(pitem, dekitem);
                }
                else
                {
                    memcpy(pitem, dekitem, fifo->dek->item_size);
                }


	        	//eo_deque_PopFront(fifo->dek);
                eo_deque_hid_QuickPopFront(fifo->dek); // can safely us this one as we know that there is an item inside
	        }
	        else 
	        {
	             res = eores_NOK_nodata; // empty
	        }  
            eov_mutex_Release(fifo->mutex);
        }
        else
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }

    return(res);    
	
}


extern eOresult_t eo_fifo_Clear(EOfifo *fifo, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == fifo) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == fifo->mutex)    
    {
        // the fifo is not protected with a mutex, thus it is simple.
        eo_deque_Clear(fifo->dek);
        res = eores_OK;    
    }
    else 
    {
        // the fifo is protected with a mutex, thus i need to check it
        // the mutex can be any type derived from mutexObj_t ... however, we use polymorphism
        if(eores_OK == eov_mutex_Take(fifo->mutex, tout)) 
        {
            eo_deque_Clear(fifo->dek);
            eov_mutex_Release(fifo->mutex);
            res = eores_OK;
        }
        else 
        {
            // unfortunately we did not get the mutex for timeout
            res = eores_NOK_timeout;
        }
    }

    return(res);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





