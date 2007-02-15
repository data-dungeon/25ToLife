////////////////////////////////////////////////////////////////////////////
//
// Participant.h
//
// Class to keep track of the data for a single participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Participant.h $
 * 
 * *****************  Version 27  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support load screen tak
 * 
 * *****************  Version 26  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support club, rattle, and blowgun in cut scene
 * 
 * *****************  Version 25  *****************
 * User: Nmartin      Date: 8/06/03    Time: 5:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * new participant type-- event trigger-- and environmental actor drop
 * shadow support
 * 
 * *****************  Version 24  *****************
 * User: Nmartin      Date: 8/02/03    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add restore_to_base_pose processing
 * 
 * *****************  Version 23  *****************
 * User: Nmartin      Date: 8/01/03    Time: 10:02a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * directional lights use rotation data, check for duplicate kfm names
 * (last 7 characters) when generating kfmlst file
 * 
 * *****************  Version 22  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participants
 * 
 * *****************  Version 21  *****************
 * User: Nmartin      Date: 7/24/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * implement motion blur
 * 
 * *****************  Version 20  *****************
 * User: Nmartin      Date: 7/24/03    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support full-screen distortion as part of cut scene
 * 
 * *****************  Version 19  *****************
 * User: Nmartin      Date: 7/22/03    Time: 8:03p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * guard against a single camera that is marked invisible-- produces no
 * datastream
 * 
 * *****************  Version 18  *****************
 * User: Nmartin      Date: 7/21/03    Time: 10:48a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add CParticipant::d_nBuildIndex so that i can make sure that curves
 * have unique names. what was happening was that there were two particle
 * emitter participants that used the same particle emitter, so their
 * names were the same, and so they ended up generating the same curve
 * names
 * 
 * also, add .kfmlst creation
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 2:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * sync the keeper and the participant array on audio participant add
 * 
 * *****************  Version 16  *****************
 * User: Nmartin      Date: 7/02/03    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process audio file attached to cinematic
 * 
 * *****************  Version 15  *****************
 * User: Nmartin      Date: 7/01/03    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove ParentPos and ParentRotation
 * 
 * *****************  Version 14  *****************
 * User: Nmartin      Date: 7/01/03    Time: 9:54a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support parents
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 6/25/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add more curves, check if any instanced curve is valid before counting
 * it, add model_name to particpant
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent and child participants
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/05/03    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * pull env_name out of tup file
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 6/02/03    Time: 2:55p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging cameras works, particle effect works some
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 6/02/03    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * convert static values to engine-friendly values before export to .cut
 * file
 * 
 * *****************  Version 7  *****************
 * User: Nmartin      Date: 6/02/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging multiple participants into a single data stream
 * 
 * *****************  Version 6  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 * 
 * *****************  Version 3  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PARTICIPANT_H
#define PARTICIPANT_H

class ParticipantChannel;
class ParticipantSourceAction;
class ParticipantInstancedAction;
class ParticipantInstancedActionBlend;

#define VISIBILITY_CHANNEL_NOT_SEARCHED	-2
#define VISIBILITY_CHANNEL_NOT_FOUND		-1

class Participant
{
public:

	enum Types
	{
		TYPE_ENVIRONMENTAL_ACTOR,
		TYPE_MAIN_CHARACTER,
		TYPE_LOAD_SCREEN_ACTOR,
		TYPE_PARTICLE_EMITTER,
		TYPE_PARTICLE_EFFECT,
		TYPE_SOUND,
		TYPE_LIGHT,
		TYPE_DISTORTION,
		TYPE_CAMERA,
		TYPE_DUMMY_PARENT,
		TYPE_MOTION_BLUR,
		TYPE_FADE,
		TYPE_TRIGGER,
		TYPE_UNKNOWN,

		/*** if you add to this list, update m_typeNames table in Participant.cpp ***/

		NUM_TYPES // MUST BE LAST
	};

	enum ManuallyAddedParticipantsNodeRefs
	{
		MANUAL_ADDED_AUDIO_NODE_REF_BASE = -1000,
	};

	static char *m_typeNames[NUM_TYPES]; // names for the types

	enum Flags
	{
		FLAGS_EXISTING_ACTOR					= 1<<0,
		FLAGS_RESTORE_TO_BASE_POSE			= 1<<1,
		FLAGS_HAS_TRANSLATE_X_CURVE		= 1<<2,
		FLAGS_HAS_TRANSLATE_Y_CURVE		= 1<<3,
		FLAGS_HAS_TRANSLATE_Z_CURVE		= 1<<4,
		FLAGS_HAS_ROTATE_X_CURVE			= 1<<5,
		FLAGS_HAS_ROTATE_Y_CURVE			= 1<<6,
		FLAGS_HAS_ROTATE_Z_CURVE			= 1<<7,
		FLAGS_HAS_VISIBILITY_TIME			= 1<<8,	// m_fVisibilityTime is valid
		FLAGS_FADE_IN							= 1<<9,
		FLAGS_FADE_OUT							= 1<<10,
		FLAGS_AMBIENT_LIGHT					= 1<<11,
		FLAGS_DIRECTIONAL_LIGHT				= 1<<12,
		FLAGS_LOCAL_LIGHT						= 1<<13,
		FLAGS_ADD_SHADOW						= 1<<14,
		FLAGS_CLUB_VISIBLE					= 1<<15,
		FLAGS_RATTLE_VISIBLE					= 1<<16,
		FLAGS_BLOWGUN_VISIBLE				= 1<<17,
	};

	Participant(void);
	~Participant();

	Types GetType(void) const;
	const char *GetTypeName(void) const;
	void SetType(Types type);

