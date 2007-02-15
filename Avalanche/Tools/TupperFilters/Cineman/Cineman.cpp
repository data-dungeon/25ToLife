////////////////////////////////////////////////////////////////////////////
//
// Cineman
//
// Class to handle the Cineman Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Cineman.cpp $
 * 
 * *****************  Version 46  *****************
 * User: Nmartin      Date: 9/15/03    Time: 10:06a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 45  *****************
 * User: Nmartin      Date: 9/15/03    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * calculate splines from maya data instead of using linear curves
 * 
 * *****************  Version 44  *****************
 * User: Nmartin      Date: 8/07/03    Time: 5:37p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * show weapon takes an int, not a string!
 * 
 * *****************  Version 43  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support load screen tak
 * 
 * *****************  Version 42  *****************
 * User: Nmartin      Date: 8/07/03    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support club, rattle, and blowgun in cut scene
 * 
 * *****************  Version 41  *****************
 * User: Nmartin      Date: 8/06/03    Time: 5:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * new participant type-- event trigger-- and environmental actor drop
 * shadow support
 * 
 * *****************  Version 40  *****************
 * User: Nmartin      Date: 8/02/03    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add restore_to_base_pose processing
 * 
 * *****************  Version 39  *****************
 * User: Nmartin      Date: 8/01/03    Time: 10:02a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * directional lights use rotation data, check for duplicate kfm names
 * (last 7 characters) when generating kfmlst file
 * 
 * *****************  Version 38  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:29a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participant
 * 
 * *****************  Version 37  *****************
 * User: Nmartin      Date: 7/25/03    Time: 4:11p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 36  *****************
 * User: Nmartin      Date: 7/25/03    Time: 1:28p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add fade in/out
 * 
 * *****************  Version 35  *****************
 * User: Nmartin      Date: 7/24/03    Time: 1:05p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 34  *****************
 * User: Nmartin      Date: 7/24/03    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support full-screen distortion as part of cut scene
 * 
 * *****************  Version 33  *****************
 * User: Nmartin      Date: 7/22/03    Time: 8:03p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * guard against a single camera that is marked invisible-- produces no
 * datastream
 * 
 * *****************  Version 32  *****************
 * User: Nmartin      Date: 7/22/03    Time: 3:26p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * check instance actions cycle value to determine loop or not
 * 
 * *****************  Version 31  *****************
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
 * *****************  Version 30  *****************
 * User: Nmartin      Date: 7/18/03    Time: 10:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * compress curve data a bit
 * 
 * *****************  Version 29  *****************
 * User: Nmartin      Date: 7/18/03    Time: 2:18p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of kfm processing (replaces MakeMDB.bat file), always output
 * initial event data
 * 
 * *****************  Version 28  *****************
 * User: Nmartin      Date: 7/09/03    Time: 1:18p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * write out binary data
 * 
 * *****************  Version 27  *****************
 * User: Nmartin      Date: 7/08/03    Time: 9:05a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent node is in charge of hide/show, fix camera rotation fixup
 * 
 * *****************  Version 26  *****************
 * User: Nmartin      Date: 7/02/03    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process audio file attached to cinematic
 * 
 * *****************  Version 25  *****************
 * User: Nmartin      Date: 7/01/03    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * compress curve data (just remove duplicate flatline splines)
 * 
 * *****************  Version 24  *****************
 * User: Nmartin      Date: 7/01/03    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove ParentPos and ParentRotation
 * 
 * *****************  Version 23  *****************
 * User: Nmartin      Date: 7/01/03    Time: 9:54a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * support parents
 * 
 * *****************  Version 22  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 21  *****************
 * User: Nmartin      Date: 6/25/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add more curves, check if any instanced curve is valid before counting
 * it, add model_name to particpant
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 6/18/03    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 3:07p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * audio clip support
 * 
 * *****************  Version 18  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 5:55p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 17  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 4:22p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 16  *****************
 * User: Nmartin      Date: 6/05/03    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * pull env_name out of tup file
 * 
 * *****************  Version 15  *****************
 * User: Nmartin      Date: 6/04/03    Time: 8:36a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * visibility curve activates/deactivates particle emitter, handle
 * transform order flags
 * 
 * *****************  Version 14  *****************
 * User: Nmartin      Date: 6/03/03    Time: 10:44a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * get frames per second, start/end from tup file
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 6/02/03    Time: 5:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * tony's cut scene is working!
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 6/02/03    Time: 2:55p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging cameras works, particle effect works some
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 11:09a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * sample animglobals code
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/02/03    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * convert static values to engine-friendly values before export to .cut
 * file
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 6/02/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging multiple participants into a single data stream
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:27p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 7  *****************
 * User: Nmartin      Date: 5/29/03    Time: 4:26p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * convert time from frames to seconds
 * 
 * *****************  Version 6  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:50p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove unneeded curve table
 * 
 * *****************  Version 5  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/27/03    Time: 10:29a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first pass of participant manager
 * 
 * *****************  Version 3  *****************
 * User: Nmartin      Date: 5/19/03    Time: 9:36a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 2  *****************
 * User: Nmartin      Date: 5/19/03    Time: 8:43a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of cutscene filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:30p
 * Created in $/Avalanche/tools/TupperFilters/Cineman
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "Cineman.h"
#include "../Export/ExportPC/ExportPch.h"
#include "../Export/ExportPC/ExportPC.h"
#include "../Export/Common/ExportDBL.h"
#include "ParticipantManager.h"
#include "Participant.h"
#include "ParticipantChannel.h"
#include "ParticipantSourceAction.h"
#include "ParticipantInstancedAction.h"
#include <GeomUtil/AnimCurve.h>
#include "CutSceneMgr.h"
#include <direct.h>

/* constructor */

CinemanFilter::CinemanFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
	m_pDataStream = NULL;
	m_pParticipantManager = NULL;
	m_pBinaryData = NULL;
	m_pCutLogFile = NULL;
	m_pKFMBuffer = NULL;
	m_nKFMBufferSize = 0;
}

CinemanFilter::~CinemanFilter()
{
	if (m_pDataStream)
		free(m_pDataStream);
	if (m_pParticipantManager)
		delete m_pParticipantManager;
	if (m_pKFMBuffer)
		free(m_pKFMBuffer);
}

int CinemanFilter::GetTotalPasses(void) const
{
	return 1;
}

void CinemanFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);

	/* get root data aggregate */

	TupperwareAggregate *pSceneAgg = dataManager.GetSceneRoot();
	assert(pSceneAgg);
	TupScene *pTupScene = new TupScene(pSceneAgg);

	/* make an exclusions list used for building an actor key/value list */

	try
	{
		// read any key exclusions
		TupString excludeKeys = options.GetValue("ExcludeKeys");
		if (excludeKeys && strlen(excludeKeys) > 0)
		{
			TupString relativePath,fullPath,name,ext;
         FilePath::GetPathsToFile(relativePath,fullPath,name,ext,excludeKeys,dataManager.GetConfigDirectory());
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Reading actor parm exclude keys file '%s'",fullPath);
			if (!m_exclusions.AddKeysFromFile(fullPath))
				throw(PluginException(ERROR_FATAL,"Error loading exlude keys file '%s'",fullPath));
		}
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ObjectAttributes - Process\r\n  %s",(const char *)(e)));
	}

	/* point to node and object pools */

	TupperwareAggregate *pNodePoolAgg = pTupScene->GetNodePool();
	assert(pNodePoolAgg);
	TupNodePool *pTupNodePool = new TupNodePool(pNodePoolAgg);
	TupperwareAggregate *pObjectPoolAgg = pTupScene->GetObjectPool();
	assert(pObjectPoolAgg);
	TupObjectPool *pTupObjectPool = new TupObjectPool(pObjectPoolAgg);

	/* must be able to get to source file for kfm processing */

	char *pSourceFileName;
	pTupScene->GetSourceFileName(&pSourceFileName);
	if (pSourceFileName == NULL)
		throw(PluginException(ERROR_FATAL, "No source file name found in .tup file-- update TupImpExp.mll"));

	/* get time conversion factor (frames per second-- maya timeline is in frames, want to convert to seconds) */

	int nUnitFlags;
	pTupScene->GetUnitFlags(nUnitFlags);
	float fFramesPerSecond;
	switch (nUnitFlags & TupScene::TIME_UNIT_MASK)
	{
	case TupScene::TIME_UNIT_HOURS:
		fFramesPerSecond = 1.0f / 3600.0f;
		break;
	case TupScene::TIME_UNIT_MINUTES:
		fFramesPerSecond = 1.0f / 60.0f;
		break;
	case TupScene::TIME_UNIT_SECONDS:
		fFramesPerSecond = 1.0f;
		break;
	case TupScene::TIME_UNIT_MILLISECONDS:
		fFramesPerSecond = 1000.0f;
		break;
	case TupScene::TIME_UNIT_GAMES:
		fFramesPerSecond = 15.0f;
		break;
	case TupScene::TIME_UNIT_FILM:
		fFramesPerSecond = 24.0f;
		break;
	case TupScene::TIME_UNIT_PAL_FRAME:
		fFramesPerSecond = 25.0f;
		break;
	case TupScene::TIME_UNIT_NTSC_FRAME:
		fFramesPerSecond = 30.0f;
		break;
	case TupScene::TIME_UNIT_SHOW_SCAN:
		fFramesPerSecond = 48.0f;
		break;
	case TupScene::TIME_UNIT_PAL_FIELD:
		fFramesPerSecond = 50.0f;
		break;
	case TupScene::TIME_UNIT_NTSC_FIELD:
		fFramesPerSecond = 60.0f;
		break;
	default:
		ASSERT(false);	// unhandled case
		fFramesPerSecond = 24.0f;
		break;
	}

	/* get cut scene start and end times */

	TupperwareAggregate *pAnimGlobalsAgg = pTupScene->GetAnimGlobals();
	ASSERT(pAnimGlobalsAgg);
	if (pAnimGlobalsAgg)
	{
		TupAnimGlobals tupAnimGlobals(pAnimGlobalsAgg);
		tupAnimGlobals.GetStartTime(m_fCutSceneStartTime);
		tupAnimGlobals.GetEndTime(m_fCutSceneEndTime);

		/* convert from frames to seconds */

		m_fCutSceneStartTime /= fFramesPerSecond;
		m_fCutSceneEndTime /= fFramesPerSecond;
	}

	/* all cameras get merged into one participant. same with particle effects, sounds */

	int nCameraParticipants = 0;
	int nParticleEffectParticipants = 0;
	int nSoundParticipants = 0;
	int nTriggerParticipants = 0;

	/* not all participants are valid for export */

	int nNotValidForExportParticipants = 0;

	/* total curves for curve data .dbl */

	int nOutputCurveCount = 0;

	/* fade flags */

	int nFadeType = CCutSceneMgr::CUT_SCENE_FADE_NONE;

	/* survey the data, calculate total number of curves needed */

	m_pParticipantManager = new ParticipantManager(pSceneAgg,&m_exclusions);

	m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "%d participants", m_pParticipantManager->GetNumParticipants());
	
	int numParticipants = m_pParticipantManager->GetNumParticipants();

	for (int participantIndex=0;participantIndex<numParticipants;participantIndex++)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(participantIndex);
		Participant::Types participantType = participant.GetType();
		int nodeRef = participant.GetNodeRef();

		m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "Participant %d: %s, type %s, %d channels",
					 participantIndex, participant.GetName(), participant.GetTypeName(), participant.GetNumChannels());

		/* convert all times into seconds */

		participant.ConvertTimeToSeconds(fFramesPerSecond);

		/* keep track of participants that will be merged into a single participant for cut scene */

		switch(participantType)
		{
		case Participant::TYPE_CAMERA:
			nCameraParticipants++;
			break;
		case Participant::TYPE_PARTICLE_EFFECT:
			nParticleEffectParticipants++;
			break;
		case Participant::TYPE_SOUND:
			nSoundParticipants++;
			break;
		case Participant::TYPE_TRIGGER:
			nTriggerParticipants++;
			break;
		/*** if you add more, add to switch statement below in pMergeParticipant setup ***/
		}

		/* not all participants are valid for export */

		switch(participantType)
		{
		case Participant::TYPE_FADE:
			participant.SetValidForExport(false);
			nNotValidForExportParticipants++;
			break;
		}

		/* some participants are always visible-- the visibility curve has no effect */

		switch(participantType)
		{
		case Participant::TYPE_DISTORTION:
		case Participant::TYPE_MOTION_BLUR:
			participant.SetAlwaysVisible();
			break;
		}

		/* make sure light participants are really maya lights */

		if (participantType == Participant::TYPE_LIGHT)
		{
			TupperwareAggregate *pNodeAgg = pTupNodePool->GetNode(participant.GetNodeRef());
			if (pNodeAgg == NULL)
				throw(PluginException(ERROR_FATAL, "Light processing error: no node aggregate"));
			TupNode tupNode(pNodeAgg);
			int nObjectRef;
			tupNode.GetObjectReference(nObjectRef);
			if (nObjectRef == -1)
				throw(PluginException(ERROR_FATAL, "Light processing error: object reference == -1"));
			TupperwareAggregate *pObjectAgg = pTupObjectPool->GetObjectAggregate(nObjectRef);
			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType() != TupObject::LIGHT)
				throw(PluginException(ERROR_FATAL, "Light processing error: object is not a Maya light"));

			/* record object aggregate for later use */

			participant.SetObjectAgg(pObjectAgg);

			/* local or infinite directional light? */

			TupLight tupLight(pObjectAgg);
			int nLightType;
			tupLight.GetLightType(nLightType);
			switch (nLightType)
			{
			case TupLight::LIGHT_AMBIENT:
				participant.SetFlags(Participant::FLAGS_AMBIENT_LIGHT);
				participant.SetAlwaysVisible();
				break;
			case TupLight::LIGHT_FREE_DIRECT:
				participant.SetFlags(Participant::FLAGS_DIRECTIONAL_LIGHT);
				participant.SetAlwaysVisible();
				break;
			case TupLight::LIGHT_OMNI:
				participant.SetFlags(Participant::FLAGS_LOCAL_LIGHT);
				break;
			default:
				throw(PluginException(ERROR_FATAL, "Light processing error: light must be ambient, directional, or point"));
			}
		}

		/* global flags from participants */

		switch(participantType)
		{
		case Participant::TYPE_FADE:
			switch (participant.GetFlags() & (Participant::FLAGS_FADE_IN | Participant::FLAGS_FADE_OUT))
			{
			case 0:
				nFadeType = CCutSceneMgr::CUT_SCENE_FADE_NONE;
				break;
			case Participant::FLAGS_FADE_IN:
				nFadeType = CCutSceneMgr::CUT_SCENE_FADE_IN;
				break;
			case Participant::FLAGS_FADE_OUT:
				nFadeType = CCutSceneMgr::CUT_SCENE_FADE_OUT;
				break;
			case (Participant::FLAGS_FADE_IN | Participant::FLAGS_FADE_OUT):
				nFadeType = CCutSceneMgr::CUT_SCENE_FADE_IN | CCutSceneMgr::CUT_SCENE_FADE_OUT;
				break;
			}
			break;
		}

		/* analyze channels */

		int numChannels = participant.GetNumChannels();
		for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
		{
			ParticipantChannel &channel = participant.GetChannel(channelIndex);
			ParticipantChannel::Types channelType = channel.GetType();
			float staticValue = channel.GetStaticValue();

			m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "   Channel %d: %s, static value = %.2f, %d source curves, %d instance curves",
						 channelIndex, channel.GetTypeName(), staticValue, channel.GetNumSourceCurves(), channel.GetNumInstancedCurves());

			/* is this channel type valid for the participant type? */

			channel.SetValidForExport(false);
			channel.SetValidForEval(false);
			switch(participantType)
			{
			case Participant::TYPE_CAMERA:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_TRANSLATION_X:
				case ParticipantChannel::TYPE_TRANSLATION_Y:
				case ParticipantChannel::TYPE_TRANSLATION_Z:
				case ParticipantChannel::TYPE_FOCAL_LENGTH:
				case ParticipantChannel::TYPE_HORIZONTAL_FILM_APERTURE:
				case ParticipantChannel::TYPE_VERTICAL_FILM_APERTURE:
					channel.SetValidForExport(true);
					break;
				case ParticipantChannel::TYPE_ROTATION_X:
				case ParticipantChannel::TYPE_ROTATION_Y:
				case ParticipantChannel::TYPE_ROTATION_Z:
					channel.SetValidForExport(true);
					break;
				case ParticipantChannel::TYPE_VISIBILITY:
					channel.SetValidForEval(true);
					break;
				}
				break;
			case Participant::TYPE_MAIN_CHARACTER:
			case Participant::TYPE_ENVIRONMENTAL_ACTOR:
			case Participant::TYPE_LOAD_SCREEN_ACTOR:
			case Participant::TYPE_DUMMY_PARENT:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_TRANSLATION_X:
				case ParticipantChannel::TYPE_TRANSLATION_Y:
				case ParticipantChannel::TYPE_TRANSLATION_Z:
				case ParticipantChannel::TYPE_ROTATION_X:
				case ParticipantChannel::TYPE_ROTATION_Y:
				case ParticipantChannel::TYPE_ROTATION_Z:
				case ParticipantChannel::TYPE_SCALE_X:
				case ParticipantChannel::TYPE_SCALE_Y:
				case ParticipantChannel::TYPE_SCALE_Z:
					channel.SetValidForExport(true);
					break;
				case ParticipantChannel::TYPE_VISIBILITY:
					channel.SetValidForEval(true);
					break;
				}
				break;
			case Participant::TYPE_PARTICLE_EFFECT:
			case Participant::TYPE_SOUND:

				/* for these types, we never do any attaching to curves in the cut scene, we just evaluate */
				/* curves here in the filter to create discrete events for the cut scene, like a sound playing */

				switch(channelType)
				{
				case ParticipantChannel::TYPE_TRANSLATION_X:
				case ParticipantChannel::TYPE_TRANSLATION_Y:
				case ParticipantChannel::TYPE_TRANSLATION_Z:
				case ParticipantChannel::TYPE_VISIBILITY:
					channel.SetValidForEval(true);
					break;
				}
				break;
			case Participant::TYPE_PARTICLE_EMITTER:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_TRANSLATION_X:
				case ParticipantChannel::TYPE_TRANSLATION_Y:
				case ParticipantChannel::TYPE_TRANSLATION_Z:
					channel.SetValidForExport(true);
					break;
				case ParticipantChannel::TYPE_VISIBILITY:
					channel.SetValidForEval(true);	// use to activate/deactivate the emitter
					break;
				}
				break;
			case Participant::TYPE_DISTORTION:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_SCALE_X:	// x scale controls amplitude
				case ParticipantChannel::TYPE_SCALE_Y:	// y scale control opacity
					channel.SetValidForExport(true);
					break;
				}
				break;
			case Participant::TYPE_MOTION_BLUR:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_SCALE_X:	// x scale controls motion blur strength
					channel.SetValidForExport(true);
					break;
				}
				break;
			case Participant::TYPE_LIGHT:
				/* different lights use different curves. ambient lights are just color values, and always visible. */
				/* directional lights get their direction from the rotation, have a color, and are always visible. */
				/* local lights have a position and color, and maybe visibility info */

				switch(participant.GetFlags() & (Participant::FLAGS_AMBIENT_LIGHT | Participant::FLAGS_DIRECTIONAL_LIGHT | Participant::FLAGS_LOCAL_LIGHT))
				{
				case Participant::FLAGS_AMBIENT_LIGHT:
					switch(channelType)
					{
					case ParticipantChannel::TYPE_COLOR_RED:
					case ParticipantChannel::TYPE_COLOR_GREEN:
					case ParticipantChannel::TYPE_COLOR_BLUE:
						channel.SetValidForExport(true);
						break;
					}
					break;
				case Participant::FLAGS_DIRECTIONAL_LIGHT:
					switch(channelType)
					{
					case ParticipantChannel::TYPE_ROTATION_X:
					case ParticipantChannel::TYPE_ROTATION_Y:
					case ParticipantChannel::TYPE_ROTATION_Z:
					case ParticipantChannel::TYPE_COLOR_RED:
					case ParticipantChannel::TYPE_COLOR_GREEN:
					case ParticipantChannel::TYPE_COLOR_BLUE:
						channel.SetValidForExport(true);
						break;
					}
					break;
				case Participant::FLAGS_LOCAL_LIGHT:
					switch(channelType)
					{
					case ParticipantChannel::TYPE_TRANSLATION_X:
					case ParticipantChannel::TYPE_TRANSLATION_Y:
					case ParticipantChannel::TYPE_TRANSLATION_Z:
					case ParticipantChannel::TYPE_COLOR_RED:
					case ParticipantChannel::TYPE_COLOR_GREEN:
					case ParticipantChannel::TYPE_COLOR_BLUE:
						channel.SetValidForExport(true);
						break;
					case ParticipantChannel::TYPE_VISIBILITY:
						channel.SetValidForEval(true);
						break;
					}
					break;
				default:
					ASSERT(false);
					break;
				}
				break;
			case Participant::TYPE_TRIGGER:
				switch(channelType)
				{
				case ParticipantChannel::TYPE_VISIBILITY:
					channel.SetValidForEval(true);
					break;
				}
				break;
			}

			/* if participant is not valid for export, neither are any curves. but it's useful to log them */

			if (participant.ValidForExport() == false)
			{
				channel.SetValidForExport(false);
				channel.SetValidForEval(false);
			}

			/* log source curves */

			int numSourceCurves = channel.GetNumSourceCurves();
			for (int sourceCurveIndex=0;sourceCurveIndex<numSourceCurves;sourceCurveIndex++)
			{
				float startTime = channel.GetSourceStartTime(sourceCurveIndex);
				float duration = channel.GetSourceDuration(sourceCurveIndex);
				int sourceFlags = channel.GetSourceFlags(sourceCurveIndex);

				m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "      Source curve %d: start %.2f, end %.2f, flags 0x%x",
							 sourceCurveIndex, startTime, startTime + duration, sourceFlags);
			}

			/* log instance curves, look for largest end time, create data for eval curves */

			bool bChannelHasValidCurveData = false;
			int numInstancedCurves = channel.GetNumInstancedCurves();
			for (int instancedCurveIndex=0;instancedCurveIndex<numInstancedCurves;instancedCurveIndex++)
			{
				float startTime = channel.GetInstanceStartTime(instancedCurveIndex);
				float cycle = channel.GetInstanceCycle(instancedCurveIndex);
				float weight = channel.GetInstanceWeight(instancedCurveIndex);
				float scale = channel.GetInstanceScale(instancedCurveIndex);
				int instanceFlags = channel.GetInstanceFlags(instancedCurveIndex);

				m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "      Instance curve %d: start %.2f, cycle %.2f, flags 0x%x",
							 instancedCurveIndex, startTime, cycle, instanceFlags);

				/* maybe build engine-compatible curve data */

				if ((channel.IsValidForExport()) || (channel.IsValidForEval()))
				{
					int sourceRef = channel.GetInstanceSourceCurveReference(instancedCurveIndex);
					TupperwareAggregate *pAnimCurveAgg = channel.GetSourceCurveData(sourceRef);
					TupAnimCurve *pTupAnimCurve = new TupAnimCurve( pAnimCurveAgg );

					/* curves need unique names */

					char CurveName[100];
					sprintf(CurveName, "%s%d_%d_%d", participant.GetName(), participant.GetBuildIndex(), participant.GetChannel(channelIndex).GetType(), instancedCurveIndex);
					char *pCurveName = CurveName;
					int nLength = strlen(pCurveName);
					if (nLength > ANIM_CURVE_NAME_LENGTH - 1)
						pCurveName += (nLength - (ANIM_CURVE_NAME_LENGTH - 1));

					/* build curve data */

					channel.ConvertSourceCurveData(participantType);	// handle any conversion that may be necessary
					ts_AnimCurve *pAnimCurveData = BuildAnimCurve(pTupAnimCurve,
																				 pCurveName,
																				 channel.GetInstanceStartTime(instancedCurveIndex),
																				 channel.GetType());
					channel.SetInstanceAnimCurveData(instancedCurveIndex, pAnimCurveData);

					/* curve is valid if it has valid data and starts before the cut scene ends */

					bool bCurveValid = ((pAnimCurveData) && (channel.GetInstanceStartTime(instancedCurveIndex) < m_fCutSceneEndTime));
					bChannelHasValidCurveData |= bCurveValid;	// keep track of channel's status

					/* clean up */

					delete pTupAnimCurve;

					/* if valid for output, a bit of setup for later */

					if (channel.IsValidForExport())
					{
						if (bCurveValid)	// is this instanced curve (out of possibly multiple curves in channel) valid?
						{
							/* setup for later processing/sorting by time */

							channel.SetInstanceProcessed(instancedCurveIndex, false);

							/* keep track of valid curves for output */

							nOutputCurveCount++;	// for .dbl chunk header
						}
						else
							channel.SetInstanceProcessed(instancedCurveIndex, true);	// ignore this curve
					}
				}
			}

			/* if none of the instanced curves had valid data, then channel is no longer valid */

			if (bChannelHasValidCurveData == false)
			{
				channel.SetValidForExport(false);
				channel.SetValidForEval(false);
			}

			/* can save a little output data by looking to see if curves provided for all components of position/rotation */

			if (channel.IsValidForExport())
			{
				switch(channelType)
				{
				case ParticipantChannel::TYPE_TRANSLATION_X:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_TRANSLATE_X_CURVE);
					break;
				case ParticipantChannel::TYPE_TRANSLATION_Y:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_TRANSLATE_Y_CURVE);
					break;
				case ParticipantChannel::TYPE_TRANSLATION_Z:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_TRANSLATE_Z_CURVE);
					break;
				case ParticipantChannel::TYPE_ROTATION_X:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_ROTATE_X_CURVE);
					break;
				case ParticipantChannel::TYPE_ROTATION_Y:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_ROTATE_Y_CURVE);
					break;
				case ParticipantChannel::TYPE_ROTATION_Z:
					participant.SetFlags(participant.GetFlags() | Participant::FLAGS_HAS_ROTATE_Z_CURVE);
					break;
				}
			}
		}

		/* log source actions */

		int numSourceActions = participant.GetNumSourceActions();
		for (int sourceActionIndex=0;sourceActionIndex<numSourceActions;sourceActionIndex++)
		{
			ParticipantSourceAction &sourceAction = participant.GetSourceAction(sourceActionIndex);
			float startTime = sourceAction.GetStartTime();
			float duration = sourceAction.GetDuration();

			m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "   Source action %d: %s, start %.2f, end %.2f",
				sourceActionIndex, sourceAction.GetName(), startTime, startTime + duration);
		}

		/* log instanced actions */

		int numInstancedActions = participant.GetNumInstancedActions();
		for (int instancedActionIndex=0;instancedActionIndex<numInstancedActions;instancedActionIndex++)
		{
			ParticipantInstancedAction &instancedAction = participant.GetInstancedAction(instancedActionIndex);
			float startTime = instancedAction.GetStartTime();
			int sourceRef = instancedAction.GetSourceReference();
			float duration = participant.GetSourceAction(sourceRef).GetDuration();

			m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "   Instanced action %d: %s, start %.2f, end %.2f",
				instancedActionIndex, instancedAction.GetName(), startTime, startTime + duration);

			/* for later processing */

			instancedAction.SetProcessed(false);
		}
	}

	/* we may need to merge multiple participants into one participant in the cut scene, for example if the cut scene */
	/* has been built with more than once camera */

	m_nDataStreamCount = numParticipants - nNotValidForExportParticipants;
	if (nCameraParticipants > 1)
		m_nDataStreamCount -= (nCameraParticipants - 1);
	if (nParticleEffectParticipants > 1)
		m_nDataStreamCount -= (nParticleEffectParticipants - 1);
	if (nSoundParticipants > 1)
		m_nDataStreamCount -= (nSoundParticipants - 1);
	if (nTriggerParticipants > 1)
		m_nDataStreamCount -= (nTriggerParticipants - 1);
	m_pDataStream = (ts_MergedDataStream *) malloc(m_nDataStreamCount * sizeof(ts_MergedDataStream));
	memset(m_pDataStream, 0, m_nDataStreamCount * sizeof(ts_MergedDataStream));
	int nDataStreamIndex = 0;
	for (participantIndex=0;participantIndex<numParticipants;participantIndex++)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(participantIndex);
		Participant::Types participantType = participant.GetType();

		/* not all participants get exported */

		if (participant.ValidForExport() == false)
			continue;

		/* if this is a type that should get merged, look to see if a merged participant has been set up for this type */

		bool bSearch = false;
		switch(participantType)
		{
		case Participant::TYPE_CAMERA:
		case Participant::TYPE_PARTICLE_EFFECT:
		case Participant::TYPE_SOUND:
		case Participant::TYPE_TRIGGER:
			bSearch = true;
			break;
		}
		int nFoundDataStreamIndex = -1;
		if (bSearch)
		{
			for (int i = 0; i < nDataStreamIndex; i++)
			{
				/* is data stream record same type as current participant? */

				ASSERT(m_pDataStream[i].nCount > 0);	// if it's in the list, it should have at least one participant index
				if (participantType == m_pParticipantManager->GetParticipant(m_pDataStream[i].nParticipantIndex[0]).GetType())
				{
					nFoundDataStreamIndex = i;
					break;
				}
			}
		}

		/* either merge current participant into existing merged participant record, or create a new one */

		if (nFoundDataStreamIndex == -1)
		{
			/* new record-- current participant being placed is either a type that doesn't get merged, or */
			/* is the first one of its type */

			m_pDataStream[nDataStreamIndex].nParticipantIndex[0] = participantIndex;
			m_pDataStream[nDataStreamIndex].nCount = 1;
			nDataStreamIndex++;
		}
		else
		{
			/* add current participant to existing record */

			if (m_pDataStream[nFoundDataStreamIndex].nCount >= MAX_PARTICIPANTS_TO_MERGE)
				throw(PluginException(ERROR_FATAL,"Increase MAX_PARTICIPANTS_TO_MERGE!"));
			m_pDataStream[nFoundDataStreamIndex].nParticipantIndex[m_pDataStream[nFoundDataStreamIndex].nCount] = participantIndex;
			m_pDataStream[nFoundDataStreamIndex].nCount++;
		}
	}
	ASSERT(nDataStreamIndex == m_nDataStreamCount);

	/* if there is only one camera, then it should always be visible-- it doesn't make sense for it to be invisible */
	/* the whole time, or for even part of the time. this protects against user error-- i've seen a single camera */
	/* marked visibility = off, so no data is output for the camera datastream, and the camera never functions */

	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		if (m_pDataStream[nDataStreamIndex].nCount == 1)
		{
			int nParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[0];
			Participant &participant = m_pParticipantManager->GetParticipant(nParticipantIndex);
			if (participant.GetType() == Participant::TYPE_CAMERA)
				participant.SetAlwaysVisible();
		}
	}

	/* set datastream export index for participants so i can hook participants up to parents */

	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		for (int i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
		{
			int nParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[i];
			Participant &participant = m_pParticipantManager->GetParticipant(nParticipantIndex);
			participant.SetExportIndex(nDataStreamIndex);
		}
	}

	/* data streams with more than one merged participant have to assume that all initial event state data is needed */
	/* because of the way we cut between them */

	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		if (m_pDataStream[nDataStreamIndex].nCount < 2)
			continue;

		/* clear participant flags */

		for (int i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
		{
			int nParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[i];
			Participant &participant = m_pParticipantManager->GetParticipant(nParticipantIndex);
			int nFlags = participant.GetFlags();
			nFlags &= ~(Participant::FLAGS_HAS_TRANSLATE_X_CURVE |
							Participant::FLAGS_HAS_TRANSLATE_Y_CURVE |
							Participant::FLAGS_HAS_TRANSLATE_Z_CURVE |
							Participant::FLAGS_HAS_ROTATE_X_CURVE |
							Participant::FLAGS_HAS_ROTATE_Y_CURVE |
							Participant::FLAGS_HAS_ROTATE_Z_CURVE);
			participant.SetFlags(nFlags);
		}
	}

	/* where possible, we prefer that the first participant in a merged participant list is visible at the start */
	/* of the cut scene. this is so checking the translate and rotate flags is valid */

	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		if (m_pDataStream[nDataStreamIndex].nCount < 2)
			continue;

		/* find first participant that is visible at time == 0-- may not be any, in which case we don't care */

		for (int i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
		{
			int nParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[i];
			Participant &participant = m_pParticipantManager->GetParticipant(nParticipantIndex);

			/* is participant visible at time == 0? */

			float fVisible = participant.EvaluateChannel(ParticipantChannel::TYPE_VISIBILITY, 0.0f);
			if (fVisible == 1.0f)
				break;
		}
		if (i == m_pDataStream[nDataStreamIndex].nCount)
			continue;	// no participant is visible, don't worry about it

		/* found a participant who is visible at time == 0. need to move him to the front? */

		if (i > 0)
		{
			/* swap him to the front of the list */

			int nParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[i];
			m_pDataStream[nDataStreamIndex].nParticipantIndex[i] = m_pDataStream[nDataStreamIndex].nParticipantIndex[0];
			m_pDataStream[nDataStreamIndex].nParticipantIndex[0] = nParticipantIndex;
		}
	}

	/* malloc a buffer for binary data */

	m_pBinaryData = (char *) malloc(100000);
	if (m_pBinaryData == NULL)
		throw(PluginException(ERROR_FATAL,"Could not malloc binary buffer."));
	m_pBinaryDataStart = m_pBinaryData;
	char HelperString[100];

	/* start log file-- this is text data that used to drive the cut scene manager, but it has been replaced by */
	/* binary data. the cut scene manager can still play the text file if needed, look for the #define BINARY_CUT_DATA */
	/* in CutSceneMgr.cpp */

	TupString outputFilename(dataManager.GetOutputFilename());
	FilePath::ChangeExtension(outputFilename,".cutlog");
	m_pCutLogFile = fopen(outputFilename, "wt");
	if (m_pCutLogFile == NULL)
		throw(PluginException(ERROR_FATAL,"Could not open .log file."));

	/* header */

	fprintf(m_pCutLogFile, "# cut scene file %s\n", outputFilename);
	fprintf(m_pCutLogFile, "# created from file %s\n", dataManager.GetInputFilename());
	fprintf(m_pCutLogFile, "# source data at %d fps\n", (int) fFramesPerSecond);
	fprintf(m_pCutLogFile, "\n");
	WriteData(CCutSceneInterpreter::CUT_SCENE_TYPE, "type", "file", "this is where the file interpreter hooks in");
	WriteData(CCutSceneInterpreter::CUT_SCENE_VERSION, "version", 1.3f);
	WriteData(CCutSceneInterpreter::CUT_SCENE_START, "start", m_fCutSceneStartTime, "starting time of cut scene (typically 0, but non-zero useful for preview), in seconds");
	WriteData(CCutSceneInterpreter::CUT_SCENE_LENGTH, "length", m_fCutSceneEndTime, "length of cut scene, in seconds");
	WriteData(CCutSceneInterpreter::CUT_SCENE_FADE, "fade", nFadeType, "fade type (0 = none, 1 = in, 2 = out, 3 = in/out)");

	/* number of participants, and labels for data. all camera participants get combined into one participant */

	WriteData(CCutSceneInterpreter::CUT_SCENE_COUNT, "count", m_nDataStreamCount, "data streams in this cut scene");
	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		sprintf(HelperString, "%d", nDataStreamIndex);
		WriteData(CCutSceneInterpreter::CUT_SCENE_AT_LABEL, "at_label", HelperString, "where to find this data");
	}
	fprintf(m_pCutLogFile, "\n");

	/* each participant (at this point, each participant may include multiple participants from .tup file, */
	/* merged together) */

	for (nDataStreamIndex = 0; nDataStreamIndex < m_nDataStreamCount; nDataStreamIndex++)
	{
		/* get first participant from merged table (may only be one) */

		int nFirstParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[0];
		Participant &FirstParticipant = m_pParticipantManager->GetParticipant(nFirstParticipantIndex);

		/* log */

		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW, "Data stream %d: %s, %s", nDataStreamIndex, FirstParticipant.GetName(), FirstParticipant.GetTypeName());
		if (m_pDataStream[nDataStreamIndex].nCount > 1)
		{
			for (int i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
				m_log.Add(LogFlags::VERBOSE_LEVEL_MED, "   Merged: %s",
					m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[i]).GetName());
		}

		/* start cut scene data */

		fprintf(m_pCutLogFile, "# cut scene 0, data stream %d-- %s\n", nDataStreamIndex, FirstParticipant.GetTypeName());
		fprintf(m_pCutLogFile, "\n");
		sprintf(HelperString, "%d", nDataStreamIndex);
		WriteData(CCutSceneInterpreter::CUT_SCENE_LABEL, "label", HelperString, "start of datastream");
		WriteData(CCutSceneInterpreter::CUT_SCENE_TYPE, "type", FirstParticipant.GetTypeName());

		/* parent datastream, to implement hierarchical animation */

		int nParentExportIndex = -1;
		if (m_pDataStream[nDataStreamIndex].nCount == 1)
		{
			nParentExportIndex = FirstParticipant.GetParent();
			if (nParentExportIndex != -1)
			{
				/* participant has a parent. what is its datastream index? */

				Participant &ParentParticipant = m_pParticipantManager->GetParticipant(nParentExportIndex);
				nParentExportIndex = ParentParticipant.GetExportIndex();
			}
		}
		WriteData(CCutSceneInterpreter::CUT_SCENE_INDEX, "index", nDataStreamIndex);
		WriteData(CCutSceneInterpreter::CUT_SCENE_PARENT, "parent", nParentExportIndex);

		/* curve count is sum of curves for all merged participants, count each curve type only once, */
		/* since only one curve of each type can be valid for evaluation at any time (well, until we get */
		/* to blending between curves!) */

		int nCurveCount = 0;
		bool bTypeUsed[ParticipantChannel::NUM_TYPES];
		memset(bTypeUsed, 0, ParticipantChannel::NUM_TYPES * sizeof(bool));
		for (int i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
		{
			int nIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[i];
			Participant &participant = m_pParticipantManager->GetParticipant(nIndex);
			int numChannels = participant.GetNumChannels();
			for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
			{
				ParticipantChannel &channel = participant.GetChannel(channelIndex);
				if ((channel.IsValidForExport()) && (channel.GetNumInstancedCurves() != 0) && (bTypeUsed[channel.GetType()] == false))
				{
					nCurveCount++;
					bTypeUsed[channel.GetType()] = true;
				}
			}
		}
		WriteData(CCutSceneInterpreter::CUT_SCENE_CURVE_COUNT, "curve_count", nCurveCount);

		/* participant-type-specific channel initialization */

		switch(FirstParticipant.GetType())
		{
		case Participant::TYPE_MAIN_CHARACTER:
		case Participant::TYPE_LOAD_SCREEN_ACTOR:
			WriteData(CCutSceneInterpreter::CUT_SCENE_NAME, "name", "tak");
			WriteData(CCutSceneInterpreter::CUT_SCENE_SHOW_CLUB, "show_club", (FirstParticipant.GetFlags() & Participant::FLAGS_CLUB_VISIBLE) ? 1 : 0);
			WriteData(CCutSceneInterpreter::CUT_SCENE_SHOW_RATTLE, "show_rattle", (FirstParticipant.GetFlags() & Participant::FLAGS_RATTLE_VISIBLE) ? 1 : 0);
			WriteData(CCutSceneInterpreter::CUT_SCENE_SHOW_BLOWGUN, "show_blowgun", (FirstParticipant.GetFlags() & Participant::FLAGS_BLOWGUN_VISIBLE) ? 1 : 0);
			OutputTransformOrder(&FirstParticipant);
			break;
		case Participant::TYPE_ENVIRONMENTAL_ACTOR:
			WriteData(CCutSceneInterpreter::CUT_SCENE_NAME, "name", FirstParticipant.GetName());
			if (FirstParticipant.GetFlags() & Participant::FLAGS_EXISTING_ACTOR)
			{
				/* hook up to existing actor in world */

				WriteData(CCutSceneInterpreter::CUT_SCENE_NEW, "new", (int) 0);

				/* may want to restore actor to base pose after cut scene is done (e.g. re-close a door or something) */

				WriteData(CCutSceneInterpreter::CUT_SCENE_RESTORE_TO_BASE_POSE, "restore_to_base_pose", (int) ((FirstParticipant.GetFlags() & Participant::FLAGS_RESTORE_TO_BASE_POSE) != 0));
			}
			else
			{
				/* must have an envname to use to create actor */

				if (strlen(FirstParticipant.GetEnvName()) == 0)
				{
					char Message[200];
					sprintf(Message, "Participant %s (%s): Must be marked as an environmental actor (e.g. Simple or Orang)", FirstParticipant.GetName(), FirstParticipant.GetTypeName());
					throw(PluginException(ERROR_WARNING, Message));
				}

				/* create a new actor for use by cut scene-- have to give enough info for CLevelScript::Simple__Create */

				WriteData(CCutSceneInterpreter::CUT_SCENE_NEW, "new", (int) 1);
				WriteData(CCutSceneInterpreter::CUT_SCENE_CLASS, "class", FirstParticipant.GetEnvName());
				WriteData(CCutSceneInterpreter::CUT_SCENE_MODEL_NAME, "model_name", FirstParticipant.GetModelName());

				/* shadow? */

				if (FirstParticipant.GetFlags() & Participant::FLAGS_ADD_SHADOW)
				{
					WriteData(CCutSceneInterpreter::CUT_SCENE_SHADOW, "shadow", (int) 1);
					WriteData(CCutSceneInterpreter::CUT_SCENE_SHADOW_RADIUS, "radius", FirstParticipant.GetShadowRadius());
					WriteData(CCutSceneInterpreter::CUT_SCENE_SHADOW_ALPHA, "alpha", FirstParticipant.GetShadowAlpha());
				}
				else
					WriteData(CCutSceneInterpreter::CUT_SCENE_SHADOW, "shadow", (int) 0);
			}
			OutputTransformOrder(&FirstParticipant);
			break;
		case Participant::TYPE_PARTICLE_EMITTER:
			WriteData(CCutSceneInterpreter::CUT_SCENE_NAME, "name", FirstParticipant.GetName());
			OutputTransformOrder(&FirstParticipant);
			break;
		case Participant::TYPE_LIGHT:
			{
			/* light type */

			int nValue = 0;
			switch(FirstParticipant.GetFlags() & (Participant::FLAGS_AMBIENT_LIGHT | Participant::FLAGS_DIRECTIONAL_LIGHT | Participant::FLAGS_LOCAL_LIGHT))
			{
			case Participant::FLAGS_AMBIENT_LIGHT:
				nValue = CCutSceneInterpreter::AMBIENT_LIGHT;
				break;
			case Participant::FLAGS_DIRECTIONAL_LIGHT:
				nValue = CCutSceneInterpreter::DIRECTIONAL_LIGHT;
				break;
			case Participant::FLAGS_LOCAL_LIGHT:
				nValue = CCutSceneInterpreter::LOCAL_LIGHT;
				break;
			default:
				ASSERT(false);
				break;
			}
			sprintf(HelperString, "%d = ambient, %d = directional, %d = local", CCutSceneInterpreter::AMBIENT_LIGHT, CCutSceneInterpreter::DIRECTIONAL_LIGHT, CCutSceneInterpreter::LOCAL_LIGHT);
			WriteData(CCutSceneInterpreter::CUT_SCENE_LIGHT_TYPE, "light_type", nValue, HelperString);

			/* light's initial color */

			WriteData(CCutSceneInterpreter::CUT_SCENE_RED, "red", FirstParticipant.EvaluateChannel(ParticipantChannel::TYPE_COLOR_RED, 0.0f));
			WriteData(CCutSceneInterpreter::CUT_SCENE_GREEN, "green", FirstParticipant.EvaluateChannel(ParticipantChannel::TYPE_COLOR_GREEN, 0.0f));
			WriteData(CCutSceneInterpreter::CUT_SCENE_BLUE, "blue", FirstParticipant.EvaluateChannel(ParticipantChannel::TYPE_COLOR_BLUE, 0.0f));

			/* directional light needs to know transform order */

			OutputTransformOrder(&FirstParticipant);
			break;
			}
		}

		/* add events over the life of the cut scene. if this participant has more than one merged participant, */
		/* only one can be active at a time. the visibility curve controls this. in the case of merging all particle */
		/* effects into one data stream, each time the visibility goes to 1.0, it is considered to drop off to 0.0 again */
		/* immediately, so multiple effects can fire at the same time */

		switch(FirstParticipant.GetType())
		{
		case Participant::TYPE_PARTICLE_EFFECT:
		case Participant::TYPE_SOUND:
		case Participant::TYPE_TRIGGER:
			/* these types are different-- each event is self-contained (like play a sound at this position) */
			/* instead of persistant (like move a character around on the screen) */

			OutputDiscreteEvents(nDataStreamIndex);
			break;
		default:
			if (OutputEvents(nDataStreamIndex) == false)
			{
				/* i'm going to call it an error if no active participant is found */

				int nFirstParticipantIndex = m_pDataStream[nDataStreamIndex].nParticipantIndex[0];
				Participant &FirstParticipant = m_pParticipantManager->GetParticipant(nFirstParticipantIndex);
				char Error[200];
				sprintf(Error, "No valid (visible) participant for datastream %s, first participant %s", FirstParticipant.GetTypeName(), FirstParticipant.GetName());
				throw(PluginException(ERROR_FATAL, Error));
			}
			break;
		}

		/* end of data stream */

		WriteData(CCutSceneInterpreter::CUT_SCENE_END, "end", (int) 1, "end of data stream");
		fprintf(m_pCutLogFile, "\n");
	}

	/* done with log file */

	fclose(m_pCutLogFile);
	m_pCutLogFile = NULL;

	/* write out the binary data */

	outputFilename = dataManager.GetOutputFilename();
	FilePath::ChangeExtension(outputFilename,".CUTBIN");
	DBLUtil::StartDBLFile(outputFilename);
	DBLUtil::StartChunk(DB_CUT_BINARY, DB_CUT_BINARY_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	if (pDBLFile == NULL)
		throw(PluginException(ERROR_FATAL,"Could not open .cutbin file."));
	fwrite(m_pBinaryDataStart, m_pBinaryData - m_pBinaryDataStart, 1, pDBLFile);
	DBLUtil::FinishChunk(false);
	DBLUtil::FinishDBLFile();
	DBLUtil::CleanUpDBLFile();
	free(m_pBinaryDataStart);
	m_pBinaryData = NULL;
	m_pBinaryDataStart = NULL;

	/* create the kfm list file to convert into a .mdb file */

	outputFilename = dataManager.GetOutputFilename();
	FilePath::ChangeExtension(outputFilename,".kfmlst");
	if (m_pKFMBuffer == NULL)
	{
		/* no kfm's used */

		remove(outputFilename);
	}
	else
	{
		FILE *pKFMListFile = fopen(outputFilename, "wt");
		if (pKFMListFile == NULL)
			throw(PluginException(ERROR_FATAL,"Could not open .kfmlst file."));

		/* we assume that kfm's are in the same directory as the .mb file */

	   char Drive[_MAX_DRIVE];
	   char Dir[_MAX_DIR];
		_splitpath(pSourceFileName, Drive, Dir, NULL, NULL);

		/* go through kfm buffer */

		char *pKFMName = m_pKFMBuffer;
		char FileName[_MAX_PATH];
		while ((pKFMName) && (*pKFMName != 0))	// out of strings when we find an empty string
		{
			fprintf(pKFMListFile, "MOTION_ID:     CUT_SCENE_ANIM\n");
			_makepath(FileName, Drive, Dir, pKFMName, ".kfm");
			fprintf(pKFMListFile, "MOTION_FILE:   %s\n", FileName);

			/* to next kfm */

			pKFMName += strlen(pKFMName) + 1;	// to next string
		}

		/* done */

		fclose(pKFMListFile);
	}

	/* create curve data .dbl */

	outputFilename = dataManager.GetOutputFilename();
	FilePath::ChangeExtension(outputFilename,".DBL");
	if (nOutputCurveCount == 0)
		remove(outputFilename.GetBuffer(10));
	else
	{
		DBLUtil::StartDBLFile(outputFilename);
		DBLUtil::StartChunk(DB_ANIM_CURVE, DB_ANIM_CURVE_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
		DBLUtil::StartRecordList(nOutputCurveCount, RECORD_LIST_HAS_OFFSET_TABLE, 0);
		FILE *pDBLFile = DBLUtil::GetDBLFile();

		/* write out curves-- order doesn't matter, they are hooked up by name */

		for (participantIndex=0;participantIndex<numParticipants;participantIndex++)
		{
			Participant &participant = m_pParticipantManager->GetParticipant(participantIndex);
			int numChannels = participant.GetNumChannels();

			/* not all participants get exported */

			if (participant.ValidForExport() == false)
				continue;

			/* loop through channels */

			for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
			{
				ParticipantChannel &channel = participant.GetChannel(channelIndex);
				if (channel.IsValidForExport() == false)
					continue;

				/* loop through instanced curves */

				int numInstancedCurves = channel.GetNumInstancedCurves();
				for (int instancedCurveIndex=0;instancedCurveIndex<numInstancedCurves;instancedCurveIndex++)
				{
					ts_AnimCurve *pAnimCurve = channel.GetInstanceAnimCurveData(instancedCurveIndex);
					if (pAnimCurve == NULL)
						continue;
					if (channel.GetInstanceStartTime(instancedCurveIndex) >= m_fCutSceneEndTime)
						continue;

					int nVector2Count = (pAnimCurve->nSplineCount * 3) + 1;
					int nSize = sizeof(ts_AnimCurve) + ((nVector2Count - 4) * sizeof(Vector2));
					DBLUtil::RecordListEntry();

					/* log */

					m_log.Add(LogFlags::VERBOSE_LEVEL_LOW, "Export curve %s, %s, start %.2f, end %.2f, %d splines",
						 pAnimCurve->Name, channel.GetTypeName(), pAnimCurve->fStartTime, pAnimCurve->fEndTime, pAnimCurve->nSplineCount);
					for (int i = 0; i < nVector2Count; i++)
						m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH, "   %d: %.2f %f", i, pAnimCurve->SplineData[i].X(), pAnimCurve->SplineData[i].Y());

					/* record is variably sized. first write out known portion (contains first spline) */

					DBLWrite::WriteRecord(pAnimCurve, pDBLFile);

					/* now write out the rest of the spline data */

					if (nVector2Count > 4)
						DBLWrite::WriteRecords(&pAnimCurve->SplineData[4], pDBLFile, nVector2Count - 4);
					DBLUtil::AlignFile();
				}
			}
		}

		/* finish .dbl */

		DBLUtil::FinishRecordList();
		DBLUtil::FinishChunk(false);
		DBLUtil::FinishDBLFile();
		DBLUtil::CleanUpDBLFile();
	}

	/* clean up-- looks like destructor is never called */

	delete pTupScene;
	delete pTupNodePool;
	delete pTupObjectPool;
	delete m_pParticipantManager;
	m_pParticipantManager = NULL;
	if (m_pDataStream)
	{
		free(m_pDataStream);
		m_pDataStream = NULL;
	}
}

