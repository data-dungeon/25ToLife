////////////////////////////////////////////////////////////////////////////
//
// ParticipantBuilder.cpp
//
// Class to build the array of participants
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantBuilder.cpp $
 * 
 * *****************  Version 25  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support load screen tak
 * 
 * *****************  Version 24  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support club, rattle, and blowgun in cut scene
 * 
 * *****************  Version 23  *****************
 * User: Nmartin      Date: 8/06/03    Time: 5:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * new participant type-- event trigger-- and environmental actor drop
 * shadow support
 * 
 * *****************  Version 22  *****************
 * User: Nmartin      Date: 8/02/03    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add restore_to_base_pose processing
 * 
 * *****************  Version 21  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participants
 * 
 * *****************  Version 20  *****************
 * User: Nmartin      Date: 7/24/03    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support full-screen distortion as part of cut scene
 * 
 * *****************  Version 19  *****************
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
 * *****************  Version 18  *****************
 * User: Nmartin      Date: 7/18/03    Time: 10:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * compress curve data a bit
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 7/18/03    Time: 3:33p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * added change node names function
 * 
 * *****************  Version 16  *****************
 * User: Nmartin      Date: 7/18/03    Time: 10:12a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 7/17/03    Time: 3:47p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 7/17/03    Time: 3:41p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * give warning when not using independent euler rotation interpolation
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 2:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * sync the keeper and the participant array on audio participant add
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:51a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent fixup bug 
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 7/02/03    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process audio file attached to cinematic
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/25/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add more curves, check if any instanced curve is valid before counting
 * it, add model_name to particpant
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent and child participants
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 6/05/03    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * pull env_name out of tup file
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 1:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * instanced curve starts at source curve time (non action type)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 2:38p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * static values from parent transform (when skin is attached)
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
 * User: Adam Clayton Date: 5/27/03    Time: 10:29a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first pass of participant manager
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "ParticipantBuilder.h"
#include "Participant.h"
#include "ParticipantChannel.h"
#include "ParticipantSourceAction.h"
#include "ParticipantInstancedAction.h"
#include "ParticipantInstancedActionBlend.h"

ParticipantBuilder::ParticipantBuilder(TupperwareAggregate *pSceneAgg,TupArray<Participant> *pParticipants,Keeper<int> *pParticipantNodeRefs) :
		m_pSceneAgg(pSceneAgg),
		m_pParticipants(pParticipants),
		m_pParticipantNodeRefs(pParticipantNodeRefs)
{
	m_exclusions = NULL;

	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);
	
	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
	
	m_pTupScene->GetRootNodeReference(m_rootNodeReference);
	assert(m_rootNodeReference>=0);
	m_pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeReference);
	assert(m_pRootNodeAgg);
	m_pTupRootNode = new TupNode(m_pRootNodeAgg);
	
	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

	m_pAnimSetPoolAgg = m_pTupScene->GetAnimSetPool();
	assert(m_pAnimSetPoolAgg);
	m_pTupAnimSetPool = new TupAnimSetPool( m_pAnimSetPoolAgg );
	
	m_pTupNodeConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);

	ChangeNodeNames();
	GetParticipants();
	AddParents();
	AddAnimation();

	/* set build indices */

	for (int i = 0; i < m_pParticipants->GetSize(); i++)
		(*m_pParticipants)[i].SetBuildIndex(i);
}

ParticipantBuilder::ParticipantBuilder(TupperwareAggregate *pSceneAgg,TupArray<Participant> *pParticipants,Keeper<int> *pParticipantNodeRefs,CExclusions	*exclusions) :
		m_pSceneAgg(pSceneAgg),
		m_pParticipants(pParticipants),
		m_pParticipantNodeRefs(pParticipantNodeRefs)
{
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);
	
	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
	
	m_pTupScene->GetRootNodeReference(m_rootNodeReference);
	assert(m_rootNodeReference>=0);
	m_pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeReference);
	assert(m_pRootNodeAgg);
	m_pTupRootNode = new TupNode(m_pRootNodeAgg);
	
	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

	m_pAnimSetPoolAgg = m_pTupScene->GetAnimSetPool();
	assert(m_pAnimSetPoolAgg);
	m_pTupAnimSetPool = new TupAnimSetPool( m_pAnimSetPoolAgg );
	
	m_pTupNodeConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);

	m_exclusions = exclusions;

	ChangeNodeNames();
	GetParticipants();
	AddParents();
	AddAnimation();

	/* set build indices */

	for (int i = 0; i < m_pParticipants->GetSize(); i++)
		(*m_pParticipants)[i].SetBuildIndex(i);
}

ParticipantBuilder::~ParticipantBuilder()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupRootNode;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupAnimSetPool;
	delete m_pTupNodeConfigRoot;
}

