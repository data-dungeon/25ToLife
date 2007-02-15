// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#include "StdAfx.h"

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_REGISTRY_H)
	#include <ExtRegistry.h>
#endif

#if (!defined __EXT_EXTINTEGRITYCHECKSUM_H)
	#include "ExtIntegrityCheckSum.h"
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#include <../profuisdll/resource.h>

#include <limits.h>

#if _MFC_VER < 0x700
	#include <../Src/occimpl.h>
#else
	#include <ocdb.h>
	#include <afxocc.h>
	#include <../src/mfc/occimpl.h>
#endif

#pragma message("   Prof-UIS is automatically linking with version.lib")
#pragma message("      (Version info support)")
#pragma comment(lib,"version.lib") 

#ifndef _AFX_OLD_EXCEPTIONS
	#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)
	#define NO_CPP_EXCEPTION(expr)
#else
	#define DELETE_EXCEPTION(e)
	#define NO_CPP_EXCEPTION(expr) expr
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CExtSafeString productsection2regkeypath(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	return CExtCmdManager::GetSubSystemRegKeyPath(
		__PROF_UIS_REG_COMMAND_MANAGER,
		sProfileName,
		sSectionNameCompany,
		sSectionNameProduct
		);
}

static bool fileobj_to_registry(
	CFile & _file,
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	ASSERT( sProfileName != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
			sProfileName,
			sSectionNameCompany,
			sSectionNameProduct
			);

	return CExtCmdManager::FileObjToRegistry( _file, sRegKeyPath );
}

static bool fileobj_from_registry(
	CFile & _file,
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	ASSERT( sProfileName != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( _file.GetLength() == 0 );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
				sProfileName,
			sSectionNameCompany,
			sSectionNameProduct
			);
	return CExtCmdManager::FileObjFromRegistry( _file, sRegKeyPath );
}

/////////////////////////////////////////////////////////////////////////////
// CExtCmdItem
// command item class

#define __MAX_TOTAL_TICK_COUNT (UINT_MAX-1)
#define __DECREASE_TICK_COUNT_BY_REF(_REF_) (_REF_) /= 2

CExtCmdItem::CExtCmdItem(
	UINT nCmdID // = IDC_STATIC
	)
	: m_pProfile( NULL )
	, m_nCmdID( nCmdID )
	, m_dwStateFlags( STATE_ENABLED )
	, m_nIconIdx( -1 )
	, m_sMenuText( _T("") )
	, m_sToolbarText( _T("") )
	, m_sTipTool( _T("") )
	, m_sTipStatus( _T("") )
	, m_sAccelText( _T("") )
	, m_nUsageTickCount( 0 )
	, m_nLParamUserData( 0L )
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	, m_nTextFieldWidth( 100 ) // __EXT_MENU_DEF_INPLACE_EDIT_WIDTH
	, m_nDropDownWidth( -2 ) // (-1) - auto calc, (-2) - same as button area
	, m_nDropDownHeightMax( 250 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
{
}

CExtCmdItem::CExtCmdItem( const CExtCmdItem & other )
	: m_pProfile( NULL )
	, m_nCmdID( (UINT)IDC_STATIC )
	, m_dwStateFlags( STATE_ENABLED )
	, m_nIconIdx( -1 )
	, m_sMenuText( _T("") )
	, m_sToolbarText( _T("") )
	, m_sTipTool( _T("") )
	, m_sTipStatus( _T("") )
	, m_sAccelText( _T("") )
	, m_nUsageTickCount( 0 )
	, m_nLParamUserData( 0L )
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	, m_nTextFieldWidth( 100 ) // __EXT_MENU_DEF_INPLACE_EDIT_WIDTH
	, m_nDropDownWidth( -2 ) // (-1) - auto calc, (-2) - same as button area
	, m_nDropDownHeightMax( 250 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
{
	CExtCmdItem::AssignFromOther(other);
}

CExtCmdItem::~CExtCmdItem()
{
}

CExtCmdItem & CExtCmdItem::operator=( const CExtCmdItem & other )
{
	AssignFromOther(other);
	return *this;
}

void CExtCmdItem::AssignFromOther( const CExtCmdItem & other )
{
LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return;
	m_pProfile = other.m_pProfile;
	m_nCmdID = other.m_nCmdID;
	m_dwStateFlags = other.m_dwStateFlags;
	m_nIconIdx = other.m_nIconIdx;
	m_sMenuText = other.m_sMenuText,
	m_sToolbarText = other.m_sToolbarText,
	m_sTipTool = other.m_sTipTool;
	m_sTipStatus = other.m_sTipStatus;
	m_sAccelText = other.m_sAccelText;
	m_nUsageTickCount = other.m_nUsageTickCount;
	m_nLParamUserData = other.m_nLParamUserData;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	m_nTextFieldWidth = other.m_nTextFieldWidth;
	m_nDropDownWidth = other.m_nDropDownWidth; // (-1) - auto calc, (-2) - same as button area
	m_nDropDownHeightMax = other.m_nDropDownHeightMax;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
}

void CExtCmdItem::ReplaceFromOtherNonEmpty( const CExtCmdItem & other )
{
LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return;
//	ASSERT( m_pProfile == other.m_pProfile );
//	ASSERT( m_nCmdID == other.m_nCmdID );
//	m_dwStateFlags = other.m_dwStateFlags;
	if( other.m_nIconIdx >= 0 )
		m_nIconIdx = other.m_nIconIdx;
	if( !other.m_sMenuText.IsEmpty() )
		m_sMenuText = other.m_sMenuText;
	if( !other.m_sToolbarText.IsEmpty() )
		m_sToolbarText = other.m_sToolbarText;
	if( !other.m_sTipTool.IsEmpty() )
		m_sTipTool = other.m_sTipTool;
	if( !other.m_sTipStatus.IsEmpty() )
		m_sTipStatus = other.m_sTipStatus;
	if( !other.m_sAccelText.IsEmpty() )
		m_sAccelText = other.m_sAccelText;
//	m_nUsageTickCount = other.m_nUsageTickCount;
//	m_nLParamUserData = other.m_nLParamUserData;
}

void CExtCmdItem::UpdateMoreExact( const CExtCmdItem & other )
{
LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return;
//	ASSERT( m_pProfile == other.m_pProfile );
//	ASSERT( m_nCmdID == other.m_nCmdID );
//	m_dwStateFlags = other.m_dwStateFlags;
	if( m_nIconIdx < 0 )
		m_nIconIdx = other.m_nIconIdx;
	if( m_sMenuText.IsEmpty() )
		m_sMenuText = other.m_sMenuText;
	if( m_sTipTool.IsEmpty() )
		m_sTipTool = other.m_sTipTool;
	if( m_sTipStatus.IsEmpty() )
		m_sTipStatus = other.m_sTipStatus;
	if( m_sAccelText.IsEmpty() )
		m_sAccelText = other.m_sAccelText;
//	m_nUsageTickCount = other.m_nUsageTickCount;
//	m_nLParamUserData = other.m_nLParamUserData;
}

bool CExtCmdItem::StateIsBasic() const
{
	if( CExtCmdManager::IsForceBasicCommand(m_nCmdID) )
		return true;
	if( CExtCmdManager::IsForceRarelyCommand(m_nCmdID) )
		return false;
bool bBasic =
		(m_dwStateFlags&STATE_BASICCMDPROP) ? true : false;
	return bBasic;
}

bool CExtCmdItem::StateIsRarelyUsed() const
{
	if( StateIsBasic() )
		return false;
UINT nUsagePercent = GetUsagePercent();
	ASSERT( m_pProfile != NULL );
	return
		(nUsagePercent < m_pProfile->m_nRarelyPercent) ?
			true : false;
}

bool CExtCmdItem::StateIsForceRarely() const
{
	if( CExtCmdManager::IsForceRarelyCommand(m_nCmdID) )
		return true;
	if( CExtCmdManager::IsForceBasicCommand(m_nCmdID) )
		return false;
bool bForceRarely =
		(m_dwStateFlags&STATE_FORCE_RARELY) ? true : false;
	return bForceRarely;
}

void CExtCmdItem::TipsClean()
{
	m_sTipTool = _T("");
	m_sTipStatus = _T("");
}

bool CExtCmdItem::TipsLoad()
{
	TipsClean();
	if( !CExtCmdManager::IsCommand(m_nCmdID) )
	{
		ASSERT( FALSE );
		return false;
	}
CExtSafeString sText;
	if( ! g_ResourceManager->LoadString( sText, m_nCmdID ) )
		return false;
	sText.Replace( _T("\t"), _T(" ") );
	sText.Replace( _T("\r"), _T("") );
	sText.TrimLeft();
	sText.TrimRight();
	if( sText.IsEmpty() )
		return false;
int nSep = sText.ReverseFind('\n');
	if( nSep < 0 )
	{
		//m_sTipTool = sText; // (- v.2.24)
		m_sTipStatus = sText; // (+ v.2.24)
		return true;
	}
int nLen = sText.GetLength();
	m_sTipTool = sText.Right( nLen - nSep );
	m_sTipTool.TrimLeft();
	m_sTipTool.TrimRight();
	m_sTipStatus = sText.Left( nSep );
	m_sTipStatus.TrimLeft();
	m_sTipStatus.TrimRight();
	return true;
}

bool CExtCmdItem::DoUpdateCmdUI(
	CCmdTarget * pCmdTarget,
	UINT nIndex // = 0
	)
{
	ASSERT( pCmdTarget != NULL );
CExtCmdItemUI cmd_ui( this );
	if( CExtCmdManager::IsCommandNeedsSpecUpdate(m_nCmdID) )
	{
		cmd_ui.Enable(TRUE);
		return true;
	} // if( CExtCmdManager::IsCommandNeedsSpecUpdate(m_nCmdID) )
	else
	{
		cmd_ui.m_nIndex = nIndex;
		BOOL bRetVal =
			cmd_ui.DoUpdate(
			pCmdTarget,
			CExtCmdManager::g_bDisableCmdIfNoHandler ?
				TRUE : FALSE
			);
		return bRetVal ? true : false;
	} // else from if( CExtCmdManager::IsCommandNeedsSpecUpdate(m_nCmdID) )
}

bool CExtCmdItem::Deliver(
	HWND hWndCmdTarget,
	bool bSend // = false
	)
{
	if( hWndCmdTarget == NULL )
		return false;
	IncrementUsageCount();
	ASSERT( ::IsWindow( hWndCmdTarget ) );
	ASSERT( CExtCmdManager::IsCommand(m_nCmdID) );
UINT nMsg = WM_COMMAND;
LPARAM lParam = 0;
	if( CExtCmdManager::IsSystemCommand(m_nCmdID) )
	{ // if we need WM_SYSCOMMAND
		nMsg = WM_SYSCOMMAND;
		POINT point = { 0, 0 };
		::GetCursorPos( &point );
		lParam =
			MAKELONG(
				point.x,
				point.y
				);
	} // if we need WM_SYSCOMMAND
	if( bSend )
	{
		if( !::SendMessage(
				hWndCmdTarget,
				nMsg,
				m_nCmdID,
				lParam
				)
			)
		{
			//ASSERT( FALSE );
			return false;
		}
	} // if( bSend )
	else
	{
		if( !::PostMessage(
				hWndCmdTarget,
				nMsg,
				m_nCmdID,
				lParam
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
	} // else from  if( bSend )
	return true;
}

bool CExtCmdItem::Deliver(
	CWnd * pWndCmdTarget,
	bool bSend // = false
	)
{
	ASSERT_VALID( pWndCmdTarget );
	return
		Deliver(
			pWndCmdTarget->GetSafeHwnd(),
			bSend
			);
}

bool CExtCmdItem::Deliver(
	CControlBar * pWndCmdSource,
	bool bSend // = false
	)
{
	if( pWndCmdSource == NULL
		|| pWndCmdSource->GetSafeHwnd() == NULL
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT_VALID( pWndCmdSource );
CWnd * pOwner = pWndCmdSource->GetOwner();
	if( pOwner == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT_VALID( pOwner );
	ASSERT( ! pOwner->IsKindOf(RUNTIME_CLASS(CControlBar)) );
	return
		Deliver(
			pOwner->GetSafeHwnd(),
			bSend
			);
}

UINT CExtCmdItem::GetUsageTickCount() const
{
	return m_nUsageTickCount;
}

UINT CExtCmdItem::GetUsagePercent() const
{
	ASSERT( m_pProfile != NULL );
	if( m_pProfile->m_nTotalTickCount == 0 )
		return 0;
	return
		(m_nUsageTickCount * 100)
		/
		m_pProfile->m_nTotalTickCount
		;
}

void CExtCmdItem::IncrementUsageCount()
{
	if( StateIsBasic()
		|| StateIsForceRarely()
		)
		return;
	ASSERT( m_pProfile != NULL );
	m_nUsageTickCount++;
	m_pProfile->m_nTotalTickCount++;
	AnalyzeGlobalUsageOverflow();
}

void CExtCmdItem::ResetUsageStatistics()
{
	if( StateIsBasic()
		|| StateIsForceRarely()
		)
		return;
	ASSERT( m_pProfile != NULL );
	m_nUsageTickCount = 0;
}

UINT CExtCmdItem::GetProfileTickCount() const
{
	ASSERT( m_pProfile != NULL );
	return m_pProfile->m_nTotalTickCount;
}

UINT CExtCmdItem::GetProfileRarelyPercent() const
{
	ASSERT( m_pProfile != NULL );
	return m_pProfile->m_nRarelyPercent;
}

void CExtCmdItem::SetProfileRarelyPercent(
	UINT nRarelyPercent
	)
{
	ASSERT( nRarelyPercent <= 100 );
	if( nRarelyPercent > 100 )
		nRarelyPercent = 100;
	ASSERT( m_pProfile != NULL );
	m_pProfile->m_nRarelyPercent = nRarelyPercent;
}

void CExtCmdItem::AnalyzeGlobalUsageOverflow()
{
	ASSERT( m_pProfile != NULL );
	if( m_pProfile->m_nTotalTickCount < __MAX_TOTAL_TICK_COUNT )
		return;
	__DECREASE_TICK_COUNT_BY_REF( m_pProfile->m_nTotalTickCount );
POSITION pos = m_pProfile->m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID;
		CExtCmdItem * pCmdItem = NULL;
		m_pProfile->m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( pCmdItem != NULL );
		__DECREASE_TICK_COUNT_BY_REF( pCmdItem->m_nUsageTickCount );
	}
}

bool CExtCmdItem::OnQueryStateSerializationNecessity() const
{
	if(		StateIsBasic()
		||	StateIsForceRarely()
		||	StateIsMenubarTemp()
		||	CExtCmdManager::IsSystemCommand( m_nCmdID )
		||	m_nUsageTickCount == 0
		)
		return false;
	return true;
}

void CExtCmdItem::OnSysColorChange()
{
	ASSERT( m_pProfile != NULL );
CExtCmdIcon * pIcon = m_pProfile->CmdGetIconPtr( m_nCmdID );
	if( pIcon != NULL )
		pIcon->SyncSysColors();
}

void CExtCmdItem::OnSettingChange(
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	ASSERT( m_pProfile != NULL );
CExtCmdIcon * pIcon = m_pProfile->CmdGetIconPtr( m_nCmdID );
	if( pIcon != NULL )
		pIcon->OnSettingChange( uFlags, lpszSection );
}

void CExtCmdItem::OnDisplayChange(
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	ASSERT( m_pProfile != NULL );
CExtCmdIcon * pIcon = m_pProfile->CmdGetIconPtr( m_nCmdID );
	if( pIcon != NULL )
		pIcon->OnDisplayChange( nDepthBPP, ptSizes );
}

void CExtCmdItem::OnThemeChanged(
	WPARAM wParam,
	LPARAM lParam
	)
{
	ASSERT( m_pProfile != NULL );
CExtCmdIcon * pIcon = m_pProfile->CmdGetIconPtr( m_nCmdID );
	if( pIcon != NULL )
		pIcon->OnThemeChanged( wParam, lParam );
}

CExtSafeString CExtCmdItem::OnGetToolBarCustomizeName()
{
	return OnGetCustomizeScriptName();
}

CExtSafeString CExtCmdItem::OnGetCustomizeScriptName()
{
	if( !m_sMenuText.IsEmpty() )
		return m_sMenuText;
	if( !m_sTipTool.IsEmpty() )
		return m_sTipTool;
	if( !m_sToolbarText.IsEmpty() )
		return m_sToolbarText;
	return CExtSafeString( _T("") );
}

CExtSafeString CExtCmdItem::OnGetCustomizeCommandDescription()
{
	if( !m_sTipStatus.IsEmpty() )
		return m_sTipStatus;
	return CExtSafeString( _T("") );
}


/////////////////////////////////////////////////////////////////////////////
// CExtCmdItemUI
// command UI update class

CExtCmdItemUI::CExtCmdItemUI(
	CExtCmdItem * pCmd,
	int nIndexMax // = 0
	)
{
	ASSERT( pCmd != NULL );
	m_pCmd = pCmd;
	m_nIndexMax = nIndexMax;
	m_nID = pCmd->m_nCmdID;
	m_pOther = NULL;
}

void CExtCmdItemUI::Enable(
	BOOL bOn // = TRUE
	)
{
	ASSERT( m_pCmd != NULL );
	CCmdUI::m_bEnableChanged = TRUE;
	m_pCmd->StateEnable( bOn ? true : false );
}

void CExtCmdItemUI::SetCheck(
	int nCheck // = 1 // 0, 1 or 2 (indeterminate)
	)
{
	ASSERT( m_pCmd != NULL );
	if( nCheck == 0 || nCheck == 1 )
		m_pCmd->StateSetCheck(
			(nCheck == 0) ? false : true
			);
	else
	{
		ASSERT( nCheck == 2 );
		m_pCmd->StateSetIndeterminate(
			true
			);
	}
}

void CExtCmdItemUI::SetRadio(
	BOOL bOn // = TRUE
	)
{
	ASSERT( m_pCmd != NULL );
	m_pCmd->StateSetRadio( bOn ? true : false );
}

void CExtCmdItemUI::SetText(
	__EXT_MFC_SAFE_LPCTSTR lpszText
	)
{
	ASSERT( m_pCmd != NULL );
	m_pCmd->m_sMenuText = lpszText;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCmdProfile
// command profile class

CExtCmdProfile::CExtCmdProfile(
	__EXT_MFC_SAFE_LPCTSTR sName // = NULL
	)
	: m_sName( (sName == NULL) ? _T("") : sName )
	, m_nRarelyPercent( __MFCEXT_DEF_RARELY_USED_PERCENT )
	, m_nTotalTickCount( 0 )
	, m_nLParamUserData( 0L )
{
}

CExtCmdProfile::CExtCmdProfile(
	const CExtCmdProfile & other
	)
	: m_sName( _T("") )
	, m_nRarelyPercent( __MFCEXT_DEF_RARELY_USED_PERCENT )
	, m_nTotalTickCount( 0 )
	, m_nLParamUserData( 0L )
{
	AssignFromOther( other );
}

CExtCmdProfile::~CExtCmdProfile()
{
	_RemoveAllIconsImpl();
	_RemoveAllCmdsImpl();
}

CExtCmdProfile & CExtCmdProfile::operator=(
	const CExtCmdProfile & other
	)
{
	AssignFromOther( other );
	return *this;
}

void CExtCmdProfile::AssignFromOther(
	const CExtCmdProfile & other
	)
{
LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return;
	m_sName = other.m_sName;
	m_nRarelyPercent = other.m_nRarelyPercent;
	m_nTotalTickCount = other.m_nTotalTickCount;
	m_nLParamUserData = other.m_nLParamUserData;

	_RemoveAllCmdsImpl();
POSITION pos = other.m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID;
		CExtCmdItem * pCmdItem = NULL;
		other.m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( pCmdItem != NULL );
		m_cmds[ nCmdID  ] = OnCreateCmdItem( *pCmdItem );
		ASSERT( m_cmds[ nCmdID  ] != NULL );
	}

	_RemoveAllIconsImpl();
	for(	int iIcon = 0;
			iIcon < other.m_icons.GetSize();
			iIcon++
			)
	{
		CExtCmdIcon * pIcon = other.m_icons[iIcon];
		ASSERT( pIcon != NULL );
		m_icons.Add( new CExtCmdIcon( *pIcon ) );
	}
}

void CExtCmdProfile::_RemoveAllCmdsImpl()
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( pCmdItem != NULL );
		delete pCmdItem;
	}
	m_cmds.RemoveAll();
}

void CExtCmdProfile::_RemoveAllIconsImpl()
{
	for( INT iIcon = 0; iIcon < m_icons.GetSize(); iIcon++ )
	{
		CExtCmdIcon * pIcon = m_icons[iIcon];
		ASSERT( pIcon != NULL );
		delete pIcon;
	}
	m_icons.RemoveAll();
}

CExtCmdItem * CExtCmdProfile::OnCreateCmdItem( const CExtCmdItem & _cmd )
{
	return new CExtCmdItem( _cmd );
}

bool CExtCmdProfile::CmdSetup(
	const CExtCmdItem & _cmd,
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool * pbWasAddedNew // = NULL
	)
{
	if( pbWasAddedNew != NULL )
		*pbWasAddedNew = false;
	if( !CExtCmdManager::IsCommand(_cmd.m_nCmdID) )
	{
		ASSERT(FALSE);
		return false;
	}

CExtCmdItem * pCmdItem = NULL;
BOOL bExist =
		m_cmds.Lookup( _cmd.m_nCmdID, pCmdItem );
	if( !bExist )
	{
		pCmdItem = OnCreateCmdItem( _cmd );
		ASSERT( pCmdItem != NULL );
		pCmdItem->m_pProfile = this;
		m_cmds.SetAt( _cmd.m_nCmdID, pCmdItem );
		if( pbWasAddedNew != NULL )
			*pbWasAddedNew = true;
		return true;
	} // if( !bExist )
	ASSERT( pCmdItem != NULL );
	ASSERT( pCmdItem->m_pProfile == this );
	if( bReplaceOld )
		pCmdItem->ReplaceFromOtherNonEmpty( _cmd ); // (+ v.2.22)
	else
		pCmdItem->UpdateMoreExact( _cmd );

	return true;
}

bool CExtCmdProfile::CmdRemove(
	UINT nCmdID,
	bool * pbWasRemoved // = NULL
	)
{
	if( pbWasRemoved != NULL )
		*pbWasRemoved = false;
	if( ! CExtCmdManager::IsCommand(nCmdID) )
	{
		ASSERT(FALSE);
		return false;
	} // if( ! CExtCmdManager::IsCommand(nCmdID) )
CExtCmdItem * pCmdItem = NULL;
BOOL bExist =
		m_cmds.Lookup( nCmdID, pCmdItem );
	if( !bExist )
		return true;
int nCmdIconIdx = pCmdItem->m_nIconIdx;
	ASSERT( pCmdItem != NULL );
	delete pCmdItem;
	m_cmds.RemoveKey( nCmdID );
	if( pbWasRemoved != NULL )
		*pbWasRemoved = true;
	if( nCmdIconIdx < 0 )
		return true;
CExtCmdIcon * pIcon = m_icons.GetAt( nCmdIconIdx );
	ASSERT( pIcon != NULL );
	delete pIcon;
	m_icons.RemoveAt( nCmdIconIdx, 1 );
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( pCmdItem != NULL );
		ASSERT( pCmdItem->m_nIconIdx != nCmdIconIdx );
		if( pCmdItem->m_nIconIdx < nCmdIconIdx )
			continue;
		pCmdItem->m_nIconIdx --;
	} // for( ; pos != NULL; )
	return true;
}

void CExtCmdProfile::CmdRemoveAll()
{
	_RemoveAllIconsImpl();
	_RemoveAllCmdsImpl();
}

void CExtCmdProfile::CmdRemoveByMask(
	DWORD dwMask,
	bool bAllBitsOnly // = false
	)
{
CList < UINT, UINT > _listCmdsToRemove;
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( pCmdItem != NULL );
		DWORD dwTest =
			pCmdItem->m_dwStateFlags & dwMask;
		if( dwTest == 0 )
			continue;
		if( bAllBitsOnly && dwTest != dwMask )
			continue;
		_listCmdsToRemove.AddTail( nCmdID );
	} // for( ; pos != NULL; )
	pos = _listCmdsToRemove.GetHeadPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = _listCmdsToRemove.GetNext( pos );
		CmdRemove( nCmdID );
	} // for( ; pos != NULL; )
}