/* convert a TupAnimCurve into game-engine-format data */

ts_AnimCurve *CinemanFilter::BuildAnimCurve(TupAnimCurve *pTupAnimCurve, const char *pCurveName, float fStartTime, int nType)
{
	/* how many splines in the curve? */

	float *pfTimeValues;
	int nEndpointCount;
	pTupAnimCurve->GetInputValues(&pfTimeValues, nEndpointCount);
	if (pfTimeValues == NULL)
	{
		ASSERT(false);
		return(NULL);
	}
	if (nEndpointCount < 2)
	{
		/* invalid curve */

		return(NULL);
	}

	/* point to curve output data (spline y-value) */

	float *pfOutputValues;
	int nConfirmEndpointCount;
	pTupAnimCurve->GetOutputValues(&pfOutputValues, nConfirmEndpointCount);
	if ((pfOutputValues == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}

	/* point to curve key flags */

	int *pnKeyFlags;
	pTupAnimCurve->GetKeyFlags(&pnKeyFlags, nConfirmEndpointCount);
	if ((pnKeyFlags == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}

	/* point to tangent data */

	float *pfInTangentAngles;
	pTupAnimCurve->GetInTangentAngles(&pfInTangentAngles, nConfirmEndpointCount);
	if ((pfInTangentAngles == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}
	float *pfInTangentWeights;
	pTupAnimCurve->GetInTangentWeights(&pfInTangentWeights, nConfirmEndpointCount);
	if ((pfInTangentWeights == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}
	float *pfOutTangentAngles;
	pTupAnimCurve->GetOutTangentAngles(&pfOutTangentAngles, nConfirmEndpointCount);
	if ((pfOutTangentAngles == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}
	float *pfOutTangentWeights;
	pTupAnimCurve->GetOutTangentWeights(&pfOutTangentWeights, nConfirmEndpointCount);
	if ((pfOutTangentWeights == NULL) || (nConfirmEndpointCount != nEndpointCount))
	{
		ASSERT(false);
		return(NULL);
	}

	/* may want to see curve data */

	m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH, "         Curve data %s (time, value, in tangent angle, weight, out tangent angle, weight, flags):", pCurveName);
	for (int nIndex = 0; nIndex < nEndpointCount; nIndex++)
		m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH, "            %d: %.2f %.2f, %.2f %.2f, %.2f %.2f, 0x%x",
					 nIndex,
					 pfTimeValues[nIndex],
					 pfOutputValues[nIndex],
					 pfInTangentAngles[nIndex],
					 pfInTangentWeights[nIndex],
					 pfOutTangentAngles[nIndex],
					 pfOutTangentWeights[nIndex],
					 pnKeyFlags[nIndex]);

	/* we can't currently handle the key flags for maya splines. one key flag says that the out tangent type is "step", */
	/* which means that the spline holds the first endpoint's value until the second endpoint, then changes discontinuously. */
	/* that's useful, but doesn't fit into our current definition of a spline (we don't have flags, for one thing). so */
	/* at this point we'll support it by adding a new key value into the data-- y-value same as first endpoint, x- (time-) */
	/* value just before second key value */

/* logic is, if not last point (since evaluating curve past last point already acts as a step), and marked */
/* as step, and next output value is different than current (if they're the same there's no discontinuity) */
#define NEEDS_INSERTED_ENDPOINT()																												\
	((i < (nEndpointCount - 1)) &&																												\
	 ((pnKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK) == TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP) &&		\
	 (pfOutputValues[i] != pfOutputValues[i + 1]))

	int i;
	int nInsertEndpointCount = 0;
	for (i = 0; i < nEndpointCount; i++)
	{
		if (NEEDS_INSERTED_ENDPOINT())
			nInsertEndpointCount++;
	}

	/* insert endpoints if needed to handle step tangents */

	if (nInsertEndpointCount != 0)
	{
		float *pfNewTimeValues = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		float *pfNewOutputValues = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		float *pfNewInTangentAngles = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		float *pfNewInTangentWeights = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		float *pfNewOutTangentAngles = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		float *pfNewOutTangentWeights = (float *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(float));
		int *pnNewKeyFlags = (int *) malloc((nEndpointCount + nInsertEndpointCount) * sizeof(int));
		int nIndex = 0;
		for (i = 0; i < nEndpointCount; i++)
		{
			/* always copy original data */

			pfNewTimeValues[nIndex] = pfTimeValues[i];
			pfNewOutputValues[nIndex] = pfOutputValues[i];
			pfNewInTangentAngles[nIndex] = pfInTangentAngles[i];
			pfNewInTangentWeights[nIndex] = pfInTangentWeights[i];
			pfNewOutTangentAngles[nIndex] = pfOutTangentAngles[i];
			pfNewOutTangentWeights[nIndex] = pfOutTangentWeights[i];
			pnNewKeyFlags[nIndex] = pnKeyFlags[i];
			nIndex++;

			/* need to insert a new endpoint? */

			if (NEEDS_INSERTED_ENDPOINT())
			{
				ASSERT(i < nEndpointCount - 1);	// we need next endpoint's output value, so there should be one
				pfNewTimeValues[nIndex] = pfTimeValues[i + 1];
				pfNewOutputValues[nIndex] = pfOutputValues[i];
				pfNewInTangentAngles[nIndex] = pfInTangentAngles[i];
				pfNewInTangentWeights[nIndex] = pfInTangentWeights[i];
				pfNewOutTangentAngles[nIndex] = pfOutTangentAngles[i];
				pfNewOutTangentWeights[nIndex] = pfOutTangentWeights[i];
				pnNewKeyFlags[nIndex] = pnKeyFlags[i];
				nIndex++;
			}
		}
		ASSERT(nIndex == nEndpointCount + nInsertEndpointCount);

		/* switch to doctored data, update count */

		pfTimeValues = pfNewTimeValues;
		pfOutputValues = pfNewOutputValues;
		pfInTangentAngles = pfNewInTangentAngles;
		pfInTangentWeights = pfNewInTangentWeights;
		pfOutTangentAngles = pfNewOutTangentAngles;
		pfOutTangentWeights = pfNewOutTangentWeights;
		pnKeyFlags = pnNewKeyFlags;
		nEndpointCount += nInsertEndpointCount;
	}

	/* calculate how much room we need for this curve. pSplineData is a variable length array, */
	/* base record holds space for first 4 points */

	int nVector2Count = ((nEndpointCount - 1) * 3) + 1;
	int nSize = sizeof(ts_AnimCurve) + ((nVector2Count - 4) * sizeof(Vector2));
	ts_AnimCurve *pAnimCurve = (ts_AnimCurve *) malloc(nSize);

	/* fill in name and spline count */

	int nLength = strlen(pCurveName);
	if (nLength > ANIM_CURVE_NAME_LENGTH - 1)
		pCurveName += (nLength - (ANIM_CURVE_NAME_LENGTH - 1));
	strcpy(pAnimCurve->Name, pCurveName);
	pAnimCurve->nSplineCount = nEndpointCount - 1;

	/* delta time between instance and source curve */

	float fDeltaTime = fStartTime - pfTimeValues[0];

	/* spline data */

	Vector2 *pSplineData = pAnimCurve->SplineData;
	for (i = 0; i < pAnimCurve->nSplineCount; i++, pSplineData += 3)	// each spline is 4 points, but first endpoint is same as previous spline's second endpoint
	{
		/* first endpoint */

		pSplineData[0].x( pfTimeValues[i] + fDeltaTime );
		pSplineData[0].y( pfOutputValues[i] );

		/* second endpoint */

		pSplineData[3].x( pfTimeValues[i + 1] + fDeltaTime );
		pSplineData[3].y( pfOutputValues[i + 1] );

		/* first tangent-- linear */

		pSplineData[1].x( pSplineData[0].x( ) + 0.3333f * ( pSplineData[3].x( ) - pSplineData[0].x( ) ) );
		pSplineData[1].y( pSplineData[0].y( ) + 0.3333f * ( pSplineData[3].y( ) - pSplineData[0].y( ) ) );

		/* second tangent-- linear */

		pSplineData[2].x( pSplineData[0].x( ) + 0.6667f * ( pSplineData[3].x( ) - pSplineData[0].x( ) ) );
		pSplineData[2].y( pSplineData[0].y( ) + 0.6667f * ( pSplineData[3].y( ) - pSplineData[0].y( ) ) );

		/* we may be able to use maya tangents to make this spline a curve instead of a straight line */

		bool bValidSpline = true;
		if ((pnKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK) == TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP)
			bValidSpline = false;	// step splines are linear
		else if ((pnKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK) == TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR)
			bValidSpline = false;	// explicitly linear
		else if ((pnKeyFlags[i + 1] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK) == TupAnimCurve::FLAGS_IN_TANGENT_TYPE_LINEAR)
			bValidSpline = false;	// explicitly linear
		else if ((pfOutTangentWeights[i] != 1.0f) || (pfInTangentWeights[i + 1] != 1.0f))
			bValidSpline = false;	// don't know how to interpret weights
		if (bValidSpline)
		{
			/* first tangent */

			float fDeltaX;
			float fDeltaY;
			fDeltaX = pSplineData[1].x() - pSplineData[0].x();	// delta time
			fDeltaY = fDeltaX * pfOutTangentAngles[i];			// tangent angle is tan(angle), tan = opp/adj, so opp = adj * tan
			pSplineData[1].y(pSplineData[0].y() + fDeltaY);

			/* second tangent */

			fDeltaX = pSplineData[3].x() - pSplineData[2].x();	// delta time
			fDeltaY = fDeltaX * pfInTangentAngles[i + 1];		// tangent angle is tan(angle), tan = opp/adj, so opp = adj * tan
			pSplineData[2].y(pSplineData[3].y() - fDeltaY);
		}
	}

	/* start and end time values for this curve */

	pAnimCurve->fStartTime = pAnimCurve->SplineData[0].x();
	pAnimCurve->fEndTime = pAnimCurve->SplineData[nVector2Count - 1].x();

#if 0
	/* may want to test spline calculation */

	m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH, "         Evaluate first spline: %.2f %.2f, %.2f %.2f, %.2f %.2f, %.2f %.2f",
				 pAnimCurve->SplineData[0].x(), pAnimCurve->SplineData[0].y(),
				 pAnimCurve->SplineData[1].x(), pAnimCurve->SplineData[1].y(),
				 pAnimCurve->SplineData[2].x(), pAnimCurve->SplineData[2].y(),
				 pAnimCurve->SplineData[3].x(), pAnimCurve->SplineData[3].y());
	CAnimCurve AnimCurve;
	AnimCurve.Initialize(pAnimCurve);
	for (nIndex = 0; nIndex <= 10; nIndex++)
	{
		float fTime = pAnimCurve->SplineData[0].x() + (((float) nIndex / 10.0f) * (pAnimCurve->SplineData[3].x() - pAnimCurve->SplineData[0].x()));
		m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH, "            %.2f: %.3f", fTime, AnimCurve.Evaluate(fTime));
	}
#endif //0/1

	/* clean up */

	if (nInsertEndpointCount != 0)
	{
		/* inserted extra endpoints, created new arrays */

		free(pfTimeValues);
		free(pfOutputValues);
		free(pfInTangentAngles);
		free(pfInTangentWeights);
		free(pfOutTangentAngles);
		free(pfOutTangentWeights);
		free(pnKeyFlags);
	}

	/* compress-- just look for almost flat line data */

	int nCurrentSplineIndex = 0;
	pSplineData = pAnimCurve->SplineData;
	while (nCurrentSplineIndex < pAnimCurve->nSplineCount - 1)
	{
		if ((Math::Zero(pSplineData[0].Y() - pSplineData[1].Y(), 0.01f)) &&	// control point
			 (Math::Zero(pSplineData[0].Y() - pSplineData[2].Y(), 0.01f)) &&	// control point
			 (Math::Zero(pSplineData[0].Y() - pSplineData[3].Y(), 0.01f)) &&	// end point
			 (Math::Zero(pSplineData[0].Y() - pSplineData[4].Y(), 0.01f)) &&	// control point
			 (Math::Zero(pSplineData[0].Y() - pSplineData[5].Y(), 0.01f)) &&	// control point
			 (Math::Zero(pSplineData[0].Y() - pSplineData[6].Y(), 0.01f)))		// end point
		{
			/* collapse the two splines into one */

			pSplineData[3] = pSplineData[6];	// copy new second endpoint

			/* recalculate tangents-- linear should be ok. first tangent */

			pSplineData[1].x( pSplineData[0].x( ) + 0.3333f * ( pSplineData[3].x( ) - pSplineData[0].x( ) ) );
			pSplineData[1].y( pSplineData[0].y( ) + 0.3333f * ( pSplineData[3].y( ) - pSplineData[0].y( ) ) );

			/* second tangent */

			pSplineData[2].x( pSplineData[0].x( ) + 0.6667f * ( pSplineData[3].x( ) - pSplineData[0].x( ) ) );
			pSplineData[2].y( pSplineData[0].y( ) + 0.6667f * ( pSplineData[3].y( ) - pSplineData[0].y( ) ) );

			/* copy subsequent splines down */

			if (pAnimCurve->nSplineCount - nCurrentSplineIndex > 2)
				memcpy(&pSplineData[4], pSplineData[7], (pAnimCurve->nSplineCount - nCurrentSplineIndex - 2) * 3 * sizeof(Vector2));

			/* one less spline in data */

			pAnimCurve->nSplineCount--;
		}
		else
		{
			/* to next spline */

			pSplineData += 3;
			nCurrentSplineIndex++;
		}
	}

	/* done */

	return(pAnimCurve);
}

/* convert curve type to cut scene manager value */

int CinemanFilter::ConvertCurveType(int nType)
{
	switch(nType)
	{
	case ParticipantChannel::TYPE_TRANSLATION_X:
		return(CCutSceneInterpreter::CURVE_TYPE_TRANSLATE_X);
	case ParticipantChannel::TYPE_TRANSLATION_Y:
		return(CCutSceneInterpreter::CURVE_TYPE_TRANSLATE_Y);
	case ParticipantChannel::TYPE_TRANSLATION_Z:
		return(CCutSceneInterpreter::CURVE_TYPE_TRANSLATE_Z);
	case ParticipantChannel::TYPE_ROTATION_X:
		return(CCutSceneInterpreter::CURVE_TYPE_ROTATE_X);
	case ParticipantChannel::TYPE_ROTATION_Y:
		return(CCutSceneInterpreter::CURVE_TYPE_ROTATE_Y);
	case ParticipantChannel::TYPE_ROTATION_Z:
		return(CCutSceneInterpreter::CURVE_TYPE_ROTATE_Z);
	case ParticipantChannel::TYPE_SCALE_X:
		return(CCutSceneInterpreter::CURVE_TYPE_SCALE_X);
	case ParticipantChannel::TYPE_SCALE_Y:
		return(CCutSceneInterpreter::CURVE_TYPE_SCALE_Y);
	case ParticipantChannel::TYPE_SCALE_Z:
		return(CCutSceneInterpreter::CURVE_TYPE_SCALE_Z);
	case ParticipantChannel::TYPE_FOCAL_LENGTH:
		return(CCutSceneInterpreter::CURVE_TYPE_FOCAL_LENGTH);
	case ParticipantChannel::TYPE_HORIZONTAL_FILM_APERTURE:
		return(CCutSceneInterpreter::CURVE_TYPE_HORIZONTAL_APERTURE);
	case ParticipantChannel::TYPE_VERTICAL_FILM_APERTURE:
		return(CCutSceneInterpreter::CURVE_TYPE_VERTICAL_APERTURE);
	case ParticipantChannel::TYPE_COLOR_RED:
		return(CCutSceneInterpreter::CURVE_TYPE_COLOR_RED);
	case ParticipantChannel::TYPE_COLOR_GREEN:
		return(CCutSceneInterpreter::CURVE_TYPE_COLOR_GREEN);
	case ParticipantChannel::TYPE_COLOR_BLUE:
		return(CCutSceneInterpreter::CURVE_TYPE_COLOR_BLUE);
	}
	ASSERT(false);
	return(CCutSceneInterpreter::CURVE_TYPE_UNKNOWN);
}

/* output the events necessary to set up static values for a participant-- values that this participant needs (differs
by type). we get the value by evaulating the channel at the passed time-- if the channel has no instanced curves, we
get the channel's static value (converted for output), otherwise we get some evaluation of the curve data */

void CinemanFilter::OutputInitialEvents(const Participant *pParticipant, const float fTime)
{
	/* start events-- do we need static position? */

	bool bWriteData = false;
	switch(pParticipant->GetType())
	{
	case Participant::TYPE_ENVIRONMENTAL_ACTOR:
	case Participant::TYPE_MAIN_CHARACTER:
	case Participant::TYPE_LOAD_SCREEN_ACTOR:
	case Participant::TYPE_PARTICLE_EMITTER:
	case Participant::TYPE_PARTICLE_EFFECT:
	case Participant::TYPE_SOUND:
	case Participant::TYPE_CAMERA:
	case Participant::TYPE_DUMMY_PARENT:
		bWriteData = true;
		break;
	case Participant::TYPE_LIGHT:
		if (pParticipant->GetFlags() & Participant::FLAGS_LOCAL_LIGHT)
			bWriteData = true;
		break;
	}
	if (bWriteData)
	{
		WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fTime);
		WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "position", CCutSceneInterpreter::DATA_STREAM_EVENT_SET_POSITION);
		WriteData(CCutSceneInterpreter::CUT_SCENE_X, "x", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_X, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Y, "y", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_Y, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Z, "z", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_Z, fTime));
	}

	/* rotation? */

	bWriteData = false;
	switch(pParticipant->GetType())
	{
	case Participant::TYPE_CAMERA:
	case Participant::TYPE_MAIN_CHARACTER:
	case Participant::TYPE_LOAD_SCREEN_ACTOR:
	case Participant::TYPE_ENVIRONMENTAL_ACTOR:
	case Participant::TYPE_PARTICLE_EMITTER:
	case Participant::TYPE_DUMMY_PARENT:
		bWriteData = true;
		break;
	case Participant::TYPE_LIGHT:
		if (pParticipant->GetFlags() & Participant::FLAGS_DIRECTIONAL_LIGHT)
			bWriteData = true;
		break;
	}
	if (bWriteData)
	{
		WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fTime);
		WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "rotation", CCutSceneInterpreter::DATA_STREAM_EVENT_SET_ROTATION);
		WriteData(CCutSceneInterpreter::CUT_SCENE_X, "x", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_ROTATION_X, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Y, "y", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_ROTATION_Y, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Z, "z", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_ROTATION_Z, fTime));
	}

	/* scale */

	bWriteData = false;
	switch(pParticipant->GetType())
	{
	case Participant::TYPE_DISTORTION:
	case Participant::TYPE_MOTION_BLUR:
		bWriteData = true;
		break;
	}
	if (bWriteData)
	{
		WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fTime);
		WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "scale", CCutSceneInterpreter::DATA_STREAM_EVENT_SET_SCALE);
		WriteData(CCutSceneInterpreter::CUT_SCENE_X, "x", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_SCALE_X, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Y, "y", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_SCALE_Y, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_Z, "z", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_SCALE_Z, fTime));
	}

	/* camera needs some extra stuff */

	if (pParticipant->GetType() == Participant::TYPE_CAMERA)
	{
		WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fTime);
		WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "camera_settings", CCutSceneInterpreter::DATA_STREAM_EVENT_CAMERA_SETTINGS);
		WriteData(CCutSceneInterpreter::CUT_SCENE_FOCAL_LENGTH, "focal_length", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_FOCAL_LENGTH, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_VERTICAL_APERTURE, "vertical_aperture", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_VERTICAL_FILM_APERTURE, fTime));
		WriteData(CCutSceneInterpreter::CUT_SCENE_HORIZONTAL_APERTURE, "horizontal_aperture", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_HORIZONTAL_FILM_APERTURE, fTime));

		/* since cameras can be merged, there is some small chance that their transform orders will be different, so */
		/* do this here instead of at very top of data stream */

		OutputTransformOrder(pParticipant);
	}
}

