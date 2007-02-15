#include "pch.h"
#include "finddialog.h"
#include "resource.h"

BEGIN_MESSAGE_MAP( FindDialog, CDialog )
	ON_COMMAND( ID_BUTTON_FIND, OnCommandFind )
	ON_COMMAND( ID_BUTTON_REPLACE, OnCommandReplace )
	ON_COMMAND( ID_BUTTON_SELECT, OnCommandSelect )
END_MESSAGE_MAP()

FindDialog::FindDialog( void )
{
	m_bMatchCase = FALSE;
	m_bMatchWholeWord = FALSE;
	m_bPreserveCase = TRUE;
	m_bAutoSelect = FALSE;
	m_bSearchParents = FALSE;
	m_bSearchChildren = FALSE;
	m_bSearchSiblings = FALSE;
	m_SearchPath = SearchPath_CurrentScreen;
}

//=============================================================================
// FindDialog::GetListBox
//
//		Returns a pointer to the "search result" list box control.
//=============================================================================

CListCtrl* FindDialog::GetListBox( void )
{
	return (CListCtrl*) GetDlgItem( ID_LIST_FOUNDITEMS );
}

//=============================================================================
// FindDialog::ClearSearchResults
//
//		Removes all items from the "search result" list box.
//=============================================================================

void FindDialog::ClearSearchResults( void )
{
	CListCtrl* pListBox = GetListBox();

	if( pListBox )
		pListBox->DeleteAllItems();

	return;
}

//=============================================================================
// FindDialog::AddSearchResult
//
//		Adds an entry to the "search result" list box.
//=============================================================================

int FindDialog::AddSearchResult( const char* pName, void* pParam )
{
	int i = 0;
	CListCtrl* pListBox = GetListBox();

	if( pListBox )
	{
		int n = pListBox->GetItemCount();

		LVITEM item;

		item.mask = LVIF_PARAM|LVIF_TEXT;
		item.iItem = n;
		item.iSubItem = 0;
		item.pszText = (LPSTR) pName;
		item.lParam = (LPARAM) pParam;

		i = pListBox->InsertItem( &item );
	}

	return i;
}

//=============================================================================
// FindDialog::SetColumn
//=============================================================================

void FindDialog::SetColumn( int i, int Column, const char* pText )
{
	CListCtrl* pListBox = GetListBox();

	if( pListBox )
	{
		LVITEM item;

		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = Column;
		item.pszText = (LPSTR) pText;

		pListBox->SetItem( &item );
	}

	return;
}

//=============================================================================
// FindDialog::CheckDlgItem
//
//		Sets the check state on the specified check box
//=============================================================================

void FindDialog::CheckDlgItem( UINT ButtonId, BOOL Checked )
{
	::CheckDlgItem( this, ButtonId, Checked );
}

//=============================================================================
// FindDialog::GetCheck
//
//		Gets the check state from the specified check box.
//=============================================================================

BOOL FindDialog::GetCheck( UINT ButtonId )
{
	return ::GetCheck( this, ButtonId );
}

//=============================================================================
// FindDialog::SetDlgItem
//
//		Updates the state of all dialog items.
//=============================================================================

void FindDialog::SetDlgItems( void )
{
	CheckDlgItem( ID_CHECK_MATCHCASE, m_bMatchCase );
	CheckDlgItem( ID_CHECK_MATCHWHOLEWORD, m_bMatchWholeWord );
	CheckDlgItem( ID_CHECK_PRESERVECASE, m_bPreserveCase );
	CheckDlgItem( ID_CHECK_AUTOSELECT, m_bAutoSelect );
	CheckDlgItem( ID_CHECK_PARENTS, m_bSearchParents );
	CheckDlgItem( ID_CHECK_CHILDREN, m_bSearchChildren );
	CheckDlgItem( ID_CHECK_SIBLINGS, m_bSearchSiblings );

	SetDlgItemText( ID_COMBO_FINDWHAT, m_szFindWhat );
	SetDlgItemText( ID_COMBO_REPLACEWITH, m_szReplaceWith );
	SetDlgItemText( ID_COMBO_EXCLUDEWHAT, m_szExcludeWhat );

	CComboBox* pComboBox = (CComboBox*) GetDlgItem( ID_COMBO_SEARCHPATH );

	if( pComboBox )
		pComboBox->SetCurSel( int(m_SearchPath) );

	return;
}

