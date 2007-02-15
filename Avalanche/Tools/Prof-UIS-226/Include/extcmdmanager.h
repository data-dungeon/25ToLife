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

#if (!defined __ExtCmdManager_H)
#define __ExtCmdManager_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __AFXMT_H__)
	#include <AfxMt.h>
#endif

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

// max text size
#define __MAX_UI_ITEM_TEXT (1024)

// mdi menu list commands
#define __ID_MDIWNDLIST_COUNT 9
#define __ID_MDIWNDLIST_FIRST AFX_IDM_FIRST_MDICHILD
#define __ID_MDIWNDLIST_LAST (__ID_MDIWNDLIST_FIRST+__ID_MDIWNDLIST_COUNT-1)
#define __ID_MDIWND_DLGWINDOWS (__ID_MDIWNDLIST_LAST+1)

#define __MFCEXT_DEF_RARELY_USED_PERCENT 10

class CExtCmdIcon;
class CExtCmdItem;
class CExtCmdItemUI;
class CExtCmdProfile;
class CExtCmdManager;

/////////////////////////////////////////////////////////////////////////////
// CExtCmdItem
// command item class

class __PROF_UIS_API CExtCmdItem
{
public:
	enum e_state_t // state flags
	{
		STATE_ENABLED				= 0x00000000,
		STATE_DISABLED				= 0x00000001,
		STATE_CHECK					= 0x00000002,
		STATE_RADIO					= 0x00000004,
		STATE_INDETERMINATE			= 0x00000008,
		STATE_BASICCMDPROP			= 0x00000010,
		STATE_FORCE_RARELY			= 0x00000020,
		STATE_MENUBAR_TMP			= 0x00000040,
		STATE_CUSTOM_TOOLBAR		= 0x00000080,
		STATE_DYNAMIC_POPUP			= 0x00000100,
		STATE_PERSISTENT_ICON		= 0x00000200,
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		STATE_TEXT_FIELD			= 0x00000400, // command is text field
		STATE_TF_NE					= 0x00000800, // text field is not editable (only with STATE_TEXT_FIELD)
		STATE_COMBO					= 0x00001000, // text field is able to drop list-box (only with STATE_TEXT_FIELD)
		STATE_RESIZABLE				= 0x00002000, // text field is resizable (only with STATE_TEXT_FIELD)
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		STATE_COLOR					= 0x00004000, // color button
		STATE_MCB_DEFAULT			= 0x00008000, // "default" button on color menu
		STATE_MCB_CUSTOM			= 0x00010000, // "custom" button on color menu
		STATE_SEPARATED_DROPDOWN	= 0x00020000, // sepatated button in toolbar
		STATE_AUTOCHANGE_ID			= 0x00040000, // separated button with auto-changable command ID
		STATE_NO_ROTATE_VL			= 0x00080000, // do not rotate in vertical layout
		STATE_CANNOT_HIDE			= 0x00100000, // can not be hidden via toolbar buttons menu
		STATE_CTRL_VIS_VL			= 0x00200000, // control is visible in vertical layout
		STATE_AUTO_IS_AUTOMATED		= 0x00400000, // reserved for Prof-Auto (command is allocated by automation calls)
		STATE_AUTO_CAN_REMOVE		= 0x00800000, // reserved for Prof-Auto (command can be removed via automation calls)
		STATE_AUTO_CONSTRUCTOR		= 0,          // reserved for Prof-Auto
		STATE_AUTO_INITIAL			= (STATE_AUTO_IS_AUTOMATED|STATE_AUTO_CAN_REMOVE), // reserved for Prof-Auto
		STATE_AUTO_MASK				= (STATE_AUTO_IS_AUTOMATED|STATE_AUTO_CAN_REMOVE), // reserved for Prof-Auto
		STATE_FF_DESIGN_COMMAND		= 0x01000000, // reserved for Frame Features
		STATE_FF_TOOLBAR_COMMAND	= 0x02000000, // reserved for Frame Features
		STATE_FF_SCROLLER			= 0x04000000, // reserved for Frame Features
		STATE_FF_HIDE_IN_ALL_CMDS	= 0x08000000, // reserved for Frame Features
		STATE_CLEAN_MASK			=
			(STATE_DYNAMIC_POPUP
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			|STATE_TEXT_FIELD|STATE_COMBO
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			|STATE_COLOR|STATE_FF_SCROLLER),
		STATE_USER_DEFINED_03		= 0x10000000,
		STATE_USER_DEFINED_02		= 0x20000000,
		STATE_USER_DEFINED_01		= 0x40000000,
		STATE_USER_DEFINED_00		= 0x80000000,
	}; // enum e_state_t

	CExtCmdProfile * m_pProfile;
	UINT m_nCmdID;
	int m_nIconIdx;
	DWORD m_dwStateFlags;
	CExtSafeString
		m_sMenuText,m_sToolbarText,
		m_sTipTool,m_sTipStatus,
		m_sAccelText
		;
	LPARAM m_nLParamUserData;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	INT m_nTextFieldWidth;
	INT m_nDropDownWidth; // (-1) - auto calc, (-2) - same as button area
	INT m_nDropDownHeightMax;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

protected:
	UINT m_nUsageTickCount;

	virtual void AssignFromOther( const CExtCmdItem & other );
	virtual void ReplaceFromOtherNonEmpty( const CExtCmdItem & other );
	virtual void AnalyzeGlobalUsageOverflow();

public:
	// construction/destruction/assignment
	CExtCmdItem(
		UINT nCmdID = IDC_STATIC
		);
	CExtCmdItem( const CExtCmdItem & other );
	virtual ~CExtCmdItem();
	CExtCmdItem & operator=( const CExtCmdItem & other );
	
