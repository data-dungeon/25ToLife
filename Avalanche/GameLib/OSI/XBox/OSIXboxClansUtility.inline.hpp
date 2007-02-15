
//-----------------------------------------------------------------------------
ClansAdapter::ClansAdapter()
: m_playerList( 0 ),
  m_clanList( 0 ),
  m_downloadClanData( 0 ),
  m_downloadClanDataTotal( 0 ),
  m_downloadClanDataFinished( 0 )
{
#ifdef _DEBUG
	++s_clansAdapterInstanceNumber;
#endif
}

//-----------------------------------------------------------------------------
ClansAdapter::~ClansAdapter()
{
	if( m_downloadClanData ) { delete [] m_downloadClanData; }
}

//-----------------------------------------------------------------------------
//static 
void ClansAdapter::OSICreateSingleton() 
{ 
	ASSERT( !ClansAdapter::m_singletonPtr ); 
	m_singletonPtr = new ClansAdapter; 
}

//-----------------------------------------------------------------------------
//static 
void ClansAdapter::OSIDestroySingleton() 
{ 
	ASSERT( ClansAdapter::m_singletonPtr ); 
	delete m_singletonPtr; 
	m_singletonPtr = 0; 
}

//-----------------------------------------------------------------------------
//static
ClansAdapter* ClansAdapter::OSIGetSingleton() 
{ 
	return m_singletonPtr; 
}


//-----------------------------------------------------------------------------
bool 
ClansAdapter::
IsDownloadClanInfoComplete() 
const
{
	#ifdef _DEBUG
	ASSERT( !m_downloadClanData || (m_downloadClanData->instanceNumberAtBegin
		== s_clansAdapterInstanceNumber) );
	#endif //_DEBUG
	
	return !m_downloadClanData
		|| (m_downloadClanDataFinished == m_downloadClanDataTotal);
}

