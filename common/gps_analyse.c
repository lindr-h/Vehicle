//#include <string.h>
#include "gps.h"

void tsip_input_proc(TSIPPKT *rpt,char inbyte)
{
	switch(rpt->status)
	{
	case TSIP_PARSED_DLE_1:    
		switch(inbyte)
		{
		case 0:
		case ETX:			// illegal TSIP data
			rpt->len=0;
			rpt->status=TSIP_PARSED_EMPTY;
			break;
		case DLE:			// try normal message start again
			rpt->len=0;
			rpt->status=TSIP_PARSED_DLE_1;
			break;
		default:			// legal TSIP  ID ;start message
			rpt->len=0;
			rpt->status=TSIP_PARSED_DATA;
			rpt->code=inbyte;
   			break;
		}
		break;
	case TSIP_PARSED_DATA:
		switch(inbyte)
		{
		case DLE:			// expect DLE or ETX next
			rpt->status=TSIP_PARSED_DLE_2;
			break;
		default:			// normal data byte
			rpt->buf[rpt->len]=inbyte;
			rpt->len++;
			break;
		}
		break;
	case TSIP_PARSED_DLE_2:
		switch(inbyte)
		{
		case DLE:			// normal data byte
			rpt->buf[rpt->len]=inbyte;
			rpt->len++;
			rpt->status=TSIP_PARSED_DATA;
			break;
		case ETX:			// end of message,return TURE here
			rpt->status=TSIP_PARSED_FULL;
			break;
		default:			// error:treat as TSIP_PARSED_DLE_1,start new report packet
			rpt->code=inbyte;
			rpt->len=0;
			rpt->status=TSIP_PARSED_DATA;
			break;
		}
        break;
	case  TSIP_PARSED_FULL:
	case  TSIP_PARSED_EMPTY:
	default:
		switch(inbyte)
		{
		case DLE:			// normal message start
			rpt->len=0;
			rpt->status=TSIP_PARSED_DLE_1;
			break;
		default:			// error:ignore inbyte
			rpt->len=0;
			rpt->status=TSIP_PARSED_EMPTY;   
			break;    
		}
		break;
	}
	
	if(rpt->len>MAX_RPTBUF)	// error:start new report packet
	{
		rpt->status=TSIP_PARSED_EMPTY;
		rpt->len=0;
	}     
}