void ParticipantBuilder::GetParticipants(void)
{
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );
		char *pNodeName;
		tupNode.GetName(&pNodeName);

		int objectRef;
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		bool bValidParticipant = false;
		// check based on object type
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::CAMERA)
			bValidParticipant = true;

		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "cut_scene_enable" );
		tupKeyValueContext.AddKey( "cut_scene_type" );
		tupKeyValueContext.AddKey( "existing_actor" );
		tupKeyValueContext.AddKey( "cut_scene_emitter" );
		tupKeyValueContext.AddKey( "cut_scene_effect" );
		tupKeyValueContext.AddKey( "cut_scene_sound" );
		tupKeyValueContext.AddKey( "class" );
		tupKeyValueContext.AddKey( "model_name" );
		tupKeyValueContext.AddKey( "fade_type" );
		tupKeyValueContext.AddKey( "restore_to_base_pose" );
		tupKeyValueContext.AddKey( "cut_scene_shadow" );
		tupKeyValueContext.AddKey( "cut_scene_shadow_diameter" );
		tupKeyValueContext.AddKey( "cut_scene_shadow_alpha" );
		tupKeyValueContext.AddKey( "cut_scene_trigger_event" );
		tupKeyValueContext.AddKey( "cut_scene_trigger_instance" );
		tupKeyValueContext.AddKey( "club_visible" );
		tupKeyValueContext.AddKey( "rattle_visible" );
		tupKeyValueContext.AddKey( "blowgun_visible" );
		
		const int CUT_SCENE_ENABLE = 0;
		const int CUT_SCENE_TYPE = 1;
		const int EXISTING_ACTOR = 2;
		const int CUT_SCENE_EMITTER = 3;
		const int CUT_SCENE_EFFECT = 4;
		const int CUT_SCENE_SOUND = 5;
		const int CLASS = 6;
		const int MODEL_NAME = 7;
		const int FADE_TYPE = 8;
		const int RESTORE_TO_BASE_POSE = 9;
		const int CUT_SCENE_SHADOW = 10;
		const int CUT_SCENE_SHADOW_DIAMETER = 11;
		const int CUT_SCENE_SHADOW_ALPHA = 12;
		const int CUT_SCENE_TRIGGER_EVENT = 13;
		const int CUT_SCENE_TRIGGER_INSTANCE = 14;
		const int CLUB_VISIBLE = 15;
		const int RATTLE_VISIBLE = 16;
		const int BLOWGUN_VISIBLE = 17;

		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
		
		// is this node marked to be in the cut scene?
		if (bValidParticipant || (tupKeyValueContext.ValueValidAt(CUT_SCENE_ENABLE) && tupKeyValueContext.GetValueAsBool(CUT_SCENE_ENABLE)))
		{
			// set up a new one
			int numParticipants = m_pParticipants->GetSize();
			m_pParticipants->SetSize(numParticipants+1);
			Participant &participant = (*m_pParticipants)[numParticipants];

			// set up keeper for fast searches
			m_pParticipantNodeRefs->Add(nodeIndex);
			m_participantChannelTypes.SetSize(numParticipants+1);
			Keeper<int> &channelTypes = m_participantChannelTypes[numParticipants];

			int flags = 0;
			TupString name(pNodeName);				// start with node name
			TupString envName("");					// for environmental actors that need to be created for cut scene
			TupString modelName("");				// for environmental actors that need to be created for cut scene
			TupString triggerInstanceName("");	// trigger target

			TupString typeName = "env_actor"; // default
			if (tupKeyValueContext.ValueValidAt(CUT_SCENE_TYPE))
			{
				typeName = tupKeyValueContext.GetValueAsString(CUT_SCENE_TYPE);
			}

			if (!strcmp("env_actor",typeName))
			{
				if (tupKeyValueContext.ValueValidAt(EXISTING_ACTOR) && tupKeyValueContext.GetValueAsBool(EXISTING_ACTOR))
					flags |= Participant::FLAGS_EXISTING_ACTOR;
				if (tupKeyValueContext.ValueValidAt(RESTORE_TO_BASE_POSE) && tupKeyValueContext.GetValueAsBool(RESTORE_TO_BASE_POSE))
					flags |= Participant::FLAGS_RESTORE_TO_BASE_POSE;
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_SHADOW) && tupKeyValueContext.GetValueAsBool(CUT_SCENE_SHADOW))
					flags |= Participant::FLAGS_ADD_SHADOW;
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_SHADOW_DIAMETER))
					participant.SetShadowRadius(0.5f * tupKeyValueContext.GetValueAsFloat(CUT_SCENE_SHADOW_DIAMETER));
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_SHADOW_ALPHA))
					participant.SetShadowAlpha(tupKeyValueContext.GetValueAsFloat(CUT_SCENE_SHADOW_ALPHA));
				participant.SetType(Participant::TYPE_ENVIRONMENTAL_ACTOR);

				/* if this is an actor that needs to be created, artist uses the objattr environmental actor */
				/* menus to mark what it is. grab some info needed to create it */

				if (tupKeyValueContext.ValueValidAt(CLASS))
					envName = tupKeyValueContext.GetValueAsString(CLASS);
				if (tupKeyValueContext.ValueValidAt(MODEL_NAME))
					modelName = tupKeyValueContext.GetValueAsString(MODEL_NAME);
				if ((modelName == "") || (modelName == "None"))
					modelName = envName;
			}
			else if (!strcmp("tak",typeName))
			{
				participant.SetType(Participant::TYPE_MAIN_CHARACTER);
				if (tupKeyValueContext.ValueValidAt(CLUB_VISIBLE) && tupKeyValueContext.GetValueAsBool(CLUB_VISIBLE))
					flags |= Participant::FLAGS_CLUB_VISIBLE;
				if (tupKeyValueContext.ValueValidAt(RATTLE_VISIBLE) && tupKeyValueContext.GetValueAsBool(RATTLE_VISIBLE))
					flags |= Participant::FLAGS_RATTLE_VISIBLE;
				if (tupKeyValueContext.ValueValidAt(BLOWGUN_VISIBLE) && tupKeyValueContext.GetValueAsBool(BLOWGUN_VISIBLE))
					flags |= Participant::FLAGS_BLOWGUN_VISIBLE;
			}
			else if (!strcmp("load_screen_tak",typeName))
			{
				participant.SetType(Participant::TYPE_LOAD_SCREEN_ACTOR);
				if (tupKeyValueContext.ValueValidAt(CLUB_VISIBLE) && tupKeyValueContext.GetValueAsBool(CLUB_VISIBLE))
					flags |= Participant::FLAGS_CLUB_VISIBLE;
				if (tupKeyValueContext.ValueValidAt(RATTLE_VISIBLE) && tupKeyValueContext.GetValueAsBool(RATTLE_VISIBLE))
					flags |= Participant::FLAGS_RATTLE_VISIBLE;
				if (tupKeyValueContext.ValueValidAt(BLOWGUN_VISIBLE) && tupKeyValueContext.GetValueAsBool(BLOWGUN_VISIBLE))
					flags |= Participant::FLAGS_BLOWGUN_VISIBLE;
			}
			else if (!strcmp("particle_emitter",typeName))
			{
				participant.SetType(Participant::TYPE_PARTICLE_EMITTER);
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_EMITTER))
					name = tupKeyValueContext.GetValueAsString(CUT_SCENE_EMITTER);
			}
			else if (!strcmp("particle_effect",typeName))
			{
				participant.SetType(Participant::TYPE_PARTICLE_EFFECT);
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_EFFECT))
					name = tupKeyValueContext.GetValueAsString(CUT_SCENE_EFFECT);
			}
			else if (!strcmp("sound",typeName))
			{
				participant.SetType(Participant::TYPE_SOUND);
				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_SOUND))
					name = tupKeyValueContext.GetValueAsString(CUT_SCENE_SOUND);
			}
			else if (!strcmp("light",typeName))
			{
				participant.SetType(Participant::TYPE_LIGHT);
			}
			else if (!strcmp("Distortion",typeName))
			{
				participant.SetType(Participant::TYPE_DISTORTION);
			}
			else if (!strcmp("motion_blur",typeName))
			{
				participant.SetType(Participant::TYPE_MOTION_BLUR);
			}
			else if (!strcmp("fade",typeName))
			{
				participant.SetType(Participant::TYPE_FADE);
				if (tupKeyValueContext.ValueValidAt(FADE_TYPE))
				{
					const char *pFadeType = tupKeyValueContext.GetValueAsString(FADE_TYPE);
					if (strcmp(pFadeType, "in") == 0)
						flags |= Participant::FLAGS_FADE_IN;
					else if (strcmp(pFadeType, "out") == 0)
						flags |= Participant::FLAGS_FADE_OUT;
					else if (strcmp(pFadeType, "in_out") == 0)
						flags |= (Participant::FLAGS_FADE_IN | Participant::FLAGS_FADE_OUT);
					else
						ASSERT(false);
				}
				else
				{
					ASSERT(false);
					flags |= (Participant::FLAGS_FADE_IN | Participant::FLAGS_FADE_OUT);	// fade both in and out is default
				}
			}
			else if (!strcmp("cut_scene_trigger",typeName))
			{
				participant.SetType(Participant::TYPE_TRIGGER);

				/* what kind of event it is (e.g. "go") goes in name */

				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_TRIGGER_EVENT))
					name = tupKeyValueContext.GetValueAsString(CUT_SCENE_TRIGGER_EVENT);

				/* target an instance */

				if (tupKeyValueContext.ValueValidAt(CUT_SCENE_TRIGGER_INSTANCE))
					triggerInstanceName = tupKeyValueContext.GetValueAsString(CUT_SCENE_TRIGGER_INSTANCE);
			}

			participant.SetFlags(flags);
			participant.SetName(name);
			char s[ 2048 ];
			s[ 0 ] = NULL;
			GetEnvStr( tupKeyValueContext , s );
			participant.SetEnvName(s);
			//participant.SetEnvName(envName);
			participant.SetModelName(modelName);
			participant.SetTriggerInstanceName(triggerInstanceName);
			participant.SetNodeRef(nodeIndex);

			int parentNodeRef;
			bool bAddedParentChannels = false;
			if(AddNodeSkinConnections(tupNode,nodeIndex,objectRef,parentNodeRef))
			{
				if(parentNodeRef!=nodeIndex)
				{
					TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(parentNodeRef);
					assert(pParentNodeAgg);
					TupNode tupParentNode(pParentNodeAgg);
					AddNodeChannels(participant,channelTypes,tupParentNode);
					bAddedParentChannels = true;
				}
			}
			if (!bAddedParentChannels)
			{
				AddNodeChannels(participant,channelTypes,tupNode);
			}
			AddObjectChannels(participant,channelTypes,objectRef);
		}
	}

	/*** may want to create additional participants by hand ***/

	/* create a new sound participant to handle soundtrack, if any */

	TupperwareAggregate *pAudioPoolAgg = m_pTupScene->GetAudioPool();
	if (pAudioPoolAgg)
	{
		TupAudioPool tupAudioPool(pAudioPoolAgg);
		int numClips = tupAudioPool.GetNumAudios();
		for (int clipIndex=0;clipIndex<numClips;clipIndex++)
		{
			/* get clip info */

			TupperwareAggregate *pAudioAgg = tupAudioPool.GetAudio(clipIndex);
			if (!pAudioAgg)
				continue;
			TupAudio tupAudio(pAudioAgg);
			char *pName;
			tupAudio.GetName(&pName);
			char *pFilename;
			tupAudio.GetFileName(&pFilename);
			float startTime;
			tupAudio.GetStartTime(startTime);

			/* really shouldn't be negative-- leading silence will have to be trimmed. for now, */
			/* warn and set to 0.0 */

			if (startTime < 0.0f)
			{
//				throw(PluginException(ERROR_WARNING,"Sound .wav file start time is negative, adjusting to 0.0"));
				startTime = 0.0f;
			}

			/* set up a new participant */

			int numParticipants = m_pParticipants->GetSize();
			m_pParticipants->SetSize(numParticipants+1);
			Participant &participant = (*m_pParticipants)[numParticipants];

			int nodeRef = Participant::MANUAL_ADDED_AUDIO_NODE_REF_BASE+clipIndex;
			m_pParticipantNodeRefs->Add(nodeRef);

			/* set type and name */
			participant.SetType(Participant::TYPE_SOUND);
			char Name[100];
			sprintf(Name, "%s.wav", pName);
			participant.SetName(Name);

			/* typically, sound participants have a visibility curve that tells when the sound fires. i don't want */
			/* to have to create a curve, so i'll just set a start time that acts like a visibility curve */

			participant.SetFlags(Participant::FLAGS_HAS_VISIBILITY_TIME);
			participant.SetVisibilityTime(startTime);

			participant.SetNodeRef(nodeRef);
		}
	}
}