	//clean tips
	virtual void TipsClean();
	// load tips
	virtual bool TipsLoad();
	// upload more full parameters from other
	virtual void UpdateMoreExact( const CExtCmdItem & other );

	// state retrieve/update
	virtual void StateSetBasic( bool bOn = true )
	{
		if(	bOn )
			m_dwStateFlags |= STATE_BASICCMDPROP;
		else
			m_dwStateFlags &= ~STATE_BASICCMDPROP;
	}
	virtual bool StateIsBasic() const;
	virtual bool StateIsRarelyUsed() const;
	virtual void StateForceRarely( bool bOn = true )
	{
		if(	bOn )
			m_dwStateFlags |= STATE_FORCE_RARELY;
		else
			m_dwStateFlags &= ~STATE_FORCE_RARELY;
	}
	virtual bool StateIsForceRarely() const;
	virtual void StateEnable( bool bOn = true )
	{
		if(	bOn )
			m_dwStateFlags &= ~STATE_DISABLED;
		else
			m_dwStateFlags |= STATE_DISABLED;
	}
	virtual bool StateIsEnable() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_DISABLED)
				? false : true;
		return bOn;
	}
	virtual void StateSetCheck( bool bOn = true )
	{
		m_dwStateFlags &=
			~(STATE_RADIO|STATE_CHECK|STATE_INDETERMINATE);
		if( bOn )
			m_dwStateFlags |= STATE_CHECK;
	}
	virtual bool StateGetCheck() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_CHECK)
				? true : false;
		return bOn;
	}
	virtual void StateSetRadio( bool bOn = true )
	{
		m_dwStateFlags &=
			~(STATE_RADIO|STATE_CHECK|STATE_INDETERMINATE);
		if( bOn )
			m_dwStateFlags |= STATE_RADIO;
	}
	virtual bool StateGetRadio() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_RADIO)
				? true : false;
		return bOn;
	}
	virtual void StateSetIndeterminate( bool bOn = true )
	{
		m_dwStateFlags &=
			~(STATE_RADIO|STATE_CHECK|STATE_INDETERMINATE);
		if( bOn )
			m_dwStateFlags |= STATE_INDETERMINATE;
	}
	virtual bool StateGetIndeterminate() const
	{
		bool bIndeterminate =
			(m_dwStateFlags&STATE_INDETERMINATE)
				? true : false;
		return bIndeterminate;
	}

	virtual void StateSetMenubarTemp( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_MENUBAR_TMP);
		if( bOn )
			m_dwStateFlags |= STATE_MENUBAR_TMP;
	}
	virtual bool StateIsMenubarTemp() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_MENUBAR_TMP)
				? true : false;
		return bOn;
	}

	virtual void StateSetCustomToolbar( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_CUSTOM_TOOLBAR);
		if( bOn )
			m_dwStateFlags |= STATE_CUSTOM_TOOLBAR;
	}
	virtual bool StateIsCustomToolbar() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_CUSTOM_TOOLBAR)
				? true : false;
		return bOn;
	}

	virtual void StateSetDynamicPopup( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_DYNAMIC_POPUP);
		if( bOn )
			m_dwStateFlags |= STATE_DYNAMIC_POPUP;
	}
	virtual bool StateIsDynamicPopup() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_DYNAMIC_POPUP)
				? true : false;
		return bOn;
	}

	virtual void StateSetPersistentIcon( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_PERSISTENT_ICON);
		if( bOn )
			m_dwStateFlags |= STATE_PERSISTENT_ICON;
	}
	virtual bool StateIsPersistentIcon() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_PERSISTENT_ICON)
				? true : false;
		return bOn;
	}

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	virtual void StateSetTextField( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_TEXT_FIELD|STATE_COMBO);
		if( bOn )
			m_dwStateFlags |= STATE_TEXT_FIELD;
	}
	virtual bool StateIsTextField() const
	{
		bool bOn =
			((m_dwStateFlags&STATE_TEXT_FIELD) == STATE_TEXT_FIELD)
				? true : false;
		return bOn;
	}

	virtual void StateSetTextFieldNotEditable( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_TF_NE);
		if( bOn )
			m_dwStateFlags |= STATE_TF_NE;
	}
	virtual bool StateIsTextFieldNotEditable() const
	{
		bool bOn =
			((m_dwStateFlags&STATE_TF_NE) != 0)
				? true : false;
		return bOn;
	}

	virtual void StateSetCombo( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_TEXT_FIELD|STATE_COMBO);
		if( bOn )
			m_dwStateFlags |= STATE_TEXT_FIELD|STATE_COMBO;
	}
	virtual bool StateIsCombo() const
	{
		bool bOn =
			( (m_dwStateFlags&(STATE_TEXT_FIELD|STATE_COMBO)) == (STATE_TEXT_FIELD|STATE_COMBO) )
				? true : false;
		return bOn;
	}

	virtual void StateSetResizable( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_RESIZABLE);
		if( bOn )
			m_dwStateFlags |= STATE_RESIZABLE;
	}
	virtual bool StateIsResizable() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_RESIZABLE)
				? true : false;
		return bOn;
	}
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	virtual void StateSetColor( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_COLOR);
		if( bOn )
			m_dwStateFlags |= STATE_COLOR;
	}
	virtual bool StateIsColor() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_COLOR)
				? true : false;
		return bOn;
	}

	virtual void StateSetColorBtnDefault( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_MCB_DEFAULT);
		if( bOn )
			m_dwStateFlags |= STATE_MCB_DEFAULT;
	}
	virtual bool StateIsColorBtnDefault() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_MCB_DEFAULT)
				? true : false;
		return bOn;
	}

	virtual void StateSetColorBtnCustom( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_MCB_CUSTOM);
		if( bOn )
			m_dwStateFlags |= STATE_MCB_CUSTOM;
	}
	virtual bool StateIsColorBtnCustom() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_MCB_CUSTOM)
				? true : false;
		return bOn;
	}

	virtual void StateSetSeparatedDD( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_SEPARATED_DROPDOWN);
		if( bOn )
			m_dwStateFlags |= STATE_SEPARATED_DROPDOWN;
	}
	virtual bool StateIsSeparatedDD() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_SEPARATED_DROPDOWN)
				? true : false;
		return bOn;
	}

	virtual void StateSetAutochangeID( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_AUTOCHANGE_ID);
		if( bOn )
			m_dwStateFlags |= STATE_AUTOCHANGE_ID;
	}
	virtual bool StateIsAutochangeID() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_AUTOCHANGE_ID)
				? true : false;
		return bOn;
	}

	virtual void StateSetNoRotateVL( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_NO_ROTATE_VL);
		if( bOn )
			m_dwStateFlags |= STATE_NO_ROTATE_VL;
	}
	virtual bool StateIsNoRotateVL() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_NO_ROTATE_VL)
				? true : false;
		return bOn;
	}

	virtual void StateSetCannotHide( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_CANNOT_HIDE);
		if( bOn )
			m_dwStateFlags |= STATE_CANNOT_HIDE;
	}
	virtual bool StateIsCannotHide() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_CANNOT_HIDE)
				? true : false;
		return bOn;
	}

	virtual void StateSetCtrlVisibleVertically( bool bOn = true )
	{
		m_dwStateFlags &= ~(STATE_CTRL_VIS_VL);
		if( bOn )
			m_dwStateFlags |= STATE_CTRL_VIS_VL;
	}
	virtual bool StateIsCtrlVisibleVertically() const
	{
		bool bOn =
			(m_dwStateFlags&STATE_CTRL_VIS_VL)
				? true : false;
		return bOn;
	}

	// update command UI
	virtual bool DoUpdateCmdUI(
		CCmdTarget * pCmdTarget,
		UINT nIndex = 0
		);
	// deliver to window
	virtual bool Deliver(
		HWND hWndCmdTarget,
		bool bSend = false
		);
	virtual bool Deliver(
		CWnd * pWndCmdTarget,
		bool bSend = false
		);
	virtual bool Deliver(
		CControlBar * pWndCmdSource,
		bool bSend = false
		);

	// usage statistics
	virtual UINT GetUsageTickCount() const;
	virtual UINT GetUsagePercent() const;
	virtual void IncrementUsageCount();
	virtual void ResetUsageStatistics();
	virtual UINT GetProfileTickCount() const;
	virtual UINT GetProfileRarelyPercent() const;
	virtual void SetProfileRarelyPercent(
		UINT nRarelyPercent
		);
	
	virtual void OnSysColorChange();
	virtual void OnSettingChange(
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		);
	virtual void OnDisplayChange(
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void OnThemeChanged(
		WPARAM wParam,
		LPARAM lParam
		);

	// get the text for toolbars list box in a customize form
	virtual CExtSafeString OnGetToolBarCustomizeName();

	// get the command name for display in
	// the customize from's category command list
	virtual CExtSafeString OnGetCustomizeScriptName();

	// get the command description text for display in
	// the customize from's description area
	virtual CExtSafeString OnGetCustomizeCommandDescription();

protected:
	virtual bool OnQueryStateSerializationNecessity() const;

	friend class CExtCmdManager;
	friend class CExtCmdProfile;
}; // class CExtCmdItem