CExtCmdItem * CExtCmdProfile::CmdAllocPtr(
	UINT nCmdID // = 0 // 0 means any free in avail range
	)
{
	if( nCmdID != 0 && CmdIsRegistered(nCmdID) )
		return NULL;
	if( nCmdID == 0 )
	{ // find free ID for new command
		nCmdID = 65534;
		if( !m_cmds.IsEmpty() )
		{
			while( true )
			{
				CExtCmdItem * pCmdItem = NULL;
				BOOL bExist =
					m_cmds.Lookup( nCmdID, pCmdItem );
				if( bExist )
				{
					ASSERT( pCmdItem != NULL );
					nCmdID --;
					if( nCmdID == 0 )
					{
						ASSERT( FALSE );
						return NULL;
					}
					continue;
				}
				ASSERT( CExtCmdManager::IsCommand(nCmdID) );
				break;
			} // while( true )
		} // if( !m_cmds.IsEmpty() )
	} // find free ID for new command
	ASSERT( !CmdIsRegistered(nCmdID) );
CExtCmdItem _cmd(nCmdID);
	if( !CmdSetup(_cmd) )
		return NULL;
CExtCmdItem * pCmdItem = CmdGetPtr(nCmdID);
	ASSERT( pCmdItem != NULL );
	ASSERT( pCmdItem->m_nCmdID == nCmdID );
	return pCmdItem;
}

CExtCmdItem * CExtCmdProfile::CmdGetPtr(
	UINT nCmdID
	)
{
	if( !CExtCmdManager::IsCommand(nCmdID) )
		return NULL;
CExtCmdItem * pCmdItem = NULL;
BOOL bExist =
		m_cmds.Lookup( nCmdID, pCmdItem );
	if( !bExist )
		return NULL;
	ASSERT( pCmdItem != NULL );
	return pCmdItem;
}

bool CExtCmdProfile::CmdSetIcon(
	UINT nCmdID,
	const CExtCmdIcon * pCmdIcon,
	bool bUseCmdIconObject
	)
{
	if( !CExtCmdManager::IsCommand(nCmdID) )
	{
		ASSERT(FALSE);
		return false;
	}
	if(		pCmdIcon != NULL
		&&	pCmdIcon->IsEmpty()
		)
	{
		if( bUseCmdIconObject )
			delete pCmdIcon;
		pCmdIcon = NULL;
	}
CExtCmdItem * pCmdItem = NULL;
BOOL bExist =
		m_cmds.Lookup( nCmdID, pCmdItem );
	if( !bExist )
		return NULL;
	if( pCmdIcon == NULL )
	{ // if remove icon query
		if( pCmdItem->m_nIconIdx < 0 )
			return true;
		ASSERT( pCmdItem->m_nIconIdx < m_icons.GetSize() );
		CExtCmdIcon * pIcon = m_icons[ pCmdItem->m_nIconIdx ];
		ASSERT( pIcon != NULL );
		delete pIcon;
		m_icons.RemoveAt( pCmdItem->m_nIconIdx, 1 ); // 2.24 fix
		POSITION pos = m_cmds.GetStartPosition();
		ASSERT( pos != NULL );
		for( ; pos != NULL; )
		{
			CExtCmdItem * pCmdItemWalk = NULL;
			UINT nCmdIDWalk = (UINT)IDC_STATIC;
			m_cmds.GetNextAssoc( pos, nCmdIDWalk, pCmdItemWalk );
			ASSERT( pCmdItemWalk != NULL );
			ASSERT( nCmdIDWalk == pCmdItemWalk->m_nCmdID );
			if( pCmdItemWalk == pCmdItem )
				continue;
			if( pCmdItemWalk->m_nIconIdx < 0 )
				continue;
			ASSERT( pCmdItemWalk->m_nIconIdx <= m_icons.GetSize() );
			ASSERT( pCmdItemWalk->m_nIconIdx != pCmdItem->m_nIconIdx );
			if( pCmdItemWalk->m_nIconIdx < pCmdItem->m_nIconIdx )
				continue;
			pCmdItemWalk->m_nIconIdx --;
		} // for( ; pos != NULL; )
		pCmdItem->m_nIconIdx = -1;
		return true;
	} // if remove icon query
	else
	{ // if add/set icon query
		ASSERT( ! pCmdIcon->IsEmpty() );
		if( pCmdItem->m_nIconIdx < 0 )
		{
			if( ! bUseCmdIconObject )
			{
				try
				{
					CExtCmdIcon * pNewCmdIcon =
						new CExtCmdIcon( *pCmdIcon );
					ASSERT( ! pNewCmdIcon->IsEmpty() );
					pCmdIcon = pNewCmdIcon;
				}
				catch( CException * pXept )
				{
					ASSERT( FALSE );
					pXept->Delete();
					return false;
				}
				catch( ... )
				{
					ASSERT( FALSE );
					return false;
				} // catch( ... )
			} // if( ! bUseCmdIconObject )
			pCmdItem->m_nIconIdx = m_icons.GetSize();
			m_icons.Add(
				const_cast < CExtCmdIcon * > ( pCmdIcon )
				);
			ASSERT( (pCmdItem->m_nIconIdx + 1) == m_icons.GetSize() );
		} // if( pCmdItem->m_nIconIdx < 0 )
		else
		{
			ASSERT( pCmdItem->m_nIconIdx < m_icons.GetSize() );
			CExtCmdIcon * pExistingCmdIcon =
				m_icons[ pCmdItem->m_nIconIdx ];
			ASSERT( pExistingCmdIcon != NULL );
			if( bUseCmdIconObject )
			{
				m_icons.SetAt(pCmdItem->m_nIconIdx, (CExtCmdIcon*)pCmdIcon );
				delete pExistingCmdIcon;
			} // if( bUseCmdIconObject )
			else
			{
				(*pExistingCmdIcon) = (*pCmdIcon);
				ASSERT( ! pExistingCmdIcon->IsEmpty() );
			} // else from if( bUseCmdIconObject )
		} // else from if( pCmdItem->m_nIconIdx < 0 )
		return true;
	} // if add/set icon query
}

bool CExtCmdProfile::CmdSetIcon(
	UINT nCmdID,
	const CExtCmdIcon & cmdIcon // if empty - remove
	)
{
	if( cmdIcon.IsEmpty() )
		return CmdSetIcon( nCmdID, (CExtCmdIcon *)NULL, false );
	return CmdSetIcon( nCmdID, &cmdIcon, false );
}

bool CExtCmdProfile::CmdSetIcon(
	UINT nCmdID,
	HBITMAP hBitmap, // if NULL - remove
	COLORREF clrTransparent, // = RGB(0,0,0)
	LPCRECT pRectBitmapSrc // = NULL
	)
{
	if( hBitmap == NULL )
		return CmdSetIcon( nCmdID, ((CExtCmdIcon *)NULL), false );
CExtCmdIcon _icon( hBitmap, clrTransparent, pRectBitmapSrc );
	return CmdSetIcon( nCmdID, &_icon, false );
}

bool CExtCmdProfile::CmdSetIcon(
	UINT nCmdID,
	HICON hIcon, // if NULL - remove
	bool bCopyIcon // = true
	)
{
	if( hIcon == NULL )
		return CmdSetIcon( nCmdID, ((CExtCmdIcon *)NULL), false );
CExtCmdIcon _icon( hIcon, bCopyIcon );
	return CmdSetIcon( nCmdID, &_icon, false );
}

CExtCmdIcon * CExtCmdProfile::CmdGetIconPtr(
	UINT nCmdID
	)
{
	if( !CExtCmdManager::IsCommand(nCmdID) )
	{
		ASSERT(FALSE);
		return NULL;
	}
CExtCmdItem * pCmdItem = NULL;
BOOL bExist =
		m_cmds.Lookup( nCmdID, pCmdItem );
	if( !bExist )
		return NULL;
	ASSERT( pCmdItem != NULL );
	if( pCmdItem->m_nIconIdx < 0 )
		return NULL;
	ASSERT( pCmdItem->m_nIconIdx < m_icons.GetSize() );
CExtCmdIcon * pIcon = m_icons[ pCmdItem->m_nIconIdx ];
	ASSERT( pIcon != NULL );
	ASSERT( !(pIcon->IsEmpty()) );
	return pIcon;
}

bool CExtCmdProfile::CmdIsRegistered(
	UINT nCmdID
	)
{
bool bRegistered =
		(CmdGetPtr(nCmdID) != NULL) ?
			true : false;
	return bRegistered;
}

