#pragma once

#if 0
class CPluginControlBar;
class CPluginManager
{
	class CPluginInfo
	{
		CString m_strName;
      CString m_strVendor;
      CString m_strDLL;
		UINT m_nBarCommandID;
		CPluginControlBar *m_pBar;
      
	public:
		CPluginInfo(
			LPCTSTR strName = NULL,
			LPCTSTR strVendor = NULL,
			LPCTSTR strDLL = NULL,
			UINT nBarCommandID = 0
			)
			: m_strName( (strName == NULL) ? _T("") : strName )
			, m_strVendor( (strVendor == NULL) ? _T("") : strVendor )
			, m_strDLL( (strDLL == NULL) ? _T("") : strDLL )
			, m_nBarCommandID( 0 )
			, m_pBar( NULL )
		{
		}
		CPluginInfo( const CPluginInfo &other )
		{
			m_nBarCommandID = other.m_nBarCommandID;
			m_pBar = other.m_pBar;
			m_strName = other.m_strName;
			m_strVendor = other.m_strVendor;
			m_strDLL = other.m_strDLL;
		}
		CPluginInfo & operator=( const CPluginInfo & other )
		{
			m_nBarCommandID = other.m_nBarCommandID;
			m_pBar = other.m_pBar;
			m_strName = other.m_strName;
			m_strVendor = other.m_strVendor;
			m_strDLL = other.m_strDLL;
			return (*this);
		}
	private:
		CPluginControlBar * GetBar(
			CFrameWnd * pFrame,
			LPCTSTR strName = NULL,
			bool bCreateNewInvisible = true
			)
		{
			if( m_pBar != NULL )
				return m_pBar;
			ASSERT_VALID( pFrame );
			ASSERT( pFrame->GetSafeHwnd() != NULL );
			if( strName != NULL )
				m_strName = strName;
			CExtCmdItem * pCmdItem = NULL;
			CString strCmdProfileName = g_CmdManager->ProfileNameFromWnd( pFrame->GetSafeHwnd() );
			ASSERT( ! strCmdProfileName.IsEmpty() );
			if( m_nBarCommandID == 0 )
			{
				pCmdItem = g_CmdManager->CmdAllocPtr( strCmdProfileName );
			} // if( m_nBarCommandID == 0 )
			else
			{
				pCmdItem = g_CmdManager->CmdGetPtr( strCmdProfileName, m_nBarCommandID );
				if( pCmdItem == NULL )
					pCmdItem = g_CmdManager->CmdAllocPtr( strCmdProfileName, m_nBarCommandID );
			} // else from if( m_nBarCommandID == 0 )
			if( pCmdItem == NULL )
			{
				ASSERT( false );
				return NULL;
			}
			m_nBarCommandID = pCmdItem->m_nCmdID;
			pCmdItem->StateSetBasic();
			if( ! m_strName.IsEmpty() )
			{
				if( pCmdItem->m_sMenuText.IsEmpty() )
					pCmdItem->m_sMenuText = m_strName;
				if( pCmdItem->m_sToolbarText.IsEmpty() )
					pCmdItem->m_sToolbarText = m_strName;
			}
			CControlBar * pBar = pFrame->GetControlBar( m_nBarCommandID );
			if( pBar != NULL )
			{
				m_pBar = DYNAMIC_DOWNCAST( CPluginControlBar, pBar );
				ASSERT( m_pBar != NULL );
				return m_pBar;
			}
			m_pBar = new CPluginControlBar();
			m_pBar->m_bAutoDelete = true;
//         if (!m_pBar->DoCreate(pFrame, m_nBarCommandID, bCreateNewInvisible))
//            return NULL;
			return m_pBar;
		}
      