/////////////////////////////////////////////////////////////////////////////
// CExtCmdItemUI
// command UI update class

class __PROF_UIS_API CExtCmdItemUI : public CCmdUI
{
	CExtCmdItem * m_pCmd;
public:
	CExtCmdItemUI(
		CExtCmdItem * pCmd,
		int nIndexMax = 0
		);
	// operations to do in ON_UPDATE_COMMAND_UI
	virtual void Enable(
		BOOL bOn = TRUE
		);
	virtual void SetCheck(
		int nCheck = 1 // 0, 1 or 2 (indeterminate)
		);
	virtual void SetRadio(
		BOOL bOn = TRUE
		);
	virtual void SetText(
		__EXT_MFC_SAFE_LPCTSTR lpszText
		);
}; // class CExtCmdItemUI

/////////////////////////////////////////////////////////////////////////////
// CExtCmdProfile
// command profile class

class __PROF_UIS_API CExtCmdProfile
{
public:

	typedef struct tag_MFC_TOOLBAR_RESOURCE_DATA
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		WORD * items()
		{
			ASSERT( this != NULL );
			return (LPWORD)( this + 1 );
		}
	} MFC_TOOLBAR_RESOURCE_DATA, * LP_MFC_TOOLBAR_RESOURCE_DATA;
	
	class __PROF_UIS_API MFC_TOOLBAR_LOADER
	{
		LPUINT m_pCommands, m_pButtons;
		INT m_nCountCommands, m_nCountButtons;
		COLORREF m_clrBmpTransparent;
		CBitmap m_bmp;
		CSize m_sizeButton;
		WORD m_nVersion;
	public:
		MFC_TOOLBAR_LOADER(
			HINSTANCE hInstResourceCommands,
			HRSRC hRsrcCommands,
			HINSTANCE hInstResourceBitmap,
			HRSRC hRsrcCommandsBitmap,
			COLORREF clrBmpTransparent = RGB(192,192,192)
			);
		~MFC_TOOLBAR_LOADER();
		WORD GetVersion() const;
		INT IsEmpty() const;
		INT GetCommandCount() const;
		INT GetButtonCount() const;
		COLORREF GetTransparentColor() const;
		UINT GetCommandIdAt( INT nCommandIdx ) const;
		UINT GetButtonIdAt( INT nButtonIdx ) const;
		UINT ExtractButtonData(
			INT nButtonIdx,
			CExtCmdIcon & icon
			) const;
		void GetCmdArray(
			LPUINT * ppCmdArray,
			LPINT pCmdCount = NULL
			) const;
	}; // class MFC_TOOLBAR_LOADER

	CExtSafeString m_sName;
	UINT m_nRarelyPercent; // values 0..100
	UINT m_nTotalTickCount;

	typedef
		CMap < UINT, UINT, CExtCmdItem *, CExtCmdItem * >
		cmd_container_t;
	typedef
		CArray < CExtCmdIcon *, CExtCmdIcon * >
		icon_container_t;

	cmd_container_t m_cmds;
	icon_container_t m_icons;

	LPARAM m_nLParamUserData;

	CExtCmdProfile(
		__EXT_MFC_SAFE_LPCTSTR sName = NULL
		);
	CExtCmdProfile(
		const CExtCmdProfile & other
		);
	virtual ~CExtCmdProfile();
	CExtCmdProfile & operator=(
		const CExtCmdProfile & other
		);

	// setup single command
	virtual bool CmdSetup(
		const CExtCmdItem & _cmd,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool * pbWasAddedNew = NULL
		);
	// remove single command
	virtual bool CmdRemove(
		UINT nCmdID,
		bool * pbWasRemoved = NULL
		);
	// remove all commands
	virtual void CmdRemoveAll();
	// remove all commands by state mask
	virtual void CmdRemoveByMask(
		DWORD dwMask,
		bool bAllBitsOnly = false
		);
	// alloc command
	virtual CExtCmdItem * CmdAllocPtr(
		UINT nCmdID = 0 // 0 means any free in avail range
		);
	// get command
	virtual CExtCmdItem * CmdGetPtr(
		UINT nCmdID
		);
	// assign icon to command
	virtual bool CmdSetIcon(
		UINT nCmdID,
		const CExtCmdIcon * pCmdIcon, // if NULL or empty - remove
		bool bUseCmdIconObject
		);
	bool CmdSetIcon(
		UINT nCmdID,
		const CExtCmdIcon & cmdIcon // if empty - remove
		);
	bool CmdSetIcon(
		UINT nCmdID,
		HBITMAP hBitmap, // if NULL - remove
		COLORREF clrTransparent = RGB(0,0,0),
		LPCRECT pRectBitmapSrc = NULL
		);
	bool CmdSetIcon(
		UINT nCmdID,
		HICON hIcon, // if NULL - remove
		bool bCopyIcon = true
		);
	// get command icon (if command and its icon exist)
	virtual CExtCmdIcon * CmdGetIconPtr(
		UINT nCmdID
		);
	// is command registered
	virtual bool CmdIsRegistered(
		UINT nCmdID
		);
	// update commands collection from menu handle
	virtual bool UpdateFromMenu(
		HMENU hMenu,
		bool bReplaceOld = false,
		bool bRecursive = true,
		bool bLoadTips = true
		);
	// update commands collection from menu resurce
	virtual bool UpdateFromMenu(
		UINT nResourceID,
		bool bReplaceOld = false,
		bool bLoadTips = true
		);
	// update commands collection from toolbar wnd
	virtual bool UpdateFromToolBar(
		CToolBar & bar,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool bLoadTips = true
		);
	// update commands collection from toolbar resource
	virtual bool UpdateFromToolBar(
		__EXT_MFC_SAFE_LPCTSTR strResourceID,
		LPUINT * ppCmdArray = NULL,
		LPINT pCmdCount = NULL,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool bLoadTips = true,
		COLORREF clrBmpTransparent = RGB(192,192,192)
		);
	// set list of commands (up to (UINT)0) as basic or non basic
	virtual bool SetBasicCommands(
		UINT * pCommands,
		bool bOn = true
		);

	// save/load command manager state
	virtual bool SerializeState(
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		bool bSave
		);
	virtual bool SerializeState(
		CArchive & ar
		);

	virtual void OnSysColorChange();
	virtual void OnSettingChange(
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		);
	virtual void OnDisplayChange(
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void OnThemeChanged(
		WPARAM wParam,
		LPARAM lParam
		);

	virtual void ResetUsageStatistics();

protected:
	virtual void AssignFromOther(
		const CExtCmdProfile & other
		);
	void _RemoveAllCmdsImpl();
	void _RemoveAllIconsImpl();
	virtual CExtCmdItem * OnCreateCmdItem( const CExtCmdItem & _cmd );
}; // class CExtCmdProfile