bool CExtCmdProfile::UpdateFromMenu(
	HMENU hMenu,
	bool bReplaceOld, // = false
	bool bRecursive, // = true
	bool bLoadTips // = true
	)
{
	if( hMenu == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( !(::IsMenu(hMenu)) )
	{
		ASSERT( FALSE );
		return false;
	}

int nMenuItemCount = ::GetMenuItemCount(hMenu);
	for(	int nMenuItemIdx=0;
			nMenuItemIdx < nMenuItemCount;
			nMenuItemIdx++
			)
	{ // for all menu level items
		// get the menu item info
		CExtSafeString sMenuText;
		MENUITEMINFO mii;
		::memset( &mii, 0, sizeof(MENUITEMINFO) );
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask =
			MIIM_CHECKMARKS
			|MIIM_DATA
			|MIIM_ID
			|MIIM_STATE
			|MIIM_SUBMENU
			|MIIM_TYPE
			;
		mii.cch = __MAX_UI_ITEM_TEXT;
		mii.dwTypeData =
			sMenuText.GetBuffer(__MAX_UI_ITEM_TEXT);
		ASSERT( mii.dwTypeData != NULL );
		if( mii.dwTypeData == NULL )
		{
			ASSERT( FALSE );
			continue;
		}
		if( !::GetMenuItemInfo(
				hMenu,
				nMenuItemIdx,
				TRUE,
				&mii
				)
			)
		{
			sMenuText.ReleaseBuffer();
			ASSERT( FALSE );
			continue;
		}
		sMenuText.ReleaseBuffer();
		if( (mii.fType & MFT_SEPARATOR) != 0 )
			continue;
		
		// if sub-menu process it
		if( mii.hSubMenu != NULL )
		{
			if( !bRecursive )
				continue;
			VERIFY(
				UpdateFromMenu(
					mii.hSubMenu,
					bReplaceOld,
					true
					)
				);
			continue;
		} // if( mii.hSubMenu != NULL )
		
		// register command
		if( !CExtCmdManager::IsCommand(mii.wID) )
			continue;

		CExtCmdItem _cmd;
		_cmd.m_nCmdID = mii.wID;

		sMenuText.Replace( _T("\n"), _T("") );
		sMenuText.Replace( _T("\r"), _T("") );
		sMenuText.TrimLeft();
		sMenuText.TrimRight();
		if( !sMenuText.IsEmpty() )
		{
			int nSep =
				sMenuText.ReverseFind( _T('\t') );
			if( nSep >= 0 )
			{
				int nLen = sMenuText.GetLength();
				_cmd.m_sAccelText = sMenuText.Right( nLen - nSep );
				_cmd.m_sAccelText.TrimLeft();
				_cmd.m_sAccelText.TrimRight();
				_cmd.m_sMenuText = sMenuText.Left( nSep );
				_cmd.m_sMenuText.TrimLeft();
				_cmd.m_sMenuText.TrimRight();
			}
			else
				_cmd.m_sMenuText = sMenuText;
		} // if( !sMenuText.IsEmpty() )

		if( bLoadTips )
			_cmd.TipsLoad();

		VERIFY( CmdSetup(_cmd,bReplaceOld) );
	} // for all menu level items

	return true;
}

bool CExtCmdProfile::UpdateFromMenu(
	UINT nResourceID,
	bool bReplaceOld, // = false
	bool bLoadTips // = true
	)
{
	__PROF_UIS_MANAGE_STATE;
CMenu menu;
	if( ! g_ResourceManager->LoadMenu( menu, nResourceID ) )
	{
		ASSERT(FALSE);
		return false;
	}
	return
		UpdateFromMenu(
			menu.GetSafeHmenu(),
			bReplaceOld,
			true, // bRecursive
			bLoadTips
			);
}

CExtCmdProfile::MFC_TOOLBAR_LOADER::MFC_TOOLBAR_LOADER(
	HINSTANCE hInstResourceCommands,
	HRSRC hRsrcCommands,
	HINSTANCE hInstResourceBitmap,
	HRSRC hRsrcCommandsBitmap,
	COLORREF clrBmpTransparent // = RGB(192,192,192)
	)
	: m_pCommands( NULL )
	, m_pButtons( NULL )
	, m_nCountCommands( 0 )
	, m_nCountButtons( 0 )
	, m_clrBmpTransparent( clrBmpTransparent )
	, m_sizeButton( 0, 0 )
	, m_nVersion( 0 )
{
	ASSERT( hInstResourceCommands != NULL );
	ASSERT( hRsrcCommands != NULL );
	ASSERT( hInstResourceBitmap != NULL );
	ASSERT( hRsrcCommandsBitmap != NULL );

HBITMAP hBitmap =
		CExtPaintManager::stat_LoadBitmapResource(
			hInstResourceBitmap,
			hRsrcCommandsBitmap
			);
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	m_bmp.Attach( hBitmap );
#ifdef _DEBUG
BITMAP _bmpInfo;
	::memset( &_bmpInfo, 0, sizeof(BITMAP) );
	::GetObject( hBitmap, sizeof(BITMAP), &_bmpInfo );
	ASSERT( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 );
#endif // _DEBUG

HGLOBAL hGlobalResourceCommands =
		::LoadResource(
			hInstResourceCommands,
			hRsrcCommands
			);
	ASSERT( hGlobalResourceCommands != NULL );
	if( hGlobalResourceCommands == NULL )
		return;
	LP_MFC_TOOLBAR_RESOURCE_DATA pData =
		(LP_MFC_TOOLBAR_RESOURCE_DATA)
			::LockResource( hGlobalResourceCommands );
	if( pData == NULL )
		return;
	m_nCountCommands = (INT)pData->wItemCount;
	ASSERT( m_nCountCommands > 0 );
	m_nVersion = pData->wVersion;
	m_pCommands = new UINT[ m_nCountCommands ];
	m_nCountButtons = 0;
	for(	INT nCommandIdx = 0;
			nCommandIdx < m_nCountCommands;
			nCommandIdx++
			)
	{
		m_pCommands[nCommandIdx] = pData->items()[nCommandIdx];
		if( m_pCommands[nCommandIdx] != ID_SEPARATOR )
			m_nCountButtons++;
	}
	ASSERT( m_nCountButtons > 0 );
	m_pButtons =  new UINT[ m_nCountButtons ];
INT nButtonIdx = 0;
	for(	nCommandIdx = 0;
			nCommandIdx < m_nCountCommands;
			nCommandIdx++
			)
	{
		if( m_pCommands[nCommandIdx] == ID_SEPARATOR )
			continue;
		ASSERT( nButtonIdx < m_nCountButtons );
		m_pButtons[ nButtonIdx ] = m_pCommands[nCommandIdx];
		nButtonIdx++;
		if( nButtonIdx == m_nCountButtons )
			break;
	}

	ASSERT( pData->wWidth > 0 && pData->wHeight > 0 );

	m_sizeButton.cx = (INT)pData->wWidth;
	m_sizeButton.cy = (INT)pData->wHeight;
	ASSERT( _bmpInfo.bmWidth >= ( LONG(m_sizeButton.cx) * LONG(m_nCountButtons) ) );
	ASSERT( _bmpInfo.bmHeight >= LONG(m_sizeButton.cy) );
	
	::UnlockResource( hGlobalResourceCommands );
	::FreeResource( hGlobalResourceCommands );
}

CExtCmdProfile::MFC_TOOLBAR_LOADER::~MFC_TOOLBAR_LOADER()
{
	if( m_pCommands != NULL )
		delete [] m_pCommands;
	if( m_pButtons != NULL )
		delete [] m_pButtons;
	if( m_bmp.GetSafeHandle() != NULL )
		m_bmp.DeleteObject();
}

WORD CExtCmdProfile::MFC_TOOLBAR_LOADER::GetVersion() const
{
	return m_nVersion;
}

INT CExtCmdProfile::MFC_TOOLBAR_LOADER::IsEmpty() const
{
	return ( m_pCommands == NULL ) ? true : false;
}

INT CExtCmdProfile::MFC_TOOLBAR_LOADER::GetCommandCount() const
{
	if( m_pCommands == NULL )
		return 0;
	ASSERT( m_nCountCommands > 0 );
	ASSERT( m_nCountButtons > 0 );
	ASSERT( m_pButtons != NULL );
	ASSERT( m_bmp.GetSafeHandle() != NULL );
	ASSERT( m_sizeButton.cx > 0 && m_sizeButton.cy > 0 );
	return m_nCountCommands;
}

INT CExtCmdProfile::MFC_TOOLBAR_LOADER::GetButtonCount() const
{
	if( m_pCommands == NULL )
		return 0;
	ASSERT( m_nCountCommands > 0 );
	ASSERT( m_nCountButtons > 0 );
	ASSERT( m_pButtons != NULL );
	ASSERT( m_bmp.GetSafeHandle() != NULL );
	ASSERT( m_sizeButton.cx > 0 && m_sizeButton.cy > 0 );
	return m_nCountButtons;
}

COLORREF CExtCmdProfile::MFC_TOOLBAR_LOADER::GetTransparentColor() const
{
	return m_clrBmpTransparent;
}

UINT CExtCmdProfile::MFC_TOOLBAR_LOADER::GetCommandIdAt( INT nCommandIdx ) const
{
	if( IsEmpty() )
	{
		ASSERT( FALSE );
		return 0;
	}
	ASSERT( m_pCommands != NULL );
	ASSERT( m_nCountCommands != NULL );
	if( nCommandIdx < 0 || nCommandIdx >= m_nCountCommands )
	{
		ASSERT( FALSE );
		return 0;
	}
	return m_pCommands[ nCommandIdx ];
}

UINT CExtCmdProfile::MFC_TOOLBAR_LOADER::GetButtonIdAt( INT nButtonIdx ) const
{
	if( IsEmpty() )
	{
		ASSERT( FALSE );
		return 0;
	}
	ASSERT( m_pButtons != NULL );
	ASSERT( m_nCountButtons != NULL );
	if( nButtonIdx < 0 || nButtonIdx >= m_nCountButtons )
	{
		ASSERT( FALSE );
		return 0;
	}
	return m_pButtons[ nButtonIdx ];
}

UINT CExtCmdProfile::MFC_TOOLBAR_LOADER::ExtractButtonData(
	INT nButtonIdx,
	CExtCmdIcon & icon
	) const
{
	icon.Empty();
	if( IsEmpty() )
	{
		ASSERT( FALSE );
		return 0;
	}
	ASSERT( m_nCountCommands > 0 );
	ASSERT( m_nCountButtons > 0 );
	ASSERT( m_pCommands != NULL );
	ASSERT( m_pButtons != NULL );
	ASSERT( m_bmp.GetSafeHandle() != NULL );
	ASSERT( m_sizeButton.cx > 0 && m_sizeButton.cy > 0 );
	if( nButtonIdx < 0 || nButtonIdx >= m_nCountButtons )
	{
		ASSERT( FALSE );
		return 0;
	}
CPoint ptItem( m_sizeButton.cx * nButtonIdx, 0 );
CRect rcItem( ptItem, m_sizeButton );
	icon.AssignFromHBITMAP(
		(HBITMAP)m_bmp.GetSafeHandle(),
		m_clrBmpTransparent,
		&rcItem
		);
	if( icon.IsEmpty() )
	{
		ASSERT( FALSE );
		return 0;
	}
	return m_pButtons[ nButtonIdx ];
}

void CExtCmdProfile::MFC_TOOLBAR_LOADER::GetCmdArray(
	LPUINT * ppCmdArray,
	LPINT pCmdCount // = NULL
	) const
{
	if( ppCmdArray != NULL )
		(*ppCmdArray) = NULL;
	if( pCmdCount != NULL )
		(*pCmdCount) = m_nCountCommands;
	if( IsEmpty() || ppCmdArray == NULL )
		return;
	ASSERT( m_nCountCommands > 0 );
	ASSERT( m_nCountButtons > 0 );
	ASSERT( m_pCommands != NULL );
	ASSERT( m_pButtons != NULL );
	(*ppCmdArray) = new UINT[ m_nCountCommands ];
	::memcpy(
		(LPVOID)(*ppCmdArray),
		(LPCVOID)m_pCommands,
		m_nCountCommands * sizeof(UINT)
		);
}

bool CExtCmdProfile::UpdateFromToolBar(
	CToolBar & bar,
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool bLoadTips // = true
	)
{
	if(		bar.GetSafeHwnd() == NULL
		||	(! ::IsWindow(bar.GetSafeHwnd()))
		)
	{
		ASSERT( FALSE );
		return false;
	}
CImageList * pImageList =
		bar.GetToolBarCtrl().GetImageList();
	if( pImageList == NULL
		|| pImageList->GetSafeHandle() == NULL
		)
	{
		ASSERT( FALSE );
		return false;
	}
int nButtonCount = bar.GetCount();
    for( int nButtonIdx = 0; nButtonIdx < nButtonCount; nButtonIdx++ )
	{
		// get button info
		UINT nCmdID = bar.GetItemID( nButtonIdx ); 
		if( nCmdID == ID_SEPARATOR )
			continue;
		ASSERT( CExtCmdManager::IsCommand( nCmdID ) );

		TBBUTTONINFO tbi;
		::memset( &tbi, 0, sizeof(TBBUTTONINFO) );
		tbi.cbSize = sizeof(TBBUTTONINFO);
		tbi.idCommand = nCmdID;
		tbi.dwMask = TBIF_IMAGE|TBIF_TEXT;
		tbi.cchText = __MAX_UI_ITEM_TEXT;
		CExtSafeString sToolbarText;
		tbi.pszText =
			sToolbarText.GetBuffer(__MAX_UI_ITEM_TEXT);
		ASSERT( tbi.pszText != NULL );
		if( tbi.pszText == NULL )
		{
			ASSERT( FALSE );
			continue;
		}
		if( !bar.GetToolBarCtrl().GetButtonInfo(nCmdID,&tbi) )
		{
			UINT nDummyID,nDummyStyle;
			bar.GetButtonInfo(
				nButtonIdx,
				nDummyID,
				nDummyStyle,
				tbi.iImage
				);
		}
		sToolbarText.ReleaseBuffer();

		// register command
		CExtCmdItem _cmd;
		_cmd.m_nCmdID = nCmdID;
		_cmd.m_sToolbarText = sToolbarText;
		if( bLoadTips )
			_cmd.TipsLoad();
		if( !CmdSetup(_cmd,bReplaceOld) )
		{
			ASSERT(FALSE);
			continue;
		}
		if( tbi.iImage < 0 )
			continue;
		ASSERT( tbi.iImage < pImageList->GetImageCount() );
		CExtCmdItem * pCmdItem = CmdGetPtr(nCmdID);
		ASSERT( pCmdItem != NULL );
		if( pCmdItem->m_nIconIdx >= 0 )
		{
			ASSERT( pCmdItem->m_nIconIdx < m_icons.GetSize() );
			continue;
		}
		HICON hIcon = pImageList->ExtractIcon( tbi.iImage );
		if( hIcon == NULL )
		{
			ASSERT(FALSE);
			continue;
		}
		CExtCmdIcon * pIcon = new CExtCmdIcon;
		pIcon->AssignFromHICON( hIcon, false );
		int nIconIndex = m_icons.Add( pIcon );
		ASSERT( nIconIndex >= 0 );
		pCmdItem->m_nIconIdx = nIconIndex;
	} // for( int nButtonIdx = 0; nButtonIdx < nButtonCount; nButtonIdx++ )
	return true;
}

bool CExtCmdProfile::UpdateFromToolBar(
	__EXT_MFC_SAFE_LPCTSTR strResourceID,
	LPUINT * ppCmdArray, // = NULL
	LPINT pCmdCount, // = NULL
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool bLoadTips, // = true
	COLORREF clrBmpTransparent // = RGB(192,192,192)
	)
{
	ASSERT( strResourceID != NULL );
	if( strResourceID == NULL )
		return false;

HINSTANCE hInstResourceCommands =
		::AfxFindResourceHandle(
			strResourceID,
			RT_TOOLBAR
			);
	if( hInstResourceCommands == NULL )
		return false;
HRSRC hRsrcCommands =
		::FindResource(
			hInstResourceCommands,
			strResourceID,
			RT_TOOLBAR
			);
	if( hRsrcCommands == NULL )
		return false;
HINSTANCE hInstResourceBitmap =
		::AfxFindResourceHandle(
			strResourceID,
			RT_BITMAP
			);
	if( hInstResourceBitmap == NULL )
		return false;
HRSRC hRsrcBitmap =
		::FindResource(
			hInstResourceBitmap,
			strResourceID,
			RT_BITMAP
			);
	if( hRsrcBitmap == NULL )
		return false;

MFC_TOOLBAR_LOADER _loader(
		hInstResourceCommands,
		hRsrcCommands,
		hInstResourceBitmap,
		hRsrcBitmap,
		clrBmpTransparent
		);
	if( _loader.IsEmpty() )
		return false;
int nButtonCount = _loader.GetButtonCount();
	ASSERT( nButtonCount > 0 );
    for( int nButtonIdx = 0; nButtonIdx < nButtonCount; nButtonIdx++ )
	{
		CExtCmdIcon * pIcon = new CExtCmdIcon;
		CExtCmdItem _cmd;
		_cmd.m_nCmdID =
			_loader.ExtractButtonData( nButtonIdx, *pIcon );
		if( _cmd.m_nCmdID == 0 || pIcon->IsEmpty() )
		{
			ASSERT( FALSE );
			delete pIcon;
			continue;
		} // if( _cmd.m_nCmdID == 0 || pIcon->IsEmpty() )
		ASSERT( CExtCmdManager::IsCommand( _cmd.m_nCmdID ) );
		if( bLoadTips )
			_cmd.TipsLoad();
		if( !CmdSetup( _cmd, bReplaceOld ) )
		{
			ASSERT(FALSE);
			delete pIcon;
			continue;
		} // if( !CmdSetup( _cmd, bReplaceOld ) )
		CExtCmdItem * pCmdItem = CmdGetPtr( _cmd.m_nCmdID );
		ASSERT( pCmdItem != NULL );
		if( pCmdItem->m_nIconIdx >= 0 )
		{
			ASSERT( pCmdItem->m_nIconIdx < m_icons.GetSize() );
			delete pIcon;
			continue;
		} // if( pCmdItem->m_nIconIdx >= 0 )
		int nIconIndex = m_icons.Add( pIcon );
		ASSERT( nIconIndex >= 0 );
		pCmdItem->m_nIconIdx = nIconIndex;
	} // for( int nButtonIdx = 0; nButtonIdx < nButtonCount; nButtonIdx++ )
	_loader.GetCmdArray( ppCmdArray, pCmdCount );
	return true;
}

bool CExtCmdProfile::SerializeState(
	CArchive & ar
	)
{
bool bRetVal = false;
	try
	{
		CExtSafeString sFriendlyVer;
		DWORD dwApiVer0 = g_CmdManager.GetVersionDWORD( false );
		DWORD dwApiVer1 = g_CmdManager.GetVersionDWORD( true );
		DWORD dwReserved = 0;

		if( ar.IsStoring() )
		{ // store state
			CExtSafeString sTmpBuffer;
			// store version info
			sFriendlyVer.Format(
				_T("Prof-UIS (v. %s) command profile"),
				g_CmdManager.GetVersionString( sTmpBuffer )
				);
			ar << sFriendlyVer;
			ar << dwApiVer0;
			ar << dwApiVer1;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;

			// store command usage information
			ar << m_nTotalTickCount;

			POSITION pos = m_cmds.GetStartPosition();
			for( ; pos != NULL; )
			{
				UINT nCmdID;
				CExtCmdItem * pCmdItem = NULL;
				m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
				ASSERT( pCmdItem != NULL );
				if( !pCmdItem->OnQueryStateSerializationNecessity() )
					continue;
				ar << pCmdItem->m_nCmdID;
				ar << pCmdItem->m_nUsageTickCount;
			}

			ar << ((UINT)(0));
		} // store state
		else
		{ // load state
			// load version info
			DWORD dwApiVer0a = 0, dwApiVer1a = 0;
			ar >> sFriendlyVer;
			ar >> dwApiVer0a;
			ar >> dwApiVer1a;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			if( dwApiVer1 != dwApiVer1a )
				return false;

			// load command usage information
			ar >> m_nTotalTickCount;
			for( ; true; )
			{
				UINT nCmdId,nUsageTickCount;
				ar >> nCmdId;
				if( nCmdId == 0 )
					break;
				ar >> nUsageTickCount;
				CExtCmdItem * pCmdItem = CmdGetPtr(nCmdId);
            //Snipe ///////////
            if (pCmdItem==NULL)
               continue;
            ///////////////////
				ASSERT( pCmdItem != NULL );
				ASSERT( !(pCmdItem->StateIsBasic()) );
				ASSERT( !(pCmdItem->StateIsForceRarely()) );
				ASSERT( !(CExtCmdManager::IsSystemCommand(pCmdItem->m_nCmdID)) );
				pCmdItem->m_nUsageTickCount = nUsageTickCount;
			}
		} // load state

		bRetVal = true;
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return bRetVal;
}

bool CExtCmdProfile::SetBasicCommands(
	UINT * pCommands,
	bool bOn // = true
	)
{
	if( pCommands == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
bool bRetVal = true;
	for(; *pCommands != 0; pCommands++ )
	{
		if( CExtCmdManager::IsSystemCommand( *pCommands ) )
		{
			ASSERT( FALSE );
			bRetVal = false; // we find some error
			continue;
		}
		CExtCmdItem * pCmdItem = CmdGetPtr( *pCommands );
		if( pCmdItem == NULL )
		{
			ASSERT( FALSE );
			bRetVal = false; // we find some error
			continue;
		}
		pCmdItem->StateSetBasic( bOn );
	} // for(; *pCommands != 0; pCommands++ )
	return bRetVal;
}

bool CExtCmdProfile::SerializeState(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	bool bSave
	)
{
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
bool bRetVal = false;
	try
	{
		CMemFile _file;
		if( bSave )
		{
			{ // BLOCK: CArchive usage
				CArchive ar(
					&_file,
					CArchive::store
					);
				if( !SerializeState(ar) )
					return false;
				ar.Flush();
			} // BLOCK: CArchive usage

			// ... write _file to registty
			_file.Seek(0,CFile::begin);
			if( !fileobj_to_registry(
					_file,
					m_sName,
					sSectionNameCompany,
					sSectionNameProduct
					)
				)
				return false;

		} // if( bSave )
		else
		{
			// ... read _file from registty
			if( !fileobj_from_registry(
					_file,
					m_sName,
					sSectionNameCompany,
					sSectionNameProduct
					)
				)
				return false;
			_file.Seek(0,CFile::begin);

			CArchive ar(
				&_file,
				CArchive::load
				);
			if( !SerializeState(ar) )
				return false;
		} // else from if( bSave )

		bRetVal = true;
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return bRetVal;
}

void CExtCmdProfile::OnSysColorChange()
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
		ASSERT( pCmdItem != NULL );
		pCmdItem->OnSysColorChange();
	} // for( ; pos != NULL; )
}

void CExtCmdProfile::OnSettingChange(
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
		ASSERT( pCmdItem != NULL );
		pCmdItem->OnSettingChange( uFlags, lpszSection );
	} // for( ; pos != NULL; )
}

void CExtCmdProfile::OnDisplayChange(
	INT nDepthBPP,
	CPoint ptSizes
	)
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
		ASSERT( pCmdItem != NULL );
		pCmdItem->OnDisplayChange( nDepthBPP, ptSizes );
	} // for( ; pos != NULL; )
}

void CExtCmdProfile::OnThemeChanged(
	WPARAM wParam,
	LPARAM lParam
	)
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
		ASSERT( pCmdItem != NULL );
		pCmdItem->OnThemeChanged( wParam, lParam );
	} // for( ; pos != NULL; )
}

void CExtCmdProfile::ResetUsageStatistics()
{
POSITION pos = m_cmds.GetStartPosition();
	for( ; pos != NULL; )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem);
		ASSERT( pCmdItem != NULL );
		pCmdItem->ResetUsageStatistics();
	}
	m_nTotalTickCount = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCmdManager
// command manager class

CExtCmdManager::CExtCmdManagerAutoPtr g_CmdManager;

volatile DWORD CExtCmdManager::CExtCmdManagerAutoPtr::g_dwVersion = 0;