/* output cut scene events for a participant */

bool CinemanFilter::OutputEvents(const int nDataStreamIndex)
{
	float fCurrentTime = 0.0f;

	/* keep processing as long as we have an active participant */

	bool bActiveParticipantFound = false;
	Participant *pActiveParticipant = NULL;
	do
	{
		/* there may be multiple participants that make up this data stream. the visibility curves control which */
		/* participant is active-- when the active participant's visibility curve drops below 1.0, control passes */
		/* to another participant */

		Participant *pLastActiveParticipant = pActiveParticipant;
		pActiveParticipant = FindNextActiveParticipant(nDataStreamIndex, fCurrentTime);	// may update fCurrentTime
		if (pActiveParticipant == NULL)
			return(bActiveParticipantFound);	// nothing more to do
		float fInactiveTime = pActiveParticipant->FindVisibleTime(false, fCurrentTime, m_fCutSceneEndTime);	// time he'll become inactive
		bActiveParticipantFound = true;

		/* if we are changing active participant, have to reset some stuff (e.g. field-of-view may change */
		/* between cameras) */

		if (pActiveParticipant != pLastActiveParticipant)
			OutputInitialEvents(pActiveParticipant, fCurrentTime);

		/* some participants show while active and hide afterwards */

#define SHOW_AND_HIDE() ((pActiveParticipant->GetType() == Participant::TYPE_PARTICLE_EMITTER) || (pActiveParticipant->GetType() == Participant::TYPE_ENVIRONMENTAL_ACTOR) || (pActiveParticipant->GetType() == Participant::TYPE_DUMMY_PARENT) || (pActiveParticipant->GetFlags() & Participant::FLAGS_LOCAL_LIGHT))
		if (SHOW_AND_HIDE())
		{
			/* if participant has a parent, parent is responsible for showing and hiding */

			if (pActiveParticipant->GetParent() == -1)
			{
				WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fCurrentTime);
				WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "show", CCutSceneInterpreter::DATA_STREAM_EVENT_SHOW);
			}
		}

		/* keep processing current participant until he becomes inactive, then look for another */

		bool bFound = true;
		while (bFound)
		{
			/* assume we won't find a curve */

			bFound = false;

			/* look through channels for next curve */

			int nChannelIndex;
			int nInstancedCurveIndex;
			int nInstancedActionIndex = -1;
			float fBestStartTime = MAXFLOAT;
			float fBestEndTime = MAXFLOAT;
			int numChannels = pActiveParticipant->GetNumChannels();
			for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
			{
				ParticipantChannel &channel = pActiveParticipant->GetChannel(channelIndex);
				if (channel.IsValidForExport() == false)
					continue;

				/* loop through instanced curves for this channel */

				int numInstancedCurves = channel.GetNumInstancedCurves();
				for (int instancedCurveIndex=0;instancedCurveIndex<numInstancedCurves;instancedCurveIndex++)
				{
					if (channel.GetInstanceProcessed(instancedCurveIndex))
						continue;

					/* curves that start and end before current time get ignored as though they had already been processed-- */
					/* must have been while participant was not active */

					float startTime = channel.GetInstanceStartTime(instancedCurveIndex);
					if (startTime < fCurrentTime)
					{
						float duration = channel.GetSourceDuration(channel.GetInstanceSourceCurveReference(instancedCurveIndex));
						float endTime = startTime + duration;
						if (endTime < fCurrentTime)
						{
							pActiveParticipant->GetChannel(nChannelIndex).SetInstanceProcessed(nInstancedCurveIndex, true);
							continue;
						}
					}

					/* if curve starts after time when participant goes inactive, ignore it (may get to it later, */
					/* but for now it's outside currently-active participant's active time) */

					if (startTime >= fInactiveTime)
						startTime = MAXFLOAT;

					/* better than current curve? */

					if (startTime < fBestStartTime)
					{
						bFound = true;
						fBestStartTime = startTime;
						nChannelIndex = channelIndex;
						nInstancedCurveIndex = instancedCurveIndex;

						/* record end time for comment below */

						float duration = channel.GetSourceDuration(channel.GetInstanceSourceCurveReference(instancedCurveIndex));
						fBestEndTime = fBestStartTime + duration;
					}
				}
			}

			/* look for actions, too */

			int numInstancedActions = pActiveParticipant->GetNumInstancedActions();
			for (int instancedActionIndex=0;instancedActionIndex<numInstancedActions;instancedActionIndex++)
			{
				ParticipantInstancedAction &instancedAction = pActiveParticipant->GetInstancedAction(instancedActionIndex);
				if (instancedAction.GetProcessed())
					continue;

				/* actions that start and end before current time get ignored as though they had already been processed-- */
				/* must have been while participant was not active */

				float startTime = instancedAction.GetStartTime();
				if (startTime < fCurrentTime)
				{
					float duration = pActiveParticipant->GetSourceAction(instancedAction.GetSourceReference()).GetDuration();
					float endTime = startTime + duration;
					if (endTime < fCurrentTime)
					{
						instancedAction.SetProcessed(true);
						continue;
					}
				}

				/* if action starts after time when participant goes inactive, ignore it (may get to it later, */
				/* but for now it's outside currently-active participant's active time) */

				if (startTime >= fInactiveTime)
					startTime = MAXFLOAT;

				/* better than current curve/action? */

				if (startTime < fBestStartTime)
				{
					bFound = true;
					fBestStartTime = startTime;
					nInstancedActionIndex = instancedActionIndex;

					/* record end time for comment below */

					float duration = pActiveParticipant->GetSourceAction(instancedAction.GetSourceReference()).GetDuration();
					fBestEndTime = fBestStartTime + duration;
				}
			}

			/* if we found a curve or action, output event and continue with participant */

			char HelperString[100];
			if (bFound)
			{
				if (nInstancedActionIndex == -1)
				{
					/* curve may have started before current time, but event should be at current time */

					fBestStartTime = Math::Max(fBestStartTime, fCurrentTime);

					/* output event for this curve */

					int nChannelType = pActiveParticipant->GetChannel(nChannelIndex).GetType();
					float fFirstValue = pActiveParticipant->EvaluateChannel(nChannelType, fBestStartTime);
					switch(nChannelType)
					{
					case ParticipantChannel::TYPE_ROTATION_X:
					case ParticipantChannel::TYPE_ROTATION_Y:
					case ParticipantChannel::TYPE_ROTATION_Z:
						fFirstValue = Math::Rad2Deg(fFirstValue);
						break;
					}
					WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fBestStartTime);
					WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "attach_to_curve", CCutSceneInterpreter::DATA_STREAM_EVENT_ATTACH_TO_CURVE);
					sprintf(HelperString, "end time: %.2f, first value: %.2f", fBestEndTime, fFirstValue);
					WriteData(CCutSceneInterpreter::CUT_SCENE_CURVE_TYPE, "curve_type", pActiveParticipant->GetChannel(nChannelIndex).GetTypeName(), ConvertCurveType(pActiveParticipant->GetChannel(nChannelIndex).GetType()), HelperString);

					/* curve name must be unique */

					char CurveName[100];
					sprintf(CurveName, "%s%d_%d_%d", pActiveParticipant->GetName(), pActiveParticipant->GetBuildIndex(), pActiveParticipant->GetChannel(nChannelIndex).GetType(), nInstancedCurveIndex);
					char *pCurveName = CurveName;
					int nLength = strlen(pCurveName);
					if (nLength > ANIM_CURVE_NAME_LENGTH - 1)
						pCurveName += (nLength - (ANIM_CURVE_NAME_LENGTH - 1));
					WriteData(CCutSceneInterpreter::CUT_SCENE_CURVE, "curve", pCurveName);

					/* processed this curve instance */

					pActiveParticipant->GetChannel(nChannelIndex).SetInstanceProcessed(nInstancedCurveIndex, true);
				}
				else
				{
					/* convert name to match engine */

					char KFMName[100];
					int sourceRef = pActiveParticipant->GetInstancedAction(nInstancedActionIndex).GetSourceReference();
					float cycles = pActiveParticipant->GetInstancedAction(nInstancedActionIndex).GetCycle();
					strcpy(KFMName, pActiveParticipant->GetSourceAction(sourceRef).GetName());
					char *pKFMName = strstr(KFMName, "_kfm");
					if (pKFMName)
						*pKFMName = 0;
					UpdateKFMList(KFMName);	// add to list of kfm's in this cut scene

					/* current length limit is 7 characters, and it's the last 7 */

					pKFMName = KFMName;
					int nLength = strlen(pKFMName);
					if (nLength > 7)
						pKFMName += (nLength - 7);

					/* output event for this action */

					sprintf(HelperString, "end: %.2f", fBestEndTime);
					WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fBestStartTime, HelperString);
					WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "animate", CCutSceneInterpreter::DATA_STREAM_EVENT_ANIMATE);
					WriteData(CCutSceneInterpreter::CUT_SCENE_KFM, "kfm", pKFMName);
					sprintf(HelperString, "cycles: %.2f", cycles);
					WriteData(CCutSceneInterpreter::CUT_SCENE_LOOP, "loop", (cycles > 1.0) ? (int) 1 : (int) 0, HelperString);

					/* processed this action */

					pActiveParticipant->GetInstancedAction(nInstancedActionIndex).SetProcessed(true);
				}

				/* advance current time */

				fCurrentTime = fBestStartTime;
			}

			/* if not found, advance past current active participant's end time. we'll look for another */
			/* active participant-- the while (bFound) loop, above, will stop */

			if (bFound == false)
				fCurrentTime = fInactiveTime;
		}

		/* some participants show while active and hide afterwards */

		if (SHOW_AND_HIDE())
		{
			/* if participant has a parent, parent is responsible for showing and hiding */

			if (pActiveParticipant->GetParent() == -1)
			{
				if (fCurrentTime < m_fCutSceneEndTime)	// don't bother if past end of cut scene
				{
					WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fCurrentTime);
					WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "hide", CCutSceneInterpreter::DATA_STREAM_EVENT_HIDE);
				}
			}
		}
	} while ((pActiveParticipant) && (fCurrentTime < m_fCutSceneEndTime));

	/* success/failure */

	return(bActiveParticipantFound);
}

