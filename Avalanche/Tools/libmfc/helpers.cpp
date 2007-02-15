#include "pch.h"
#include "helpers.h"

void CheckDlgItem( CWnd* pWnd, UINT ButtonId, BOOL Checked )
{
	if( pWnd )
	{
		if( Checked )
			pWnd->CheckDlgButton( ButtonId, TRUE );
		else
			pWnd->CheckDlgButton( ButtonId, FALSE );
	}

	return;
}

BOOL GetCheck( CWnd* pWnd, UINT ButtonId )
{
	if( pWnd )
	{
		UINT Result = pWnd->IsDlgButtonChecked( ButtonId );

		if( Result == BST_CHECKED || Result == BST_INDETERMINATE )
			return TRUE;
	}

	return FALSE;
}

void ToolBarMFC_SetButtonStyle( CToolBar *pToolBar, UINT ButtonId, UINT Style )
{
	if( pToolBar )
	{
		int Index = pToolBar->CommandToIndex( ButtonId );

		if( Index != -1 )
			pToolBar->SetButtonStyle( Index, Style );
	}

	return;
}