DWORD CExtCmdManager::CExtCmdManagerAutoPtr::GetVersionDWORD(
	bool bForSerialization // = false
	)
{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

DWORD dwVersion = 0;
static CCriticalSection scs;
CSingleLock sl( &scs );
BYTE * pByteVerInfo = NULL;
	sl.Lock();
	try
	{
		if( g_dwVersion == 0 )
		{
			HANDLE hModule =
				::GetModuleHandle( __PROF_UIS_MODULE_NAME );
			ASSERT( hModule != NULL );
			TCHAR szFileName[MAX_PATH];
			VERIFY(
				::GetModuleFileName(
					HINSTANCE(hModule),
					szFileName,
					MAX_PATH
					)
				);
			DWORD dwFVIS =
				::GetFileVersionInfoSize(
					szFileName,
					&dwFVIS
					);
			pByteVerInfo = new BYTE[4096];
			::memset( pByteVerInfo, 0, 4096 );
			VERIFY(
				::GetFileVersionInfo(
					szFileName,
					NULL,
					dwFVIS,
					pByteVerInfo
					)
				);
			UINT uNum = 0;
			VS_FIXEDFILEINFO * lpv = NULL;
			VERIFY(
				::VerQueryValue(
					pByteVerInfo,
					_T("\\"),
					((LPVOID*)&lpv),
					&uNum
					)
				);
			ASSERT( lpv != NULL );
			g_dwVersion = 
				 (DWORD(HIWORD(lpv->dwFileVersionMS)&0x0FF)<<24)
				|(DWORD(LOWORD(lpv->dwFileVersionMS)&0x0FF)<<16)
				|(DWORD(HIWORD(lpv->dwFileVersionLS)&0x0FF)<<8)
				|(DWORD(LOWORD(lpv->dwFileVersionLS)&0x0FF))
				;
			if( pByteVerInfo != NULL )
			{
				delete [] pByteVerInfo;
				pByteVerInfo = NULL;
			}
		} // if( g_dwVersion == 0 )
		dwVersion = g_dwVersion;
		if( bForSerialization )
			dwVersion >>= 16;
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	sl.Unlock();
	if( pByteVerInfo != NULL )
		delete [] pByteVerInfo;
	return dwVersion;

#else // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

DWORD dwVersion = __PROF_UIS_VERSION_DWORD;
	if( g_dwVersion == 0 )
	{
		static CCriticalSection scs;
		CSingleLock sl( &scs );
		sl.Lock();
		g_dwVersion = dwVersion;
		sl.Unlock();
	}
	if( bForSerialization )
		dwVersion >>= 16;
	return dwVersion;

#endif // else from #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
}

__EXT_MFC_SAFE_LPCTSTR CExtCmdManager::CExtCmdManagerAutoPtr::GetVersionString(
	CExtSafeString & strBuff,
	__EXT_MFC_SAFE_TCHAR tchrSeparator // = _T('.')
	)
{
DWORD dwVersion = GetVersionDWORD( false );
	ASSERT( dwVersion != 0 );
	strBuff.Format(
		_T("%d%c%d%c%d%c%d"),
		INT( (dwVersion>>24)&0x0FF ),
		tchrSeparator,
		INT( (dwVersion>>16)&0x0FF ),
		tchrSeparator,
		INT( (dwVersion>>8)&0x0FF ),
		tchrSeparator,
		INT( dwVersion&0x0FF )
		);
	return strBuff;
}

CExtCmdManager::CExtCmdManagerAutoPtr::CExtCmdManagerAutoPtr()
{
	m_pCmdManager = new CExtCmdManager;
}

CExtCmdManager::CExtCmdManagerAutoPtr::~CExtCmdManagerAutoPtr()
{
	ASSERT( m_pCmdManager != NULL );
	if( m_pCmdManager != NULL )
	{
		delete m_pCmdManager;
		m_pCmdManager = NULL;
	}
}

CExtCmdManager * CExtCmdManager::CExtCmdManagerAutoPtr::operator->()
{
	ASSERT( m_pCmdManager != NULL );
	return m_pCmdManager;
}

void CExtCmdManager::CExtCmdManagerAutoPtr::OnSysColorChange(
	CWnd * pWndNotifySrc
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL && ::IsWindow(pWndNotifySrc->GetSafeHwnd()) );
	ASSERT( m_pCmdManager != NULL );
	if( !g_PaintManager.IsWndUpdateSource(pWndNotifySrc) )
		return;
	m_pCmdManager->OnSysColorChange();
}

void CExtCmdManager::CExtCmdManagerAutoPtr::OnSettingChange(
	CWnd * pWndNotifySrc,
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL && ::IsWindow(pWndNotifySrc->GetSafeHwnd()) );
	ASSERT( m_pCmdManager != NULL );
	if( !g_PaintManager.IsWndUpdateSource(pWndNotifySrc) )
		return;
	m_pCmdManager->OnSettingChange( uFlags, lpszSection );
}

void CExtCmdManager::CExtCmdManagerAutoPtr::OnDisplayChange(
	CWnd * pWndNotifySrc,
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL && ::IsWindow(pWndNotifySrc->GetSafeHwnd()) );
	ASSERT( m_pCmdManager != NULL );
	if( !g_PaintManager.IsWndUpdateSource(pWndNotifySrc) )
		return;
	m_pCmdManager->OnDisplayChange( nDepthBPP, ptSizes );
}

void CExtCmdManager::CExtCmdManagerAutoPtr::OnThemeChanged(
	CWnd * pWndNotifySrc,
	WPARAM wParam,
	LPARAM lParam
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL && ::IsWindow(pWndNotifySrc->GetSafeHwnd()) );
	ASSERT( m_pCmdManager != NULL );
	if( !g_PaintManager.IsWndUpdateSource(pWndNotifySrc) )
		return;
	m_pCmdManager->OnThemeChanged( wParam, lParam );
}

bool CExtCmdManager::g_bDisableCmdIfNoHandler = true;

CExtCmdManager::CExtCmdManager()
{
	ProfileSetup(
		__EXTMFC_DEF_PROFILE_NAME
		);
}

CExtCmdManager::~CExtCmdManager()
{
	_RemoveAllProfilesImpl();
}

void CExtCmdManager::_RemoveAllProfilesImpl()
{
	POSITION pos = m_profiles.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtSafeString sProfileName;
		CExtCmdProfile * pProfile = NULL;
		m_profiles.GetNextAssoc( pos, sProfileName, (void *&)pProfile );
		ASSERT( pProfile != NULL );
		delete pProfile;
	} // for( ; pos != NULL; )
	m_profiles.RemoveAll();
	m_profile_wnds.RemoveAll();
}

// setup single profile
bool CExtCmdManager::ProfileSetup(
	__EXT_MFC_SAFE_LPCTSTR sProfileName, //  = NULL
	HWND hProfileWnd, // = NULL
	CExtCmdProfile * pNewProfileInstance // = NULL
	)
{
	if(		sProfileName == NULL
		||	_tcslen( sProfileName ) == 0
		)
		sProfileName = __EXTMFC_DEF_PROFILE_NAME;
	m_cs.Lock();
CExtCmdProfile * pProfile = NULL;
BOOL bExist = m_profiles.Lookup( sProfileName, (void *&)pProfile );
	if( !bExist )
	{
		if( pNewProfileInstance != NULL )
		{
			pProfile = pNewProfileInstance;
			// avoid invalid method usage
			ASSERT( pNewProfileInstance->m_sName == LPCTSTR( sProfileName ) );
		} // if( pNewProfileInstance != NULL )
		else
			pProfile = new CExtCmdProfile( sProfileName );
		m_profiles.SetAt( sProfileName, pProfile );
	} // if( !bExist )
#ifdef _DEBUG
	else
	{
		// avoid leaks when invalid method usage
		ASSERT( pNewProfileInstance == NULL );
	} // else from if( !bExist )
#endif // _DEBUG
	ASSERT( pProfile != NULL );
	ASSERT( pProfile->m_sName == LPCTSTR(sProfileName) );
	m_cs.Unlock();
	if( hProfileWnd != NULL )
		return ProfileWndAdd( sProfileName, hProfileWnd );
	else
		return true;
}

// setup profile window
bool CExtCmdManager::ProfileWndAdd(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	HWND hProfileWnd
	)
{
	ASSERT( hProfileWnd != NULL );
	if( hProfileWnd == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( ::IsWindow(hProfileWnd) );
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
CExtCmdProfile * pProfile2 = NULL;
BOOL bExist = m_profile_wnds.Lookup( hProfileWnd, pProfile2 );
	if( !bExist )
		m_profile_wnds.SetAt( hProfileWnd, pProfile );
#ifdef _DEBUG
	else
	{
		ASSERT( pProfile2 != NULL );
		ASSERT( pProfile2 == pProfile );
	}
#endif // _DEBUG
	m_cs.Unlock();
	return true;
}

// remove profile window
bool CExtCmdManager::ProfileWndRemove(
	HWND hProfileWnd,
	bool bRemoveProfileIfLastHWND, // = false
	bool * p_bProfileWasRemoved // = NULL
	)
{
	ASSERT( hProfileWnd != NULL );
	if( p_bProfileWasRemoved  != NULL )
		*p_bProfileWasRemoved = false;
	if( hProfileWnd == NULL )
		return false;
	//ASSERT( ::IsWindow(hProfileWnd) );
	m_cs.Lock();

CExtCmdProfile * pProfile = NULL;
BOOL bExists = m_profile_wnds.Lookup( hProfileWnd, pProfile );
	if( bExists )
	{
		ASSERT( pProfile != NULL );
		VERIFY( m_profile_wnds.RemoveKey(hProfileWnd) );
		if( bRemoveProfileIfLastHWND )
		{
			int nProfileFoundWindowCount = 0;
			for(	POSITION pos = m_profile_wnds.GetStartPosition();
					pos != NULL;
					)
			{
				HWND hWndWalk = NULL;
				CExtCmdProfile * pProfileWalk = NULL;
				m_profile_wnds.GetNextAssoc(
					pos,
					hWndWalk,
					pProfileWalk
					);
				ASSERT( hWndWalk != NULL );
				ASSERT( pProfileWalk != NULL );
				if( pProfileWalk == pProfile )
				{
					nProfileFoundWindowCount++;
					break;
				}
			}
			if( nProfileFoundWindowCount == 0 )
			{
				if( p_bProfileWasRemoved  != NULL )
					*p_bProfileWasRemoved = true;
				CExtSafeString sProfileName( pProfile->m_sName );
				delete pProfile;
				VERIFY( m_profiles.RemoveKey(LPCTSTR(sProfileName)) );
			} // if( nProfileFoundWindowCount == 0 )
		} // if( bRemoveProfileIfLastHWND )
	} // if( bExists )
	m_cs.Unlock();
	return bExists ? true : false;
}

// remove all profile windows
int CExtCmdManager::ProfileWndRemoveAll(
	__EXT_MFC_SAFE_LPCTSTR sProfileName
	)
{
	if(		sProfileName == NULL
		||	_tcslen( sProfileName ) == 0
		)
	{
		ASSERT( FALSE );
		return 0;
	}
	m_cs.Lock();
int nRemovedCount = 0;
CExtCmdProfile * pProfile = NULL;
BOOL bExists = m_profiles.Lookup( sProfileName, (void *&)pProfile );
	if( bExists )
	{
		ASSERT( pProfile != NULL );
		ASSERT( pProfile->m_sName == LPCTSTR(sProfileName) );
		CList < HWND, HWND & > list;
		_ProfileGetWndListImpl( pProfile, list );
		for(	POSITION pos = list.GetHeadPosition();
				pos != NULL;
				)
		{
			HWND hWndWalk = list.GetNext( pos );
			ASSERT( hWndWalk == NULL );
			VERIFY( m_profile_wnds.RemoveKey(hWndWalk) );
			nRemovedCount ++;
		}
		ASSERT( nRemovedCount == list.GetCount() );
	} // if( bExists )
	m_cs.Unlock();
	return nRemovedCount;
}

// get profile window list
void CExtCmdManager::ProfileGetWndList(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	CList < HWND, HWND & > & list
	)
{
	m_cs.Lock();
	_ProfileGetWndListImpl( sProfileName, list );
	m_cs.Unlock();
}

void CExtCmdManager::_ProfileGetWndListImpl(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	CList < HWND, HWND & > & list
	)
{
	list.RemoveAll();
	if(		sProfileName == NULL
		||	_tcslen( sProfileName ) == 0
		)
	{
		ASSERT( FALSE );
		return;
	}
CExtCmdProfile * pProfile = NULL;
BOOL bExists = m_profiles.Lookup( sProfileName, (void *&)pProfile );
	if( ! bExists )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pProfile != NULL );
	ASSERT( pProfile->m_sName == LPCTSTR(sProfileName) );
	_ProfileGetWndListImpl( pProfile, list );
}

void CExtCmdManager::_ProfileGetWndListImpl(
	CExtCmdProfile * pProfile,
	CList < HWND, HWND & > & list
	)
{
	ASSERT( pProfile != NULL );
	ASSERT( ! pProfile->m_sName.IsEmpty() );
	ASSERT( list.GetCount() == 0 );
	for(	POSITION pos = m_profile_wnds.GetStartPosition();
			pos != NULL;
			)
	{
		HWND hWndWalk = NULL;
		CExtCmdProfile * pProfileWalk = NULL;
		m_profile_wnds.GetNextAssoc(
			pos,
			hWndWalk,
			pProfileWalk
			);
		ASSERT( hWndWalk != NULL );
		ASSERT( pProfileWalk != NULL );
		if( pProfileWalk == pProfile )
			list.AddTail( hWndWalk );
	}
}

// remove profile from command manager
bool CExtCmdManager::ProfileDestroy(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	bool bDestroyIfHasWindows // = false
	)
{
	if(		sProfileName == NULL
		||	_tcslen( sProfileName ) == 0
		)
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
CExtCmdProfile * pProfile = NULL;
bool bProfileDestroyed = false;
BOOL bExists = m_profiles.Lookup( sProfileName, (void *&)pProfile );
	if( bExists )
	{
		ASSERT( pProfile != NULL );
		ASSERT( pProfile->m_sName == LPCTSTR(sProfileName) );
		CList < HWND, HWND & > list;
		_ProfileGetWndListImpl( pProfile, list );
		int nWndCount = list.GetCount();
		if( nWndCount > 0 && bDestroyIfHasWindows )
		{
			POSITION pos = list.GetHeadPosition();
			ASSERT( pos != NULL );
			for( ; pos != NULL; )
			{
				HWND hWndWalk = list.GetNext( pos );
				ASSERT( hWndWalk != NULL );
				VERIFY( m_profile_wnds.RemoveKey( hWndWalk ) );
				ASSERT( nWndCount > 0 );
				nWndCount--;
			} // for( ; pos != NULL; )
			ASSERT( nWndCount == 0 );
		} // if( nWndCount > 0 && bDestroyIfHasWindows )
		if( nWndCount == 0 )
		{
			bProfileDestroyed = true;
			CExtSafeString sProfileName2( pProfile->m_sName );
			delete pProfile;
			VERIFY( m_profiles.RemoveKey(LPCTSTR(sProfileName2)) );
		} // if( nWndCount == 0 )
	} // if( bExists )
	m_cs.Unlock();
	return bProfileDestroyed;
}

// rename profile
bool CExtCmdManager::ProfileRename(
	__EXT_MFC_SAFE_LPCTSTR sProfileNameOld,
	__EXT_MFC_SAFE_LPCTSTR sProfileNameNew
	)
{
	if(		sProfileNameOld == NULL
		||	sProfileNameNew == NULL
		||	_tcslen( sProfileNameOld ) == 0
		||	_tcslen( sProfileNameNew ) == 0
		||	_tcscmp( sProfileNameOld, __EXTMFC_DEF_PROFILE_NAME ) == 0
		||	_tcscmp( sProfileNameNew, __EXTMFC_DEF_PROFILE_NAME ) == 0
		)
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bProfileRenamed = false;
CExtCmdProfile * pProfile = NULL;
BOOL bExists = m_profiles.Lookup( sProfileNameOld, (void *&)pProfile );
	if( bExists )
	{
		ASSERT( pProfile != NULL );
		ASSERT( pProfile->m_sName == LPCTSTR(sProfileNameOld) );
		if(	LPVOID(LPCTSTR(sProfileNameOld)) == LPVOID(LPCTSTR(sProfileNameNew)) )
			bProfileRenamed = true;
		else
		{
			CExtCmdProfile * pProfileOther = NULL;
			BOOL bExistsOther = m_profiles.Lookup( sProfileNameNew, (void *&)pProfileOther );
			if( !bExistsOther )
			{
				bProfileRenamed = true;
				CExtSafeString sProfileName = LPCTSTR(sProfileNameOld);
				VERIFY( m_profiles.RemoveKey(LPCTSTR(sProfileName)) );
				pProfile->m_sName = sProfileNameNew;
				m_profiles.SetAt( sProfileNameNew, pProfile );
			} // if( !bExistsOther )
#ifdef _DEBUG
			else
			{
				ASSERT( pProfileOther != NULL );
				ASSERT( pProfileOther->m_sName == LPCTSTR(sProfileNameNew) );
			} // else from if( !bExistsOther )
#endif // _DEBUG
		} // else from if(	LPVOID(LPCTSTR(sProfileNameOld)) == LPVOID(LPCTSTR(sProfileNameNew)) )
	} // if( bExists )
	m_cs.Unlock();
	return bProfileRenamed;
}

// get profile
CExtCmdProfile * CExtCmdManager::ProfileGetPtr(
	__EXT_MFC_SAFE_LPCTSTR sProfileName //  = NULL
	)
{
	if( sProfileName == NULL )
		sProfileName = __EXTMFC_DEF_PROFILE_NAME;
	m_cs.Lock();
CExtCmdProfile * pProfile = NULL;
BOOL bExists = m_profiles.Lookup( sProfileName, (void *&)pProfile );
	if( bExists )
	{
		ASSERT( pProfile != NULL );
		ASSERT( pProfile->m_sName == LPCTSTR(sProfileName) );
	} // if( bExists )
	else
		pProfile = NULL;
	m_cs.Unlock();
	return pProfile;
}

// get profile name for window
__EXT_MFC_SAFE_LPCTSTR CExtCmdManager::ProfileNameFromWnd(
	HWND hWnd
	)
{
	ASSERT( hWnd != NULL );
	if( hWnd == NULL )
		return NULL;
	ASSERT( ::IsWindow(hWnd) );
__EXT_MFC_SAFE_LPCTSTR sProfileName = NULL;
	m_cs.Lock();
	for(	;
			sProfileName == NULL && hWnd != NULL;
			hWnd = GetParent(hWnd) //::GetWindow(hWnd,GW_OWNER)
			)
	{
		CExtCmdProfile * pProfile = NULL;
		BOOL bExists = m_profile_wnds.Lookup( hWnd, pProfile );
		if( !bExists )
			continue;
		ASSERT( pProfile != NULL );
		ASSERT( !pProfile->m_sName.IsEmpty() );
		sProfileName = pProfile->m_sName;
		break;
	}
	m_cs.Unlock();
	return sProfileName;
}

// reset the toolbar/menu command statistics
bool CExtCmdManager::ProfileResetUsageStatistics(
	__EXT_MFC_SAFE_LPCTSTR sProfileName
	)
{
	m_cs.Lock();
bool bRetVal = false;
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile != NULL )
	{
		pProfile->ResetUsageStatistics();
		bRetVal = true;
	} // if( pProfile != NULL )
	m_cs.Unlock();
	return bRetVal;
}

// setup single command
bool CExtCmdManager::CmdSetup(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	const CExtCmdItem & _cmd,
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool * pbWasAddedNew // = NULL
	)
{
	if( pbWasAddedNew != NULL )
		*pbWasAddedNew = false;
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal = pProfile->CmdSetup(_cmd,bReplaceOld,pbWasAddedNew);
	m_cs.Unlock();
	return bRetVal;
}

