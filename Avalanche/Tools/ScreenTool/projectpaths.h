#ifndef _PROJECTPATHS_H_
#define _PROJECTPATHS_H_

class ProjectPaths : public CPropertyPage
{
	public:
		ProjectPaths( void );

		tstring m_GameDir;
		tstring m_DataDir;
		tstring m_DataPC;
		tstring m_DataDX;
		tstring m_DataPS2;
		tstring m_DataGCN;
		tstring m_DataXBOX;
		tstring m_DataPath;

	protected:
		//////////////////////////////////////////////////////////////////////////
		// overridable functions

		virtual BOOL OnInitDialog( void );
		virtual void OnOK( void );
		virtual void OnCancel( void );

	protected:
		void OnBrowse( UINT ParentId, UINT Id );

	protected:
		afx_msg void OnBrowseGameDir( void );
		afx_msg void OnBrowseDataDir( void );
		afx_msg void OnBrowseDataPC( void );
		afx_msg void OnBrowseDataDX( void );
		afx_msg void OnBrowseDataPS2( void );
		afx_msg void OnBrowseDataGCN( void );
		afx_msg void OnBrowseDataXBOX( void );
		afx_msg void OnBrowseDataPath( void );
		afx_msg void OnDefaultDir( void );
		afx_msg void OnUpdateCommandUI( CCmdUI *pCmdUI );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _PROJECTPATHS_H_