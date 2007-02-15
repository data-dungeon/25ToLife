////////////////////////////////////////////////////////////////////////////
//
// Participant
//
// Class to keep track of the data for a single participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Participant.cpp $
 * 
 * *****************  Version 23  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support load screen tak
 * 
 * *****************  Version 22  *****************
 * User: Nmartin      Date: 8/06/03    Time: 5:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * new participant type-- event trigger-- and environmental actor drop
 * shadow support
 * 
 * *****************  Version 21  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participants
 * 
 * *****************  Version 20  *****************
 * User: Nmartin      Date: 7/24/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * implement motion blur
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
 * User: Nmartin      Date: 7/02/03    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process audio file attached to cinematic
 * 
 * *****************  Version 16  *****************
 * User: Nmartin      Date: 7/01/03    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove ParentPos and ParentRotation
 * 
 * *****************  Version 15  *****************
 * User: Nmartin      Date: 7/01/03    Time: 9:54a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support parents
 * 
 * *****************  Version 14  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 6/25/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add more curves, check if any instanced curve is valid before counting
 * it, add model_name to particpant
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent and child participants
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 6/05/03    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * pull env_name out of tup file
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/04/03    Time: 8:36a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * visibility curve activates/deactivates particle emitter, handle
 * transform order flags
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 6/02/03    Time: 5:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * tony's cut scene is working!
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 6/02/03    Time: 2:55p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging cameras works, particle effect works some
 * 
 * *****************  Version 7  *****************
 * User: Nmartin      Date: 6/02/03    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * convert static values to engine-friendly values before export to .cut
 * file
 * 
 * *****************  Version 6  *****************
 * User: Nmartin      Date: 6/02/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging multiple participants into a single data stream
 * 
 * *****************  Version 5  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "Participant.h"
#include "ParticipantChannel.h"
#include "ParticipantSourceAction.h"
#include "ParticipantInstancedAction.h"
#include "ParticipantInstancedActionBlend.h"

char *Participant::m_typeNames[Participant::NUM_TYPES] =
{
	"EnvironmentalActor",	// TYPE_ENVIRONMENTAL_ACTOR
	"MainCharacter",			// TYPE_MAIN_CHARACTER
	"LoadScreenActor",		// TYPE_LOAD_SCREEN_ACTOR
	"ParticleEmitter",		// TYPE_PARTICLE_EMITTER
	"ParticleEffect",			// TYPE_PARTICLE_EFFECT
	"Sound",						// TYPE_SOUND
	"Light",						// TYPE_LIGHT
	"Distortion",				// TYPE_DISTORTION
	"Camera",					// TYPE_CAMERA
	"DummyParent",				// DUMMY_PARENT
	"MotionBlur",				// MOTION_BLUR
	"Fade",						// FADE
	"Trigger",					// TRIGGER
	"Unknown",					// TYPE_UNKNOWN
};

Participant::Participant(void) :
	m_type(TYPE_UNKNOWN),
	m_nodeRef(-1),
	m_parentIndex(-1)
{
	m_pChannels = new TupArray<ParticipantChannel>;
	m_pSourceActions = new TupArray<ParticipantSourceAction>;
	m_pInstancedActions = new TupArray<ParticipantInstancedAction>;
	m_pInstancedActionBlends = new TupArray<ParticipantInstancedActionBlend>;
	m_nVisibilityChannelIndex = VISIBILITY_CHANNEL_NOT_SEARCHED;
	m_bValidForExport = true;
	m_nBuildIndex = -1;
	m_nExportIndex = -1;
	m_fShadowRadius = 0.5f;
	m_fShadowAlpha = 1.0f;
	m_fVisibilityTime = 0.0f;
	m_pObjectAgg = NULL;
}

Participant::~Participant()
{
	delete m_pChannels;
	delete m_pSourceActions;
	delete m_pInstancedActions;
	delete m_pInstancedActionBlends;
}

Participant::Types Participant::GetType(void) const
{
	return m_type;
}

const char *Participant::GetTypeName(void) const
{
	return m_typeNames[m_type];
}