// remove single command
bool CExtCmdManager::CmdRemove(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID,
	bool * pbWasRemoved // = NULL
	)
{
	if( pbWasRemoved != NULL )
		*pbWasRemoved = false;
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal = pProfile->CmdRemove(nCmdID,pbWasRemoved);
	m_cs.Unlock();
	return bRetVal;
}

void CExtCmdManager::CmdRemoveAll(
	__EXT_MFC_SAFE_LPCTSTR sProfileName
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	m_cs.Lock();
	pProfile->CmdRemoveAll();
	m_cs.Unlock();
}

bool CExtCmdManager::CmdRemoveByMask(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	DWORD dwMask,
	bool bAllBitsOnly // = false
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
	pProfile->CmdRemoveByMask( dwMask, bAllBitsOnly );
	m_cs.Unlock();
	return true;
}

// alloc command
CExtCmdItem * CExtCmdManager::CmdAllocPtr(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID // = 0 // 0 means any free in avail range
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	m_cs.Lock();
CExtCmdItem * pCmdItem = pProfile->CmdAllocPtr(nCmdID);
	m_cs.Unlock();
	return pCmdItem;
}

// get command
CExtCmdItem * CExtCmdManager::CmdGetPtr(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	m_cs.Lock();
CExtCmdItem * pCmdItem = pProfile->CmdGetPtr(nCmdID);
	m_cs.Unlock();
	return pCmdItem;
}

// assign icon to command

bool CExtCmdManager::CmdSetIcon(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID,
	const CExtCmdIcon * pCmdIcon, // if NULL or empty - remove
	bool bUseCmdIconObject
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal = pProfile->CmdSetIcon( nCmdID, pCmdIcon, bUseCmdIconObject );
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::CmdSetIcon(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID,
	const CExtCmdIcon & cmdIcon // if empty - remove
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->CmdSetIcon(
			nCmdID,
			cmdIcon
			);
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::CmdSetIcon(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID,
	HBITMAP hBitmap, // if NULL - remove
	COLORREF clrTransparent, // = RGB(0,0,0)
	LPCRECT pRectBitmapSrc // = NULL
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->CmdSetIcon(
			nCmdID,
			hBitmap,
			clrTransparent,
			pRectBitmapSrc
			);
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::CmdSetIcon(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID,
	HICON hIcon, // if NULL - remove
	bool bCopyIcon // = true
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->CmdSetIcon(
			nCmdID,
			hIcon,
			bCopyIcon
			);
	m_cs.Unlock();
	return bRetVal;
}

// get command icon (if command and its icon exist)
CExtCmdIcon * CExtCmdManager::CmdGetIconPtr(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	m_cs.Lock();
CExtCmdIcon * pIcon = pProfile->CmdGetIconPtr(nCmdID);
	m_cs.Unlock();
	return pIcon;
}

// is command registered
bool CExtCmdManager::CmdIsRegistered(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nCmdID
	)
{
	bool bRegistered =
		(CmdGetPtr(sProfileName,nCmdID) != NULL) ?
			true : false;
	return bRegistered;
}

// update commands collection from menu handle
bool CExtCmdManager::UpdateFromMenu(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	HMENU hMenu,
	bool bReplaceOld, // = false
	bool bRecursive, // = true
	bool bLoadTips // = true
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->UpdateFromMenu(
			hMenu, bReplaceOld, bRecursive, bLoadTips );
	m_cs.Unlock();
	return bRetVal;
}

// update commands collection from menu resurce
bool CExtCmdManager::UpdateFromMenu(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nResourceID,
	bool bReplaceOld, // = false
	bool bLoadTips // = true
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->UpdateFromMenu(
			nResourceID, bReplaceOld, bLoadTips );
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::UpdateFromToolBar(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	CToolBar & bar,
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool bLoadTips // = true
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->UpdateFromToolBar(
			bar, bReplaceOld, bLoadTips );
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::UpdateFromToolBar(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT nResourceID,
	LPUINT * ppCmdArray, // = NULL
	LPINT pCmdCount, // = NULL
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool bLoadTips, // = true
	COLORREF clrBmpTransparent // = RGB(192,192,192)
	)
{
	return
		UpdateFromToolBar(
			sProfileName,
			MAKEINTRESOURCE( nResourceID ),
			ppCmdArray,
			pCmdCount,
			bReplaceOld,
			bLoadTips,
			clrBmpTransparent
			);
}

bool CExtCmdManager::UpdateFromToolBar(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR strResourceID,
	LPUINT * ppCmdArray, // = NULL
	LPINT pCmdCount, // = NULL
	bool bReplaceOld, // = false // but force set images anywhere if was empty
	bool bLoadTips, // = true
	COLORREF clrBmpTransparent // = RGB(192,192,192)
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->UpdateFromToolBar(
			strResourceID,
			ppCmdArray,
			pCmdCount,
			bReplaceOld,
			bLoadTips,
			clrBmpTransparent
			);
	m_cs.Unlock();
	return bRetVal;
}


// set list of commands (up to (UINT)0) as basic or non basic
bool CExtCmdManager::SetBasicCommands(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	UINT * pCommands,
	bool bOn // = true
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->SetBasicCommands( pCommands, bOn );
	m_cs.Unlock();
	return bRetVal;
}

#define __REG_LINES_IN_BLOCK	128
#define __REG_LINE_SIZE			16
#define __REG_LINE_FMT			_T("data_0x%08lX")
#define __REG_BLOCK_FMT			_T("block_0x%08lX")
#define __REG_VAR_DATA_SIZE		_T("data_size")
#define __REG_VAR_DATA_CHECK	_T("data_integrity")
#define __REG_VAR_GENERATOR		_T("data_generator")
#define __REG_FMT_GENERATOR		_T("Prof-UIS v. %s registry archiver")

CExtSafeString CExtCmdManager::GetSubSystemRegKeyPath(
	__EXT_MFC_SAFE_LPCTSTR sSubSystemName,
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	ASSERT( sSubSystemName != NULL );
	ASSERT( sProfileName != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
CExtSafeString s;
	s.Format(
		_T("Software\\%s\\%s\\%s\\%s\\%s\\%s"),
		sSectionNameCompany,
		sSectionNameProduct,
		__PROF_UIS_REG_SECTION,
		__PROF_UIS_REG_PROFILES,
		sProfileName,
		sSubSystemName
		);
//	s.Replace(' ','_');
	s.Replace('\r','_');
	s.Replace('\t','_');
	s.Replace('\n','_');
	s.Replace('?','_');
	s.Replace('*','_');
//	s.Replace('.','_'); // (- v.2.23)
//	s.Replace(',','_'); // (- v.2.23)
	return s;
}

bool CExtCmdManager::FileObjToRegistry(
	CFile & _file,
	__EXT_MFC_SAFE_LPCTSTR sRegKeyPath,
	HKEY hKeyRoot // = HKEY_CURRENT_USER
	)
{
	ASSERT( sRegKeyPath != NULL );
	if( sRegKeyPath == NULL )
		return false;
	CExtRegistry::RegDeleteKey(
		hKeyRoot, // HKEY_CURRENT_USER
		sRegKeyPath,
		NULL
		);
	_file.Seek( 0, CFile::begin );
DWORD dwLen = (DWORD)_file.GetLength();
CExtRegistry reg;
	if( !reg.Create(
			hKeyRoot, // HKEY_CURRENT_USER
			sRegKeyPath,
			KEY_ALL_ACCESS
			)
		)
		return false;
	if(	!reg.SaveNumber(
			__REG_VAR_DATA_SIZE,
			dwLen
			)
		)
		return false;
BYTE buffer[__REG_LINE_SIZE];
ULONG nCount, nPortion=0;
CExtIntegrityCheckSum _ExtIntegrityCheckSum;
	for( ; (nCount=_file.Read(&buffer,__REG_LINE_SIZE)) > 0;  )
	{
		CExtSafeString sBlockSubKey;
		ULONG nBlockNo = nPortion/__REG_LINES_IN_BLOCK;
		sBlockSubKey.Format( __REG_BLOCK_FMT, nBlockNo );
		CExtSafeString sRegKeyPath2( sRegKeyPath );
		sRegKeyPath2 += _T('\\');
		sRegKeyPath2 += sBlockSubKey;
		CExtRegistry reg;
		if( !reg.Create(
				hKeyRoot, // HKEY_CURRENT_USER
				sRegKeyPath2,
				KEY_ALL_ACCESS
				)
			)
			return false;

		CExtSafeString sVarName;
		sVarName.Format(
			__REG_LINE_FMT,
			nPortion++
			);
		if(	!reg.SaveBinary(
				sVarName,
				buffer,
				nCount
				)
			)
			return false;
		_ExtIntegrityCheckSum.Update(
			buffer,
			nCount
			);
	}
USES_CONVERSION;
CExtSafeString sExtIntegrityCheckSum = _ExtIntegrityCheckSum.Final();
	ASSERT( !sExtIntegrityCheckSum.IsEmpty() );
	if(	!reg.SaveString(
			__REG_VAR_DATA_CHECK,
			sExtIntegrityCheckSum
			)
		)
		return false;
	
CExtSafeString sTmpBuffer;
CExtSafeString sGenerator;
	sGenerator.Format(
		__REG_FMT_GENERATOR,
		g_CmdManager.GetVersionString( sTmpBuffer )
		);
	VERIFY(
		reg.SaveString(
			__REG_VAR_GENERATOR,
			sGenerator
			)
		);

	return true;
}

bool CExtCmdManager::FileObjFromRegistry(
	CFile & _file,
	__EXT_MFC_SAFE_LPCTSTR sRegKeyPath,
	HKEY hKeyRoot // = HKEY_CURRENT_USER
	)
{
	ASSERT( sRegKeyPath != NULL );
	if( sRegKeyPath == NULL )
		return false;
CExtRegistry reg;
	if( !reg.Open(
			hKeyRoot, // HKEY_CURRENT_USER
			sRegKeyPath,
			KEY_READ
			)
		)
		return false;
DWORD dwLen = 0;
	if(	!reg.LoadNumber(
			__REG_VAR_DATA_SIZE,
			&dwLen
			)
		)
		return false;
	if( dwLen == 0 )
		return true;
BYTE buffer[__REG_LINE_SIZE];
ULONG nCount = __REG_LINE_SIZE, nPortion = 0;
DWORD dwLoaded = 0;
CExtIntegrityCheckSum _ExtIntegrityCheckSum;
	for( ; true;  )
	{
		CExtSafeString sBlockSubKey;
		ULONG nBlockNo = nPortion/__REG_LINES_IN_BLOCK;
		sBlockSubKey.Format( __REG_BLOCK_FMT, nBlockNo );
		CExtSafeString sRegKeyPath2( sRegKeyPath );
		sRegKeyPath2 += _T('\\');
		sRegKeyPath2 += sBlockSubKey;
		CExtRegistry reg;
		if( !reg.Open(
				hKeyRoot, // HKEY_CURRENT_USER
				sRegKeyPath2,
				KEY_READ
				)
			)
			return false;
	
		CExtSafeString sVarName;
		sVarName.Format(
			__REG_LINE_FMT,
			nPortion++
			);
		if( dwLen-dwLoaded < __REG_LINE_SIZE )
			nCount = dwLen-dwLoaded;
		ASSERT( nCount > 0 && nCount <= __REG_LINE_SIZE );
		if(	!reg.LoadBinary(
				sVarName,
				buffer,
				nCount
				)
			)
			return false;
		dwLoaded += nCount;
		ASSERT( dwLoaded <= dwLen );

		_ExtIntegrityCheckSum.Update(
			buffer,
			nCount
			);
		_file.Write(
			buffer,
			nCount
			);

		if( dwLoaded == dwLen )
			break;
	}
USES_CONVERSION;
CExtSafeString sExtIntegrityCheckSum = _ExtIntegrityCheckSum.Final();
	ASSERT( !sExtIntegrityCheckSum.IsEmpty() );

CExtSafeString sExtIntegrityCheckSumA;
int nSz = sExtIntegrityCheckSum.GetLength() + 1;
	if(	!reg.LoadString(
			__REG_VAR_DATA_CHECK,
			sExtIntegrityCheckSumA.GetBuffer( nSz ),
			nSz
			)
		)
	{
		sExtIntegrityCheckSumA.ReleaseBuffer();
		return false;
	}
	sExtIntegrityCheckSumA.ReleaseBuffer();

	if( sExtIntegrityCheckSumA != sExtIntegrityCheckSum )
	{
		ASSERT( FALSE );
		return false;
	}

	_file.Seek( 0, CFile::begin );

	return true;
}

// save/load command manager state
bool CExtCmdManager::SerializeState(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	bool bSave
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->SerializeState(
			sSectionNameCompany,
			sSectionNameProduct,
			bSave
			);
	m_cs.Unlock();
	return bRetVal;
}

bool CExtCmdManager::SerializeState(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	CArchive & ar
	)
{
CExtCmdProfile * pProfile = ProfileGetPtr( sProfileName );
	if( pProfile == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_cs.Lock();
bool bRetVal =
		pProfile->SerializeState( ar );
	m_cs.Unlock();
	return bRetVal;
}

void CExtCmdManager::OnSysColorChange()
{
	m_cs.Lock();
POSITION pos = m_profiles.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtSafeString sProfileName;
		CExtCmdProfile * pProfile = NULL;
		m_profiles.GetNextAssoc( pos, sProfileName, (void *&)pProfile );
		ASSERT( pProfile != NULL );
		pProfile->OnSysColorChange();
	} // for( ; pos != NULL; )
	m_cs.Unlock();
}

void CExtCmdManager::OnSettingChange(
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	m_cs.Lock();
POSITION pos = m_profiles.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtSafeString sProfileName;
		CExtCmdProfile * pProfile = NULL;
		m_profiles.GetNextAssoc( pos, sProfileName, (void *&)pProfile );
		ASSERT( pProfile != NULL );
		pProfile->OnSettingChange( uFlags, lpszSection );
	} // for( ; pos != NULL; )
	m_cs.Unlock();
}

void CExtCmdManager::OnDisplayChange(
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	m_cs.Lock();
POSITION pos = m_profiles.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtSafeString sProfileName;
		CExtCmdProfile * pProfile = NULL;
		m_profiles.GetNextAssoc( pos, sProfileName, (void *&)pProfile );
		ASSERT( pProfile != NULL );
		pProfile->OnDisplayChange( nDepthBPP, ptSizes );
	} // for( ; pos != NULL; )
	m_cs.Unlock();
}

void CExtCmdManager::OnThemeChanged(
	WPARAM wParam,
	LPARAM lParam
	)
{
	m_cs.Lock();
POSITION pos = m_profiles.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtSafeString sProfileName;
		CExtCmdProfile * pProfile = NULL;
		m_profiles.GetNextAssoc( pos, sProfileName, (void *&)pProfile );
		ASSERT( pProfile != NULL );
		pProfile->OnThemeChanged( wParam, lParam );
	} // for( ; pos != NULL; )
	m_cs.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CExtResourceManager
// resource manager class

CExtResourceManager::CExtResourceManagerAutoPtr g_ResourceManager;

WORD CExtResourceManager::g_nLangIdNeutral = __EXT_MFC_LANG_ID_DEFAULT_NEUTRAL;

CExtResourceManager::CExtResourceManagerAutoPtr::CExtResourceManagerAutoPtr()
	: m_pResourceManager( NULL )
{
}

CExtResourceManager::CExtResourceManagerAutoPtr::~CExtResourceManagerAutoPtr()
{
	if( m_pResourceManager != NULL )
		delete m_pResourceManager;
}

// resource manager instance access
CExtResourceManager * CExtResourceManager::CExtResourceManagerAutoPtr::operator->()
{
	if( m_pResourceManager != NULL )
		return m_pResourceManager;
	InstallResourceManager();
	return m_pResourceManager;
}

void CExtResourceManager::CExtResourceManagerAutoPtr::InstallResourceManager(
	CExtResourceManager * pNewResourceManager // = NULL // NULL means install default resource manager
	)
{
	if( m_pResourceManager != NULL )
		delete m_pResourceManager;
	if( pNewResourceManager == NULL )
		m_pResourceManager = new CExtResourceManager;
	else
		m_pResourceManager = pNewResourceManager;
}

CExtResourceManager::CExtResourceMemoryBuffer::CExtResourceMemoryBuffer()
	: m_pBuffer( NULL )
	, m_dwBufferSize( 0 )
{
}

CExtResourceManager::CExtResourceMemoryBuffer::CExtResourceMemoryBuffer(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	)
	: m_pBuffer( NULL )
	, m_dwBufferSize( 0 )
{
	AssignFromOther( other );
}

CExtResourceManager::CExtResourceMemoryBuffer::~CExtResourceMemoryBuffer()
{
	Empty();
}

#ifdef _DEBUG
void CExtResourceManager::CExtResourceMemoryBuffer::Dump()
{
	for( DWORD i = 0; i < m_dwBufferSize; i++ )
	{
		TCHAR _tchr[2] = { m_pBuffer[ i ], _T('\0') };
		afxDump << _tchr;
		afxDump.Flush();
	}
}
#endif // _DEBUG

CExtResourceManager::CExtResourceMemoryBuffer &
	CExtResourceManager::CExtResourceMemoryBuffer::operator =(
		const CExtResourceManager::CExtResourceMemoryBuffer & other
		)
{
	AssignFromOther( other );
	return (*this);
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator ! () const
{
	return (! IsEmpty() );
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator ==(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) == 0 ) ? true : false;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator !=(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) != 0 ) ? true : false;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator <(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) < 0 ) ? true : false;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator <=(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) <= 0 ) ? true : false;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator >(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) > 0 ) ? true : false;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::operator >=(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	return ( CompareWithOther( other ) >= 0 ) ? true : false;
}

BYTE CExtResourceManager::CExtResourceMemoryBuffer::operator [] ( DWORD dwByteIndex ) const
{
	ASSERT( m_pBuffer != NULL && dwByteIndex < m_dwBufferSize );
	return m_pBuffer[ dwByteIndex ];
}

CExtResourceManager::CExtResourceMemoryBuffer::operator BYTE * ()
{
	return m_pBuffer;
}

CExtResourceManager::CExtResourceMemoryBuffer::operator const BYTE * ()
{
	return m_pBuffer;
}

void CExtResourceManager::CExtResourceMemoryBuffer::AssignFromOther(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	)
{
	if( LPVOID(this) == LPVOID(&other) )
		return;
	Empty();
	if( other.IsEmpty() )
		return;
DWORD dwBufferSize = other.GetSize();
	if( ! Alloc( dwBufferSize, false ) )
		return;
	::memcpy( GetBuffer(), other.GetBuffer(), dwBufferSize );
}

int CExtResourceManager::CExtResourceMemoryBuffer::CompareWithOther(
	const CExtResourceManager::CExtResourceMemoryBuffer & other
	) const
{
	if( IsEmpty() )
	{
		if( other.IsEmpty() )
			return 0;
		return -1;
	} // if( IsEmpty() )
	else
	{
		if( other.IsEmpty() )
			return 1;
		DWORD dwSizeThis = GetSize();
		DWORD dwSizeOther = other.GetSize();
		DWORD dwCmpSize = min( dwSizeThis, dwSizeOther );
		ASSERT( dwCmpSize > 0 );
		int nCmpResult = ::memcmp( GetBuffer(), other.GetBuffer(), dwCmpSize );
		if( nCmpResult != 0 )
			return nCmpResult;
		if( dwSizeThis == dwSizeOther )
			return 0;
		if( dwSizeThis < dwSizeOther )
			return -1;
		return 1;
	} // else from if( IsEmpty() )
}

DWORD CExtResourceManager::CExtResourceMemoryBuffer::GetSize() const
{
	return m_dwBufferSize;
}

BYTE * CExtResourceManager::CExtResourceMemoryBuffer::GetBuffer()
{
	return m_pBuffer;
}

const BYTE * CExtResourceManager::CExtResourceMemoryBuffer::GetBuffer() const
{
	return m_pBuffer;
}

HGLOBAL CExtResourceManager::CExtResourceMemoryBuffer::AllocGlobalBufferBuffer(
	UINT nFlags // = GPTR
	) const
{
	if( IsEmpty() )
		return NULL;
HGLOBAL hGlobal = ::GlobalAlloc( nFlags, GetSize() );
	ASSERT( hGlobal != NULL );
	if( hGlobal == NULL )
		return NULL;
LPVOID pMem = ::GlobalLock( hGlobal );
	ASSERT( pMem != NULL );
	::memcpy( pMem, GetBuffer(), GetSize() );
	::GlobalUnlock( hGlobal );
	return hGlobal;
}


bool CExtResourceManager::CExtResourceMemoryBuffer::IsEmpty() const
{
	return (m_dwBufferSize == 0) ? true : false;
}

void CExtResourceManager::CExtResourceMemoryBuffer::Empty()
{
	if( m_pBuffer != NULL )
	{
		ASSERT( m_dwBufferSize > 0 );
		::free( m_pBuffer );
		m_pBuffer = NULL;
		m_dwBufferSize = 0;
	}
#ifdef _DEBUG
	else
	{
		ASSERT( m_dwBufferSize == 0 );
	}
#endif // _DEBUG
}

bool CExtResourceManager::CExtResourceMemoryBuffer::Alloc(
	DWORD dwBufferSize,
	bool bZeroMemory // = true
	)
{
	ASSERT( dwBufferSize > 0 );
	if( dwBufferSize == 0 )
		return false;
	Empty();
	m_pBuffer = (BYTE *)::malloc( dwBufferSize );
	if( m_pBuffer == NULL )
		return false;
	m_dwBufferSize = dwBufferSize;
	if( bZeroMemory )
		::memset( m_pBuffer, 0, m_dwBufferSize );
	return true;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::Load(
	HINSTANCE hModule,
	HRSRC hResource
	)
{
HGLOBAL hGlobal = LoadResource( hModule, hResource );
	if( hGlobal == NULL )
		return false;
LPVOID pResourceData = LockResource( hGlobal );
bool bRetVal = false;
	if( pResourceData != NULL )
	{
		DWORD dwSizeofResource = SizeofResource( hModule, hResource );
		if( dwSizeofResource == 0 )
		{
			Empty();
			bRetVal = true;
		} // if( dwSizeofResource == 0 )
		else if( Alloc( dwSizeofResource ) )
		{
			::memcpy(
				m_pBuffer,
				pResourceData,
				dwSizeofResource
				);
			bRetVal = true;
		} // else if( Alloc( dwSizeofResource ) )
		UnlockResource( hGlobal );
	} // if( pResourceData != NULL )
	FreeResource( hGlobal );
	return bRetVal;
}

bool CExtResourceManager::CExtResourceMemoryBuffer::FindAndLoad(
	HINSTANCE hModule,
    __EXT_MFC_SAFE_LPCTSTR strResourceType,
    UINT nResourceName,
	WORD nLangIdDesired
	)
{
	Empty();
HRSRC hResource =
		::FindResourceEx(
			hModule,
			strResourceType,
			MAKEINTRESOURCE(nResourceName),
			nLangIdDesired
			);
	if( hResource == NULL )
		return false;
	return Load( hModule, hResource );
}

CExtResourceManager::CExtResourceManager(
	bool bAllowCustomLang, // = false
	WORD nLangIdDesired // = __EXT_MFC_LANG_ID_DEFAULT_NEUTRAL
	)
	: m_bAllowCustomLang( bAllowCustomLang )
	, m_nLangIdDesired( nLangIdDesired )
{
}

CExtResourceManager::~CExtResourceManager()
{
}

bool CExtResourceManager::IsCustomLangAllowed() const
{
	return m_bAllowCustomLang;
}

WORD CExtResourceManager::GetLangIdDesired() const
{
	return m_nLangIdDesired;
}

WORD CExtResourceManager::GetLangIdNeutral() const
{
	return g_nLangIdNeutral;
}

int	CExtResourceManager::GetLocaleInfo(
	LCTYPE LCType,
	__EXT_MFC_SAFE_LPTSTR lpLCData,
	int cchData
	)
{
int nRetVal;
	if( IsCustomLangAllowed() )
	{
		nRetVal =
			::GetLocaleInfo(
				( MAKELCID( GetLangIdDesired(), SORT_DEFAULT ) ),
				LCType,
				lpLCData,
				cchData
				);
		if( nRetVal != 0 )
			return nRetVal;
		nRetVal =
			::GetLocaleInfo(
				( MAKELCID( GetLangIdNeutral(), SORT_DEFAULT ) ),
				LCType,
				lpLCData,
				cchData
				);
		if( nRetVal != 0 )
			return nRetVal;
	} // if( IsCustomLangAllowed() )
	nRetVal =
		::GetLocaleInfo(
			LOCALE_SYSTEM_DEFAULT,
			LCType,
			lpLCData,
			cchData
			);
	if( nRetVal != 0 )
		return nRetVal;
	nRetVal =
		::GetLocaleInfo(
			LOCALE_USER_DEFAULT,
			LCType,
			lpLCData,
			cchData
			);
	if( nRetVal != 0 )
		return nRetVal;
	nRetVal =
		::GetLocaleInfo(
			LOCALE_NEUTRAL,
			LCType,
			lpLCData,
			cchData
			);
	return nRetVal;
}

HINSTANCE CExtResourceManager::FindResourceHandleSingleLanguage(
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD * p_wLangIdOut, // = NULL
	HRSRC * p_hResourceOut // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT( strResourceType != NULL );
	if( p_wLangIdOut != NULL )
		(*p_wLangIdOut) = GetLangIdNeutral();
	if( p_hResourceOut != NULL )
		(*p_hResourceOut) = NULL;
	if( ! IsCustomLangAllowed() )
	{
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hInst == NULL )
			return NULL;
		HRSRC hResource = 
			::FindResource(
				hInst,
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hResource == NULL )
			return NULL;
		if( p_hResourceOut != NULL )
			(*p_hResourceOut) = hResource;
		return hInst;
	} // if( ! IsCustomLangAllowed() )

HINSTANCE hInstance = NULL;
AFX_MODULE_STATE * pModuleState = ::AfxGetModuleState();
	if( ! pModuleState->m_bSystem )
	{
		hInstance = AfxGetResourceHandle();
		HRSRC hResource =
			::FindResourceEx(
				hInstance,
				LPCTSTR(strResourceType),
				LPCTSTR(nResourceName),
				nLangIdDesired
				);
		if( hResource != NULL )
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hResourceOut != NULL )
				(*p_hResourceOut) = hResource;
			return hInstance;
		} // if( hResource != NULL )
	} // if( ! pModuleState->m_bSystem )

