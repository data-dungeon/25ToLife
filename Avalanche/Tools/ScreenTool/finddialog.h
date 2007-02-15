#ifndef _FINDDIALOG_H_
#define _FINDDIALOG_H_

#include <afxwin.h>
#include <afxdlgs.h>

#define ID_FINDDIALOG_FIND		(0x100)
#define ID_FINDDIALOG_REPLACE	(0x101)

class FindDialog : public CDialog
{
	public:
		typedef enum
		{
			SearchPath_EntireProject,
			SearchPath_CurrentScreen,
			SearchPath_SelectionOnly,
		}
		SearchPath;

	public:
		FindDialog( void );
		CListCtrl* GetListBox( void );
		void ClearSearchResults( void );
		int AddSearchResult( const char* pName, void* pParam );
		void SetColumn( int i, int Column, const char* pText );

		void CheckDlgItem( UINT ButtonId, BOOL Checked );
		BOOL GetCheck( UINT ButtonId );
		void SetDlgItems( void );
		void GetDlgItems( void );

		CString m_szFindWhat;
		CString m_szReplaceWith;
		CString m_szExcludeWhat;
		BOOL m_bMatchCase;
		BOOL m_bMatchWholeWord;
		BOOL m_bPreserveCase;
		BOOL m_bAutoSelect;
		BOOL m_bSearchParents;
		BOOL m_bSearchChildren;
		BOOL m_bSearchSiblings;
		SearchPath m_SearchPath;

	protected:
		afx_msg BOOL OnInitDialog( void );
		afx_msg void OnCommandFind( void );
		afx_msg void OnCommandReplace( void );
		afx_msg void OnCommandSelect( void );

	DECLARE_MESSAGE_MAP();
};

#endif // _FINDDIALOG_H_