void Participant::SetType(Participant::Types type)
{
	m_type = type;
}

int Participant::GetFlags(void) const
{
	return m_flags;
}

void Participant::SetFlags(int flags)
{
	m_flags = flags;
}

int Participant::GetTransformFlags(void) const
{
	return m_transformFlags;
}

void Participant::SetTransformFlags(int flags)
{
	m_transformFlags = flags;
}

const char *Participant::GetName(void) const
{
	return m_name;
}

void Participant::SetName(const char *pName)
{
	m_name = pName;
}

const char *Participant::GetEnvName(void) const
{
	return m_envName;
}

void Participant::SetEnvName(const char *pName)
{
	m_envName = pName;
}

const char *Participant::GetModelName(void) const
{
	return m_modelName;
}

void Participant::SetModelName(const char *pName)
{
	m_modelName = pName;
}

const char *Participant::GetTriggerInstanceName(void) const
{
	return m_triggerInstanceName;
}

void Participant::SetTriggerInstanceName(const char *pName)
{
	m_triggerInstanceName = pName;
}

int Participant::GetNodeRef(void) const
{
	return m_nodeRef;
}

void Participant::SetNodeRef(int nodeRef)
{
	m_nodeRef = nodeRef;
}

int Participant::GetNumChildren(void) const
{
	return m_childrenIndices.GetSize();
}

int Participant::GetChildIndex(int childIndex) const
{
	return m_childrenIndices[childIndex];
}

void Participant::AddChild(int index)
{
	m_childrenIndices.Add(index);
}

int Participant::GetParent(void) const
{
	return m_parentIndex;
}

void Participant::SetParent(int index)
{
	m_parentIndex = index;
}

int Participant::GetNumChannels(void) const
{
	return m_pChannels->GetSize();
}

const ParticipantChannel &Participant::GetChannel(int channelIndex) const
{
	return (*m_pChannels)[channelIndex];
}

ParticipantChannel &Participant::GetChannel(int channelIndex)
{
	return (*m_pChannels)[channelIndex];
}

int Participant::AddChannel(void)
{
	int newChannelIndex = m_pChannels->GetSize();
	m_pChannels->SetSize(newChannelIndex+1);
	return newChannelIndex;
}

int Participant::GetNumSourceActions(void) const
{
	return m_pSourceActions->GetSize();
}

const ParticipantSourceAction &Participant::GetSourceAction(int actionIndex) const
{
	return (*m_pSourceActions)[actionIndex];
}

ParticipantSourceAction &Participant::GetSourceAction(int actionIndex)
{
	return (*m_pSourceActions)[actionIndex];
}

int Participant::AddSourceAction(void)
{
	int newActionIndex = m_pSourceActions->GetSize();
	m_pSourceActions->SetSize(newActionIndex+1);
	return newActionIndex;
}

int Participant::GetNumInstancedActions(void) const
{
	return m_pInstancedActions->GetSize();
}

const ParticipantInstancedAction &Participant::GetInstancedAction(int actionIndex) const
{
	return (*m_pInstancedActions)[actionIndex];
}

ParticipantInstancedAction &Participant::GetInstancedAction(int actionIndex)
{
	return (*m_pInstancedActions)[actionIndex];
}

int Participant::AddInstancedAction(void)
{
	int newActionIndex = m_pInstancedActions->GetSize();
	m_pInstancedActions->SetSize(newActionIndex+1);
	return newActionIndex;
}

int Participant::GetNumInstancedActionBlends(void) const
{
	return m_pInstancedActionBlends->GetSize();
}

const ParticipantInstancedActionBlend &Participant::GetInstancedActionBlend(int blendIndex) const
{
	return (*m_pInstancedActionBlends)[blendIndex];
}

ParticipantInstancedActionBlend &Participant::GetInstancedActionBlend(int blendIndex)
{
	return (*m_pInstancedActionBlends)[blendIndex];
}

int Participant::AddInstancedActionBlend(void)
{
	int newBlendIndex = m_pInstancedActionBlends->GetSize();
	m_pInstancedActionBlends->SetSize(newBlendIndex+1);
	return newBlendIndex;
}

