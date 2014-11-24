/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoError.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoError.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// it contains a pair {value, string}
typedef struct
{
    uint32_t        value;
    const char*     string;
} eoerror_valuestring_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint32_t s_eoerror_maxvalue_in_category[] =
{
    eoerror_value_SYS_numberof,
    eoerror_value_HW_numberof,
    eoerror_value_MC_numberof
};  EO_VERIFYsizeof(s_eoerror_maxvalue_in_category, eoerror_category_numberof*sizeof(const uint32_t));    

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOerror_code_t eoerror_code_dummy = EOERROR_CODE_DUMMY;
const eOerror_value_t eoerror_value_dummy = EOERROR_VALUE_DUMMY;


// - begin of: section for error strings 
// - fill strings in here in accordance with relevant enums 


const eoerror_valuestring_t eoerror_valuestrings_SYS[] =
{   // very important: fill table with order of eOerror_value_SYS_t
    //                 in case of holes, use {0, NULL}
    {eoerror_value_SYS_unspecified,             "SYS: unspecified code. It may be that EOtheErrorManager is still called with a NULL eOerrmanDescriptor_t* param"},  
    {eoerror_value_SYS_tobedecided,             "SYS: tbd code. We must decide how to manage this code. it it wip"},  
    {eoerror_value_SYS_memory_zerorequested,    "SYS: the EOtheMemoryPool is requested 0 bytes"},       
    {eoerror_value_SYS_memory_notinitialised,   "SYS: the EOtheMemoryPool is requested bytes but it is not yet initialised"},        
    {eoerror_value_SYS_memory_missing,          "SYS: the EOtheMemoryPool cannot give any more memory"},     
    {eoerror_value_SYS_mutex_timeout,           "SYS: a mutex has timed out unexpectedly"},     
    {eoerror_value_SYS_wrongparam,              "SYS: a function has a wrong parameter"},  
    {eoerror_value_SYS_wrongusage,              "SYS: a function is used in the wrong way"},  
    {eoerror_value_SYS_runtimeerror,            "SYS: detected a runtime error"},  
    {eoerror_value_SYS_runninghappily,          "SYS: the board is running happily"},  
    {eoerror_value_SYS_ctrloop_execoverflowRX,  "SYS: the RX phase of the control loop has last more than wanted. In param there is the total execution time in usec"},  
    {eoerror_value_SYS_ctrloop_execoverflowDO,  "SYS: the DO phase of the control loop has last more than wanted. In param there is the total execution time in usec"},    
    {eoerror_value_SYS_ctrloop_execoverflowTX,  "SYS: the TX phase of the control loop has last more than wanted. In param there is the total execution time in usec"},  
    {eoerror_value_SYS_udptxfailure,            "SYS: a UDP packet could not be transmitted"},  
    {eoerror_value_SYS_ropparsingerror,         "SYS: there was a parsing error of a received ROP"},  
    {eoerror_value_SYS_halerror,                "SYS: the HAL triggered an error. In param there is the relevant hal code"},  
    {eoerror_value_SYS_osalerror,               "SYS: the OSAL triggered an error. In param there is the relevant osal code"},  
    {eoerror_value_SYS_ipalerror,               "SYS: the IPAL triggered an error. In param there is the relevant ipal code"}, 
    {eoerror_value_SYS_dispatcherfifooverflow,  "SYS: the EOtheInfoDispatcher could not accept a eOmn_info_properties_t item inside its transmitting queue."} 
};  EO_VERIFYsizeof(eoerror_valuestrings_SYS, eoerror_value_SYS_numberof*sizeof(const eoerror_valuestring_t)); 


const eoerror_valuestring_t eoerror_valuestrings_HW[] =
{   // very important: fill table with order of eOerror_value_HW_t
    //                 in case of holes, use {0, NULL}
    {eoerror_value_HW_first,                "HW: first error message of category hw"},       
    {eoerror_value_HW_second,               "HW: second error message of category hw"}
};  EO_VERIFYsizeof(eoerror_valuestrings_HW, eoerror_value_HW_numberof*sizeof(const eoerror_valuestring_t)); 


const eoerror_valuestring_t eoerror_valuestrings_MC[] =
{   // very important: fill table with order of eOerror_value_MC_t
    //                 in case of holes, use {0, NULL}
    {eoerror_value_MC_first,                "MC: first error message of category hw"},  
    {eoerror_value_MC_second,               "hi there, i am the second error message of category hw"}, 
};  EO_VERIFYsizeof(eoerror_valuestrings_MC, eoerror_value_HW_numberof*sizeof(const eoerror_valuestring_t)); 



const eoerror_valuestring_t * const eoerror_valuestrings[eoerror_category_numberof] =
{   // very important: fill table with order of eOerror_category_t: pos 0 is eoerror_category_EthBoardSystem etc.
    //                 in case of holes use: NULL,
    (const eoerror_valuestring_t *)&eoerror_valuestrings_SYS,   
    (const eoerror_valuestring_t *)&eoerror_valuestrings_HW,        
    (const eoerror_valuestring_t *)&eoerror_valuestrings_MC
};  EO_VERIFYsizeof(eoerror_valuestrings, eoerror_category_numberof*sizeof(const eoerror_valuestring_t *));  


// - end of: section for error strings 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOerror_code_t eoerror_code_get(eOerror_category_t cat, eOerror_value_t val)
{
    eOerror_code_t ec = 0;
    
    uint32_t c = (uint32_t)cat;
    
    if(c > eoerror_category_numberof)
    {
        return(eoerror_code_dummy);
    }
    
    if(val > s_eoerror_maxvalue_in_category[c])
    {
        return(eoerror_code_dummy);
    }
     
    ec = EOERRORCODE(c, val);

    return(ec);
}


extern eOerror_category_t eoerror_code2category(eOerror_code_t code)
{
    uint32_t cat = (code & EOERROR_CATEG_MASK) >> EOERROR_CATEG_SHIFT;
        
    if(cat > eoerror_category_numberof)
    {
        return(eoerror_category_dummy);
    }
    
    return((eOerror_category_t)cat);    
}


extern eOerror_value_t eoerror_code2value(eOerror_code_t code)
{  
    eOerror_category_t cat = eoerror_code2category(code);
    uint32_t val = code & EOERROR_VALUE_MASK;
    
    if(eoerror_category_dummy == cat)
    {
        return(eoerror_value_dummy);
    }
       
    if(val > s_eoerror_maxvalue_in_category[cat])
    {
        return(eoerror_code_dummy);
    }
        
    return(val);    
}


extern const char* eoerror_code2string(eOerror_code_t code)
{
    static const char interror_codeunrecognised[] = "eoerror_INTERNALERROR__codeunrecognised";
    static const char interror_categoryisempty[] = "eoerror_INTERNALERROR__categoryisempty";
    static const char interror_valueisempty[] = "eoerror_INTERNALERROR__valueisempty";
    static const char interror_stringatwrongplace[] = "eoerror_INTERNALERROR__stringatwrongplace";
       
    eOerror_category_t cat = eoerror_code2category(code);
    eOerror_value_t val = eoerror_code2value(code);
    
    if((eoerror_category_dummy == cat) || (eoerror_value_dummy == val))
    {
        return(interror_codeunrecognised);
    }
    
    if(NULL == eoerror_valuestrings[cat])
    {
        return(interror_categoryisempty);
    }

    if(NULL == eoerror_valuestrings[cat][val].string)
    {
        return(interror_valueisempty);
    }

    if(val != eoerror_valuestrings[cat][val].value)
    {
        return(interror_stringatwrongplace);
    }
    
    return(eoerror_valuestrings[cat][val].string);
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