/////////////////////////////////////////////////////////////////////////////
// CExtCmdManager
// command manager class

class __PROF_UIS_API CExtCmdManager
{
public:

	// for compatibility with old versions
	typedef CExtCmdIcon icon_t;
	typedef CExtCmdItem cmd_t;
	typedef CExtCmdItemUI cmd_ui_t;
	typedef CExtCmdProfile cmd_profile_t;

	static inline bool IsCommand(UINT nCmdID)
	{
		//if( ((INT)nCmdID) <= 0 )
		if( nCmdID == 0
			|| ((INT)nCmdID) <= ((INT)(-1))
			)
			return false;
		return true;
	}
	static inline bool IsSystemCommand( UINT nCmd )
	{
		bool bSystemCommand =
			(nCmd >= 0xF000 && nCmd < 0xF1F0) ?
				true : false;
		return bSystemCommand;
	}
	static inline bool IsAfxOleCommand( UINT nCmd )
	{
		if( ID_OLE_VERB_FIRST <= nCmd
			&& nCmd <= ID_OLE_VERB_LAST
			)
			return true;
		return false;
	}
	static inline bool IsAfxMdiWindowsCommand( UINT nCmd )
	{
		if( (	__ID_MDIWNDLIST_FIRST <= nCmd
				&&	nCmd <= __ID_MDIWNDLIST_LAST )
			|| nCmd == __ID_MDIWND_DLGWINDOWS
			)
			return true;
		return false;
	}
	static inline bool IsAfxFileMruCommand( UINT nCmd )
	{
		if( ID_FILE_MRU_FIRST <= nCmd
			&&	nCmd <= ID_FILE_MRU_LAST
			)
			return true;
		return false;
	}
	static inline bool IsForceBasicCommand( UINT nCmd )
	{
		if(		IsAfxMdiWindowsCommand( nCmd )
			||	IsAfxFileMruCommand( nCmd )
			||	IsSystemCommand( nCmd )
			||	IsAfxOleCommand( nCmd )
			)
			return true;
		return false;
	}
	static inline bool IsForceRarelyCommand( UINT nCmd )
	{
		nCmd;
		return false;
	}
	static inline bool IsCommandNeedsSpecUpdate( UINT nCmd )
	{
		if(	IsSystemCommand( nCmd )
			|| IsAfxMdiWindowsCommand( nCmd )
			)
			return true;
		return false;
	}

