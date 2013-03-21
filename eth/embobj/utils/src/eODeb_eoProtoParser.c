/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/* @file       eOlowLevelParser_embObjProto.c
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"

#include "EOropframe_hid.h"
#include "EOrop_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eODeb_eoProtoParser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eODeb_eoProtoParser_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define ROPFRAME_HEADER_SIZE        sizeof(EOropframeHeader_t)
#define ROP_HEADER_SIZE             sizeof(eOrophead_t)

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
static eOresult_t s_eodeb_eoProtoParser_CheckNV(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr);
static uint8_t s_eodeb_eoProtoParser_NVisrequired(eODeb_eoProtoParser *p, eOnvEP_t ep, eOnvID_t nvid);
static uint8_t s_eodeb_eoProtoParser_CheckSeqnum(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr, 
                                                uint32_t *rec_seqnum, uint32_t *expeted_seqnum);
static uint8_t s_eodeb_eoProtoParser_isvalidropframe(uint8_t *payload);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static eODeb_eoProtoParser s_debParser_singleton = 
{
    EO_INIT(.initted)   0
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eODeb_eoProtoParser * eODeb_eoProtoParser_Initialise(const eODeb_eoProtoParser_cfg_t *cfg)
{
    //se gia configurato fatto qc????
    
    if(NULL == cfg)
    {
        //cfg = &eo_ethLowLevParser_DefaultCfg;
        return(NULL);
    }
    
    memcpy(&s_debParser_singleton.cfg, cfg, sizeof(eOethLowLevParser_cfg_t));
    s_debParser_singleton.initted = 1;
    
    return(&s_debParser_singleton);
}

extern eODeb_eoProtoParser * eODeb_eoProtoParser_GetHandle(void)
{
    return( (s_debParser_singleton.initted == 1)? &s_debParser_singleton : NULL);
}



extern eOresult_t eODeb_eoProtoParser_RopFrameDissect(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
{
    uint32_t rec_seqnum, expeted_seqnum;
    if((NULL == p) || (NULL == pktInfo_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    //1) verify if i received a valid ropframe
    if(!(s_eodeb_eoProtoParser_isvalidropframe(pktInfo_ptr->payload_ptr)))
    {
        if(p->cfg.checks.invalidRopFrame.cbk != NULL)
        {
            p->cfg.checks.invalidRopFrame.cbk(pktInfo_ptr);
        }
        return(eores_NOK_generic); 
    }
    
    //2) if a check sequence number callback has been configured, I check seqNum
    if(p->cfg.checks.seqNum.cbk_onErrSeqNum != NULL)
    {
        if(!s_eodeb_eoProtoParser_CheckSeqnum(p, pktInfo_ptr, &rec_seqnum, &expeted_seqnum))
        {
            p->cfg.checks.seqNum.cbk_onErrSeqNum(pktInfo_ptr, rec_seqnum, expeted_seqnum);
        }
    }
    
    //3) search nv configured in received ropframe
    if(p->cfg.checks.nv.cbk_onNVfound != NULL)
    {
        s_eodeb_eoProtoParser_CheckNV(p, pktInfo_ptr);
    }
    
    return(eores_OK);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static uint8_t s_eodeb_eoProtoParser_isvalidropframe(uint8_t *payload)
{
    EOropframeHeader_t *ropframeHdr = (EOropframeHeader_t *)payload;
    
    if(ropframeHdr->startofframe == 0x12345678)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}


static eOresult_t s_eodeb_eoProtoParser_CheckNV(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
{
    EOropframeHeader_t *ropframeheader;
	eOrophead_t *ropheader;
	uint8_t *rop_ptr;
    uint16_t i;


	//get ropframe header
	ropframeheader = (EOropframeHeader_t *)pktInfo_ptr->payload_ptr;

    //get rop to first rop
	rop_ptr = &pktInfo_ptr->payload_ptr[ROPFRAME_HEADER_SIZE];
	ropheader = (eOrophead_t*)rop_ptr;

	for(i=0; i<ropframeheader->ropsnumberof; i++)
	{		
        //if((cfg.checks.nv.reqnv.ep == ropheader->endp) && (cfg.checks.nv.reqnv.nvid == ropheader->nvid))
        if(s_eodeb_eoProtoParser_NVisrequired(p, ropheader->endp, ropheader->nvid))
		{
			uint8_t *enddata_ptr;
            eODeb_eoProtoParser_ropAdditionalInfo_t ropAddInfo = {0};
           
            //prepare rop additional info
            ropAddInfo.desc.configuration.plussign = ropheader->ctrl.plussign;
            ropAddInfo.desc.configuration.plustime = ropheader->ctrl.plustime;
            ropAddInfo.desc.ropcode = ropheader->ropc;
            ropAddInfo.desc.ep = ropheader->endp;
            ropAddInfo.desc.id = ropheader->nvid;
            ropAddInfo.desc.data = &rop_ptr[ROP_HEADER_SIZE];
            ropAddInfo.desc.size = ropheader->dsiz;
            ropAddInfo.desc.signature = EOK_uint32dummy;
            ropAddInfo.time = EOK_uint64dummy;
            
            
			if(ropheader->ctrl.plussign == 1)
            {
                int32_t fill= ropheader->dsiz%4;
                int32_t howmuch = 0;
                if(fill!=0)
                {
                    fill=4-fill;
                }
                howmuch = (ropheader->dsiz) + fill;
                enddata_ptr = &rop_ptr[howmuch];
                ropAddInfo.desc.signature = *((uint32_t*)enddata_ptr);
                enddata_ptr = &rop_ptr[howmuch+4/*signature size*/];
                ropAddInfo.time = *((uint64_t*)enddata_ptr);
            }
            else if(ropheader->ctrl.plustime == 1)
            {
                int32_t fill= ropheader->dsiz%4;
                int32_t howmuch = 0;
                if(fill!=0)
                {
                    fill=4-fill;
                }
                howmuch = (ropheader->dsiz) + fill;
                enddata_ptr = &rop_ptr[howmuch];
                ropAddInfo.time = *((uint64_t*)enddata_ptr);  
            }
            
            p->cfg.checks.nv.cbk_onNVfound(pktInfo_ptr, &ropAddInfo);
 		}
		
        //go to next rop
		rop_ptr = &rop_ptr[8+ropheader->dsiz + 8];
		ropheader = (eOrophead_t*)rop_ptr;
	}

    return(eores_OK);
}