static bool IsNumber( const char* string, float& value )
{
	const char* p;
	for ( p = string ; *p ; p++ )
		if ( !isdigit( *p ) && *p != '-' && *p != '.' && *p != 'e' )
			return false;
	if ( p == string )
		return false;

	sscanf( string, "%f", &value );
	return true;
}

char *ParticipantBuilder::GetEnvStr( TupKeyValueContext &tupKeyValueContext , char *s )
{
	// print the parameters
	for ( int k = 0 ; k < tupKeyValueContext.GetNumAttribs() ; k++ )
	{
		const char* key   = tupKeyValueContext.GetKeyAsString( k );
		const char* value = tupKeyValueContext.GetValueAsString( k );
//		m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"   '%s'='%s'", key, value );
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"   '%s'='%s'", key, value);

		if ( strlen( value ) > 0 )
		{
			if ( ! m_exclusions->IsExcluded( key ) )
			{
				float fValue;
				if ( IsNumber( value, fValue ) )
				{
					// trim the number so it takes less space and is easier to parse
					char num[ 32 ], *p;
					sprintf( num, "%.7f", fValue );
					for ( p = num + strlen( num ) - 1 ; p >= num && *p == '0' ; )
						*p-- = 0;
					if ( p >= num && *p == '.' )
						*p-- = 0;

					sprintf( s , "%s;%s=%s" , s , key , num );
				}
				else
				{
					sprintf( s , "%s;%s=%s" , s , key , value );
				}
			}
		}
	}

	return s;
}