	static bool g_bDisableCmdIfNoHandler;

private:

	typedef
		CExtSafeMapStringToPtr
		profile_container_t;
	profile_container_t m_profiles;

	typedef
		CMap< HWND, HWND, CExtCmdProfile *, CExtCmdProfile *& >
		profile_wnd_container_t;
	profile_wnd_container_t m_profile_wnds;

	void _RemoveAllProfilesImpl();
	void _ProfileGetWndListImpl(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		CList < HWND, HWND & > & list
		);
	void _ProfileGetWndListImpl(
		CExtCmdProfile * pProfile,
		CList < HWND, HWND & > & list
		);

	CCriticalSection m_cs;

public:
	// global auto-pointer class for CExtCmdManager
	class __PROF_UIS_API CExtCmdManagerAutoPtr
	{
		CExtCmdManager * m_pCmdManager;
		static volatile DWORD g_dwVersion;
	public:

		CExtCmdManagerAutoPtr();
		~CExtCmdManagerAutoPtr();

		// command manager instance access
		CExtCmdManager * operator->();

		// Prof-UIS version info
		static DWORD GetVersionDWORD(
			bool bForSerialization = false
			);
		static __EXT_MFC_SAFE_LPCTSTR GetVersionString(
			CExtSafeString & strBuff,
			__EXT_MFC_SAFE_TCHAR tchrSeparator = _T('.')
			);
		void OnSysColorChange(
			CWnd * pWndNotifySrc
			);
		void OnSettingChange(
			CWnd * pWndNotifySrc,
			UINT uFlags,
			__EXT_MFC_SAFE_LPCTSTR lpszSection
			);
		void OnDisplayChange(
			CWnd * pWndNotifySrc,
			INT nDepthBPP,
			CPoint ptSizes
			);
		void OnThemeChanged(
			CWnd * pWndNotifySrc,
			WPARAM wParam,
			LPARAM lParam
			);
	}; // class CExtCmdManagerAutoPtr

public:
	// construction/destruction
	CExtCmdManager();
	~CExtCmdManager();

public:
#define __EXTMFC_DEF_PROFILE_NAME _T("default")
	// setup single profile
	bool ProfileSetup(
		__EXT_MFC_SAFE_LPCTSTR sProfileName = NULL,
		HWND hProfileWnd = NULL,
		CExtCmdProfile * pNewProfileInstance = NULL
		);
	// get profile
	CExtCmdProfile * ProfileGetPtr(
		__EXT_MFC_SAFE_LPCTSTR sProfileName = NULL
		);
	// setup profile window
	bool ProfileWndAdd(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		HWND hProfileWnd
		);
	// remove profile window
	bool ProfileWndRemove(
		HWND hProfileWnd,
		bool bRemoveProfileIfLastHWND = false,
		bool * p_bProfileWasRemoved = NULL
		);
	// remove all profile windows
	int ProfileWndRemoveAll(
		__EXT_MFC_SAFE_LPCTSTR sProfileName
		);
	// get profile window list
	void ProfileGetWndList(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		CList < HWND, HWND & > & list
		);
	// remove profile from command manager
	bool ProfileDestroy(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		bool bDestroyIfHasWindows = false
		);
	// rename profile
	bool ProfileRename(
		__EXT_MFC_SAFE_LPCTSTR sProfileNameOld,
		__EXT_MFC_SAFE_LPCTSTR sProfileNameNew
		);
	// get profile name for window
	__EXT_MFC_SAFE_LPCTSTR ProfileNameFromWnd(
		HWND hWnd
		);
	// reset the toolbar/menu command statistics
	bool ProfileResetUsageStatistics(
		__EXT_MFC_SAFE_LPCTSTR sProfileName
		);