static uint8_t s_eodeb_eoProtoParser_NVisrequired(eODeb_eoProtoParser *p, eOnvEP_t ep, eOnvID_t nvid)
{
    uint8_t i, max;
    eODeb_eoProtoParser_nvidEp_couple_t *couple;
    
    max = p->cfg.checks.nv.NVs2searchArray.head.size;
    
    for(i=0; i<max; i++)
    {
        couple  = (eODeb_eoProtoParser_nvidEp_couple_t *)eo_array_At((EOarray*)&p->cfg.checks.nv.NVs2searchArray, i);
        if((couple->id == nvid)&&(couple->ep == ep))
        {
            return(1);
        }
    }
    return(0);
}


static uint8_t s_eodeb_eoProtoParser_CheckSeqnum(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr, uint32_t *rec_seqnum, uint32_t *expeted_seqnum)
{
    static uint8_t isfirstframe = 1;
    static uint32_t curr_seqNum; //current sequence number
    
    EOropframeHeader_t *ropframeHdr = (EOropframeHeader_t *)pktInfo_ptr->payload_ptr;
    uint32_t *u32ptr =  (uint32_t*)&ropframeHdr->sequencenumber;
    *rec_seqnum = *u32ptr;


    //if it is first pkt received i save start seqNum (initialized curr_seqNum with received seqnum)
    if(isfirstframe)
    {
        curr_seqNum = *rec_seqnum;
        *expeted_seqnum = curr_seqNum;
        isfirstframe = 0;
        return(1);
    }
    
    //calculate expected seqnum
    *expeted_seqnum = curr_seqNum+1;
    
    
    if(*expeted_seqnum == *rec_seqnum)
    {
        curr_seqNum = *expeted_seqnum;
        return(1);
    }

    //if i'm here i lost a packt or packets arrived not in order.
    
    if(*rec_seqnum > curr_seqNum)
    {
        //i lost a pkt, so i restart with received seqnum
        curr_seqNum = *rec_seqnum;
    }
    
    //if i received a pkt with seqnum<curr_seqNum then i mantein my curr_seqNum
    
    return(0);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