void ParticipantBuilder::AddParents(void)
{
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		int participantIndex = m_pParticipantNodeRefs->GetIndex(nodeIndex);
		if (participantIndex==-1)
			continue;

		AddParents(nodeIndex);
	}
}

void ParticipantBuilder::AddParents(int nodeRef)
{
	TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeRef);
	
	int participantIndex = m_pParticipantNodeRefs->GetIndex(nodeRef);
	Participant &participant = (*m_pParticipants)[participantIndex];

	TupNode tupNode(pNodeAgg);
	int parentRef;
	tupNode.GetParentNodeReference(parentRef);

	if (parentRef==m_rootNodeReference) // parent is the root node of the scene
		return;

	TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(parentRef);
	TupNode tupParentNode(pParentNodeAgg);

	int parentParticipantIndex = m_pParticipantNodeRefs->GetIndex(parentRef);


	if (parentParticipantIndex!=-1) // if already added
	{
		// add me as a child
		Participant &parentParticipant = (*m_pParticipants)[parentParticipantIndex];
		parentParticipant.AddChild(participantIndex);
		participant.SetParent(parentParticipantIndex);
	}
	else // new parent
	{
		char *pNodeName;
		tupParentNode.GetName(&pNodeName);

		// set up a new one
		int numParticipants = m_pParticipants->GetSize();
		parentParticipantIndex = numParticipants;
		m_pParticipants->SetSize(numParticipants+1);
		Participant &parentParticipant = (*m_pParticipants)[parentParticipantIndex];
		Participant &participant = (*m_pParticipants)[participantIndex];

		// set up keeper for fast searches
		m_pParticipantNodeRefs->Add(parentRef);
		m_participantChannelTypes.SetSize(numParticipants+1);
		Keeper<int> &channelTypes = m_participantChannelTypes[parentParticipantIndex];

		parentParticipant.SetType(Participant::TYPE_DUMMY_PARENT);

		parentParticipant.SetName(pNodeName);
		parentParticipant.SetNodeRef(parentRef);

		AddNodeChannels(parentParticipant,channelTypes,tupParentNode);
		parentParticipant.AddChild(participantIndex);
		participant.SetParent(parentParticipantIndex);
		AddParents(parentRef); // keep going up
	}
}


void ParticipantBuilder::AddChannel(Participant &participant,Keeper<int> &channelTypes,float value,int type)
{
	int channelIndex = participant.AddChannel(); 
	ParticipantChannel &channel = participant.GetChannel(channelIndex);
	channelTypes.Add(type);
	channel.SetType((ParticipantChannel::Types)type);
	channel.SetStaticValue(value);
}

void ParticipantBuilder::AddChannels(Participant &participant,Keeper<int> &channelTypes,float *pValues,int numValues,int startType)
{
	for (int index=0;index<numValues;index++)
	{
		AddChannel(participant,channelTypes,pValues[index],startType+index);
	}
}

void ParticipantBuilder::AddNodeChannels(Participant &participant,Keeper<int> &channelTypes,TupNode &tupNode)
{
	TupperwareAggregate *pTransformAgg = tupNode.GetTransform();

	TupTransform tupTransform(pTransformAgg);
	if (tupTransform.GetTransformType() == TupTransform::STATIC_MATRIX)
	{
		TupStaticMatrix tupStaticMatrix(pTransformAgg);
		int flags;
		tupStaticMatrix.GetFlags(flags);
		participant.SetTransformFlags(flags);

		float *pValues;
		tupStaticMatrix.GetTranslation(&pValues);	AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_TRANSLATION_X);
		tupStaticMatrix.GetRotation(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_ROTATION_X);
		tupStaticMatrix.GetScale(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_SCALE_X);
		tupStaticMatrix.GetScalePivot(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_SCALE_PIVOT_X);
		tupStaticMatrix.GetRotationPivot(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_ROTATION_PIVOT_X);
		tupStaticMatrix.GetScalePivotTranslation(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_SCALE_PIVOT_TRANSLATION_X);
		tupStaticMatrix.GetRotationPivotTranslation(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_ROTATION_PIVOT_TRANSLATION_X);
		tupStaticMatrix.GetRotationOrientation(&pValues); AddChannels(participant,channelTypes,pValues,3,ParticipantChannel::TYPE_ROTATION_ORIENTATION_X);
	}

	float visibilty = 1.0f;
	int nodeFlags;
	tupNode.GetFlags(nodeFlags);
	if (nodeFlags&TupNode::FLAGS_HIDDEN)
		visibilty = 0.0f;
	AddChannel(participant,channelTypes,visibilty,ParticipantChannel::TYPE_VISIBILITY);

}