/* adjust all time values to seconds */

void Participant::ConvertTimeToSeconds(float fFramesPerSecond)
{
	/* maybe nothing to do */

	if (fFramesPerSecond == 1.0f)
		return;

	/* let channels take care of themselves */

	for (int i = 0; i < GetNumChannels(); i++)
		GetChannel(i).ConvertTimeToSeconds(fFramesPerSecond);

	/* actions */

	int numSourceActions = GetNumSourceActions();
	for (int sourceActionIndex=0;sourceActionIndex<numSourceActions;sourceActionIndex++)
	{
		ParticipantSourceAction &sourceAction = GetSourceAction(sourceActionIndex);
		sourceAction.SetStartTime(sourceAction.GetStartTime() / fFramesPerSecond);
		sourceAction.SetDuration(sourceAction.GetDuration() / fFramesPerSecond);
	}
	int numInstancedActions = GetNumInstancedActions();
	for (int instancedActionIndex=0;instancedActionIndex<numInstancedActions;instancedActionIndex++)
	{
		ParticipantInstancedAction &instancedAction = GetInstancedAction(instancedActionIndex);
		instancedAction.SetStartTime(instancedAction.GetStartTime() / fFramesPerSecond);
	}

	/* visibility time that can override visibility curve */

	if (m_flags & FLAGS_HAS_VISIBILITY_TIME)
		m_fVisibilityTime /= fFramesPerSecond;
}

/* find a value for requested channel type at requested time */

float Participant::EvaluateChannel(int nType, float fTime) const
{
	for (int i = 0; i < GetNumChannels(); i++)
	{
		const ParticipantChannel &channel = GetChannel(i);
		if (channel.GetType() == nType)
			return(channel.Evaluate(fTime));
	}
	return(0.0f);	// participant does not have channel of that type
}

/* get static value from requested channel */

float Participant::GetStaticValue(int nType) const
{
	for (int i = 0; i < GetNumChannels(); i++)
	{
		const ParticipantChannel &channel = GetChannel(i);
		if (channel.GetType() == nType)
			return(channel.GetStaticValue());
	}
	return(0.0f);	// participant does not have channel of that type
}

/* find change in visible state. this is used to determine who is the active participant in a merged data stream-- it's the
one who's visible. typically, at the passed time the participant is in the opposite state of bVisible */

float Participant::FindVisibleTime(const bool bVisible, float fTime, const float fMaxTime)
{
	/* a valid m_fVisibilityTime overides any visibility curve (if there is a valid one; there's probably no */
	/* visibility curve) */

	if (m_flags & FLAGS_HAS_VISIBILITY_TIME)
	{
		if (bVisible)
		{
			/* becomes visible at m_fVisibilityTime, never again */

			if (fTime <= m_fVisibilityTime)
				return(m_fVisibilityTime);
			return(fMaxTime + 1.0f);	// never becomes visible again
		}
		else
		{
			/* only visible for a tiny period */

			if ((fTime >= m_fVisibilityTime) && (fTime <= m_fVisibilityTime + 0.01f))
				return(m_fVisibilityTime + 0.01f);
			return(fTime);	// currently not visible
		}
	}

	/* found visibility channel yet? */

	if (m_nVisibilityChannelIndex == VISIBILITY_CHANNEL_NOT_SEARCHED)
	{
		m_nVisibilityChannelIndex = VISIBILITY_CHANNEL_NOT_FOUND;
		for (int i = 0; i < GetNumChannels(); i++)
		{
			if (GetChannel(i).GetType() == ParticipantChannel::TYPE_VISIBILITY)
				m_nVisibilityChannelIndex = i;
		}
	}

	/* if no visibility channel, assume always visible */

	if (m_nVisibilityChannelIndex == VISIBILITY_CHANNEL_NOT_FOUND)
		return(bVisible ? fTime : fMaxTime + 1.0f);	// visible now, never not visible

	/* start looking for when channel matches requested state */

	const ParticipantChannel &channel = GetChannel(m_nVisibilityChannelIndex);
	return(channel.FindVisibleTime(bVisible, fTime, fMaxTime));
}