	// setup single command
	bool CmdSetup(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		const CExtCmdItem & _cmd,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool * pbWasAddedNew = NULL
		);
	// remove single command
	bool CmdRemove(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID,
		bool * pbWasRemoved = NULL
		);
	// remove all commands
	virtual void CmdRemoveAll(
		__EXT_MFC_SAFE_LPCTSTR sProfileName
		);
	// remove all commands by state mask
	bool CmdRemoveByMask(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		DWORD dwMask,
		bool bAllBitsOnly = false
		);
	// get command
	CExtCmdItem * CmdGetPtr(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID
		);
	// alloc command
	CExtCmdItem * CmdAllocPtr(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID = 0 // 0 means any free in avail range
		);
	// is command registered
	bool CmdIsRegistered(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID
		);
	// assign icon to command
	bool CmdSetIcon(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID,
		const CExtCmdIcon * pCmdIcon, // if NULL or empty - remove
		bool bUseCmdIconObject
		);
	bool CmdSetIcon(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID,
		const CExtCmdIcon & cmdIcon // if empty - remove
		);
	bool CmdSetIcon(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID,
		HBITMAP hBitmap, // if NULL - remove
		COLORREF clrTransparent = RGB(0,0,0),
		LPCRECT pRectBitmapSrc = NULL
		);
	bool CmdSetIcon(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID,
		HICON hIcon, // if NULL - remove
		bool bCopyIcon = true
		);

	// get command icon (if command and its icon exist)
	CExtCmdIcon * CmdGetIconPtr(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID
		);
	// get command hIcon
	HICON CmdGetHICON(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nCmdID
		)
	{
		CExtCmdIcon * pIcon =
			CmdGetIconPtr(sProfileName,nCmdID);
		if( pIcon == NULL )
			return NULL;
		ASSERT( !(pIcon->IsEmpty()) );
		HICON hIcon = (*pIcon);
		ASSERT( hIcon != NULL );
		return hIcon;
	}

	// update commands collection from menu handle
	bool UpdateFromMenu(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		HMENU hMenu,
		bool bReplaceOld = false,
		bool bRecursive = true,
		bool bLoadTips = true
		);
	// update commands collection from menu resurce
	bool UpdateFromMenu(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nResourceID,
		bool bReplaceOld = false,
		bool bLoadTips = true
		);
	// update commands collection from toolbar wnd
	bool UpdateFromToolBar(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		CToolBar & bar,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool bLoadTips = true
		);
	// update commands collection from toolbar resource
	bool UpdateFromToolBar(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT nResourceID,
		LPUINT * ppCmdArray = NULL,
		LPINT pCmdCount = NULL,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool bLoadTips = true,
		COLORREF clrBmpTransparent = RGB(192,192,192)
		);
	bool UpdateFromToolBar(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		__EXT_MFC_SAFE_LPCTSTR strResourceID,
		LPUINT * ppCmdArray = NULL,
		LPINT pCmdCount = NULL,
		bool bReplaceOld = false, // but force set images anywhere if was empty
		bool bLoadTips = true,
		COLORREF clrBmpTransparent = RGB(192,192,192)
		);
	// set list of commands (up to (UINT)0) as basic or non basic
	bool SetBasicCommands(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		UINT * pCommands,
		bool bOn = true
		);

	// save/load command manager state
	bool SerializeState(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		bool bSave
		);
	bool SerializeState(
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		CArchive & ar
		);

	static bool FileObjToRegistry(
		CFile & _file,
		__EXT_MFC_SAFE_LPCTSTR sRegKeyPath,
		HKEY hKeyRoot = HKEY_CURRENT_USER
		);
	static bool FileObjFromRegistry(
		CFile & _file,
		__EXT_MFC_SAFE_LPCTSTR sRegKeyPath,
		HKEY hKeyRoot = HKEY_CURRENT_USER
		);

	static CExtSafeString GetSubSystemRegKeyPath(
		__EXT_MFC_SAFE_LPCTSTR sSubSystemName,
		__EXT_MFC_SAFE_LPCTSTR sProfileName,
		__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		);