void ParticipantBuilder::AddObjectChannels(Participant &participant,Keeper<int> &channelTypes,int objectRef)
{
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	switch (tupObject.GetObjectType())
	{
		case TupObject::CAMERA:
			{
				participant.SetType(Participant::TYPE_CAMERA); // make this a camera

				TupCamera tupCamera(pObjectAgg);
				float value;
				tupCamera.GetFocalLength(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_FOCAL_LENGTH);
				tupCamera.GetCameraScale(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_CAMERA_SCALE);
				tupCamera.GetNearClipPlane(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_NEAR_CLIP_PLANE);
				tupCamera.GetFarClipPlane(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_FAR_CLIP_PLANE);
				tupCamera.GetHorizontalFilmAperture(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_HORIZONTAL_FILM_APERTURE);
				tupCamera.GetVerticalFilmAperture(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_VERTICAL_FILM_APERTURE);
				tupCamera.GetLensSqueezeRatio(value); AddChannel(participant,channelTypes,value,ParticipantChannel::TYPE_LENS_SQUEEZE_RATIO);
			}
			break;
		case TupObject::LIGHT:
			{
				participant.SetType(Participant::TYPE_LIGHT); // make this a camera

				TupLight tupLight(pObjectAgg);
				float *pValue;
				tupLight.GetLightColor(&pValue);
				AddChannel(participant,channelTypes,pValue[0],ParticipantChannel::TYPE_COLOR_RED);
				AddChannel(participant,channelTypes,pValue[1],ParticipantChannel::TYPE_COLOR_GREEN);
				AddChannel(participant,channelTypes,pValue[2],ParticipantChannel::TYPE_COLOR_BLUE);
			}
			break;
	}
}

