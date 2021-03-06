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
#ifndef _EOARRAY_H_
#define _EOARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOarray.h
    @brief      This header file implements public interface to a array object.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/

/** @defgroup eo_array Object EOarray
    The EOarray object is a container which is work-in-progress. It could be used as a slim vector
    in which the content of memory is exposed outsize via a number of data strcuctures such as eOarrayofbytes_t
    or eOarrayofhalves_t or eOarrayofwords_t
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef struct
{
    uint8_t         capacity;       /**< it is the maximum possible value of size in array */
    uint8_t         itemsize;       /**< it is the size of a single item in bytes */
    uint8_t         size;           /**< it keeps the number of items in the array */
    eObool_t        internalmem;    /**< if eobool_true the object use internally allocated memory, else it uses externally allocated memory */  
} eOarray_head_t;   EO_VERIFYsizeof(eOarray_head_t, 4);

typedef struct
{
    eOarray_head_t          head;
    uint8_t                 data[4];
} EOarray_of;


/** @typedef    typedef struct EOarray_hid EOarray
    @brief      EOarray is an opaque struct. It is used to implement data abstraction for the  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
//typedef struct EOarray_hid EOarray;

typedef EOarray_of EOarray;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOarray* eo_array_New(uint8_t capacity, uint8_t sizeofitem, void *memory)
    @brief      Creates a new EOarray object. If the argument @e memory is not NULL, then it is used for storage
                inside the object. The function resets
                the data. If we pass a non NULL @e memory then we consume @a capacity bytes + 4 bytes of memory.
    @param      capacity        The capacity of the array.
    @param      itemsize        If not zero, then the array contains fixed-sized items, otherwise error. 
    @param      memory          If not NULL the memory to use for the object. It is required a size equal to:
                                4 + @e capacity * @e itemsize.
    @return     The pointer to the required object.
    @warning    The function resets the memory pointed by @e memory for the size it requires which is:
                4+ @e capacity * @e itemsize and puts  @e capacity and @e itemsize at its beginning
 **/
extern EOarray* eo_array_New(uint8_t capacity, uint8_t itemsize, void *memory);


/** @fn         extern void eo_array_Delete(EOarray *p)
    @brief      deletes the object but only if the ram is internally allocated.
    @param      p               The pointer to the array object.
 **/
extern void eo_array_Delete(EOarray *p);


/** @fn         extern eOresult_t eo_array_Reset(EOarray *p)
    @brief      Resets the array. It sets its size to zero and memset to zero all the data for p->capacity * p->itemsize bytes.
    @param      p               The pointer to the array object.
    @return     eores_OK upon success, eores_NOK_nullpointer if p is NULL
 **/
extern eOresult_t eo_array_Reset(EOarray *p);


/** @fn         extern void eo_array_Resize(EOarray *p, uint8_t size)
    @brief      Resizes the array. It deos actions only if the new size can be contained in the array. It clears the removed or added elements.
    @param      p               The pointer to the array object.
    @param      size            The new size.
 **/ 
extern void eo_array_Resize(EOarray *p, uint8_t size);


/** @fn         extern uint8_t eo_array_Capacity(EOarray *p)
    @brief      tells the capacity of the array.
    @param      p               The pointer to the array object.
    @return     the capacity
 **/ 
extern uint8_t eo_array_Capacity(EOarray *p);


/** @fn         extern uint8_t eo_array_ItemSize(EOarray *p)
    @brief      tells the item size of the array.
    @param      p               The pointer to the array object.
    @return     the item size
 **/ 
extern uint8_t eo_array_ItemSize(EOarray *p);


/** @fn         extern uint8_t eo_array_Size(EOarray *p)
    @brief      tells the size of the array.
    @param      p               The pointer to the array object.
    @return     the size
 **/ 
extern uint8_t eo_array_Size(EOarray *p);


/** @fn         extern uint8_t eo_array_Available(EOarray *p)
    @brief      tells the how many items can be stored in the array. the number is equal to capacity - size
    @param      p               The pointer to the array object.
    @return     the available free items
 **/ 
extern uint8_t eo_array_Available(EOarray *p);


/** @fn         extern eObool_t eo_array_Full(EOarray *p)
    @brief      tells if the array is full
    @param      p               The pointer to the array object.
    @return     eobool_true or eobool_false
 **/ 
extern eObool_t eo_array_Full(EOarray *p);


/** @fn         extern uint16_t eo_array_UsedBytes(EOarray *p)
    @brief      tells how much of the memory pointer by p is used (header + size*itemsize).
    @param      p               The pointer to the array object.
    @return     the used bytes including header
 **/ 
extern uint16_t eo_array_UsedBytes(EOarray *p);


/** @fn         extern eOresult_t eo_array_PushBack(EOarray *p, const void *item)
    @brief      Adds an item at the back of the array. It does that only if the EOarray was created with non-zero itemsize
    @param      p               The pointer to the array object.
    @param      item            The item to be pushed back
    @return     eores_OK upon success, eores_NOK_nullpointer if @e p or @e data are NULL, eores_NOK_generic if @e data
                cannot be pushed inside.
 **/
extern eOresult_t eo_array_PushBack(EOarray *p, const void *item);


/** @fn         extern void * eo_array_At(EOarray *p, uint8_t pos)
    @brief      Gets a pointer to the item in position @e pos inside the object 
    @param      p               The pointer to the object.
    @param      pos             The position of the iterm
    @return     The pointer, or NULL upon failure.
 **/
extern void * eo_array_At(EOarray *p, uint8_t pos);


/** @fn         extern eOresult_t eo_array_PushBackItem(EOarray *p, const void *item)
    @brief      Adds an item at the back of the array. It does that only if the EOarray was created with non-zero itemsize
    @param      p               The pointer to the array object.
    @param      item            The item to be pushed back
    @return     eores_OK upon success, eores_NOK_nullpointer if @e p or @e data are NULL, eores_NOK_generic if @e data
                cannot be pushed inside.
 **/
extern eOresult_t eo_array_PopBack(EOarray *p);


/** @fn         extern void eo_array_Assign(EOarray *p, uint8_t pos, const void *items, uint8_t nitems)
    @brief      puts nitems pointed by items in positions starting from pos. the items must be disposed in a 
                consecutive mode as in an array. if final position pos+nitems is higher than current size and lower
                equal than capacity, then it resizes the array. if it is higher that capacity it does nothing.
    @param      p               The pointer to the array object.
    @param      pos             The position where to put the first item
    @param      items           pointer to the items
    @param      nitems          number of the items
 **/
extern void eo_array_Assign(EOarray *p, uint8_t pos, const void *items, uint8_t nitems);



/** @}            
    end of group eo_array  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

