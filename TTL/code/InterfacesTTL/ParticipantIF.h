#pragma once

#ifndef PARTICIPANT_IF_H
#define PARTICIPANT_IF_H

#include "Interfaces/Interface.h"
#include "main/TTLConstants.h"
#include "main/ParticipantConfig.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CNetParticipant
{
  public:
	virtual void Initialize(uint8 i_id, NParticipantConfig::TConfig i_config,
			bool i_ticketIssued) {}
	virtual void SetId(uint8 i_id) {}
	virtual void RequestTeam(uint8 i_team) {}
	virtual void SetActive(bool i_active) {}
	virtual void SetSpawnTicket(bool i_ticketIssued) {}
	virtual uint8 GetId(void) const {return(INVALID_PARTICIPANT);}
	virtual uint8 GetRequestedTeam(void) const {return(INVALID_TEAM);}
	virtual bool GetActive(void) const {return(false);}
	virtual bool GetSpawnTicket(void) const {return(false);}
};

class CParticipantIF : public CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_participantID));}
	static CParticipantIF* GetInterface(CActor* i_actor);
	static CParticipantIF* GetInterface(ActorHandle i_handle);

	CParticipantIF() {d_netParticipant = NULL;}
	virtual uint8 GetTeamId(void) {return(INVALID_TEAM);}
	virtual int GetHitPoints(uint8 i_surfaceType) {return(0);}
	virtual float GetDamageMultiplier(uint8 i_surfaceType) {return(1.0f);}
	virtual char * GetSurfaceName(uint8 i_surfaceType) {return(NULL);}
	virtual bool IsAliveOrDying(void) {return(GetAlive());}

	// Manage a distributed representation
	void SetNetParticipant(CNetParticipant* i_netParticipant);
	void MakeAvatar(uint8 i_id);
	void SetParticipantId(uint8 i_id);
	void RequestTeam(uint8 i_team);
	void SetAlive(bool i_alive);
	void SetSpawnTicket(bool i_ticketIssued);
	uint8 GetParticipantId(void);
	uint8 GetRequestedTeamId(void);
	bool GetAlive(void);
	bool GetSpawnTicket(void);

	static uint8 GetParticipantIdFromActor(CActor* i_actor);
	static uint8 GetParticipantIdFromActor(ActorHandle i_actor);

  protected:
	// Local version of potentially distributed data.
	virtual void LocalMakeAvatar(uint8 i_id) {}
	virtual void SetLocalParticipantId(uint8 i_id) {}
	virtual void LocalRequestTeam(uint8 i_team) {}
	virtual void SetLocalAlive(bool i_alive) {}
	virtual void SetLocalSpawnTicket(bool i_ticketIssued) {}
	virtual uint8 GetLocalParticipantId(void) {return(INVALID_PARTICIPANT);}
	virtual uint8 GetLocalRequestedTeamId(void) {return(INVALID_TEAM);}
	virtual bool GetLocalAlive(void) {return(false);}
	virtual bool GetLocalSpawnTicket(void) {return(false);}

  private:
	static uint s_participantID;
	CNetParticipant* d_netParticipant;
};

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::SetNetParticipant(CNetParticipant* i_netParticipant)
{
	if (i_netParticipant == d_netParticipant)
		return;

	uint8 id;
	NParticipantConfig::TConfig config;
	bool spawnTicketIssued;

	NParticipantConfig::SetAssignedTeam(config, GetTeamId());
	NParticipantConfig::SetGhost(config, false);
	if (d_netParticipant != NULL)
	{
		id = d_netParticipant->GetId();
		NParticipantConfig::SetRequestedTeam(config,
				d_netParticipant->GetRequestedTeam());
		NParticipantConfig::SetActive(config, d_netParticipant->GetActive());
		spawnTicketIssued = d_netParticipant->GetSpawnTicket();
	}
	else
	{
		id = GetLocalParticipantId();
		NParticipantConfig::SetRequestedTeam(config, GetLocalRequestedTeamId());
		NParticipantConfig::SetActive(config, GetLocalAlive());
		spawnTicketIssued = GetLocalSpawnTicket();
	}

	d_netParticipant = i_netParticipant;

	if (d_netParticipant != NULL)
		d_netParticipant->Initialize(id, config, spawnTicketIssued);
	else
	{
		SetLocalParticipantId(id);
		LocalRequestTeam(NParticipantConfig::RequestedTeam(config));
		SetLocalAlive(NParticipantConfig::Active(config));
		SetLocalSpawnTicket(spawnTicketIssued);
	}
}

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::MakeAvatar(uint8 i_id)
{
	ASSERT(d_netParticipant == NULL);
	if (d_netParticipant == NULL)
		LocalMakeAvatar(i_id);
}

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::SetParticipantId(uint8 i_id)
{
	if (d_netParticipant != NULL)
		d_netParticipant->SetId(i_id);
	else
		SetLocalParticipantId(i_id);
}

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::RequestTeam(uint8 i_team)
{
	if (d_netParticipant != NULL)
		d_netParticipant->RequestTeam(i_team);
	else
		LocalRequestTeam(i_team);
}

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::SetAlive(bool i_alive)
{
	if (d_netParticipant != NULL)
		d_netParticipant->SetActive(i_alive);
	else
		SetLocalAlive(i_alive);
}

/******************************************************************************/
/******************************************************************************/
inline void CParticipantIF::SetSpawnTicket(bool i_ticketIssued)
{
	if (d_netParticipant != NULL)
		d_netParticipant->SetSpawnTicket(i_ticketIssued);
	else
		SetLocalSpawnTicket(i_ticketIssued);
}

/******************************************************************************/
/******************************************************************************/
inline uint8 CParticipantIF::GetParticipantId(void)
{
	if (d_netParticipant != NULL)
		return(d_netParticipant->GetId());
	return(GetLocalParticipantId());
}

/******************************************************************************/
/******************************************************************************/
inline uint8 CParticipantIF::GetRequestedTeamId(void)
{
	if (d_netParticipant != NULL)
		return(d_netParticipant->GetRequestedTeam());
	return(GetLocalRequestedTeamId());
}

/******************************************************************************/
/******************************************************************************/
inline bool CParticipantIF::GetAlive(void)
{
	if (d_netParticipant != NULL)
		return(d_netParticipant->GetActive());
	return(GetLocalAlive());
}

/******************************************************************************/
/******************************************************************************/
inline bool CParticipantIF::GetSpawnTicket(void)
{
	if (d_netParticipant != NULL)
		return(d_netParticipant->GetSpawnTicket());
	return(GetLocalSpawnTicket());
}

#endif // PARTICIPANT_IF_H
