#include "helper.h"
#include <xsi_value.h>
#include <xsi_status.h>
#include <xsi_decl.h>
#include <SIBCUtil.h>

XSI::CStatus CValueTOSIVariant(XSI::CValue& in_value, SI_TinyVariant* io_slVariant)
{
	if(io_slVariant == NULL) return XSI::CStatus::Fail;

	switch(in_value.m_t)
	{	
	case XSI::CValue::siInt2:
		io_slVariant->variantType =SI_VT_SHORT ;
		io_slVariant->sVal = (short)in_value;
		break;
	case XSI::CValue::siInt4:		
		io_slVariant->variantType = SI_VT_INT;
		io_slVariant->nVal = (long)in_value;
		break;
	case XSI::CValue::siFloat:
		io_slVariant->variantType = SI_VT_FLOAT;
		io_slVariant->fVal = (float)in_value;
		break;
	case XSI::CValue::siDouble:
		io_slVariant->variantType = SI_VT_FLOAT;
		io_slVariant->fVal = (float)(double)in_value;
		break;
	case XSI::CValue::siBool:
		io_slVariant->variantType = SI_VT_BOOL;
		io_slVariant->boolVal = (bool)in_value;
		break;
	case XSI::CValue::siString:
		{
			io_slVariant->variantType = SI_VT_PCHAR;
			char* pStr = NULL;
			XSIW2A(&pStr,in_value.m_u.pwval);
			io_slVariant->p_cVal = _SI_STRDUP(pStr); 
		}		
		break;
	case XSI::CValue::siEmpty:
	case XSI::CValue::siRef:
	case XSI::CValue::siArray:
	case XSI::CValue::siPtr:
		break;
	default:
		return XSI::CStatus::Unexpected;
	};

	return XSI::CStatus::OK;
}

