#ifndef __INTERACTIONMONITOR_H_INCLUDED__
#define __INTERACTIONMONITOR_H_INCLUDED__

#include "Components/Component.h"
#include "Interfaces/Interface.h"

class InteractionMonitor;

class InteractionMonitor : public CActorComponent
{
public:

	typedef enum
	{
		INTERACT_NONE = -1,
		INTERACT_RADIUS, // interact with things within a specified distance from the actor
		INTERACT_FORWARDRADIUS, // interact with things within a specified distance, but only in front of the actor
		INTERACT_FORWARDVECTOR, // interact things that hit a vector shot out along the actor's facing
		INTERACT_FORWARDWEDGE, // interact with things in a 90 degree forward wedge
	}InteractionSearchType;

	// BLATANTLY OBVIOUS NEED:
	// Some parts of this should be game-specific.  Unfortunately, the best solution I can
	// come up with (having the game register interaction types, ranges, surface types, and methods)
	// kinda sucks ass.  Ok, it really sucks ass.  Almost as bad as making the whole thing game-specific
	typedef enum
	{
		INTERACTION_NONE = -1,
		INTERACTION_DOOR_LOCKED,
		INTERACTION_DOOR,
		INTERACTION_DOOR_SLIDE_OPEN,
		INTERACTION_DOOR_SLIDE_CLOSE,
		INTERACTION_DOOR_KICK_OPEN,
		INTERACTION_DOOR_KICK_CLOSE,
		INTERACTION_DOOR_PULL_CLOSE,
		INTERACTION_LADDER,
		INTERACTION_FENCE,
		INTERACTION_TAG_FRAME,
		INTERACTION_VAULT,
		INTERACTION_CLIMB,
		INTERACTION_LIGHT_SWITCH_ON,
		INTERACTION_LIGHT_SWITCH_OFF,
		INTERACTION_SWITCH_OPEN,
		INTERACTION_SWITCH_CLOSE,
		INTERACTION_ELEVATOR_LOCKED,
		INTERACTION_ELEVATOR_SWITCH,
		INTERACTION_HOSTAGE,
		INTERACTION_RELEASEHOSTAGE,
		INTERACTION_APPREHEND,
		INTERACTION_SWAP_WEAPON,
		INTERACTION_SWAP_LOADOUT,
		INTERACTION_JUKEBOX_AUDIO_TRAC,

		INTERACTION_COUNT
	}InteractionType;

	typedef enum
	{
		OBJECT_NONE = 0,
		OBJECT_SPLINE,
		OBJECT_ACTOR,
		OBJECT_STATICOBB,
	}InteractionObjectType;

	typedef InteractionType(*InteractionQuery)( CCompActor &queryActor, void *targetObject, InteractionObjectType objType, InteractionType type );

	// component base stuff
	InteractionMonitor( CCompActor& i_actor );
	virtual ~InteractionMonitor();
	const char *ComponentName()  { return "InteractionMonitor"; }

	void SearchForwardRadius( Vector3CRef offset, float radius ) { searchType = INTERACT_FORWARDRADIUS; searchOffset = offset; searchDistance = radius; }
	void SearchRadius( Vector3CRef offset, float radius ) { searchType = INTERACT_RADIUS; searchOffset = offset; searchDistance = radius; }
	void SearchForwardVector( Vector3CRef offset, float length ) { ASSERTS( 0, "This doesn't actually work completely.  Talk to Travis if you need it to" ); searchType = INTERACT_FORWARDVECTOR; searchOffset = offset; searchDistance = length; }
	void SearchForwardWedge( Vector3CRef offset, float length, float arc ) { searchType = INTERACT_FORWARDWEDGE; searchOffset = offset; searchDistance = length; searchArc = arc; searchArcCos = Math::Cos( arc ); }

	CProtagonist *GetInteractionTargetActor( void ) { if( targetType == OBJECT_ACTOR ) return CProtagonist::FromHandle(currentTargetHandle); else return NULL; }
	void *GetInteractionTargetObject( void ) { return currentTarget; }
	InteractionType GetInteractionType( void ) { return currentInteractionType; }
	InteractionObjectType GetInteractionObjectType( void ) { return targetType; }
	const char *GetInteractionText( void ) { return currentInteractionText; }
	const char *GetInteractionText2( void ) { return currentInteractionText2; }

	float InteractionRangeSquared( InteractionType type );
	
	void SetInteractionText( CProtagonist *actor );
	InteractionType InteractionPossible( CProtagonist *actor );
	bool InFront( Vector3CRef p );
	bool ActorInteractionClear( CProtagonist &interactor, CProtagonist &interactee );

	void BeginFrame( void );
	void BeginUpdate( void );

	void SetInteractionQueryCallback( InteractionType type, InteractionQuery queryFunc ) { pQuery[type] = queryFunc; }
	void ClearViewCursorEntity( void ) { viewCursorEntity = NULL; }
	void ViewCursorEntity( CStageEntity *entity, u8 material );

private:
	void *currentTarget;
	ActorHandle currentTargetHandle;
	InteractionObjectType targetType;
	InteractionType currentInteractionType;
	InteractionSearchType searchType;
	const char *currentInteractionText;
	const char *currentInteractionText2;
	Vector3 searchOffset;
	float searchDistance;
	float searchArc;
	float searchArcCos;

	InteractionQuery pQuery[INTERACTION_COUNT];
	
	u8 fenceMaterial;
	u8 ladderMaterial;
	u8 vaultMaterial;
	u8 climbMaterial;

	CStageEntity *viewCursorEntity;
};

class InteractionIF : public CInterface
{
public:
	static uint GetID(void) {return(CInterface::GetValidID(s_interactionID));}
	virtual InteractionMonitor::InteractionType QueryInteractionType( CCompActor *whoWantsToKnow ) { return InteractionMonitor::INTERACTION_NONE; }

private:
	static uint s_interactionID;
};

#endif //__INTERACTIONMONITOR_H_INCLUDED__
