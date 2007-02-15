#include "OSIPS2Alerts.hpp"

///-------------------------------------------------------------------------
/// Constructor
///-------------------------------------------------------------------------
OSIPS2Alerts::OSIPS2Alerts() :
	_areNewFriendInvitations( false ),
	_areNewClanInvitations( false ),
	_areNewMatchInvitations( false )
{ }

///-------------------------------------------------------------------------
/// Do any of the current alerts from the server differ from our cached version?
///-------------------------------------------------------------------------
void OSIPS2Alerts::CheckAlerts()
{
	// check for new friend invitations
	_areNewFriendInvitations = _checkNewAlerts(
		&alertsCurrent.m_friendInvitations, &_alertsCache.m_friendInvitations
	);
	// check for new clan invitations
	_areNewClanInvitations = _checkNewAlerts(
		&alertsCurrent.m_clanInvitations, &_alertsCache.m_clanInvitations
	);
	// check for new match invitations
	_areNewMatchInvitations = _checkNewAlerts(
		&alertsCurrent.m_matchInvitations, &_alertsCache.m_matchInvitations
	);
}

///-------------------------------------------------------------------------
/// Clear out all data related to alerts
///-------------------------------------------------------------------------
void OSIPS2Alerts::Clear()
{
	alertsCurrent.m_friendInvitations.clear();
	alertsCurrent.m_clanInvitations.clear();
	alertsCurrent.m_matchInvitations.clear();

	_alertsCache.m_friendInvitations.clear();
	_alertsCache.m_clanInvitations.clear();
	_alertsCache.m_matchInvitations.clear();

	_areNewFriendInvitations = false;
	_areNewClanInvitations = false;
	_areNewMatchInvitations = false;
}

///-------------------------------------------------------------------------
/// Check the current list with the cached list to see if they are different
/// and cache off the results if they.
///-------------------------------------------------------------------------
bool OSIPS2Alerts::_checkNewAlerts( qList<OSIu32> *current, qList<OSIu32> *cache )
{
	// check what we have in cache with the new data we have now for differences
	const size_t currentSize = current->size();
	const size_t cacheSize = cache->size();
	// if our current size is 0, clear out the cache
	if( currentSize == 0 )
	{
		cache->clear();
		return false;
	}
	// If the lists are different, update cache and set flag
	if( _areSortedListsDifferent( *current, *cache ) )
	{
		*cache = *current;
		return true;
	}

	return false;
}

///-------------------------------------------------------------------------
/// Are the given sorted lists of unsigned ints different?
///-------------------------------------------------------------------------
bool OSIPS2Alerts::_areSortedListsDifferent( const qList<OSIu32> &one, const qList<OSIu32> &two )
{
	const size_t firstSize = one.size();
	const size_t secondSize = two.size();
	// if the sizes are different, then the lists are not the same
	if( firstSize != secondSize )
	{
		return true;
	}

	// These lists are guaranteed in ascending order so we can make some assumptions
	// to speed up our search since all we care about is if the lists are different.
	qList<OSIu32>::const_iterator firstIter = one.begin();
	qList<OSIu32>::const_iterator secondIter = two.begin();
	// for every item
	for( ; firstIter != one.end(); ++firstIter, ++secondIter )
	{
		// we have to have an existing item in the exact same spot
		const OSIu32 firstListItem = *firstIter;
		const OSIu32 secondListItem = *secondIter;
		if( firstListItem != secondListItem )
		{
			return true;
		}
	}
	
	return false;
}