	virtual void OnSysColorChange();
	virtual void OnSettingChange(
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		);
	virtual void OnDisplayChange(
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void OnThemeChanged(
		WPARAM wParam,
		LPARAM lParam
		);

	friend class CExtCmdItem;
	friend class CExtCmdProfile;
}; // class CExtCmdManager

#define __EXT_MFC_LANG_ID_CHINESE_SIMPLIFIED	( MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED) )
#define __EXT_MFC_LANG_ID_CZECH_DEFAULT			( MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_ENGLISH_US			( MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US) )
#define __EXT_MFC_LANG_ID_FRENCH				( MAKELANGID(LANG_FRENCH,SUBLANG_FRENCH) )
#define __EXT_MFC_LANG_ID_GERMAN				( MAKELANGID(LANG_GERMAN,SUBLANG_GERMAN) )
#define __EXT_MFC_LANG_ID_ITALIAN				( MAKELANGID(LANG_ITALIAN,SUBLANG_ITALIAN) )
#define __EXT_MFC_LANG_ID_JAPANESE_DEFAULT		( MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_KOREAN_DEFAULT		( MAKELANGID(LANG_KOREAN,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_POLISH_DEFAULT		( MAKELANGID(LANG_POLISH,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_RUSSIAN_DEFAULT		( MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_SPANISH_MODERN		( MAKELANGID(LANG_SPANISH,SUBLANG_SPANISH_MODERN) )
#define __EXT_MFC_LANG_ID_SWEDISH_DEFAULT		( MAKELANGID(LANG_SWEDISH,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_TURKISH_DEFAULT		( MAKELANGID(LANG_TURKISH,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_LATVIAN_DEFAULT		( MAKELANGID(0x26,SUBLANG_DEFAULT) )
#define __EXT_MFC_LANG_ID_UKRAINIAN_DEFAULT		( MAKELANGID(0x22,SUBLANG_DEFAULT) )

#define __EXT_MFC_LANG_ID_DEFAULT_NEUTRAL		__EXT_MFC_LANG_ID_ENGLISH_US

class __PROF_UIS_API CExtResourceManager
{
public:
	// global auto-pointer class for CExtResourceManager
	class __PROF_UIS_API CExtResourceManagerAutoPtr
	{
		CExtResourceManager * m_pResourceManager;
	public:
		CExtResourceManagerAutoPtr();
		~CExtResourceManagerAutoPtr();
		// resource manager instance access
		CExtResourceManager * operator->();
		void InstallResourceManager(
			CExtResourceManager * pNewResourceManager = NULL // NULL means install default resource manager
			);
	}; // class CExtResourceManagerAutoPtr

	// helper resource buffer class
	class __PROF_UIS_API CExtResourceMemoryBuffer
	{
		void AssignFromOther(
			const CExtResourceMemoryBuffer & other
			);
		int CompareWithOther(
			const CExtResourceMemoryBuffer & other
			) const;
		BYTE * m_pBuffer;
		DWORD m_dwBufferSize;
	public:
		CExtResourceMemoryBuffer();
		CExtResourceMemoryBuffer(
			const CExtResourceMemoryBuffer & other
			);
		~CExtResourceMemoryBuffer();
#ifdef _DEBUG
		void Dump();
#endif // _DEBUG
		CExtResourceMemoryBuffer & operator =(
			const CExtResourceMemoryBuffer & other
			);
		bool operator ! () const;
		bool operator ==(
			const CExtResourceMemoryBuffer & other
			) const;
		bool operator !=(
			const CExtResourceMemoryBuffer & other
			) const;
		bool operator <(
			const CExtResourceMemoryBuffer & other
			) const;
		bool operator <=(
			const CExtResourceMemoryBuffer & other
			) const;
		bool operator >(
			const CExtResourceMemoryBuffer & other
			) const;
		bool operator >=(
			const CExtResourceMemoryBuffer & other
			) const;
		BYTE operator [] ( DWORD dwByteIndex ) const;
		operator BYTE * ();
		operator const BYTE * ();
		DWORD GetSize() const;
		BYTE * GetBuffer();
		const BYTE * GetBuffer() const;
		HGLOBAL AllocGlobalBufferBuffer(
			UINT nFlags = GPTR
			) const;
		bool IsEmpty() const;
		void Empty();
		bool Alloc(
			DWORD dwBufferSize,
			bool bZeroMemory = true
			);
		bool Load(
			HINSTANCE hModule,
			HRSRC hResource
			);
		bool FindAndLoad(
			HINSTANCE hModule,
			__EXT_MFC_SAFE_LPCTSTR strResourceType,
			UINT nResourceName,
			WORD nLangIdDesired
			);
	}; // class CExtResourceMemoryBuffer

	bool m_bAllowCustomLang:1;
	WORD m_nLangIdDesired;
	static WORD g_nLangIdNeutral;

	CExtResourceManager(
		bool bAllowCustomLang = false,
		WORD nLangIdDesired = __EXT_MFC_LANG_ID_DEFAULT_NEUTRAL
		);
	virtual ~CExtResourceManager();

	virtual bool IsCustomLangAllowed() const;
	virtual WORD GetLangIdDesired() const;
	virtual WORD GetLangIdNeutral() const;
	virtual int	GetLocaleInfo(
		LCTYPE LCType,
		__EXT_MFC_SAFE_LPTSTR lpLCData,
		int cchData
		);

