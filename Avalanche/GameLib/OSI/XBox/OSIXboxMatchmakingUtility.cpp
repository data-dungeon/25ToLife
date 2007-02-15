
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "OSIXboxMatchmakingUtility.hpp"

///----------------------------------------------------------------------------
MatchSearchHelper
::MatchSearchHelper()
{
}

///----------------------------------------------------------------------------
MatchSearchHelper
::~MatchSearchHelper()
{
}

///----------------------------------------------------------------------------
void 
MatchSearchHelper
::Reset()
{
	const int searchCnt = m_searches.Count();
	for( int i = 0; i < searchCnt; i++ )
	{
		m_searches[ searchCnt ]->query.Cancel();
		delete m_searches[ searchCnt ];
	}
	m_searches.Reset();
}

///----------------------------------------------------------------------------
bool
MatchSearchHelper
::IsAlreadySearchingFor(
	const OSIMatchID& matchID )
const
{
	const int searchCnt = m_searches.Count();
	for( int i = 0; i < searchCnt; i++ )
	{
		if( m_searches[ i ]->matchID.quadword == matchID.quadword )
		{
			return true;
		}
	}
	return false;
}

///----------------------------------------------------------------------------
void 
MatchSearchHelper
::AddSearch(
    const OSIMatchID& matchID )
{
	if( IsAlreadySearchingFor( matchID ) )
	{
		ASSERT( !IsAlreadySearchingFor( matchID ) );
		return;
	}

	XNKID xnkid;
	OSIMatchID_to_XNKID( matchID, xnkid );
	MatchSearch* newInfo = new MatchSearch( matchID );
	m_searches.Add( newInfo, -1 );
}

///----------------------------------------------------------------------------
void
MatchSearchHelper
::CancelSearch(
   const OSIMatchID& matchID )
{
	const int searchCnt = m_searches.Count();
	for( int i = 0; i < searchCnt; i++ )
	{
		if( m_searches[ i ]->matchID.quadword == matchID.quadword )
		{
			m_searches[ i ]->query.Cancel();
			m_searches.Delete( i );
			return;
		}
	}
}

///----------------------------------------------------------------------------
void 
MatchSearchHelper
::Update()
{
	// Iterate through all of the active searches and give them all a chance to 
	// do any processing they need to.
	const int searchCnt = m_searches.Count();
	for( int i = 0; i < searchCnt; i++ )
	{
		MatchSearch* const searchPtr = m_searches[ i ];
		if( searchPtr->isCompleted )
		{
			continue;
		}

		// Give the search a chance to pump its task.
		const HRESULT hr = searchPtr->query.Process();
		
		// Just go on to the next search if this one hasn't finished.
		if( XONLINETASK_S_RUNNING == hr )
		{
			continue;
		}

		// Mark this search as "completed".
		searchPtr->isCompleted = true;
	}
}

///----------------------------------------------------------------------------
bool
MatchSearchHelper
::GetSearchResults(
	const OSIMatchID& matchID,
	OSIMatch& results,
	bool& isError )
{
	isError = false;

	const int searchCnt = m_searches.Count();
	for( int i = 0; i < searchCnt; i++ )
	{
		if( m_searches[ i ]->matchID.quadword != matchID.quadword )
		{
			continue;
		}
		
		if( !m_searches[ i ]->isCompleted )
		{
			return false;
		}

		// Copy the results.
		MatchSearch* search = m_searches[ i ];
        CSpecificMatchQueryResults& queryResult = search->query.Results;
		CSpecificMatchResult& searchResult = queryResult[0];
		CSpecificMatchSearchResult_to_OSIMatch( searchResult, results );
		isError = (TRUE != search->query.Succeeded());

		ASSERTS( !isError, "MatchSearchHelper::GetSearchResults, match search failed" );
		
		// Free this search.
		m_searches.Delete( i );
		
		return true;
	}

	return false;
}