#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	AfxLockGlobals( CRIT_DYNLINKLIST );
	for(	CDynLinkLibrary * pDLL = pModuleState->m_libraryList;
			pDLL != NULL;
			pDLL = pDLL->m_pNextDLL
			)
	{
		if(		(! pDLL->m_bSystem)
			&&	pDLL->m_hResource != NULL
			)
		{
			HRSRC hResource =
				::FindResourceEx(
					pDLL->m_hResource,
					LPCTSTR(strResourceType),
					LPCTSTR(nResourceName),
					nLangIdDesired
					);
			if( hResource != NULL )
			{
				if( p_wLangIdOut != NULL )
					(*p_wLangIdOut) = nLangIdDesired;
				if( p_hResourceOut != NULL )
					(*p_hResourceOut) = hResource;
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return pDLL->m_hResource;
			} // if( hResource != NULL )
		}
	}
	AfxUnlockGlobals( CRIT_DYNLINKLIST );
	hInstance = pModuleState->m_appLangDLL;
	if( hInstance != NULL )
	{
		HRSRC hResource =
			::FindResourceEx(
				hInstance,
				LPCTSTR(strResourceType),
				LPCTSTR(nResourceName),
				nLangIdDesired
				);
		if( hResource != NULL )
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hResourceOut != NULL )
				(*p_hResourceOut) = hResource;
			return hInstance;
		} // if( hResource != NULL )
	}
#endif // (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

	if( pModuleState->m_bSystem )
	{
		hInstance = AfxGetResourceHandle();
		HRSRC hResource =
			::FindResourceEx(
				hInstance,
				LPCTSTR(strResourceType),
				LPCTSTR(nResourceName),
				nLangIdDesired
				);
		if( hResource != NULL )
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hResourceOut != NULL )
				(*p_hResourceOut) = hResource;
			return hInstance;
		} // if( hResource != NULL )
	} // if( pModuleState->m_bSystem )

#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	AfxLockGlobals(CRIT_DYNLINKLIST);
	for(	pDLL = pModuleState->m_libraryList;
			pDLL != NULL;
			pDLL = pDLL->m_pNextDLL
			)
	{
		if(		pDLL->m_bSystem
			&&	pDLL->m_hResource != NULL
			)
		{
			HRSRC hResource =
				::FindResourceEx(
					pDLL->m_hResource,
					LPCTSTR(strResourceType),
					LPCTSTR(nResourceName),
					nLangIdDesired
					);
			if( hResource != NULL )
			{
				if( p_wLangIdOut != NULL )
					(*p_wLangIdOut) = nLangIdDesired;
				if( p_hResourceOut != NULL )
					(*p_hResourceOut) = hResource;
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return pDLL->m_hResource;
			} // if( hResource != NULL )
		}
	}
	AfxUnlockGlobals( CRIT_DYNLINKLIST );
#endif // (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

	hInstance = AfxGetResourceHandle();
HRSRC hResource =
		::FindResourceEx(
			hInstance,
			LPCTSTR(strResourceType),
			LPCTSTR(nResourceName),
			nLangIdDesired
			);
	if( hResource != NULL )
	{
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = nLangIdDesired;
		if( p_hResourceOut != NULL )
			(*p_hResourceOut) = hResource;
		return hInstance;
	} // if( hResource != NULL )
	return NULL;
}

HINSTANCE CExtResourceManager::FindResourceHandleEx(
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD nLangIdNeutral, // = CExtResourceManager::g_nLangIdNeutral
	WORD * p_wLangIdOut, // = NULL
	HRSRC * p_hResourceOut // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT( strResourceType != NULL );
	if( p_wLangIdOut != NULL )
		(*p_wLangIdOut) = GetLangIdNeutral();
	if( p_hResourceOut != NULL )
		(*p_hResourceOut) = NULL;
	if( ! IsCustomLangAllowed() )
	{
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hInst == NULL )
			return NULL;
		HRSRC hResource = 
			::FindResource(
				hInst,
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hResource == NULL )
			return NULL;
		if( p_hResourceOut != NULL )
			(*p_hResourceOut) = hResource;
		return hInst;
	} // if( ! IsCustomLangAllowed() )
HINSTANCE hInst =
		FindResourceHandleSingleLanguage(
			strResourceType,
			nResourceName,
			nLangIdDesired,
			p_wLangIdOut,
			p_hResourceOut
			);
	if( hInst != NULL )
		return hInst;
	if( nLangIdDesired == nLangIdNeutral )
		return NULL;
	hInst = 
		FindResourceHandleSingleLanguage(
			strResourceType,
			nResourceName,
			nLangIdNeutral,
			p_wLangIdOut,
			p_hResourceOut
			);
	return hInst;
}

HINSTANCE CExtResourceManager::FindResourceHandle(
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD * p_wLangIdOut, // = NULL
	HRSRC * p_hResourceOut // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT( strResourceType != NULL );
	if( p_wLangIdOut != NULL )
		(*p_wLangIdOut) = GetLangIdNeutral();
	if( p_hResourceOut != NULL )
		(*p_hResourceOut) = NULL;
	if( ! IsCustomLangAllowed() )
	{
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hInst == NULL )
			return NULL;
		HRSRC hResource = 
			::FindResource(
				hInst,
				LPCTSTR(nResourceName),
				LPCTSTR(strResourceType)
				);
		if( hResource == NULL )
			return NULL;
		if( p_hResourceOut != NULL )
			(*p_hResourceOut) = hResource;
		return hInst;
	} // if( ! IsCustomLangAllowed() )
	return
		FindResourceHandleEx(
			strResourceType,
			nResourceName,
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hResourceOut
			);
}

bool CExtResourceManager::LoadResourceBufferSingleLanguage(
	CExtResourceManager::CExtResourceMemoryBuffer & bufferOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	
	if( ! IsCustomLangAllowed() )
	{
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				strResourceType
				);
		if( p_hInstOut != NULL )
			(*p_hInstOut) = hInst;
		if( hInst == NULL )
			return false;
		bufferOut.Empty();
		return
			bufferOut.FindAndLoad(
				hInst,
				strResourceType,
				nResourceName,
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT)
				);
	} // if( ! IsCustomLangAllowed() )

	ASSERT( strResourceType != NULL );
	if( p_wLangIdOut != NULL )
		(*p_wLangIdOut) = GetLangIdNeutral();
	if( p_hInstOut != NULL )
		(*p_hInstOut) = NULL;
HINSTANCE hInstance = NULL;
AFX_MODULE_STATE * pModuleState = ::AfxGetModuleState();
	if( ! pModuleState->m_bSystem )
	{
		hInstance = AfxGetResourceHandle();
		if( bufferOut.FindAndLoad(
				hInstance,
				strResourceType,
				nResourceName,
				nLangIdDesired
				)
			)
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hInstOut != NULL )
				(*p_hInstOut) = hInstance;
			return true;
		}
	} // if( ! pModuleState->m_bSystem )

#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	AfxLockGlobals( CRIT_DYNLINKLIST );
	for(	CDynLinkLibrary * pDLL = pModuleState->m_libraryList;
			pDLL != NULL;
			pDLL = pDLL->m_pNextDLL
			)
	{
		if(		(! pDLL->m_bSystem)
			&&	pDLL->m_hResource != NULL
			)
		{
			if( bufferOut.FindAndLoad(
					pDLL->m_hResource,
					strResourceType,
					nResourceName,
					nLangIdDesired
					)
				)
			{
				if( p_wLangIdOut != NULL )
					(*p_wLangIdOut) = nLangIdDesired;
				if( p_hInstOut != NULL )
					(*p_hInstOut) = pDLL->m_hResource;
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return true;
			}
		}
	}
	AfxUnlockGlobals( CRIT_DYNLINKLIST );
	hInstance = pModuleState->m_appLangDLL;
	if( hInstance != NULL )
	{
		if( bufferOut.FindAndLoad(
				hInstance,
				strResourceType,
				nResourceName,
				nLangIdDesired
				)
			)
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hInstOut != NULL )
				(*p_hInstOut) = hInstance;
			return true;
		}
	}
#endif // (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

	if( pModuleState->m_bSystem )
	{
		hInstance = AfxGetResourceHandle();
		if( bufferOut.FindAndLoad(
				hInstance,
				strResourceType,
				nResourceName,
				nLangIdDesired
				)
			)
		{
			if( p_wLangIdOut != NULL )
				(*p_wLangIdOut) = nLangIdDesired;
			if( p_hInstOut != NULL )
				(*p_hInstOut) = hInstance;
			return true;
		}
	} // if( pModuleState->m_bSystem )

#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	AfxLockGlobals(CRIT_DYNLINKLIST);
	for(	pDLL = pModuleState->m_libraryList;
			pDLL != NULL;
			pDLL = pDLL->m_pNextDLL
			)
	{
		if(		pDLL->m_bSystem
			&&	pDLL->m_hResource != NULL
			)
		{
			if( bufferOut.FindAndLoad(
					pDLL->m_hResource,
					strResourceType,
					nResourceName,
					nLangIdDesired
					)
				)
			{
				if( p_wLangIdOut != NULL )
					(*p_wLangIdOut) = nLangIdDesired;
				if( p_hInstOut != NULL )
					(*p_hInstOut) = pDLL->m_hResource;
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return true;
			}
		}
	}
	AfxUnlockGlobals( CRIT_DYNLINKLIST );
#endif // (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

	hInstance = AfxGetResourceHandle();
	if(	bufferOut.FindAndLoad(
			hInstance,
			strResourceType,
			nResourceName,
			nLangIdDesired
			)
		)
	{
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = nLangIdDesired;
		if( p_hInstOut != NULL )
			(*p_hInstOut) = hInstance;
		return true;
	}
	return false;
}

bool CExtResourceManager::LoadResourceBufferEx(
	CExtResourceManager::CExtResourceMemoryBuffer & bufferOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD nLangIdNeutral, // = CExtResourceManager::g_nLangIdNeutral
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	if( p_wLangIdOut != NULL )
		(*p_wLangIdOut) = GetLangIdNeutral();
	if( p_hInstOut != NULL )
		(*p_hInstOut) = NULL;
	if( ! IsCustomLangAllowed() )
	{
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				strResourceType
				);
		if( p_hInstOut != NULL )
			(*p_hInstOut) = hInst;
		if( hInst == NULL )
			return false;
		bufferOut.Empty();
		return
			bufferOut.FindAndLoad(
				hInst,
				strResourceType,
				nResourceName,
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT)
				);
	} // if( ! IsCustomLangAllowed() )
	if(	LoadResourceBufferSingleLanguage(
			bufferOut,
			strResourceType,
			nResourceName,
			nLangIdDesired,
			p_wLangIdOut,
			p_hInstOut
			)
		)
		return true;
	if( nLangIdDesired == nLangIdNeutral )
		return false;
	return
		LoadResourceBufferSingleLanguage(
			bufferOut,
			strResourceType,
			nResourceName,
			nLangIdNeutral,
			p_wLangIdOut,
			p_hInstOut
			);
}

bool CExtResourceManager::LoadResourceBuffer(
	CExtResourceManager::CExtResourceMemoryBuffer & bufferOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceType,
	UINT nResourceName,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		HINSTANCE hInst =
			::AfxFindResourceHandle(
				LPCTSTR(nResourceName),
				strResourceType
				);
		if( p_hInstOut != NULL )
			(*p_hInstOut) = hInst;
		if( hInst == NULL )
			return false;
		bufferOut.Empty();
		return
			bufferOut.FindAndLoad(
				hInst,
				strResourceType,
				nResourceName,
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT)
				);
	} // if( ! IsCustomLangAllowed() )

	return
		LoadResourceBufferEx(
			bufferOut,
			strResourceType,
			nResourceName,
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hInstOut
			);
}

