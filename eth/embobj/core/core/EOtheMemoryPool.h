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
#ifndef _EOTHEMEMORYPOOL_H_
#define _EOTHEMEMORYPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheMemoryPool.h
	@brief      This header file contains public interfaces for the memory pool singleton object.
	@author     marco.accame@iit.it
	@date       08/03/2011
**/


/** @defgroup eo_thememorypool Object EOtheMemoryPool
    The EOtheMemoryPool is a memory manager for the embOBJ. It is a singleton which can be initialised to work
    with the heap (mode is eo_mempool_alloc_dynamic), with user-defined static memory (mode is eo_mempool_alloc_static),
    or with a mixture of them (mode is eo_mempool_alloc_mixed).
    If not initialised, the EOtheMemoryPool uses the heap with standard functions: calloc(), realloc(), free(). 
    If initialised to work with heap, the EOtheMemoryPool can be passed user-defined function for allocation, reallocation,
    and release of memory (e.g., the ones from OSAL).
    If initialised to work in static mode, the user must pass to the singleton some memory pools where to get memory. If it is
    defined the mixed mode, the singleton shall get memory from the heap if the pool is not defined.
    In static and mixed mode it is possible to allocate memory but not to reallocate and release it.
        
    It is responsibility of the object EOVtheSystem (via its derived object) to initialise the EOtheMemoryPool. 


    
    @{		
 */
 

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVmutex.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/**	@typedef    EOtheMemoryPool_hid EOtheMemoryPool
 	@brief 		EOtheMemoryPool is an opaque struct. It is used to implement data abstraction for the memory pool
                object so that the user cannot see its private fields so that he/she is forced to manipulate the 
                object only with the proper public functions. 
 **/  
typedef struct EOtheMemoryPool_hid EOtheMemoryPool;



/**	@typedef    typedef enum eOmempool_allocmode_t 
 	@brief      Contains the allocation mode for the memory. 
 **/ 
typedef enum
{
    eo_mempool_alloc_dynamic    = 0,
    eo_mempool_alloc_static     = 1,
    eo_mempool_alloc_mixed      = 2
} eOmempool_alloc_mode_t;

typedef struct 
{
    eOvoidp_fp_uint32_t         allocate;
    eOvoidp_fp_voidp_uint32_t   reallocate;
    eOvoid_fp_voidp_t           release;   
} eOmempool_heap_config_t;


typedef struct 
{
    uint32_t                    size08;
    uint8_t*                    data08;
    uint32_t                    size16;
    uint16_t*                   data16;    
    uint32_t                    size32;
    uint32_t*                   data32;    
    uint32_t                    size64;
    uint64_t*                   data64;    
} eOmempool_pool_config_t;

typedef union
{
    eOmempool_pool_config_t     pool;
    eOmempool_heap_config_t     heap;
} eOmempool_alloc_config_t;


/**	@typedef    typedef struct eOmempool_cfg_t 
 	@brief      Contains the configuration for the EOtheMemoryPool. 
 **/ 
typedef struct
{
    eOmempool_alloc_mode_t              mode;
    const eOmempool_alloc_config_t*     conf;       
} eOmempool_cfg_t;


/**	@typedef    typedef enum eOmempool_alignment_t 
 	@brief      Contains the alignment types for the memory. it is relevant only to non-heap allocation (eo_mempool_alloc_static or 
                eo_mempool_alloc_mixed modes). in eo_mempool_alloc_dynamic mode the singleton always use eo_mempool_align_auto. 
 **/  
typedef enum 
{
    eo_mempool_align_auto   = 0,    /**< used with eo_mempool_alloc_dynamic mode. in eo_mempool_alloc_static it forces 8-byte alignment */
    eo_mempool_align_08bit  = 1,    /**< used with eo_mempool_alloc_static or eo_mempool_alloc_mixed to force 1-byte alignment */             
    eo_mempool_align_16bit  = 2,    /**< used with eo_mempool_alloc_static or eo_mempool_alloc_mixed to force 2-bytes alignment */
    eo_mempool_align_32bit  = 4,    /**< used with eo_mempool_alloc_static or eo_mempool_alloc_mixed to force 4-bytes alignment */
    eo_mempool_align_64bit  = 8     /**< used with eo_mempool_alloc_static or eo_mempool_alloc_mixed to force 8-bytes alignment */
} eOmempool_alignment_t;
   
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmempool_cfg_t eom_mempool_DefaultCfg; // = {eo_mempool_alloc_dynamic, NULL};


// - declaration of extern public functions ---------------------------------------------------------------------------





/** @fn         extern EOtheMemoryPool * eo_mempool_Initialise(const eOmempool_cfg_t *cfg)
    @brief      Initialise the singleton EOtheMemoryPool. 
    @param      cfg             It specifies the working mode of the allocator. In case of 
                                eo_mempool_alloc_static the memory is assigned using user-defined memory pools
                                aligned at 1, 2, 4, and 8 bytes. In such a case it is necessary to fill
                                the pointers and the size of the four user-defined memory pools.
                                In case of eo_mempool_alloc_mixed the memory is assigned
                                both from the memory pools but it is also used the heap for those memory pools with NULL
                                pointer or zero size.
                                In case of eo_mempool_alloc_dynamic the memory is assigned
                                only from the heap. 
                                A NULL value for cfg is a shortcut for the mode eo_mempool_alloc_dynamic.
    @return     Pointer to the required EOtheMemoryPool singleton (or NULL upon un-initialised singleton).
 **/
extern EOtheMemoryPool * eo_mempool_Initialise(const eOmempool_cfg_t *cfg);