//=============================================================================
// FindDialog::GetDlgItems
//
//		Gets the state of all dialog items.
//=============================================================================

void FindDialog::GetDlgItems( void )
{
	m_bMatchCase = GetCheck( ID_CHECK_MATCHCASE );
	m_bMatchWholeWord = GetCheck( ID_CHECK_MATCHWHOLEWORD );
	m_bPreserveCase = GetCheck( ID_CHECK_PRESERVECASE );
	m_bAutoSelect = GetCheck( ID_CHECK_AUTOSELECT );
	m_bSearchParents = GetCheck( ID_CHECK_PARENTS );
	m_bSearchChildren = GetCheck( ID_CHECK_CHILDREN );
	m_bSearchSiblings = GetCheck( ID_CHECK_SIBLINGS );

	GetDlgItemText( ID_COMBO_FINDWHAT, m_szFindWhat );
	GetDlgItemText( ID_COMBO_REPLACEWITH, m_szReplaceWith );
	GetDlgItemText( ID_COMBO_EXCLUDEWHAT, m_szExcludeWhat );

	CComboBox* pComboBox = (CComboBox*) GetDlgItem( ID_COMBO_SEARCHPATH );

	if( pComboBox )
		m_SearchPath = SearchPath( pComboBox->GetCurSel() );

	return;
}

//=============================================================================
// FindDialog::OnInitDialog
//
//		This is called by the framework when the dialog box is initialized.
//=============================================================================

BOOL FindDialog::OnInitDialog( void )
{
	CListCtrl* pListBox = GetListBox();

	if( pListBox )
	{
		pListBox->InsertColumn( 0, "Search Results", LVCFMT_LEFT, 140 );
		pListBox->InsertColumn( 1, "Index", LVCFMT_LEFT, 25 );
		pListBox->InsertColumn( 2, "Parents", LVCFMT_LEFT, 300 );
	}

	CComboBox* pComboBox = (CComboBox*) GetDlgItem( ID_COMBO_SEARCHPATH );

	if( pComboBox )
	{
		pComboBox->InsertString( int(SearchPath_EntireProject), "Entire Project" );
		pComboBox->InsertString( int(SearchPath_CurrentScreen), "Current Screen" );
		pComboBox->InsertString( int(SearchPath_SelectionOnly), "Selection Only" );
	}

	SetDlgItems();

	return CDialog::OnInitDialog();
}

//=============================================================================
// FindDialog::OnCommandFind
//
//		This is called by the framework when the user pressed the "Find" button.
//=============================================================================


void FindDialog::OnCommandFind( void )
{
	GetDlgItems();

	CWnd* pWnd = GetParent();

	if( pWnd )
		pWnd->SendMessage( WM_COMMAND, MAKEWPARAM(ID_BUTTON_FIND, 0), 0);

	return;
}

//=============================================================================
// FindDialog::OncommandReplace
//
//		This is called by the framework when the user pressed the "Replace"
//		button.
//=============================================================================

void FindDialog::OnCommandReplace( void )
{
	GetDlgItems();

	CWnd* pWnd = GetParent();

	if( pWnd )
		pWnd->SendMessage( WM_COMMAND, MAKEWPARAM(ID_BUTTON_REPLACE, 0), 0);

	return;
}

//=============================================================================
// FindDialog::OncommandSelect
//
//		This is called by the framework when the user pressed the "Select"
//		button.
//=============================================================================

void FindDialog::OnCommandSelect( void )
{
	GetDlgItems();

	CWnd* pWnd = GetParent();

	if( pWnd )
		pWnd->SendMessage( WM_COMMAND, MAKEWPARAM(ID_BUTTON_SELECT, 0), 0);

	return;
}