bool CExtResourceManager::LoadStringEx(
	CExtSafeString & strOut,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD nLangIdNeutral, // = CExtResourceManager::g_nLangIdNeutral
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		return strOut.LoadString( nResourceName ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
	strOut.Empty();
CExtResourceMemoryBuffer bufferOut;
	if( ! LoadResourceBufferEx(
			bufferOut,
			RT_STRING,
			(nResourceName>>4)+1,
			nLangIdDesired,
			nLangIdNeutral,
			p_wLangIdOut,
			p_hInstOut
			)
		)
	{
		//ASSERT( FALSE );
		return false;
	}
	ASSERT( ! bufferOut.IsEmpty() );
	ASSERT( bufferOut.GetBuffer() > 0 );
LPWORD pSource = (LPWORD)bufferOut.GetBuffer();
	ASSERT( pSource != NULL );
UINT nTableEntryIdx = nResourceName&0x0F;
	for( UINT nIdx = 0; nIdx < nTableEntryIdx; nIdx++ )
	{
		WORD wStrLen = pSource[0];
		pSource += wStrLen + 1;
	} // for( UINT nIdx = 0; nIdx < nTableEntryIdx; nIdx++ )
WORD wStrLen = pSource[0];
	if( wStrLen == 0 )
		return true;
	pSource ++;
CExtSafeString strTemp(
		((wchar_t*)(pSource)),
		wStrLen
		);
	strOut = strTemp;
	return true;
}

bool CExtResourceManager::LoadString(
	CExtSafeString & strOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceName,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		return
			strOut.LoadString( UINT(strResourceName) ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
	return
		LoadStringEx(
			strOut,
			UINT(strResourceName),
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hInstOut
			);
}

bool CExtResourceManager::LoadString(
	CExtSafeString & strOut,
	UINT nResourceName,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		return strOut.LoadString( nResourceName ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
	return
		LoadStringEx(
			strOut,
			nResourceName,
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hInstOut
			);
}

bool CExtResourceManager::LoadMenuEx(
	CMenu & menuOut,
	UINT nResourceName,
	WORD nLangIdDesired,
	WORD nLangIdNeutral, // = CExtResourceManager::g_nLangIdNeutral
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( menuOut.GetSafeHmenu() != NULL )
		menuOut.DestroyMenu();
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		return menuOut.LoadMenu( nResourceName ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
CExtResourceMemoryBuffer bufferOut;
	if( ! LoadResourceBufferEx(
			bufferOut,
			RT_MENU,
			nResourceName,
			nLangIdDesired,
			nLangIdNeutral,
			p_wLangIdOut,
			p_hInstOut
			)
		)
	{
		//ASSERT( FALSE );
		return false;
	}
	ASSERT( ! bufferOut.IsEmpty() );
	ASSERT( bufferOut.GetBuffer() > 0 );
LPMENUTEMPLATE pTemplate = LPMENUTEMPLATE( bufferOut.GetBuffer() );
HMENU hMenu = ::LoadMenuIndirect( pTemplate );
	if( hMenu == NULL )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	menuOut.Attach( hMenu );
	return true;
}

bool CExtResourceManager::LoadMenu(
	CMenu & menuOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceName,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		if( menuOut.GetSafeHmenu() != NULL )
			menuOut.DestroyMenu();
		return menuOut.LoadMenu( strResourceName ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
	return
		LoadMenuEx(
			menuOut,
			UINT(strResourceName),
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hInstOut
			);
}

bool CExtResourceManager::LoadMenu(
	CMenu & menuOut,
	UINT nResourceName,
	WORD * p_wLangIdOut, // = NULL
	HINSTANCE * p_hInstOut // = NULL
	)
{
	if( ! IsCustomLangAllowed() )
	{
		__PROF_UIS_MANAGE_STATE;
		if( p_wLangIdOut != NULL )
			(*p_wLangIdOut) = GetLangIdNeutral();
		if( p_hInstOut != NULL )
			(*p_hInstOut) = ::AfxGetResourceHandle();
		if( menuOut.GetSafeHmenu() != NULL )
			menuOut.DestroyMenu();
		return menuOut.LoadMenu( nResourceName ) ? true : false;
	} // if( ! IsCustomLangAllowed() )
	return
		LoadMenuEx(
			menuOut,
			nResourceName,
			GetLangIdDesired(),
			GetLangIdNeutral(),
			p_wLangIdOut,
			p_hInstOut
			);
}

/////////////////////////////////////////////////////////////////////////////
// CExtResDlg window

IMPLEMENT_DYNCREATE( CExtResDlg, CDialog )

CExtResDlg::CExtResDlg()
	: m_lpszTemplateName_SAVED( NULL )
{
}

CExtResDlg::CExtResDlg(
	UINT nIDTemplate,
	CWnd * pParentWnd // = NULL
	)
	: CDialog( nIDTemplate, pParentWnd )
	, m_lpszTemplateName_SAVED( NULL )
{
}

CExtResDlg::CExtResDlg(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	CWnd * pParentWnd // = NULL
	)
	: CDialog( lpszTemplateName, pParentWnd )
	, m_lpszTemplateName_SAVED( NULL )
{
}

CExtResDlg::~CExtResDlg()
{
}

BEGIN_MESSAGE_MAP(CExtResDlg, CDialog)
	//{{AFX_MSG_MAP(CExtResDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtResDlg::Create(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	CWnd * pParentWnd // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT(
			HIWORD(lpszTemplateName) == 0
		||	AfxIsValidString( lpszTemplateName )
		);

	m_lpszTemplateName = lpszTemplateName;  // used for help
	if(		HIWORD(m_lpszTemplateName) == 0
		&&	m_nIDHelp == 0
		)
		m_nIDHelp = LOWORD((DWORD)m_lpszTemplateName);
	m_lpszTemplateName_SAVED = m_lpszTemplateName;
HRSRC hResource = NULL;
HINSTANCE hInst = 
		g_ResourceManager->FindResourceHandle(
			RT_DIALOG,
			UINT(lpszTemplateName),
			NULL,
			&hResource
			);
	ASSERT( hInst != NULL );
	ASSERT( hResource != NULL );
	if( hInst == NULL )
		return FALSE;
HGLOBAL hTemplate = LoadResource( hInst, hResource );
	ASSERT( hTemplate != NULL );
BOOL bResult = CreateIndirect( hTemplate, pParentWnd, hInst );
	FreeResource( hTemplate );
	return bResult;
}

BOOL CExtResDlg::Create(
	UINT nIDTemplate,
	CWnd * pParentWnd // = NULL
	)
{
	return Create( MAKEINTRESOURCE(nIDTemplate), pParentWnd );
}

DLGTEMPLATE * CExtResDlg::stat_ChangeDlgFont(
	const DLGTEMPLATE * pTemplate
	)
{
CDialogTemplate dlgTemplate( pTemplate );
CString strFaceDefault, strFace;
WORD wSizeDefault, wSize;
	if(	CExtResPP::stat_GetPropSheetFont(
			strFaceDefault,
			wSizeDefault,
			FALSE
			)
		)
	{
		if(	(		(! CDialogTemplate::GetFont(
						pTemplate,
						strFace,
						wSize
						) )
				||	(strFace != strFaceDefault)
				||	(wSize != wSizeDefault)
			)
			&&	(	(	strFace == _T("MS Sans Serif")
					||	strFace == _T("MS Shell Dlg")
					)
				&&	(wSize == wSizeDefault)
				)
			)
			dlgTemplate.SetFont(
				strFaceDefault,
				wSizeDefault
				);
	}
	return (DLGTEMPLATE*)dlgTemplate.Detach();
}

int CExtResDlg::DoModal()
{
	__PROF_UIS_MANAGE_STATE;
	m_lpszTemplateName_SAVED = m_lpszTemplateName;
	return _DoModalImpl();
}

int CExtResDlg::_DoModalImpl()
{
	ASSERT(
			m_lpszTemplateName != NULL
		||	m_hDialogTemplate != NULL
		||	m_lpDialogTemplate != NULL
		);
LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
HGLOBAL hDialogTemplate = m_hDialogTemplate;
HINSTANCE hInst = ::AfxGetResourceHandle();
	if( m_lpszTemplateName != NULL )
	{
		HRSRC hResource = NULL;
		hInst = 
				g_ResourceManager->FindResourceHandle(
					RT_DIALOG,
					UINT(m_lpszTemplateName),
					NULL,
					&hResource
					);
		ASSERT( hInst != NULL );
		ASSERT( hResource != NULL );
		//hInst = AfxFindResourceHandle( m_lpszTemplateName, RT_DIALOG );
		//HRSRC hResource = ::FindResource( hInst, m_lpszTemplateName, RT_DIALOG );
		hDialogTemplate = ::LoadResource( hInst, hResource );
	} // if( m_lpszTemplateName != NULL )
	if( hDialogTemplate != NULL )
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource( hDialogTemplate );
	if( lpDialogTemplate == NULL )
		return -1;
HWND hWndParent = PreModal();
	::AfxUnhookWindowCreate();
BOOL bEnableParent = FALSE;
	if( hWndParent != NULL && ::IsWindowEnabled( hWndParent ) )
	{
		::EnableWindow( hWndParent, FALSE );
		bEnableParent = TRUE;
	} // if( hWndParent != NULL && ::IsWindowEnabled( hWndParent ) )
	TRY
	{
		::AfxHookWindowCreate( this );
		if( CreateDlgIndirect(
				lpDialogTemplate,
				CWnd::FromHandle( hWndParent ),
				hInst
				)
			)
		{
			if( (m_nFlags&WF_CONTINUEMODAL) != 0 )
			{
				DWORD dwFlags = MLF_SHOWONIDLE;
				if(	(GetStyle()&DS_NOIDLEMSG) != 0 )
					dwFlags |= MLF_NOIDLEMSG;
				int nModalResult = RunModalLoop( dwFlags );
				VERIFY( nModalResult == m_nModalResult );
				nModalResult;
			} // if( (m_nFlags&WF_CONTINUEMODAL) != 0 )
			if(	m_hWnd != NULL )
				SetWindowPos(
					NULL, 0, 0, 0, 0,
					SWP_HIDEWINDOW
					|SWP_NOSIZE|SWP_NOMOVE
					|SWP_NOACTIVATE|SWP_NOZORDER
					);
		}
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION( e );
		m_nModalResult = -1;
	}
	END_CATCH_ALL
	if( bEnableParent )
		::EnableWindow( hWndParent, TRUE );
	if( hWndParent != NULL && ::GetActiveWindow() == m_hWnd )
		::SetActiveWindow( hWndParent );
	DestroyWindow();
	PostModal();
	if( m_lpszTemplateName != NULL || m_hDialogTemplate != NULL )
		UnlockResource( hDialogTemplate );
	if (m_lpszTemplateName != NULL)
		FreeResource( hDialogTemplate );
	return m_nModalResult;
}

BOOL CExtResDlg::CreateDlgIndirect(
	LPCDLGTEMPLATE lpDialogTemplate,
	CWnd * pParentWnd,
	HINSTANCE hInst
	)
{
	ASSERT( lpDialogTemplate != NULL);
	lpDialogTemplate = (LPCDLGTEMPLATE)stat_ChangeDlgFont( lpDialogTemplate );
#ifdef _DEBUG
	if( pParentWnd != NULL )
		ASSERT_VALID( pParentWnd );
#endif // _DEBUG
	if( hInst == NULL )
		hInst = ::AfxGetInstanceHandle();
_AFX_OCC_DIALOG_INFO occDialogInfo;
COccManager * pOccManager = afxOccManager;
HGLOBAL hTemplate = NULL;
HWND hWnd = NULL;
#ifdef _DEBUG
	DWORD dwError = 0;
#endif
	TRY
	{
		VERIFY( AfxDeferRegisterClass( AFX_WNDCOMMCTLS_REG ) );
		AfxDeferRegisterClass( AFX_WNDCOMMCTLSNEW_REG );
		if( pOccManager != NULL )
		{
			if( ! SetOccDialogInfo( &occDialogInfo ) )
				return FALSE;
			lpDialogTemplate =
				pOccManager->PreCreateDialog(
				&occDialogInfo,
				lpDialogTemplate
				);
		}
		if( lpDialogTemplate == NULL )
			return FALSE;
		CString strFace;
		WORD wSize = 0;
		BOOL bSetSysFont =
			! CDialogTemplate::GetFont(
				lpDialogTemplate,
				strFace,
				wSize
				);
		if(		(!bSetSysFont)
			&&	GetSystemMetrics( SM_DBCSENABLED )
			)
		{
			bSetSysFont =
					(strFace == _T("MS Shell Dlg")
				||	strFace == _T("MS Sans Serif")
				||	strFace == _T("Helv")
				);
			if( bSetSysFont && (wSize == 8) )
				wSize = 0;
		}
		if( bSetSysFont )
		{
			CDialogTemplate dlgTemp( lpDialogTemplate );
			dlgTemp.SetSystemFont( wSize );
			hTemplate = dlgTemp.Detach();
		}
		if( hTemplate != NULL )
			lpDialogTemplate = (DLGTEMPLATE*)GlobalLock( hTemplate );
		m_nModalResult = -1;
		m_nFlags |= WF_CONTINUEMODAL;
		::AfxHookWindowCreate( this );
		hWnd =
			::CreateDialogIndirect(
				hInst,
				lpDialogTemplate,
				pParentWnd->GetSafeHwnd(),
				AfxDlgProc
				);
#ifdef _DEBUG
		dwError = ::GetLastError();
#endif
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		m_nModalResult = -1;
	}
	END_CATCH_ALL
	if( pOccManager != NULL )
	{
		pOccManager->PostCreateDialog( &occDialogInfo );
		if( hWnd != NULL )
			SetOccDialogInfo( NULL );
	}
	if( !AfxUnhookWindowCreate() )
		PostNcDestroy();
	if(		hWnd != NULL
		&&	( (m_nFlags&WF_CONTINUEMODAL) == 0 )
		)
	{
		::DestroyWindow(hWnd);
		hWnd = NULL;
	}
	if( hTemplate != NULL )
	{
		GlobalUnlock( hTemplate );
		GlobalFree( hTemplate );
	}
	if( hWnd == NULL && (m_nFlags&WF_CONTINUEMODAL) != 0 )
	{
#ifdef _DEBUG
		if (afxOccManager == NULL)
		{
			TRACE0(">>> If this dialog has OLE controls:\n");
			TRACE0(">>> AfxEnableControlContainer has not been called yet.\n");
			TRACE0(">>> You should call it in your app's InitInstance function.\n");
		}
		else if( dwError != 0 )
		{
			TRACE1("Warning: Dialog creation failed!  GetLastError returns 0x%8.8X\n", dwError);
		}
#endif //_DEBUG
		return FALSE;
	}
	ASSERT( hWnd == m_hWnd );
	return TRUE;
}

BOOL CExtResDlg::CreateDlgIndirect(
	LPCDLGTEMPLATE lpDialogTemplate,
	CWnd * pParentWnd
	)
{
	return CreateDlgIndirect( lpDialogTemplate, pParentWnd, NULL );
}

BOOL CExtResDlg::CreateIndirect(
	LPCDLGTEMPLATE lpDialogTemplate,
	CWnd * pParentWnd, // = NULL
	void * lpDialogInit // = NULL
	)
{
	return
		CreateIndirect(
			lpDialogTemplate,
			pParentWnd,
			lpDialogInit,
			NULL
			);
}

BOOL CExtResDlg::CreateIndirect(
	HGLOBAL hDialogTemplate,
	CWnd * pParentWnd // = NULL
	)
{
	return
		CreateIndirect(
			hDialogTemplate,
			pParentWnd,
			NULL
			);
}

BOOL CExtResDlg::CreateIndirect(
	LPCDLGTEMPLATE lpDialogTemplate,
	CWnd * pParentWnd,
	void * lpDialogInit,
	HINSTANCE hInst
	)
{
	ASSERT( lpDialogTemplate != NULL );
	if( pParentWnd == NULL )
		pParentWnd = ::AfxGetMainWnd();
	m_lpDialogInit = lpDialogInit;
	return
		CreateDlgIndirect(
			lpDialogTemplate,
			pParentWnd,
			hInst
			);
}

BOOL CExtResDlg::CreateIndirect(
	HGLOBAL hDialogTemplate,
	CWnd * pParentWnd,
	HINSTANCE hInst
	)
{
	ASSERT(hDialogTemplate != NULL);
LPCDLGTEMPLATE lpDialogTemplate =
		(LPCDLGTEMPLATE)LockResource( hDialogTemplate );
BOOL bResult =
		CreateIndirect(
			lpDialogTemplate,
			pParentWnd,
			NULL,
			hInst
			);
	UnlockResource( hDialogTemplate );
	return bResult;
}


BOOL CExtResDlg::OnInitDialog()
{
	if( m_lpszTemplateName_SAVED != NULL )
	{
		m_lpDialogInit = NULL;
		ExecuteDlgInit( m_lpszTemplateName_SAVED );
	}
void * _lpDialogInit = m_lpDialogInit;
	m_lpDialogInit = NULL;
LPCTSTR _lpszTemplateName = m_lpszTemplateName;
	m_lpszTemplateName = NULL;
BOOL bRetVal = CDialog::OnInitDialog();
	m_lpDialogInit = _lpDialogInit;
	m_lpszTemplateName = _lpszTemplateName;
	return bRetVal;
}

BOOL CExtResDlg::stat_ExecuteDlgInit(
	CWnd * pWnd,
	LPVOID pResource
	)
{
BOOL bSuccess = TRUE;
	if( pResource != NULL )
	{
		UNALIGNED WORD * pResWalk = (WORD*)pResource;
		for( ; bSuccess && (*pResWalk) != 0; )
		{
			WORD nIDC = *pResWalk++;
			WORD nMsg = *pResWalk++;
			DWORD dwLen = *((UNALIGNED DWORD*&)pResWalk)++;
#define WIN16_LB_ADDSTRING  0x0401
#define WIN16_CB_ADDSTRING  0x0403
#define AFX_CB_ADDSTRING    0x1234
			if( nMsg == AFX_CB_ADDSTRING )
				nMsg = CBEM_INSERTITEM;
			else if (nMsg == WIN16_LB_ADDSTRING)
				nMsg = LB_ADDSTRING;
			else if (nMsg == WIN16_CB_ADDSTRING)
				nMsg = CB_ADDSTRING;
#ifdef _DEBUG
			if(		nMsg == LB_ADDSTRING
				||	nMsg == CB_ADDSTRING
				||	nMsg == CBEM_INSERTITEM
				)
				ASSERT( *((LPBYTE)pResWalk + (UINT)dwLen - 1) == 0 );
#endif
			if( nMsg == CBEM_INSERTITEM )
			{
				USES_CONVERSION;
				COMBOBOXEXITEM item;
				item.mask = CBEIF_TEXT;
				item.iItem = -1;
				item.pszText = A2T( LPSTR(pResWalk) );
				if(	::SendDlgItemMessage(
						pWnd->m_hWnd,
						nIDC,
						nMsg,
						0,
						(LPARAM)&item
						) == - 1
					)
					bSuccess = FALSE;
			} // if( nMsg == CBEM_INSERTITEM )
			else if( nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING )
			{
				LPCSTR strToInsert = LPSTR(pResWalk);
				if(	::SendDlgItemMessageA(
						pWnd->m_hWnd,
						nIDC,
						nMsg,
						0,
						(LPARAM)strToInsert
						) == -1
					)
					bSuccess = FALSE;
			} // else if( nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING )
			pResWalk = (WORD*)((LPBYTE)pResWalk + (UINT)dwLen);
		} // for( ; bSuccess && (*pResWalk) != 0; )
	} // if( pResource != NULL )
	if( bSuccess )
		pWnd->SendMessageToDescendants(
			WM_INITIALUPDATE,
			0,
			0,
			FALSE,
			FALSE
			);
	return bSuccess;
}

BOOL CExtResDlg::ExecuteDlgInit( __EXT_MFC_SAFE_LPCTSTR lpszResourceName )
{
LPVOID pResource = NULL;
HGLOBAL hGlobalResource = NULL;
	if (lpszResourceName != NULL)
	{
		HRSRC hResource = NULL;
		HINSTANCE hInst = 
				g_ResourceManager->FindResourceHandle(
					RT_DLGINIT,
					UINT(m_lpszTemplateName),
					NULL,
					&hResource
					);
		if( hInst != NULL )
		{
			ASSERT( hResource != NULL );
			hGlobalResource = LoadResource( hInst, hResource );
			if( hGlobalResource == NULL )
				return FALSE;
			pResource = LockResource( hGlobalResource );
			ASSERT( pResource != NULL );
		} // if( hInst != NULL )
	}
BOOL bResult = ExecuteDlgInit( pResource );
	if( pResource != NULL && hGlobalResource != NULL )
	{
		UnlockResource( hGlobalResource );
		FreeResource( hGlobalResource );
	}
	return bResult;
}

BOOL CExtResDlg::ExecuteDlgInit( LPVOID pResource )
{
	return stat_ExecuteDlgInit( this, pResource );
}

/////////////////////////////////////////////////////////////////////////////
// CExtResPP window

IMPLEMENT_DYNCREATE( CExtResPP, CPropertyPage )

CExtResPP::CExtResPP()
	: m_lpszTemplateName_SAVED( NULL )
{
	CommonConstruct( NULL, 0 );
}

CExtResPP::CExtResPP(
	UINT nIDTemplate,
	UINT nIDCaption // = 0
	)
{
	ASSERT( nIDTemplate != 0 );
	CommonConstruct( MAKEINTRESOURCE(nIDTemplate), nIDCaption );
}

CExtResPP::CExtResPP(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	UINT nIDCaption // = 0
	)
{
	ASSERT(
			HIWORD(lpszTemplateName) == 0
		||	AfxIsValidString( lpszTemplateName )
		);
	CommonConstruct( lpszTemplateName, nIDCaption );
}

CExtResPP::~CExtResPP()
{
}

BEGIN_MESSAGE_MAP(CExtResPP, CPropertyPage)
	//{{AFX_MSG_MAP(CExtResPP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtResPP::OnInitDialog()
{
	if( m_lpszTemplateName_SAVED != NULL )
	{
		m_lpDialogInit = NULL;
		ExecuteDlgInit( m_lpszTemplateName_SAVED );
	}
void * _lpDialogInit = m_lpDialogInit;
	m_lpDialogInit = NULL;
LPCTSTR _lpszTemplateName = m_lpszTemplateName;
	m_lpszTemplateName = NULL;
BOOL bRetVal = CPropertyPage::OnInitDialog();
	m_lpDialogInit = _lpDialogInit;
	m_lpszTemplateName = _lpszTemplateName;
	return bRetVal;
}

BOOL CExtResPP::ExecuteDlgInit( __EXT_MFC_SAFE_LPCTSTR lpszResourceName )
{
LPVOID pResource = NULL;
HGLOBAL hGlobalResource = NULL;
	if (lpszResourceName != NULL)
	{
		HRSRC hResource = NULL;
		HINSTANCE hInst = 
				g_ResourceManager->FindResourceHandle(
					RT_DLGINIT,
					UINT(m_lpszTemplateName),
					NULL,
					&hResource
					);
		if( hInst != NULL )
		{
			ASSERT( hResource != NULL );
			hGlobalResource = LoadResource( hInst, hResource );
			if( hGlobalResource == NULL )
				return FALSE;
			pResource = LockResource( hGlobalResource );
			ASSERT( pResource != NULL );
		} // if( hInst != NULL )
	}
BOOL bResult = ExecuteDlgInit( pResource );
	if( pResource != NULL && hGlobalResource != NULL )
	{
		UnlockResource( hGlobalResource );
		FreeResource( hGlobalResource );
	}
	return bResult;
}

BOOL CExtResPP::ExecuteDlgInit( LPVOID pResource )
{
	return CExtResDlg::stat_ExecuteDlgInit( this, pResource );
}

#define IDD_PROPSHEET   1006
#define IDD_WIZARD      1020

BOOL CExtResPP::stat_GetPropSheetFont(
	CExtSafeString & strFace,
	WORD & wSize,
	BOOL bWizard
	)
{
struct FAKE_PROPPAGEFONTINFO : public CNoTrackObject
{
	LPTSTR m_pszFaceName;
	WORD m_wSize;
	FAKE_PROPPAGEFONTINFO() : m_pszFaceName(NULL), m_wSize(0) {}
	~FAKE_PROPPAGEFONTINFO() { GlobalFree(m_pszFaceName); }
};
static FAKE_PROPPAGEFONTINFO _afxPropPageFontInfo;
FAKE_PROPPAGEFONTINFO * pFontInfo = &_afxPropPageFontInfo;
	if( pFontInfo->m_wSize == 0 )
	{
		ASSERT( pFontInfo->m_pszFaceName == NULL );
		HINSTANCE hInst =
			::GetModuleHandleA( "COMCTL32.DLL" );
		if( hInst != NULL )
		{
//			HRSRC hResource =
//				::FindResource(
//					hInst,
//					MAKEINTRESOURCE( (bWizard ? IDD_WIZARD : IDD_PROPSHEET) ),
//					RT_DIALOG
//					);
//			HGLOBAL hTemplate = ::LoadResource( hInst, hResource );
//			if( hTemplate != NULL )
//				CDialogTemplate::GetFont(
//					(DLGTEMPLATE*)hTemplate,
//					strFace,
//					wSize
//					);
			CExtResourceManager::CExtResourceMemoryBuffer bufferOut;
			if( bufferOut.FindAndLoad(
					hInst,
					RT_DIALOG,
					bWizard ? IDD_WIZARD : IDD_PROPSHEET,
					g_ResourceManager->GetLangIdNeutral()
					)
				)
			{
				CString _strFace;
				CDialogTemplate::GetFont(
					(DLGTEMPLATE*)bufferOut.GetBuffer(),
					_strFace,
					wSize
					);
				strFace = LPCTSTR( _strFace );
			}
		} // if( hInst != NULL )
		pFontInfo->m_pszFaceName =
			(LPTSTR)::GlobalAlloc(
				GPTR,
				sizeof(TCHAR) * ( strFace.GetLength() + 1 )
				);
		lstrcpy( pFontInfo->m_pszFaceName, LPCTSTR(strFace) );
		pFontInfo->m_wSize = wSize;
	} // if( pFontInfo->m_wSize == 0 )
	strFace = pFontInfo->m_pszFaceName;
	wSize = pFontInfo->m_wSize;
	return ( wSize != 0xFFFF) ? TRUE : FALSE;
}

DLGTEMPLATE * CExtResPP::stat_ChangePropPageFont(
	const DLGTEMPLATE * pTemplate,
	BOOL bWizard
	)
{
CString strFaceDefault;
WORD wSizeDefault;
	if( ! stat_GetPropSheetFont(
			strFaceDefault,
			wSizeDefault,
			bWizard
			)
		)
		return NULL;
CString strFace;
WORD wSize;
	if(		(! CDialogTemplate::GetFont(
				pTemplate,
				strFace,
				wSize
				) )
		||	(strFace != strFaceDefault)
		||	(wSize != wSizeDefault)
		)
	{
		CDialogTemplate dlgTemplate( pTemplate );
		dlgTemplate.SetFont( strFaceDefault, wSizeDefault );
		return (DLGTEMPLATE*)dlgTemplate.Detach();
	}
	return NULL;
}

void CExtResPP::PreProcessPageTemplate(
	PROPSHEETPAGE & psp,
	BOOL bWizard
	)
{
const DLGTEMPLATE * pTemplate;
CExtResourceManager::CExtResourceMemoryBuffer bufferOut;
	if( psp.dwFlags & PSP_DLGINDIRECT )
	{
		pTemplate = psp.pResource;
		m_lpszTemplateName_SAVED = NULL;
	}
	else
	{
//		HRSRC hResource =
//			::FindResource(
//				psp.hInstance,
//				psp.pszTemplate,
//				RT_DIALOG
//				);
//		HGLOBAL hTemplate = LoadResource(psp.hInstance,
//			hResource);
//		pTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
		HRSRC hResource = NULL;
		HINSTANCE hInst = 
				g_ResourceManager->FindResourceHandle(
					RT_DIALOG,
					UINT(psp.pszTemplate),
					NULL,
					&hResource
					);
		ASSERT( hInst != NULL );
		ASSERT( hResource != NULL );
		ASSERT( psp.hInstance == hInst );
		HGLOBAL hTemplate = LoadResource(psp.hInstance,
			hResource);
		pTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
	}

	ASSERT( pTemplate != NULL );
#ifdef _DEBUG
	if( ( ( DLGTEMPLATEEX*)pTemplate )->signature == 0xFFFF )
	{
		DWORD dwVersion = ::GetVersion();
		if( dwVersion & 0x80000000 )
		{
			HINSTANCE hInst = LoadLibrary(_T("COMCTL32.DLL"));
			ASSERT(hInst != NULL);
			if (hInst != NULL)
			{
				FARPROC proc = GetProcAddress(hInst, "DllGetVersion");
				if (proc == NULL)
					ASSERT(FALSE);
				FreeLibrary(hInst);
			}
		} // if( dwVersion & 0x80000000 )
		else if (LOBYTE(LOWORD(dwVersion)) == 3)
		{
			ASSERT(FALSE);
		} // else from if( dwVersion & 0x80000000 )
	} // if( ( ( DLGTEMPLATEEX*)pTemplate )->signature == 0xFFFF )
#endif // _DEBUG

	if( afxOccManager != NULL )
		pTemplate = InitDialogInfo(pTemplate);
HGLOBAL hTemplate = stat_ChangePropPageFont( pTemplate, bWizard );
	if( m_hDialogTemplate != NULL )
	{
		GlobalFree( m_hDialogTemplate );
		m_hDialogTemplate = NULL;
	}
	if( hTemplate != NULL )
	{
		pTemplate = (LPCDLGTEMPLATE)hTemplate;
		m_hDialogTemplate = hTemplate;
	}
	psp.pResource = pTemplate;
	psp.dwFlags |= PSP_DLGINDIRECT;
}

void CExtResPP::Construct(
	UINT nIDTemplate,
	UINT nIDCaption
	)
{
	ASSERT( nIDTemplate != 0 );
	CommonConstruct( MAKEINTRESOURCE(nIDTemplate), nIDCaption );
}

void CExtResPP::Construct(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	UINT nIDCaption
	)
{
	ASSERT(
			HIWORD(lpszTemplateName) == 0
		||	AfxIsValidString( lpszTemplateName )
		);
	CommonConstruct( lpszTemplateName, nIDCaption );
}

UINT CALLBACK CExtResPP::stat_PropPageCallback( HWND, UINT message, LPPROPSHEETPAGE pPropPage )
{
	switch (message)
	{
	case PSPCB_CREATE:
		{
#if _MFC_VER < 0x700
			ASSERT( AfxIsValidAddress( pPropPage, sizeof(AFX_OLDPROPSHEETPAGE) ) );
#endif
			ASSERT( AfxIsValidAddress( pPropPage, pPropPage->dwSize ) );
			CPropertyPage * pPage =
				STATIC_DOWNCAST( CPropertyPage, (CObject*)(pPropPage->lParam) );
			ASSERT_VALID( pPage );
			TRY
			{
				AfxHookWindowCreate( pPage );
			}
			CATCH_ALL( e )
			{
				return FALSE;
			}
			END_CATCH_ALL
		}
		return TRUE;
	case PSPCB_RELEASE:
		AfxUnhookWindowCreate();
		break;
	}

	return 0;
}

void CExtResPP::CommonConstruct(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	UINT nIDCaption
	)
{
	memset( &m_psp, 0, sizeof(m_psp) );
	m_psp.dwSize = sizeof(m_psp);
	m_psp.dwFlags = PSP_USECALLBACK;
	if( lpszTemplateName != NULL )
		//m_psp.hInstance = AfxFindResourceHandle(lpszTemplateName, RT_DIALOG);
		m_psp.hInstance =
			g_ResourceManager->FindResourceHandle(
				RT_DIALOG,
				UINT( lpszTemplateName )
				);
		
	m_psp.pszTemplate = lpszTemplateName;
	m_psp.pfnDlgProc = AfxDlgProc;
	m_psp.lParam = (LPARAM)this;
	m_psp.pfnCallback = stat_PropPageCallback;
	if( nIDCaption != 0 )
	{
		CExtSafeString _strCaption;
		VERIFY( g_ResourceManager->LoadString( _strCaption, nIDCaption ) );
		m_strCaption = LPCTSTR(_strCaption);
		m_psp.pszTitle = m_strCaption;
		m_psp.dwFlags |= PSP_USETITLE;
	}
	if( AfxHelpEnabled() )
		m_psp.dwFlags |= PSP_HASHELP;
	if( HIWORD(lpszTemplateName) == 0 )
		m_nIDHelp = LOWORD((DWORD)lpszTemplateName);
	m_lpszTemplateName = m_psp.pszTemplate;
	m_bFirstSetActive = TRUE;
}

#ifdef _DEBUG
void CExtResPP::AssertValid() const
{
	CWnd::AssertValid();
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtResPS window

IMPLEMENT_DYNCREATE( CExtResPS, CPropertySheet )

CExtResPS::CExtResPS()
{
}

CExtResPS::CExtResPS(
	UINT nIDCaption,
	CWnd * pParentWnd, // = NULL
	UINT iSelectPage // = 0
	)
	: CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
{
CExtSafeString strCaption;
	if( g_ResourceManager->LoadString( strCaption, nIDCaption ) )
	{
		m_strCaption = LPCTSTR( strCaption );
		m_psh.pszCaption = m_strCaption;
	}
}

CExtResPS::CExtResPS(
	__EXT_MFC_SAFE_LPCTSTR pszCaption,
	CWnd * pParentWnd, // = NULL
	UINT iSelectPage // = 0
	)
	: CPropertySheet( LPCTSTR(pszCaption), pParentWnd, iSelectPage )
{
}

CExtResPS::~CExtResPS()
{
}

BEGIN_MESSAGE_MAP(CExtResPS, CPropertySheet)
	//{{AFX_MSG_MAP(CExtResPS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtResPS::InitLocalizedChildren()
{
// TO DO: NOT IMPL YET

//	HINSTANCE hInst =
//		::GetModuleHandleA( "COMCTL32.DLL" );
//	if( hInst != NULL )
//	{
//		CExtResourceManager::CExtResourceMemoryBuffer bufferOut;
//		if( bufferOut.FindAndLoad(
//				hInst,
//				RT_DIALOG,
//				IDD_WIZARD,
//				g_ResourceManager->GetLangIdNeutral()
//				)
//			)
//		{
//			//CDialogTemplate dlgTemplate( (DLGTEMPLATE*)bufferOut.GetBuffer() );
//			//dlgTemplate.
//		}
//	} // if( hInst != NULL )
}

LRESULT CExtResPS::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
LRESULT lResult = CPropertySheet::WindowProc( message,  wParam, lParam );
	switch( message )
	{
	case WM_CREATE:
		InitLocalizedChildren();
		break;
	} // switch( message )
	return lResult;
}

BOOL CExtResPS::OnInitDialog()
{
BOOL bRetVal = CPropertySheet::OnInitDialog();
	InitLocalizedChildren();
	return bRetVal;
}

void CExtResPS::BuildPropPageArray()
{
#if _MFC_VER < 0x700
	if( m_psh.ppsp != NULL )
		delete [] (PROPSHEETPAGE *) m_psh.ppsp;
	m_psh.ppsp = NULL;
AFX_OLDPROPSHEETPAGE * ppsp =
		new AFX_OLDPROPSHEETPAGE[ m_pages.GetSize() ];
	m_psh.ppsp = (LPPROPSHEETPAGE) ppsp;
BOOL bWizard = ( m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97) );
	for( int i = 0; i < m_pages.GetSize(); i++ )
	{
		CPropertyPage * pPage = GetPage(i);
		memcpy(&ppsp[i], & pPage->m_psp, sizeof( pPage->m_psp ) );
		CExtResPP * pExtResourcePropertyPage =
			DYNAMIC_DOWNCAST( CExtResPP, pPage );
		if( pExtResourcePropertyPage != NULL )
			pExtResourcePropertyPage->PreProcessPageTemplate(
				(PROPSHEETPAGE&) ppsp[i],
				bWizard
				);
		else
		{
			struct friendly_property_page_t : public CPropertyPage
			{
			public:
				friend class CExtResPS;
			};
			((friendly_property_page_t *)pPage)->PreProcessPageTemplate(
				(PROPSHEETPAGE&) ppsp[i],
				bWizard
				);
		}
	} // for( int i = 0; i < m_pages.GetSize(); i++ )
	m_psh.nPages = m_pages.GetSize();
#else
	if( m_psh.ppsp != NULL )
	{
		free((void*)m_psh.ppsp);
		m_psh.ppsp = NULL;
	}
	int i;
	int nBytes = 0;
	for( i = 0; i < m_pages.GetSize(); i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		nBytes += pPage->m_psp.dwSize;
	}
	PROPSHEETPAGE * ppsp = (PROPSHEETPAGE *)malloc( nBytes );
	if( ppsp == NULL )
		AfxThrowMemoryException();
	m_psh.ppsp = ppsp;
	BOOL bWizard = (m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97));
	for( i = 0; i < m_pages.GetSize(); i++ )
	{
		struct friendly_property_page_t : public CPropertyPage
		{
		public:
			friend class CExtResPS;
		};
		friendly_property_page_t * pPage =
			(friendly_property_page_t *) GetPage(i);
		::memcpy( ppsp, &pPage->m_psp, pPage->m_psp.dwSize );
		if (!pPage->m_strHeaderTitle.IsEmpty())
		{
			ppsp->pszHeaderTitle = pPage->m_strHeaderTitle;
			ppsp->dwFlags |= PSP_USEHEADERTITLE;
		}
		if (!pPage->m_strHeaderSubTitle.IsEmpty())
		{
			ppsp->pszHeaderSubTitle = pPage->m_strHeaderSubTitle;
			ppsp->dwFlags |= PSP_USEHEADERSUBTITLE;
		}
		CExtResPP * pExtResourcePropertyPage =
			DYNAMIC_DOWNCAST( CExtResPP, pPage );
		if( pExtResourcePropertyPage != NULL )
			pExtResourcePropertyPage->PreProcessPageTemplate(
				*ppsp,
				bWizard
				);
		else
			pPage->PreProcessPageTemplate(
				*ppsp,
				bWizard
				);
		(BYTE*&)ppsp += ppsp->dwSize;
	} // for( i = 0; i < m_pages.GetSize(); i++ )
	m_psh.nPages = (int)m_pages.GetSize();
#endif
}