	virtual HINSTANCE FindResourceHandleSingleLanguage(
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD * p_wLangIdOut = NULL,
		HRSRC * p_hResourceOut = NULL
		);
	virtual HINSTANCE FindResourceHandleEx(
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD nLangIdNeutral = CExtResourceManager::g_nLangIdNeutral,
		WORD * p_wLangIdOut = NULL,
		HRSRC * p_hResourceOut = NULL
		);
	virtual HINSTANCE FindResourceHandle(
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD * p_wLangIdOut = NULL,
		HRSRC * p_hResourceOut = NULL
		);
	virtual bool LoadResourceBufferSingleLanguage(
		CExtResourceMemoryBuffer & bufferOut,
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadResourceBufferEx(
		CExtResourceMemoryBuffer & bufferOut,
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD nLangIdNeutral = CExtResourceManager::g_nLangIdNeutral,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadResourceBuffer(
		CExtResourceMemoryBuffer & bufferOut,
		__EXT_MFC_SAFE_LPCTSTR strResourceType,
		UINT nResourceName,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadStringEx(
		CExtSafeString & strOut,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD nLangIdNeutral = CExtResourceManager::g_nLangIdNeutral,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadString(
		CExtSafeString & strOut,
		__EXT_MFC_SAFE_LPCTSTR strResourceName,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadString(
		CExtSafeString & strOut,
		UINT nResourceName,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadMenuEx(
		CMenu & menuOut,
		UINT nResourceName,
		WORD nLangIdDesired,
		WORD nLangIdNeutral = CExtResourceManager::g_nLangIdNeutral,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadMenu(
		CMenu & menuOut,
		__EXT_MFC_SAFE_LPCTSTR strResourceName,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
	virtual bool LoadMenu(
		CMenu & menuOut,
		UINT nResourceName,
		WORD * p_wLangIdOut = NULL,
		HINSTANCE * p_hInstOut = NULL
		);
}; // class CExtResourceManager

extern __PROF_UIS_API CExtCmdManager::CExtCmdManagerAutoPtr g_CmdManager;
extern __PROF_UIS_API CExtResourceManager::CExtResourceManagerAutoPtr g_ResourceManager;

/////////////////////////////////////////////////////////////////////////////
// CExtResDlg window

class __PROF_UIS_API CExtResDlg : public CDialog
{
public:
	DECLARE_DYNCREATE( CExtResDlg )
	
	CExtResDlg();
	CExtResDlg(
		UINT nIDTemplate,
		CWnd * pParentWnd = NULL
		);
	CExtResDlg(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd = NULL
		);
	virtual ~CExtResDlg();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtResDlg)
	//}}AFX_VIRTUAL

// Implementation
public:

	BOOL Create(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd = NULL
		);
	BOOL Create(
		UINT nIDTemplate,
		CWnd * pParentWnd = NULL
		);
	virtual int DoModal();
	virtual BOOL OnInitDialog();
	static DLGTEMPLATE * stat_ChangeDlgFont(
		const DLGTEMPLATE * pTemplate
		);
	BOOL ExecuteDlgInit( __EXT_MFC_SAFE_LPCTSTR lpszResourceName );
	BOOL ExecuteDlgInit( LPVOID pResource );
	static BOOL stat_ExecuteDlgInit(
		CWnd * pWnd,
		LPVOID pResource
		);
protected:
	virtual int _DoModalImpl();
public:
	BOOL CreateDlgIndirect(
		LPCDLGTEMPLATE lpDialogTemplate,
		CWnd * pParentWnd
		);
	BOOL CreateDlgIndirect(
		LPCDLGTEMPLATE lpDialogTemplate,
		CWnd * pParentWnd,
		HINSTANCE hInst
		);
	BOOL CreateIndirect(
		LPCDLGTEMPLATE lpDialogTemplate,
		CWnd * pParentWnd = NULL,
		void * lpDialogInit = NULL
		);
	BOOL CreateIndirect(
		HGLOBAL hDialogTemplate,
		CWnd * pParentWnd = NULL
		);
	BOOL CreateIndirect(
		LPCDLGTEMPLATE lpDialogTemplate,
		CWnd * pParentWnd,
		void * lpDialogInit,
		HINSTANCE hInst
		);
	BOOL CreateIndirect(
		HGLOBAL hDialogTemplate,
		CWnd * pParentWnd,
		HINSTANCE hInst
		);
private:
	__EXT_MFC_SAFE_LPCTSTR m_lpszTemplateName_SAVED;

// Generated message map functions
protected:
	//{{AFX_MSG(CExtResDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

}; // class CExtResDlg

/////////////////////////////////////////////////////////////////////////////
// CExtResPP window

class __PROF_UIS_API CExtResPP : public CPropertyPage
{
public:
	DECLARE_DYNCREATE( CExtResPP )
	
	CExtResPP();
	CExtResPP(
		UINT nIDTemplate,
		UINT nIDCaption = 0
		);
	CExtResPP(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		UINT nIDCaption = 0
		);
	virtual ~CExtResPP();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtResPP)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL OnInitDialog();
	BOOL ExecuteDlgInit( __EXT_MFC_SAFE_LPCTSTR lpszResourceName );
	BOOL ExecuteDlgInit( LPVOID pResource );
	static BOOL CExtResPP::stat_GetPropSheetFont(
		CExtSafeString & strFace,
		WORD & wSize,
		BOOL bWizard
		);
	static DLGTEMPLATE * stat_ChangePropPageFont(
		const DLGTEMPLATE * pTemplate,
		BOOL bWizard
		);
	void PreProcessPageTemplate(
		PROPSHEETPAGE & psp,
		BOOL bWizard
		);
private:
	__EXT_MFC_SAFE_LPCTSTR m_lpszTemplateName_SAVED;
	static UINT CALLBACK stat_PropPageCallback( HWND, UINT message, LPPROPSHEETPAGE pPropPage );
protected:
	void CommonConstruct(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		UINT nIDCaption
		);
public:
	void Construct(
		UINT nIDTemplate,
		UINT nIDCaption = 0
		);
	void Construct(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		UINT nIDCaption = 0
		);
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CExtResPP)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

}; // class CExtResPP


/////////////////////////////////////////////////////////////////////////////
// CExtResPS window

class __PROF_UIS_API CExtResPS : public CPropertySheet
{
public:
	DECLARE_DYNCREATE( CExtResPS )

	CExtResPS();
	CExtResPS(
		UINT nIDCaption,
		CWnd * pParentWnd = NULL,
		UINT iSelectPage = 0
		);
	CExtResPS(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd * pParentWnd = NULL,
		UINT iSelectPage = 0
		);
	virtual ~CExtResPS();
		
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtResPS)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL OnInitDialog();
	virtual void BuildPropPageArray();
	void InitLocalizedChildren();

// Generated message map functions
protected:
	//{{AFX_MSG(CExtResPS)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

}; // class CExtResPS

#endif // __ExtCmdManager_H

