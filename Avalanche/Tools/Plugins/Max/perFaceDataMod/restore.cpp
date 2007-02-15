#include "pfdModPCH.h"
#include "restore.h"
#include "ModData.h"
#include "perFaceDataMod.h"

CSelectRestoreObj::CSelectRestoreObj(CPerFaceDataMod* m, CModData* data) 
{
	m_pPFDMod = m;
	m_nLevel = m_pPFDMod->m_dwSelLevel;
	m_pModData = data;
	m_pModData->m_bHeld = TRUE;
	
	switch ( m_nLevel ) 
	{
		case SELECT_OBJECT:	DbgAssert(0); break;
		case SELECT_VERTEX:	m_baUsel = m_pModData->m_baVertSel; break;
		case SELECT_EDGE:		m_baUsel = m_pModData->m_baEdgeSel; break;
		default:			m_baUsel = m_pModData->m_baFaceSel; break;
	}
}

CSelectRestoreObj::CSelectRestoreObj(CPerFaceDataMod* m, CModData* data, int sLevel) 
{
	m_pPFDMod			= m;
	m_nLevel			= sLevel;
	m_pModData			= data;
	m_pModData->m_bHeld = TRUE;
	
	switch ( m_nLevel ) 
	{
		case SELECT_OBJECT:	DbgAssert(0); break;
		case SELECT_VERTEX:	m_baUsel = m_pModData->m_baVertSel; break;
		case SELECT_EDGE:		m_baUsel = m_pModData->m_baEdgeSel; break;
		default:			m_baUsel = m_pModData->m_baFaceSel; break;
	}
}

int CSelectRestoreObj::Size()
{
	return 1;
}

void CSelectRestoreObj::EndHold()
{
	m_pModData->m_bHeld = FALSE; 
}

TSTR CSelectRestoreObj::Description()
{
	return TSTR(_T("Restore Selection")); 
}

void CSelectRestoreObj::Restore( int isUndo ) 
{
	if ( isUndo ) 
	{
		switch ( m_nLevel ) 
		{			
			case SELECT_VERTEX:	m_baRsel = m_pModData->m_baVertSel; break;
			case SELECT_FACE: 
			case SELECT_POLY:
			case SELECT_ELEMENT:	m_baRsel = m_pModData->m_baFaceSel; break;
			case SELECT_EDGE:		m_baRsel = m_pModData->m_baEdgeSel; break;
		}
	}

	switch ( m_nLevel ) 
	{		
		case SELECT_VERTEX:	m_pModData->m_baVertSel = m_baUsel; break;
		case SELECT_FACE:
		case SELECT_POLY:
		case SELECT_ELEMENT:	m_pModData->m_baFaceSel = m_baUsel; break;
		case SELECT_EDGE:		m_pModData->m_baEdgeSel = m_baUsel; break;
	}
	m_pPFDMod->LocalDataChanged();
}

void CSelectRestoreObj::Redo() 
{
	switch ( m_nLevel ) 
	{		
		case SELECT_VERTEX:	m_pModData->m_baVertSel = m_baRsel; break;
		case SELECT_FACE:
		case SELECT_POLY:
		case SELECT_ELEMENT:	m_pModData->m_baFaceSel = m_baRsel; break;
		case SELECT_EDGE:		m_pModData->m_baEdgeSel = m_baRsel; break;
	}
	m_pPFDMod->LocalDataChanged ();
}