/* this routine is for data streams that don't attach to curves-- each event is discrete */

void CinemanFilter::OutputDiscreteEvents(const int nDataStreamIndex)
{
	/* nobody is locked out yet */

	int i;
	for (i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[i]);
		participant.SetLockout(0.0f);
	}

	/* keep looking for activations, sorted by time. the cut scene events are triggered by a participant becoming visible. */
	/* once visible, the participant is locked out until he becomes invisible again-- the leading edge triggers the event */

	float fCurrentTime = 0.0f;
	while (fCurrentTime < m_fCutSceneEndTime)
	{
		float fNextTime = MAXFLOAT;
		Participant *pParticipant;
		for (i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
		{
			/* next event can either be a participant activating, or the end of a participant's lockout. only */
			/* the activation will trigger a cut scene event */

			Participant &participant = m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[i]);

			/* if locked out, see if the end of the lockout is the next thing to handle */

			if (participant.GetLockout() > fCurrentTime)
			{
				if (participant.GetLockout() < fNextTime)
				{
					fNextTime = participant.GetLockout();
					pParticipant = NULL;	// this does not trigger a cut scene event
				}
			}
			else
			{
				float fVisibleTime = participant.FindVisibleTime(true, fCurrentTime, m_fCutSceneEndTime);
				if (fVisibleTime < fNextTime)
				{
					fNextTime = fVisibleTime;
					pParticipant = &participant;
				}
			}
		}

		/* maybe trigger an event to the cut scene */

		if ((fNextTime < m_fCutSceneEndTime) && (pParticipant))
		{
			/* some special processing for .wav sound event */

			bool bWavFile = false;
			if (pParticipant->GetType() == Participant::TYPE_SOUND)
			{
				if ((strstr(pParticipant->GetName(), ".wav")) || (strstr(pParticipant->GetName(), ".WAV")))
					bWavFile = true;
			}

			/* position needed? */

			bool bOutputPosition = true;
			if (bWavFile)
				bOutputPosition = false;
			if (pParticipant->GetType() == Participant::TYPE_TRIGGER)
				bOutputPosition = false;

			/* set position */

			if (bOutputPosition)
			{
				WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fNextTime);
				WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "position", CCutSceneInterpreter::DATA_STREAM_EVENT_SET_POSITION);
				WriteData(CCutSceneInterpreter::CUT_SCENE_X, "x", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_X, fNextTime));
				WriteData(CCutSceneInterpreter::CUT_SCENE_Y, "y", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_Y, fNextTime));
				WriteData(CCutSceneInterpreter::CUT_SCENE_Z, "z", pParticipant->EvaluateChannel(ParticipantChannel::TYPE_TRANSLATION_Z, fNextTime));
			}

			/* trigger event */

			switch(pParticipant->GetType())
			{
			case Participant::TYPE_PARTICLE_EFFECT:
				WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fNextTime);
				WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "go", CCutSceneInterpreter::DATA_STREAM_EVENT_GO);
				WriteData(CCutSceneInterpreter::CUT_SCENE_PARTICLE_EFFECT, "effect", pParticipant->GetName());
				break;
			case Participant::TYPE_SOUND:
				/* early versions of this filter didn't query the audio file they were choreographing to, so the artist */
				/* had to set it as a separate sound event. don't export .wav file twice */

				if ((bWavFile) && (m_pDataStream[nDataStreamIndex].u32Flags & DATASTREAM_WAV_EVENT_OUTPUT))
					break;
				if (bWavFile)
					m_pDataStream[nDataStreamIndex].u32Flags |= DATASTREAM_WAV_EVENT_OUTPUT;

				/* output event */

				WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fNextTime);
				WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "go", CCutSceneInterpreter::DATA_STREAM_EVENT_GO);
				WriteData(CCutSceneInterpreter::CUT_SCENE_SOUND, "sound", pParticipant->GetName());
				break;
			case Participant::TYPE_TRIGGER:
				WriteData(CCutSceneInterpreter::CUT_SCENE_TIME, "time", fNextTime);
				WriteData(CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE, "event_type", "go", CCutSceneInterpreter::DATA_STREAM_EVENT_GO);
				WriteData(CCutSceneInterpreter::CUT_SCENE_TRIGGER_EVENT, "trigger_event", pParticipant->GetName());
				WriteData(CCutSceneInterpreter::CUT_SCENE_NAME, "name", pParticipant->GetTriggerInstanceName());
				break;
			default:
				ASSERT(false);	// unsupported
				break;
			}

			/* lock out participant until he becomes inactive (vsiibility curve controls it) */

			pParticipant->SetLockout(pParticipant->FindVisibleTime(false, fNextTime, m_fCutSceneEndTime));
		}

		/* update current time and look some more */

		fCurrentTime = fNextTime;
	}
}