		void Serialize( CArchive & ar )
		{
			if( ar.IsStoring() )
			{
				ar << m_strName;
			   ar << m_strVendor;
			   ar << m_strDLL;
				ar << m_nBarCommandID;
			}
			else
			{
				ar >> m_strName;
            ar >> m_strVendor;
            ar >> m_strDLL;
				ar >> m_nBarCommandID;
			}
		}
		bool OnCmdMsg(
			UINT nID,
			int nCode,
			void * pExtra,
			AFX_CMDHANDLERINFO * pHandlerInfo
			)
		{
			pHandlerInfo;
			if( nID != m_nBarCommandID )
				return false;
			if( m_pBar->GetSafeHwnd() == NULL )
				return false;
			if( m_pBar->m_pDockSite == NULL )
				return false;
			if( nCode == CN_COMMAND )
			{
				if( CExtControlBar::DoFrameBarCheckCmd( m_pBar->m_pDockSite, nID ) )
					return true;
				return false;
			}
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI =  (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				CExtControlBar::DoFrameBarCheckUpdate( m_pBar->m_pDockSite, pCmdUI );
				return true;
			}
			return false;
		}
		void DoUnInstall(
			CFrameWnd * pFrame,
			bool bRecalcFrameLayout = true
			)
		{
			if( m_pBar != NULL )
			{
				if( m_pBar->m_hWnd == NULL )
				{
					delete m_pBar;
				} // if( m_pBar->m_hWnd == NULL )
				else
				{
					if(	m_pBar->AutoHideModeGet() )
						m_pBar->AutoHideModeSet(
							false,
							false,
							bRecalcFrameLayout,
							true
							);
					else
						pFrame->ShowControlBar(
							m_pBar,
							false,
							bRecalcFrameLayout ? false : true
							);
					pFrame->RemoveControlBar( m_pBar );
					m_pBar->DestroyWindow();
				} // else from if( m_pBar->m_hWnd == NULL )
				m_pBar = NULL;
			} // if( m_pBar != NULL )
			if( m_nBarCommandID != 0 )
			{
				CString strCmdProfileName = g_CmdManager->ProfileNameFromWnd( pFrame->GetSafeHwnd() );
				ASSERT( ! strCmdProfileName.IsEmpty() );
				if( g_CmdManager->CmdGetPtr(
						strCmdProfileName,
						m_nBarCommandID
						) != NULL
					)
					g_CmdManager->CmdRemove(
						strCmdProfileName,
						m_nBarCommandID
						);
				m_nBarCommandID = 0;
			} // if( m_nBarCommandID != 0 )
		}
		friend class CPluginManager;
	};
	CArray < CPluginInfo, CPluginInfo & > m_arrPlugins;
public:
	CPluginManager()
	{
	}
private:
	void Serialize( CArchive & ar )
	{
		DWORD dwPluginCount;
		if( ar.IsStoring() )
		{
			dwPluginCount = m_arrPlugins.GetSize();
			ar << dwPluginCount;
			for( DWORD dwPluginIdx = 0; dwPluginIdx < dwPluginCount; dwPluginIdx++ )
			{
				CPluginInfo & _PluginInfo =
					m_arrPlugins.ElementAt( dwPluginIdx );
				_PluginInfo.Serialize( ar );
			}
		}
		else
		{
			ASSERT( m_arrPlugins.GetSize() == 0 );
			ar >> dwPluginCount;
			for( DWORD dwPluginIdx = 0; dwPluginIdx < dwPluginCount; dwPluginIdx++ )
			{
				CPluginInfo _PluginInfo;
				_PluginInfo.Serialize( ar );
				m_arrPlugins.Add( _PluginInfo );
			}
			ASSERT( DWORD(m_arrPlugins.GetSize()) == dwPluginCount );
		}
	}
	static CString CalcSubsystemRegKeyPath(
		LPCTSTR sSubsystemName,
		LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
		LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
		)
	{
		ASSERT( sSubsystemName != NULL );
		ASSERT( sSectionNameCompany != NULL );
		ASSERT( sSectionNameProduct != NULL );
		// use the same version numbers with Prof-UIS
		DWORD dwVersion = g_CmdManager.GetVersionDWORD( false );
		ASSERT( dwVersion != 0 );
		CString s;
		s.Format(
			_T("Software\\%s\\%s\\%s"),
			sSectionNameCompany,
			sSectionNameProduct,
			sSubsystemName
			);
		s.Replace('\r','_');
		s.Replace('\t','_');
		s.Replace('\n','_');
		s.Replace('?','_');
		s.Replace('*','_');
//		s.Replace('.','_'); SNIPE
//		s.Replace(',','_'); SNIPE
		return s;
	}
	static CString CalcSubsystemRegKeyPath(
		LPCTSTR sSubsystemName
		)
	{
		CWinApp * pApp = ::AfxGetApp();
		ASSERT( pApp != NULL );
		ASSERT( pApp->m_pszRegistryKey != NULL );
		ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
		ASSERT( pApp->m_pszProfileName != NULL );
		ASSERT( pApp->m_pszProfileName[0] != _T('\0') );
		return
			CalcSubsystemRegKeyPath(
				sSubsystemName,
				pApp->m_pszRegistryKey,
				pApp->m_pszProfileName
				);
	}
public:
	bool RegistryStateLoad(
		CFrameWnd * pFrame,
		bool bEnsureAllBarsCreated = true
		) throw()
	{
		ASSERT_VALID( pFrame );
		try
		{
			CString strRegKeyPath =
				CalcSubsystemRegKeyPath(
					_T("PluginManagerForControlBars")
					);
			CMemFile _file;
			if( ! CExtCmdManager::FileObjFromRegistry(
					_file,
					strRegKeyPath
					)
				)
			{
				// first run
				return true;
			}
			CArchive ar( &_file, CArchive::load );
			Serialize( ar );
			if( !bEnsureAllBarsCreated )
				return true;
			return EnsureAllBarsCreated( pFrame );
		}
		catch( CException * pException )
		{
			ASSERT( false );
			pException->Delete();
			return false;
		}
		catch( ... )
		{
			ASSERT( false );
			return false;
		}
	}
	bool RegistryStateSave(
		) throw()
	{
		try
		{
			CString strRegKeyPath =
				CalcSubsystemRegKeyPath(
					_T("PluginManagerForControlBars")
					);
			CMemFile _file;
			CArchive ar( &_file, CArchive::store );
			Serialize( ar );
			ar.Flush();
			ar.Close();
			if( ! CExtCmdManager::FileObjToRegistry(
					_file,
					strRegKeyPath
					)
				)
			{
				ASSERT( false );
				return false;
			}
			return true;
		}
		catch( CException * pException )
		{
			ASSERT( false );
			pException->Delete();
			return false;
		}
		catch( ... )
		{
			ASSERT( false );
			return false;
		}
	}