	int GetFlags(void) const;
	void SetFlags(int flags);

	int GetTransformFlags(void) const;
	void SetTransformFlags(int flags);

	const char *GetName(void) const;
	void SetName(const char *pName);

	const char *GetEnvName(void) const;
	void SetEnvName(const char *pName);

	const char *GetModelName(void) const;
	void SetModelName(const char *pName);

	const char *GetTriggerInstanceName(void) const;
	void SetTriggerInstanceName(const char *pName);

	int GetNumChannels(void) const;
	const ParticipantChannel &GetChannel(int channelIndex) const;
	ParticipantChannel &GetChannel(int channelIndex);
	int AddChannel(void);

	int GetNumSourceActions(void) const;
	const ParticipantSourceAction &GetSourceAction(int actionIndex) const;
	ParticipantSourceAction &GetSourceAction(int actionIndex);
	int AddSourceAction(void);

	int GetNumInstancedActions(void) const;
	const ParticipantInstancedAction &GetInstancedAction(int actionIndex) const;
	ParticipantInstancedAction &GetInstancedAction(int actionIndex);
	int AddInstancedAction(void);

	int GetNumInstancedActionBlends(void) const;
	const ParticipantInstancedActionBlend &GetInstancedActionBlend(int blendIndex) const;
	ParticipantInstancedActionBlend &GetInstancedActionBlend(int blendIndex);
	int AddInstancedActionBlend(void);

	int GetNodeRef(void) const;
	void SetNodeRef(int nodeRef);

	int GetNumChildren(void) const;
	int GetChildIndex(int childIndex) const; // get participant index for child in table
	void AddChild(int index);

	int GetParent(void) const;
	void SetParent(int index);


	/* adjust all time values to seconds */

	void ConvertTimeToSeconds(float fFramesPerSecond);

	/* evaluate a channel of a given type at a given time */
	/*** note: result is in converted, game-compatible format ***/

	float EvaluateChannel(int nType, float fTime) const;

	/* access to requested channel's static value */
	/*** note: result is in maya format ***/

	float GetStaticValue(int nType) const;

	/* find point at which visibility state matches passed state, starting at passed time */

	float FindVisibleTime(const bool bVisible, float fTime, const float fMaxTime);

	/* valid if FLAGS_HAS_VISIBILITY_TIME is set, acts like a visibility curve */

	void SetVisibilityTime(float fTime)				{ m_fVisibilityTime = fTime; }
	float GetVisibilityTime()							{ return(m_fVisibilityTime); }

	/* lockout for CinemanFilter::OutputDiscreteEvents processing */

	void SetLockout(float fTime)						{ m_fLockout = fTime; }
	float GetLockout()									{ return(m_fLockout); }

	/* not all participants get exported */

	void SetValidForExport(bool bState)				{ m_bValidForExport = bState; }
	bool ValidForExport()								{ return(m_bValidForExport); }

	/* build index (order in participant list) */

	void SetBuildIndex(int nIndex)					{ m_nBuildIndex = nIndex; }
	int GetBuildIndex()									{ ASSERT(m_nBuildIndex != -1); return(m_nBuildIndex); }

	/* export index (which datastream participant is in) */

	void SetExportIndex(int nIndex)					{ m_nExportIndex = nIndex; }
	int GetExportIndex()									{ ASSERT(m_nExportIndex != -1); return(m_nExportIndex); }

	/* shadow info for environmental actors */

	void SetShadowRadius(float fRadius)				{ m_fShadowRadius = fRadius; }
	float GetShadowRadius()								{ return(m_fShadowRadius); }
	void SetShadowAlpha(float fAlpha)				{ m_fShadowAlpha = fAlpha; }
	float GetShadowAlpha()								{ return(m_fShadowAlpha); }

	/* access to object aggregate-- typically left at NULL, but useful for digging down into tupperware file */

	void SetObjectAgg(TupperwareAggregate *pAgg)	{ m_pObjectAgg = pAgg; }
	TupperwareAggregate *GetObjectAgg()				{ return(m_pObjectAgg); }

	/* set visibility channel index to VISIBILITY_CHANNEL_NOT_FOUND to ignore the possible existence of visibility */
	/* curve and think the participant is always visible. see the routine Participant::FindVisibleTime. this */
	/* helps with a user-error single camera bug, where the camera has a visibility channel, static value is */
	/* off, no instance curve, and the never-visible camera causes the program to ignore all camera curves */

	void SetAlwaysVisible()								{ m_nVisibilityChannelIndex = VISIBILITY_CHANNEL_NOT_FOUND; }

private:

	Types m_type;
	TupArray<ParticipantChannel> *m_pChannels;
	int m_nodeRef;
	TupString m_name;
	TupString m_envName;
	TupString m_modelName;
	TupString m_triggerInstanceName;
	int m_flags;
	int m_transformFlags;
	int m_nVisibilityChannelIndex;	// if any
	float m_fVisibilityTime;			// valid if FLAGS_HAS_VISIBILITY_TIME is set, acts like a visibility curve
	float m_fLockout;
	Keeper<int> m_childrenIndices;
	int m_parentIndex;
	bool m_bValidForExport;
	int m_nBuildIndex;
	int m_nExportIndex;
	float m_fShadowRadius;
	float m_fShadowAlpha;
	TupperwareAggregate *m_pObjectAgg;	// for digging into tupperware data if needed (used for lights)

	TupArray<ParticipantSourceAction> *m_pSourceActions;
	TupArray<ParticipantInstancedAction> *m_pInstancedActions;
	TupArray<ParticipantInstancedActionBlend> *m_pInstancedActionBlends;
};

#endif