/* output transform order to use when combining x/y/z rotations into a transform matrix */

void CinemanFilter::OutputTransformOrder(const Participant *pParticipant)
{
	/* check out the order */

	bool bSupportedOrder = true;
	switch(pParticipant->GetTransformFlags() & TupStaticMatrix::FLAGS_ROTATION_ORDER_MASK)
	{
	default:
		bSupportedOrder = false;

		/*** and fall through ***/
	case TupStaticMatrix::FLAGS_ROTATION_ORDER_XZY:
		WriteData(CCutSceneInterpreter::CUT_SCENE_TRANSFORM_XZY, "transform_xzy", (int) 1);
		break;
	case TupStaticMatrix::FLAGS_ROTATION_ORDER_XYZ:
		WriteData(CCutSceneInterpreter::CUT_SCENE_TRANSFORM_XZY, "transform_xzy", (int) 0);
		break;
	}

	/* some error checking */

	if (bSupportedOrder == false)
	{
		/* order other than x-y-z or x-z-y */

		char Warning[200];
		sprintf(Warning, "Participant %s (%s): unsupported Rotate Order", pParticipant->GetName(), pParticipant->GetTypeName());
		throw(PluginException(ERROR_WARNING,Warning));
	}
}

/* write data out to binary and log files */

void CinemanFilter::WriteData(const int nKey, const char *pKey, const int nValue, const char *pComment /*= NULL*/)
{
	/* binary data */

	ASSERT(m_pBinaryData);
	*(int *) m_pBinaryData = nKey;
	m_pBinaryData += sizeof(int);
	*(int *) m_pBinaryData = nValue;
	m_pBinaryData += sizeof(int);

	/* log file */

	ASSERT(m_pCutLogFile);
	if (pComment)
		fprintf(m_pCutLogFile, "%s=%d # %s\n", pKey, nValue, pComment);
	else
		fprintf(m_pCutLogFile, "%s=%d\n", pKey, nValue);
}
void CinemanFilter::WriteData(const int nKey, const char *pKey, const float fValue, const char *pComment /*= NULL*/)
{
	/* binary data */

	ASSERT(m_pBinaryData);
	*(int *) m_pBinaryData = nKey;
	m_pBinaryData += sizeof(int);
	*(float *) m_pBinaryData = fValue;
	m_pBinaryData += sizeof(float);

	/* log file */

	ASSERT(m_pCutLogFile);
	if (pComment)
		fprintf(m_pCutLogFile, "%s=%f # %s\n", pKey, fValue, pComment);
	else
		fprintf(m_pCutLogFile, "%s=%f\n", pKey, fValue);
}
void CinemanFilter::WriteData(const int nKey, const char *pKey, const char *pValue, const char *pComment /*= NULL*/)
{
	/* binary data */

	ASSERT(m_pBinaryData);
	*(int *) m_pBinaryData = nKey;
	m_pBinaryData += sizeof(int);
	strncpy(m_pBinaryData, pValue, strlen(pValue) + 1);	// include terminating 0
	m_pBinaryData += (strlen(pValue) + 1 + 3) & ~3;			// keep it 4-byte aligned

	/* log file */

	ASSERT(m_pCutLogFile);
	if (pComment)
		fprintf(m_pCutLogFile, "%s=%s # %s\n", pKey, pValue, pComment);
	else
		fprintf(m_pCutLogFile, "%s=%s\n", pKey, pValue);
}