   bool RegisterPluginControlBars(CFrameWnd * pFrame)
   {
   	if ( RegistryStateLoad(pFrame) )
		   return false;
/*
      int count = theApp.m_hxtpluginreg.m_pluginentries.GetSize();
      for (int i = 0; i < count ; i++){
         HxtPluginEntry *entry = theApp.m_hxtpluginreg.m_pluginentries[i];
         String strDLL= entry->m_filename.GetNameOnlyWithoutSuffix();
         POSITION infopos;
         for (infopos = entry->m_controlbarinfos.GetHeadPosition(); infopos ; ) {
            PluginControlbarInfo info = entry->m_controlbarinfos.GetNext(infopos);
            UINT nCommandID;
            CPluginControlBar *pBar = m_pluginmanager.FindBar(info.m_strName.Get(), info.m_strVendor.Get(), strDLL.Get(), &nCommandID);
            if (!pBar)
               m_pluginmanager.PluginInstall( this, info.m_strName.Get(), info.m_strVendor.Get(), strDLL.Get(), &nCommandID, &pBar );

            if (pBar) 
               pBar->m_bInstalledSuccessful = pBar->DoCreate(this, nCommandID, entry, info.m_nIndex);
         }
      }
*/      
      PluginUninstallAllInvalid(pFrame);
      return true;
   }

private:
	bool EnsureAllBarsCreated( CFrameWnd * pFrame )
	{
		ASSERT_VALID( pFrame );
		DWORD dwPluginCount = m_arrPlugins.GetSize();
		DWORD dwBarsCreated = 0;
		for( DWORD dwPluginIdx = 0; dwPluginIdx < dwPluginCount; dwPluginIdx++ )
		{
			CPluginInfo & _PluginInfo =
				m_arrPlugins.ElementAt( dwPluginIdx );
			CExtControlBar * pBar = _PluginInfo.GetBar( pFrame );
			if( pBar == NULL )
			{
				ASSERT( false );
				continue;
			}
			dwBarsCreated++;
		}
		if( dwBarsCreated != dwPluginCount )
			return false;
		return true;
	}
public:
	int PluginGetCount() const
	{
		return m_arrPlugins.GetSize();
	}
	bool PluginInfoGetAt(
		int nPluginIdx,
		UINT * p_nCommandID = NULL,
		CString * p_strName = NULL,
		CString * p_strVendor = NULL,
		CString * p_strDLL = NULL,
		CPluginControlBar ** p_pBar = NULL
		) const
	{
		if( p_nCommandID != NULL )
			(*p_nCommandID) = 0;
		if( p_strName != NULL )
			(*p_strName).Empty();
		if( p_strVendor != NULL )
			(*p_strVendor).Empty();
		if( p_strDLL != NULL )
			(*p_strDLL).Empty();
		if( p_pBar != NULL )
			(*p_pBar) = NULL;
		if( 0 > nPluginIdx || nPluginIdx > m_arrPlugins.GetSize() )
		{
			ASSERT( false );
			return false;
		}
		CPluginInfo & _PluginInfo =
			( const_cast < CPluginManager * > ( this ) ) ->
				m_arrPlugins.ElementAt( nPluginIdx );
		if( p_nCommandID != NULL )
			(*p_nCommandID) = _PluginInfo.m_nBarCommandID;
		if( p_strName != NULL )
			(*p_strName) = _PluginInfo.m_strName;
		if( p_strVendor != NULL )
			(*p_strVendor) = _PluginInfo.m_strVendor;
		if( p_strDLL != NULL )
			(*p_strDLL) = _PluginInfo.m_strDLL;
		if( p_pBar != NULL )
			(*p_pBar) = _PluginInfo.m_pBar;
		return true;
	}
   