bool ParticipantBuilder::AddNodeSkinConnections(TupNode &tupNode,int nodeRef,int objectRef,int &parentNodeRef)
{
	bool bFoundSkin = false;
	parentNodeRef = nodeRef;

	TupArray<int> modifierReferenceArray;
	TupArray<TupperwareAggregate *> modifierContextAggArray;
	AnnotateHelper::FindModifiers(*m_pTupObjectPool,objectRef,modifierReferenceArray,modifierContextAggArray);
	int numModifiers = modifierReferenceArray.GetSize();
	for (int modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
	{
		int modifierReference = modifierReferenceArray[modifierIndex];
		assert(modifierReference!=-1);
		TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
		assert(pModifierAgg);
		TupModifier tupModifier(pModifierAgg);
		TupModifier::ModifierTypes modifierType = tupModifier.GetModifierType();
		if (modifierType == TupModifier::SKIN_MODIFIER)
		{
			TupSkinModifier tupSkinModifier(pModifierAgg);
			int flags;
			tupSkinModifier.GetFlags(flags);
			if (flags&TupSkinModifier::FLAGS_ENABLED)
			{
				int nBonesRefs = tupSkinModifier.GetNumBones();

				for (int boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
				{
					TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
					TupSkinModifierBone tupBone(pBoneAgg);
					int boneReference;
					tupBone.GetNodeReference(boneReference);

					int numOldConnectedSourceNodeRefs = m_connectedSourceNodeRefs.GetSize();
					int connectedSourceNodeIndex = m_connectedSourceNodeRefs.Add(boneReference);
					// if new
					if (numOldConnectedSourceNodeRefs==connectedSourceNodeIndex)
					{
						// check if this is a parent bone (its parent is not a bone)
						bool bModelBone = true;
						TupperwareAggregate *pBoneNodeAgg = m_pTupNodePool->GetNode(boneReference);
						TupNode tupBoneNode(pBoneNodeAgg);
						int parentRef;
						tupBoneNode.GetParentNodeReference(parentRef);
						TupperwareAggregate *pParentBoneNodeAgg = m_pTupNodePool->GetNode(parentRef);
						TupNode tupParentBoneNode(pParentBoneNodeAgg);
						int parentObjectRef;
						tupParentBoneNode.GetObjectReference(parentObjectRef);
						if (parentObjectRef!=-1)
						{
							TupperwareAggregate *pParentObjectAgg = m_pTupObjectPool->GetObjectAggregate(parentObjectRef);
							TupObject parentObject(pParentObjectAgg);
							if (parentObject.GetObjectType()==TupObject::BONE)
							{
								bModelBone = false;
							}
						}
						ConnectedNode connectedNode;
						connectedNode.m_type = bModelBone ? ConnectedNode::ADD_CHANNELS : ConnectedNode::MAKE_ACTION;
						connectedNode.m_targetNode = nodeRef; // connect to node for mesh
						m_connectedNodes.Add(connectedNode);
						bFoundSkin = true; // we found one
					}
				}
			}
		}
	}
	return bFoundSkin;
}

void ParticipantBuilder::AddAnimation(void)
{
	int numAnimSets = m_pTupAnimSetPool->GetNumAnimSets();
	m_participantSourceClips.SetSize(m_pParticipants->GetSize()); 
	m_participantInstancedClips.SetSize(m_pParticipants->GetSize()); 

	for (int nAnimSet = 0; nAnimSet < numAnimSets; nAnimSet++)
	{
		TupperwareAggregate *pAnimSetAgg = m_pTupAnimSetPool->GetAnimSet(nAnimSet);
		TupAnimSet tupAnimSet(pAnimSetAgg);

		/* go through this anim set's channels */
		TupperwareAggregate *pChannelPoolAgg = tupAnimSet.GetChannels();
		if (pChannelPoolAgg)
		{
			TupAnimChannelPool tupAnimChannelPool(pChannelPoolAgg);
			int numAnimChannels = tupAnimChannelPool.GetNumAnimChannels();

			for (int nChannel = 0; nChannel < numAnimChannels; nChannel++)
			{
				TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nChannel);
				TupAnimChannel tupAnimChannel(pAnimChannelAgg);

				if (tupAnimChannel.GetAnimChannelType() != TupAnimChannel::NODE)
					continue;

				TupperwareAggregate *pCurvePoolAgg = tupAnimChannel.GetCurves();
				if (!pCurvePoolAgg)
					continue; // if no curves here

				int nodeRef;
				tupAnimChannel.GetReference(nodeRef);

				int participantIndex = m_pParticipantNodeRefs->GetIndex(nodeRef);
				// not on a participant
				if (participantIndex==-1)
				{
					// is this a node I care about for another participant?
					int connectedNodeIndex = m_connectedSourceNodeRefs.GetIndex(nodeRef);
					// if not then on to the next...
					if (connectedNodeIndex==-1)
						continue;
					
					ConnectedNode &connectedNode = m_connectedNodes[connectedNodeIndex];
					participantIndex = m_pParticipantNodeRefs->GetIndex(connectedNode.m_targetNode);
					assert(participantIndex!=-1); // this should not happen
					// if this is not an add channels type then skip
					if (connectedNode.m_type!=ConnectedNode::ADD_CHANNELS)
					{
						continue;
					}
					// keep going if add_channels type (changed the participantIndex)
				}

				char *pAttributeName;
				tupAnimChannel.GetAttribute(&pAttributeName);
				int channelType = GetChannelType(pAttributeName);

				int participantChannelIndex = m_participantChannelTypes[participantIndex].GetIndex(channelType);
				// not already a member of the participant
				if (participantChannelIndex==-1) 
					continue;

				// we need to be added 
				Participant &participant = (*m_pParticipants)[participantIndex];
				ParticipantChannel &channel = participant.GetChannel(participantChannelIndex);
				int sourceCurveIndex = channel.AddSourceCurve();

				TupAnimCurvePool tupAnimCurvePool(pCurvePoolAgg);
				TupperwareAggregate *pCurveAgg = tupAnimCurvePool.GetAnimCurve(0); // get first one

				TupAnimCurve tupAnimCurve(pCurveAgg);
				float startTime,endTime;
				tupAnimCurve.GetStartTime(startTime);
				tupAnimCurve.GetEndTime(endTime);
				float duration = endTime-startTime;

				int curveFlags;
				tupAnimCurve.GetFlags(curveFlags);
				switch (curveFlags & TupAnimCurve::FLAGS_ROTATION_INTERPOLATION_MASK)
				{
					case TupAnimCurve::FLAGS_ROTATION_INDEPENDENT_EULER:
						break;
					case TupAnimCurve::FLAGS_ROTATION_SYNCRONIZED_EULER:
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE,"Warning - Curve %s.%s is using Syncronized Euler rotation interpolation - please use independent euler.",participant.GetName(),pAttributeName);
						break;
					case TupAnimCurve::FLAGS_ROTATION_SYNCRONIZED_QUATERNION:
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE,"Warning - Curve %s.%s is using Syncronized Quaternion rotation interpolation - please use independent euler.",participant.GetName(),pAttributeName);
						break;
				}

				// add a source curve and an instance 
				channel.SetSourceCurveData(sourceCurveIndex,pCurveAgg);
				channel.SetSourceFlags(sourceCurveIndex,0);
				channel.SetSourceStartTime(sourceCurveIndex,startTime);
				channel.SetSourceDuration(sourceCurveIndex,duration);

				int instancedCurveIndex = channel.AddInstancedCurve();

				channel.SetInstanceSourceCurveReference(instancedCurveIndex,sourceCurveIndex);
				channel.SetInstanceStartTime(instancedCurveIndex,startTime);
				channel.SetInstanceFlags(instancedCurveIndex,TupAnimInstancedClip::FLAGS_ABSOLUTE | TupAnimInstancedClip::FLAGS_ENABLE);
				channel.SetInstanceScale(instancedCurveIndex,1.0);
				channel.SetInstanceWeight(instancedCurveIndex,1.0);
				channel.SetInstanceCycle(instancedCurveIndex,1.0);
			}
		}
		// handle the source and instanced clips
		TupperwareAggregate *pSourceClipsAgg = tupAnimSet.GetSourceClips();
		TupperwareAggregate *pInstancedClipsAgg = tupAnimSet.GetInstancedClips();
		if (pSourceClipsAgg && pInstancedClipsAgg)
		{
			// build an array of what instanced clips reference each source clip
			TupAnimClipPool tupInstancedClipPool(pInstancedClipsAgg);
			int numInstancedClips = tupInstancedClipPool.GetNumAnimClips();
			TupAnimClipPool tupSourceClipPool(pSourceClipsAgg);
			int numSourceClips = tupSourceClipPool.GetNumAnimClips();
			TupArray<TupArray<int> >instancedClipIndices;
			instancedClipIndices.SetSize(numSourceClips);
			// arrays to cache instance information
			TupArray<float> instancedStartTimes; instancedStartTimes.SetSize(numInstancedClips);
			TupArray<int> instancedFlags; instancedFlags.SetSize(numInstancedClips);
			TupArray<float> instancedWeights; instancedWeights.SetSize(numInstancedClips);
			TupArray<float> instancedCycles; instancedCycles.SetSize(numInstancedClips);
			TupArray<float> instancedScales; instancedScales.SetSize(numInstancedClips);
			TupArray<char *> instancedNames; instancedNames.SetSize(numInstancedClips);
			for (int instancedClipIndex=0;instancedClipIndex<numInstancedClips;instancedClipIndex++)
			{
				TupperwareAggregate *pInstancedClipAgg = tupInstancedClipPool.GetAnimClip(instancedClipIndex);
				TupAnimInstancedClip tupInstancedClip(pInstancedClipAgg);
				int sourceClipReference;
				tupInstancedClip.GetSourceClipReference(sourceClipReference);
				if (sourceClipReference!=-1)
				{
					instancedClipIndices[sourceClipReference].Add(instancedClipIndex);
				}
				tupInstancedClip.GetName(&(instancedNames[instancedClipIndex]));
				tupInstancedClip.GetStartTime(instancedStartTimes[instancedClipIndex]);
				tupInstancedClip.GetFlags(instancedFlags[instancedClipIndex]);
				tupInstancedClip.GetWeight(instancedWeights[instancedClipIndex]);
				tupInstancedClip.GetCycle(instancedCycles[instancedClipIndex]);
				tupInstancedClip.GetScale(instancedScales[instancedClipIndex]);
			}

			// go through each source clip
			for (int sourceClipIndex=0;sourceClipIndex<numSourceClips;sourceClipIndex++)
			{
				TupperwareAggregate *pSourceClipAgg = tupSourceClipPool.GetAnimClip(sourceClipIndex);
				if (!pSourceClipAgg)
					continue;

				TupAnimSourceClip tupAnimSourceClip(pSourceClipAgg);
				float startTime,duration;
				int flags;
				char *pSourceClipName;
				tupAnimSourceClip.GetName(&pSourceClipName);
				tupAnimSourceClip.GetStartTime(startTime);
				tupAnimSourceClip.GetDuration(duration);
				tupAnimSourceClip.GetFlags(flags);

				TupperwareAggregate *pChannelPoolAgg = tupAnimSourceClip.GetChannels();
				if (pChannelPoolAgg)
				{
					TupAnimChannelPool tupAnimChannelPool(pChannelPoolAgg);
					int numAnimChannels = tupAnimChannelPool.GetNumAnimChannels();
					
					for (int nChannel = 0; nChannel < numAnimChannels; nChannel++)
					{
						TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nChannel);
						TupAnimChannel tupAnimChannel(pAnimChannelAgg);
						
						if (tupAnimChannel.GetAnimChannelType() != TupAnimChannel::NODE)
							continue;
						
						TupperwareAggregate *pCurvePoolAgg = tupAnimChannel.GetCurves();
						if (!pCurvePoolAgg)
							continue; // if no curves here
						
						int nodeRef;
						tupAnimChannel.GetReference(nodeRef);
						
						int participantIndex = m_pParticipantNodeRefs->GetIndex(nodeRef);
						// not on a participant
						if (participantIndex==-1)
						{
							// is this a node I care about for another participant?
							int connectedNodeIndex = m_connectedSourceNodeRefs.GetIndex(nodeRef);
							// if not then on to the next...
							if (connectedNodeIndex==-1)
								continue;

							ConnectedNode &connectedNode = m_connectedNodes[connectedNodeIndex];
							participantIndex = m_pParticipantNodeRefs->GetIndex(connectedNode.m_targetNode);
							assert(participantIndex!=-1); // this should not happen
							// make an action for this source clip
							if (connectedNode.m_type==ConnectedNode::MAKE_ACTION)
							{
								ParticipantClip newSourceClip;
								newSourceClip.m_setIndex = nAnimSet;
								newSourceClip.m_clipIndex = sourceClipIndex;

								int currentNumSourceClips = m_participantSourceClips[participantIndex].GetSize();
								int newSourceClipIndex = m_participantSourceClips[participantIndex].Add(newSourceClip);
								// was I just added
								if (newSourceClipIndex==currentNumSourceClips)
								{
									// add source action
									Participant &participant = (*m_pParticipants)[participantIndex];
									int sourceActionIndex = participant.AddSourceAction();
									ParticipantSourceAction &sourceAction = participant.GetSourceAction(sourceActionIndex);
									sourceAction.SetName(pSourceClipName);
									sourceAction.SetStartTime(startTime);
									sourceAction.SetDuration(duration);
									sourceAction.SetFlags(flags);

									// add instanced actions
									int numSourceInstances = instancedClipIndices[sourceClipIndex].GetSize();
									for (int sourceInstanceIndex=0;sourceInstanceIndex<numSourceInstances;sourceInstanceIndex++)
									{
										instancedClipIndex = instancedClipIndices[sourceClipIndex][sourceInstanceIndex];
										int instancedActionIndex = participant.AddInstancedAction();
										ParticipantInstancedAction &instancedAction = participant.GetInstancedAction(instancedActionIndex);
										instancedAction.SetSourceReference(sourceActionIndex);
										instancedAction.SetName(instancedNames[instancedClipIndex]);
										instancedAction.SetStartTime(instancedStartTimes[instancedClipIndex]);
										instancedAction.SetFlags(instancedFlags[instancedClipIndex]);
										instancedAction.SetScale(instancedScales[instancedClipIndex]);
										instancedAction.SetWeight(instancedWeights[instancedClipIndex]);
										instancedAction.SetCycle(instancedCycles[instancedClipIndex]);
										// keep track of this instanced clip for handling blends
										ParticipantClip newInstancedClip;
										newInstancedClip.m_setIndex = nAnimSet;
										newInstancedClip.m_clipIndex = instancedClipIndex;
										m_participantInstancedClips[participantIndex].Add(newInstancedClip);
									}
								}
								continue; // on to the next one
							}
							// remaped participant so continue with following code
						}
						
						char *pAttributeName;
						tupAnimChannel.GetAttribute(&pAttributeName);
						int channelType = GetChannelType(pAttributeName);
						
						int participantChannelIndex = m_participantChannelTypes[participantIndex].GetIndex(channelType);
						// not already a member of the participant
						if (participantChannelIndex==-1) 
							continue;
						
						// we need to be added 
						Participant &participant = (*m_pParticipants)[participantIndex];
						ParticipantChannel &channel = participant.GetChannel(participantChannelIndex);
						int sourceCurveIndex = channel.AddSourceCurve();
						
						TupAnimCurvePool tupAnimCurvePool(pCurvePoolAgg);
						TupperwareAggregate *pCurveAgg = tupAnimCurvePool.GetAnimCurve(0); // get first one
						
						// add a source curve and an instance 
						channel.SetSourceCurveData(sourceCurveIndex,pCurveAgg);
						channel.SetSourceFlags(sourceCurveIndex,flags);
						channel.SetSourceStartTime(sourceCurveIndex,startTime);
						channel.SetSourceDuration(sourceCurveIndex,duration);

						int numSourceInstances = instancedClipIndices[sourceClipIndex].GetSize();
						for (int sourceInstanceIndex=0;sourceInstanceIndex<numSourceInstances;sourceInstanceIndex++)
						{
							instancedClipIndex = instancedClipIndices[sourceClipIndex][sourceInstanceIndex];
							int instancedCurveIndex = channel.AddInstancedCurve();
							channel.SetInstanceSourceCurveReference(instancedCurveIndex,sourceCurveIndex);
							channel.SetInstanceStartTime(instancedCurveIndex,instancedStartTimes[instancedClipIndex]);
							channel.SetInstanceFlags(instancedCurveIndex,instancedFlags[instancedClipIndex]);
							channel.SetInstanceScale(instancedCurveIndex,instancedScales[instancedClipIndex]);
							channel.SetInstanceWeight(instancedCurveIndex,instancedWeights[instancedClipIndex]);
							channel.SetInstanceCycle(instancedCurveIndex,instancedCycles[instancedClipIndex]);
						}
					}
				}
			}
			// handle the clip blends
			TupperwareAggregate *pClipBlendsAgg = tupAnimSet.GetClipBlends();
			if (pClipBlendsAgg)
			{
				TupAnimClipBlendPool tupAnimClipBlendPool(pClipBlendsAgg);
				int numClipBlends = tupAnimClipBlendPool.GetNumAnimClipBlends();
				for (int blendIndex=0;blendIndex<numClipBlends;blendIndex++)
				{
					TupperwareAggregate *pAnimClipBlendAgg = tupAnimClipBlendPool.GetAnimClipBlend(blendIndex);
					TupAnimClipBlend tupAnimClipBlend(pAnimClipBlendAgg);
					ParticipantClip clip1;
					clip1.m_setIndex = nAnimSet;
					tupAnimClipBlend.GetClipInstance1Reference(clip1.m_clipIndex);
					ParticipantClip clip2;
					clip2.m_setIndex = nAnimSet;
					tupAnimClipBlend.GetClipInstance2Reference(clip2.m_clipIndex);
					char *pClipBlendName;
					tupAnimClipBlend.GetName(&pClipBlendName);
					int clipBlendFlags;
					tupAnimClipBlend.GetFlags(clipBlendFlags);
					TupperwareAggregate *pBlendCurveAgg = NULL;
					TupperwareAggregate *pBlendCurvesAgg = tupAnimClipBlend.GetCurves();
					if (pBlendCurvesAgg)
					{
						TupAnimCurvePool tupAnimCurvePool(pBlendCurvesAgg);
						if (tupAnimCurvePool.GetNumAnimCurves())
						{
							pBlendCurveAgg = tupAnimCurvePool.GetAnimCurve(0);
						}
					}

					// now loop through all the participants and see if they contain these instances
					int numParticipants = m_pParticipants->GetSize();
					for (int participantIndex=0;participantIndex<numParticipants;participantIndex++)
					{
						int clip1Index = m_participantInstancedClips[participantIndex].GetIndex(clip1);
						int clip2Index = m_participantInstancedClips[participantIndex].GetIndex(clip2);

						if (clip1Index!=-1 && clip2Index!=-1)
						{
							Participant &participant = (*m_pParticipants)[participantIndex];

							int newClipBlendIndex = participant.AddInstancedActionBlend();
							ParticipantInstancedActionBlend &newBlend = participant.GetInstancedActionBlend(newClipBlendIndex);
							newBlend.SetName(pClipBlendName);
							newBlend.SetInstancedClip1Reference(clip1Index);
							newBlend.SetInstancedClip2Reference(clip2Index);
							newBlend.SetFlags(clipBlendFlags);
							newBlend.SetCurve(pBlendCurveAgg);
						}
					}
				}
			}
		}
	}
}