/* this one is special-- one space optimization is that in log file the value is specified as a string, but in binary as
an int-- this is useful for known values, like event types */

void CinemanFilter::WriteData(const int nKey, const char *pKey, const char *pValue, const int nValue, const char *pComment /*= NULL*/)
{
	/* binary data */

	ASSERT(m_pBinaryData);
	*(int *) m_pBinaryData = nKey;
	m_pBinaryData += sizeof(int);
	*(int *) m_pBinaryData = nValue;
	m_pBinaryData += sizeof(int);

	/* log file */

	ASSERT(m_pCutLogFile);
	if (pComment)
		fprintf(m_pCutLogFile, "%s=%s # %s\n", pKey, pValue, pComment);
	else
		fprintf(m_pCutLogFile, "%s=%s\n", pKey, pValue);
}

/* find the next active participant in a posibly-merged data stream */

Participant *CinemanFilter::FindNextActiveParticipant(const int nDataStreamIndex, float &fTime) const
{
	ASSERT(nDataStreamIndex < m_nDataStreamCount);

	/* if only one participant, that's the one. though if he does not become visible, he doesn't qualify */

	if (m_pDataStream[nDataStreamIndex].nCount == 1)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[0]);

		/* look for when participant becomes visible */

		float fVisibleTime = participant.FindVisibleTime(true, fTime, m_fCutSceneEndTime);
		if (fVisibleTime < m_fCutSceneEndTime)
		{
			/* return time at which he becomes visible (usually same as passed time) */

			fTime = fVisibleTime;
			return(&participant);
		}
		return(NULL);	// participant is invisible from passed time through end of cut scene
	}

	/* start at the top, find first participant that is active at passed time. if nobody active, choose */
	/* participant that is active soonest after passed time */

	int i;
	for (i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[i]);
		if (participant.EvaluateChannel(ParticipantChannel::TYPE_VISIBILITY, fTime) == 1.0f)
			return(&participant);	// we have a winner!
	}

	/* no participant is visible at passed time. search again, looking for first participant to become visible */

	Participant *pBestParticipant = NULL;
	float fBestTime = MAXFLOAT;
	for (i = 0; i < m_pDataStream[nDataStreamIndex].nCount; i++)
	{
		Participant &participant = m_pParticipantManager->GetParticipant(m_pDataStream[nDataStreamIndex].nParticipantIndex[i]);
		float fVisibleTime = participant.FindVisibleTime(true, fTime, m_fCutSceneEndTime);
		if (fVisibleTime < fBestTime)
		{
			fBestTime = fVisibleTime;
			pBestParticipant = &participant;
		}
	}

	/* do we have anyone? */

	if (fBestTime < m_fCutSceneEndTime)
	{
		/* return time at which he becomes visible */

		fTime = fBestTime;
		return(pBestParticipant);
	}
	return(NULL);
}

