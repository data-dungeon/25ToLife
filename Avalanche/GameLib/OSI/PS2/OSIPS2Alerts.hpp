#ifndef _OSIPS2ALERTS_HPP_
#define _OSIPS2ALERTS_HPP_

#include "OSI/OSIPlatformTypes.h"
#include "OSI/PS2/EidosAlertsDDL.h"

///------------------------------------------------------------------
/// Used for keeping track of new alerts from RendezVous so an
/// icon can be displayed in the UI to alert the player.
///------------------------------------------------------------------
class OSIPS2Alerts
{
public:
	EidosAlerts alertsCurrent; ///< The currently populated alerts from the server

public:
	OSIPS2Alerts();

	///< Do any of the current alerts from the server differ from our cached version?
	void CheckAlerts();
	///< Clear out all data related to alerts
	void Clear();

	bool AreNewFriendInvitations();
	bool AreNewClanInvitations();
	bool AreNewMatchInvitations();

protected:
	EidosAlerts _alertsCache; ///< The cached alerts from our last call to the server
	bool _areNewFriendInvitations;
	bool _areNewClanInvitations;
	bool _areNewMatchInvitations;

private:

	bool _checkNewAlerts( qList<OSIu32> *current, qList<OSIu32> *cache );
	bool _areSortedListsDifferent( const qList<OSIu32> &one, const qList<OSIu32> &two );
};

inline bool OSIPS2Alerts::AreNewFriendInvitations()
{
	bool isNew = _areNewFriendInvitations;
	_areNewFriendInvitations = false;
	return isNew;
}

inline bool OSIPS2Alerts::AreNewClanInvitations()
{
	bool isNew = _areNewClanInvitations;
	_areNewClanInvitations = false;
	return isNew;
}

inline bool OSIPS2Alerts::AreNewMatchInvitations()
{
	bool isNew = _areNewMatchInvitations;
	_areNewMatchInvitations = false;
	return isNew;
}

#endif /* _OSIPS2ALERTS_HPP_ */