   CPluginControlBar *FindBar(const CString &strMatchName, const CString &strMatchVendor, const CString &strMatchDLL, UINT *nOutCommandID )
   {
      CString strName, strVendor, strDLL;
      CPluginControlBar *pBar;
      for (int i=0; i<PluginGetCount(); i++) {
         PluginInfoGetAt( i, nOutCommandID, &strName, &strVendor, &strDLL, &pBar);
         if (strMatchName == strName &&
          strVendor == strMatchVendor &&
          strDLL == strMatchDLL)
            return pBar;
      }
      return NULL;
   }
   
	int PluginInstall( // returns plugin's index or -1 on error
		CFrameWnd * pFrame,
		LPCTSTR strName,
		LPCTSTR strVendor,
		LPCTSTR strDLL,
      UINT *nOutCommandID,
      CPluginControlBar **pOutBar,
		bool bCreateInvisible = true)
	{
		ASSERT_VALID( pFrame );
      if (pOutBar)
         *pOutBar = NULL;

      if (nOutCommandID)
         *nOutCommandID = 0;
               
		int nPluginIdx = m_arrPlugins.GetSize();
		CPluginInfo _PluginInfo;
		CPluginControlBar *pBar =
			_PluginInfo.GetBar( pFrame, strName, bCreateInvisible );
		if( pBar == NULL )
		{
			ASSERT( false );
			return -1;
		}
      _PluginInfo.m_strVendor = strVendor;
      _PluginInfo.m_strDLL = strDLL;
		m_arrPlugins.Add( _PluginInfo );
      if (pOutBar)
         *pOutBar = _PluginInfo.m_pBar;

      if (nOutCommandID)
         *nOutCommandID = _PluginInfo.m_nBarCommandID;
         
		return nPluginIdx;
	}
	bool PluginUnInstall(
		CFrameWnd * pFrame,
		int nPluginIdx,
		bool bRecalcFrameLayout = true
		)
	{
		ASSERT_VALID( pFrame );
		if( 0 > nPluginIdx || nPluginIdx > m_arrPlugins.GetSize() )
		{
			ASSERT( false );
			return false;
		}
		CPluginInfo & _PluginInfo =
			m_arrPlugins.ElementAt( nPluginIdx );
		_PluginInfo.DoUnInstall( pFrame, bRecalcFrameLayout );
		m_arrPlugins.RemoveAt( nPluginIdx );
		return true;
	}
	bool PluginUnInstallAll(
		CFrameWnd * pFrame,
		bool bRecalcFrameLayout = true
		)
	{
		while( PluginGetCount() > 0 )
		{
			if( ! PluginUnInstall( pFrame, 0, bRecalcFrameLayout ) )
				return false;
		}
		return true;
	}
   
   bool PluginUninstallAllInvalid(CFrameWnd * pFrame,
		bool bRecalcFrameLayout = true)
   {
		DWORD dwPluginCount = m_arrPlugins.GetSize();
		for( DWORD dwPluginIdx = 0; dwPluginIdx < dwPluginCount; dwPluginIdx++ )
		{
			CPluginInfo &_PluginInfo = m_arrPlugins.ElementAt( dwPluginIdx );
         if (_PluginInfo.m_pBar==NULL || _PluginInfo.m_pBar->m_bInstalledSuccessful == false)
			   PluginUnInstall( pFrame, dwPluginIdx, bRecalcFrameLayout );
      }
      return true;
   }

	bool OnCmdMsg(
		UINT nID,
		int nCode,
		void * pExtra,
		AFX_CMDHANDLERINFO * pHandlerInfo
		)
	{
		DWORD dwPluginCount = m_arrPlugins.GetSize();
		for( DWORD dwPluginIdx = 0; dwPluginIdx < dwPluginCount; dwPluginIdx++ )
		{
			CPluginInfo & _PluginInfo =
				m_arrPlugins.ElementAt( dwPluginIdx );
			if( _PluginInfo.OnCmdMsg(
					nID,
					nCode,
					pExtra,
					pHandlerInfo
					)
				)
				return true;
		}
		return false;
	}
};
#endif