/* update the list of kfm's used in this cut scene */

void CinemanFilter::UpdateKFMList(const char *pNewKFMName)
{
	/* already in there? */

	char *pExistingKFMName = m_pKFMBuffer;
	while ((pExistingKFMName) && (*pExistingKFMName != 0))	// out of strings when we find an empty string
	{
		if (stricmp(pExistingKFMName, pNewKFMName) == 0)
			return;	// it's there, no need to add it

		/* .mdb file currently saves last 7 letters of kfm name. make sure names are unique. it's an error if not, */
		/* since an actor will be hooked up to the wrong kfm */

		int nLength;
		char *pShortNewKFMName = (char *) pNewKFMName;
		nLength = strlen(pShortNewKFMName);
		if (nLength > 7)
			pShortNewKFMName += (nLength - 7);
		char *pShortExistingKFMName = pExistingKFMName;
		nLength = strlen(pShortExistingKFMName);
		if (nLength > 7)
			pShortExistingKFMName += (nLength - 7);
		if (stricmp(pShortExistingKFMName, pShortNewKFMName) == 0)
		{
			char ErrorMessage[200];
			sprintf(ErrorMessage, "Last 7 characters of .kfm names not unique: %s, %s", pExistingKFMName, pNewKFMName);
			throw(PluginException(ERROR_FATAL, ErrorMessage));
		}

		/* to next string in buffer */

		pExistingKFMName += strlen(pExistingKFMName) + 1;
	}

	/* will it fit? */

	int nNewOffset = (int) pExistingKFMName - (int) m_pKFMBuffer;
	if (nNewOffset + (int) strlen(pNewKFMName) + 1 >= m_nKFMBufferSize)	// must be >= since there has to be a 0 following the null terminator
	{
		m_nKFMBufferSize += 1000;
		char *pNewBuffer = (char *) malloc(m_nKFMBufferSize);
		memset(pNewBuffer, 0, m_nKFMBufferSize);
		if (m_pKFMBuffer)
		{
			memcpy(pNewBuffer, m_pKFMBuffer, nNewOffset);
			free(m_pKFMBuffer);	// bye bye old buffer
		}
		m_pKFMBuffer = pNewBuffer;
	}

	/* add it */

	strncpy(m_pKFMBuffer + nNewOffset, pNewKFMName, strlen(pNewKFMName) + 1);	// copy null terminator, too
}