struct ChannelNameAndType
{
	char *pName;
	int type;
};

static ChannelNameAndType channelNamesAndTypes[] = 
{
	// general
	{ "visibility",										ParticipantChannel::TYPE_VISIBILITY},
	// transformation
	{ "translate.translateX",							ParticipantChannel::TYPE_TRANSLATION_X},
	{ "translate.translateY",							ParticipantChannel::TYPE_TRANSLATION_Y},
	{ "translate.translateZ",							ParticipantChannel::TYPE_TRANSLATION_Z},
	{ "rotate.rotateX",									ParticipantChannel::TYPE_ROTATION_X},
	{ "rotate.rotateY",									ParticipantChannel::TYPE_ROTATION_Y},
	{ "rotate.rotateZ",									ParticipantChannel::TYPE_ROTATION_Z},
	{ "scale.scaleX",										ParticipantChannel::TYPE_SCALE_X},
	{ "scale.scaleY",										ParticipantChannel::TYPE_SCALE_Y},
	{ "scale.scaleZ",										ParticipantChannel::TYPE_SCALE_Z},
	{ "shear.shearX",										ParticipantChannel::TYPE_SHEAR_X},
	{ "shear.shearY",										ParticipantChannel::TYPE_SHEAR_Y},
	{ "shear.shearZ",										ParticipantChannel::TYPE_SHEAR_Z},
	// camera
	{ "cameraAperture.horizontalFilmAperture",	ParticipantChannel::TYPE_HORIZONTAL_FILM_APERTURE},
	{ "cameraAperture.verticalFilmAperture",		ParticipantChannel::TYPE_VERTICAL_FILM_APERTURE},
	{ "focalLength",										ParticipantChannel::TYPE_FOCAL_LENGTH},
	{ "lensSqueezeRatio",								ParticipantChannel::TYPE_LENS_SQUEEZE_RATIO},
	// light
	{ "color.colorR",										ParticipantChannel::TYPE_COLOR_RED},
	{ "color.colorG",										ParticipantChannel::TYPE_COLOR_GREEN},
	{ "color.colorB",										ParticipantChannel::TYPE_COLOR_BLUE},
	// unknown
	{ NULL,													ParticipantChannel::TYPE_UNKNOWN},

};

int ParticipantBuilder::GetChannelType(const char *pAttributeName)
{
	int index = 0;
	while(channelNamesAndTypes[index].pName)
	{
		if (!strcmp(pAttributeName,channelNamesAndTypes[index].pName))
			return channelNamesAndTypes[index].type;
		index++;
	}
	return ParticipantChannel::TYPE_UNKNOWN;
}

void ParticipantBuilder::ChangeNodeNames(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);
		TupString oldName(pName);
		
		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );
		
		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "override_instance_name" );
		tupKeyValueContext.AddKey( "instance_name" );
		
		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
		
		if (tupKeyValueContext.ValueValidAt(0) && tupKeyValueContext.GetValueAsBool(0) && tupKeyValueContext.ValueValidAt(1))
		{
			const char *pName = tupKeyValueContext.GetValueAsString(1);
			if (strlen(pName))
			{
				tupNode.AddName(pName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Node %s name changed to %s",(const char *)oldName,pName);
			}
		}
	}
}
