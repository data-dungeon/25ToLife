#ifndef _OPTIONSDLG_H_
#define _OPTIONSDLG_H_

class OptionsDialog : public CDialog
{
	public:
		void SetExportUtility( LPCTSTR pFileName );
		void GetExportUtility( LPTSTR pFileName, int MaxName );
		void GetExportUtility( tstring &FileName );
		void SetGameDataPath( LPCTSTR pPath );
		void GetGameDataPath( LPTSTR pPath, int MaxPath );
		void GetGameDataPath( tstring &Path );

	protected:
		tstring m_ExportUtilityPath;
		tstring m_GameDataPath;

		//////////////////////////////////////////////////////////////////////////
		// overridable functions

		virtual BOOL OnInitDialog( void );
		virtual void OnOK( void );
		virtual void OnCancel( void );

		//////////////////////////////////////////////////////////////////////////
		// afx message handlers

		afx_msg void OnBrowse1( void );
		afx_msg void OnBrowse2( void );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _OPTIONSDLG_H_