/** @fn         extern EOtheMemoryPool* eo_mempool_GetHandle(void)
    @brief      Returns a handle to the singleton EOtheMemoryPool. The singleton must have been initialised
                with mempool_Initialise(), otherwise this function call will return NULL.
    @return     Pointer to the required EOtheMemoryPool singleton (or NULL upon un-initialised singleton).
 **/
extern EOtheMemoryPool * eo_mempool_GetHandle(void);


/** @fn         extern eOresult_t eo_mempool_SetMutex(EOtheMemoryPool *p, EOVmutex *mutex, eOreltime_t tout
    @brief      Sets an internal mutex to protect concurrent access to the memory pool. In the multi-task execution
                environment there is no need to use the mutex if we use the heap on the MDK-ARM environment. 
                In a single-task execution environment if the EOtheMemoryPool is not used
                within an ISR (sic!), there is no need to pass a mutex.
    @param      mutex           The mutex.
    @param      tout            The waiting timeout
    @return     eores_OK upon succesful assignment of the passed mutex, eores_NOK_generic if a non-NULL mutex 
                was previously assigned.
    @warning    It is better to call this function before the start of the MEE.
 **/
extern eOresult_t eo_mempool_SetMutex(EOtheMemoryPool *p, EOVmutexDerived *mutex, eOreltime_t tout);

 
/** @fn         extern void * eo_mempool_GetMemory(EOtheMemoryPool *p, eOmempool_alignment_t alignmode, 
                                                   uint16_t size, uint16_t number)
    @brief      Gives back memory for @e number consecutive objects each of @e size bytes and with an alignment
                given by @e alignmmode. If no memory is available the function directly calls the error manager.
    @param      p               The mempool singleton                
    @param      alignmode       The alignment required for the object (08-, 16-, 32-, 64-bit).
    @param      size            The size of the object in bytes. Use sizeof(EoMyObject).
    @param      number          The number of consecutive objects.     
    @return     The required memory if available. NULL if the requedsted memory was zero but with a warning given
                to the EOtheErrorManager. Issues a fatal error to the EOtheErrorManager if there was not memory anymore. 
    @warning    This function is thread-safe in static or mixed mode only if the EOtheMemoryPool has been protected 
                by a proper mutex.
 **/ 
extern void * eo_mempool_GetMemory(EOtheMemoryPool *p, eOmempool_alignment_t alignmode, uint16_t size, uint16_t number);


/** @fn         extern void * eo_mempool_Realloc(EOtheMemoryPool *p, void *m, uint32_t size)
    @brief      reallocates memory using heap. If the singleton handler is NULL or if it was not initialised in dynamic mode,
                it uses the default realloc() function. 
                If no memory is available the function directly calls the error manager.
    @param      p               The mempool singleton   
    @param      m               The pointer to the heap that we want to reassign.    
    @param      size            The size of the memory.   
    @return     The required memory if available. NULL if the requested memory was zero but with a warning given
                to the EOtheErrorManager. Issues a fatal error to the EOtheErrorManager if there was not memory anymore. 
    @warning    This function is thread-safe in static or mixed mode only if the EOtheMemoryPool has been protected 
                by a proper mutex.
 **/ 
extern uint32_t eo_mempool_SizeOfAllocated(EOtheMemoryPool *p);

extern eOmempool_alloc_mode_t eo_mempool_alloc_mode_Get(EOtheMemoryPool *p);


/** @fn         extern void * eo_mempool_New(EOtheMemoryPool *p, uint32_t size)
    @brief      Gives back memory using heap. If the singleton handler is NULL or if it was not initialised in dynamic mode,
                it uses the default calloc() function. 
                If no memory is available the function directly calls the error manager.
    @param      p               The mempool singleton                
    @param      size            The size of the memory.   
    @return     The required memory if available. NULL if the requested memory was zero but with a warning given
                to the EOtheErrorManager. Issues a fatal error to the EOtheErrorManager if there was not memory anymore. 
    @warning    This can be used also if the singleton is in static/mixed mode. It uses heap, however.
 **/ 
extern void * eo_mempool_New(EOtheMemoryPool *p, uint32_t size);


/** @fn         extern void * eo_mempool_Realloc(EOtheMemoryPool *p, void *m, uint32_t size)
    @brief      reallocates memory using heap. If the singleton handler is NULL or if it was not initialised in dynamic mode,
                it uses the default realloc() function. 
                If no memory is available the function directly calls the error manager.
    @param      p               The mempool singleton   
    @param      m               The pointer to the heap that we want to reassign.    
    @param      size            The size of the memory.   
    @return     The required memory if available. NULL if the requested memory was zero but with a warning given
                to the EOtheErrorManager. Issues a fatal error to the EOtheErrorManager if there was not memory anymore. 
    @warning    This can be used also if the singleton is in static/mixed mode. It uses heap, however. 
                VERY IMPORTANT: it cannot be used with pointers coming from static allocation. The user MUST pay attention to use it properly.
 **/ 
extern void * eo_mempool_Realloc(EOtheMemoryPool *p, void *m, uint32_t size);
 
 
/** @fn         extern void eo_mempool_Delete(EOtheMemoryPool *p, void *m)
    @brief      deletes memory using heap. If the singleton handler is NULL or if it was not initialised in dynamic mode,
                it uses the default free() function. 
                If pointer to deallocate is NULL it does not execute.
    @param      p               The mempool singleton   
    @param      m               The pointer to the heap that we want to delete.    
    @warning    This can be used also if the singleton is in static/mixed mode. It deletes heap, however. 
                VERY IMPORTANT: it cannot be used with pointers coming from static allocation. The user MUST pay attention 
                to use it properly.
 **/  
extern void eo_mempool_Delete(EOtheMemoryPool *p, void *m);



/** @}            
    end of group eo_thememorypool  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

