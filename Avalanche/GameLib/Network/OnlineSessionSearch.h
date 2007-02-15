/*
**
**  File:   NetZSessionSearch.h
**  Date:   October 22, 2004
**  By:     Chris Stark
**  Desc:   Search for Online sessions.
**
*/

#ifndef ONLINE_SESSION_SEARCH_H
#define ONLINE_SESSION_SEARCH_H

// GameLib includes
#include "Network/SessionSearch.h"
#include "OSI/OSIMatchList.hpp"

class COnlineSessionSearch : public CSessionSearch
{
public:
	// Construction
	COnlineSessionSearch();
	~COnlineSessionSearch() { }

	// Conduct search.
	virtual void Begin(void);
	virtual bool Update(bool reFresh = false);
	virtual EEndStatus End(void);

	// Access search results.
	virtual int GetNumFound(void);
	virtual const char* GetName(int i_index);
	virtual const char* GetMapName(int i_index);
	virtual int			GetMapIndex(int i_index);
	virtual const char* GetMode(int i_index);
	virtual const char* GetPlayers(int i_index);
	virtual const char* GetPing(int i_index);
	virtual const char* GetSessionID(int i_index);

	virtual const char* GetConfig(int i_index);

	virtual bool		SpecifyFoundSession(int i_index, CSessionSpec &i_sessionSpec);

	void SetResults(OSIMatchList &results);
	OSIMatchID GetSelectedMatchID() { return _matchID; }
	void MatchJoinResult(bool success);

	static const int GameTypeIndexToStringLUTSize = 7;
	static const char* const GameTypeIndexToStringLUT[ 7 ]; // See the XML code in the .cpp file to see where 6 comes from.

private:


	void _ConvertOSIMatchOptionsToTTLConfigString( 
		const OSIMatch* const match, 
		char* outputString, 
		const unsigned int outputStringMaxLength );

	OSIMatchList	_resultsList;
	OSIStringList	_TTLConfigStringList;	///< This is parallel to _resultsList and represents the TTL config strings generated from the OSIMatchOptions for each match.
	OSIMatchID		_matchID;

	bool				_newResults;
	enum
	{
		IDLE,
		JOINING,
		JOIN_SUCCESS,
		JOIN_FAILURE
	}					_joiningState;
	CSessionSpec*	_sessionSpec;
};

#endif // ONLINE_SESSION_SEARCH_H
