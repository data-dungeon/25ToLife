/////////////////////////////////////////////////////////////////////////////
// Simple shadow component
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Components/ComponentCreator.h"
#include "components/footfall.h"
#include "Components/GroundHeight.h"

STANDARD_COMPONENT_CREATOR(Footfall, Footfall, COMPONENT_NEUTRAL_PRIORITY);

Footfall::StepLookup	Footfall::s_stepLookup[] =
{
	{ "concrete", 		0,	"hard" },
	{ "ndconcrete",	0,	"hard" },
	{ "pavement", 		0,	"hard" },
	{ "ndpavement",	0,	"hard" },
	{ "carpet", 		0,	"carpet" },
	{ "ndcarpet", 		0,	"carpet" },
	{ "grass", 			0,	"grass" },
	{ "dirt", 			0,	"dirt" },
	{ "sand", 			0,	"dirt" },
	{ "wood", 			0,	"wood" },
	{ "heavywood",		0,	"wood" },
	{ "ndwood",			0,	"wood" },
	{ "ndheavywood",	0,	"wood" },
	{ "tile", 			0,	"hard" },
	{ "ndtile", 	  	0,	"hard" },
	{ "glass", 			0,	"hard" },
	{ "opaqueglass",	0,	"hard" },
	{ "car", 			0,	"metal" },
	{ "metal", 			0,	"metal" },
	{ "ndmetal", 	  	0,	"metal" },
	{ "thinmetal", 	0,	"metal" },
	{ "ndthinmetal", 	0,	"metal" },
	{ "shallowwater", 0,	"water" },
	{ "deepwater", 	0,	"water" },
	{ "default",		0, "default" },
	{ "fence", 			0,	"metal" },
	{ "chainlink", 	0,	"metal" },
	{ "vegitation", 	0,	"grass" },
	{ "sheetrock", 	0,	"hard" },
	{ "ndsheetrock", 	0,	"hard" },
	{ "cardboard", 	0,	"carpet" },
	{ "ndcardboard", 	0,	"carpet" },
	{ "softanddusty",	0,	"carpet" },
	{ "cloth",			0,	"carpet" },
	{ "forcefield",	0,	NULL },
	{ "nothing", 		0,	NULL }
};
int Footfall::s_stepLookups = sizeof(Footfall::s_stepLookup) / sizeof(Footfall::StepLookup);
int Footfall::s_references = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<FootfallParams>::RegisterVariables()
{
   RegisterVariable(d_data.speedDivisor, "speedDivisor", 3.5f, 1.0f, 20.0f);
   RegisterVariable(d_data.minVolume, "minVolume", 0.25f, 0.0f, 1.0f);

   RegisterVariable(d_data.bloodyPrintWidth, "bloodyPrintWidth", 0.1f, 0.0f, 1.0f);
   RegisterVariable(d_data.bloodyPrintLength, "bloodyPrintLength", 0.14f, 0.0f, 1.0f);
   RegisterVariable(d_data.bloodyAngleJitter, "bloodyAngleJitter", 8.0f, 0.0f, 45.0f);
   RegisterVariable(d_data.bloodyStepFade, "bloodyStepFade", 8, 1, 16);
   RegisterVariable(d_data.bloodyStepMinAlpha, "bloodyStepMinAlpha", 0.25f, 0.0f, 1.0f);
   RegisterVariable(d_data.bloodyStanceWidth, "bloodyStanceWidth", 0.08f, 0.0f, 1.0f);
}

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
Footfall::Footfall(
CCompActor &owner) :
CActorComponent(owner)
{
	m_params = NULL;
	m_bloodyFootstep = NULL;
	m_bloodyFootsteps = 0;
	m_footFall = 0;

	AquireReference();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Footfall::~Footfall()
{
	if (m_bloodyFootstep)
		g_footsteps.DestroyEmitter(m_bloodyFootstep);
	Sfx::Stop(m_footFall);
	ReleaseReference();
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void Footfall::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	// Holy magic
	m_params = CParam<FootfallParams>::GetParam("footfall", "var");
	ASSERT_PTR(m_params);

	// Make our blood step
	m_bloodStep.Initialize("BloodStep",
				  					MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_HANDLE_IF_MASTER,
				  					actor.GetMessageOwner(), this);

	m_checkpointResetHandler.Initialize("KillAllAtCheckpoint",
														MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
														actor.GetMessageOwner(), this);

	m_landedHandler.Initialize("Land", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	m_bloodyFootstep = NULL;
	m_bloodyFootsteps = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Reset
/////////////////////////////////////////////////////////////////////////////
void Footfall::Reset()
{
	// Clean this up
	m_bloodyFootsteps = 0;
	Sfx::Stop(m_footFall);
}

#ifdef DEBUGLAND
static bool hitLand = false;
static Vector3 startLand, endLand, hitLandAt;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Footfall::EndUpdate(float i_deltaSec)
{
#ifdef DEBUGLAND
	DrawUtility::Line(startLand, endLand, DrawUtility::YELLOW);
	if (hitLand)
		DrawUtility::Point(hitLandAt, DrawUtility::YELLOW);
#endif

	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);
}

/////////////////////////////////////////////////////////////////////////////
// Notify of a footfall
/////////////////////////////////////////////////////////////////////////////
void Footfall::Step(
Type type,
Foot foot)
{
	// Find the sound
	const char *soundPrefix = StepSoundPrefix(FindMaterial(type));
	if (!soundPrefix)
		return;

	// Build the sound name
	char sound[48];
	strcpy(sound, soundPrefix);
#ifndef HAVESCUFFS
 	if (type == FOOTFALL_WALK)
		strcat(sound, "_walk");
	else
		strcat(sound, "_run");
#else
 	if (type == FOOTFALL_RUN)
		strcat(sound, "_run");
	else if (type == FOOTFALL_WALK)
		strcat(sound, "_walk");
	else //if (type == FOOTFALL_SIDESTEP)
		strcat(sound, "_scuff");
#endif

	// What is the volume?
	float speed = actor.GetVelocityInWorld().Length();
	float volume = Math::Clamp(speed / m_params->speedDivisor, m_params->minVolume, 1.0f);

	// Play the sound
 	Sfx::Stop(m_footFall);
 	Sfx::Play(sound, actor, Sfx::ACTORPLAYPOS_BODY, volume, 0.0f, &m_footFall);
#if 0//defined(FOOD) && defined(_DEBUG)
	TRACE("Footfall volume %d\n", (int)(volume * 100.0f));
#endif

	// A test for the gross-out
	if (m_bloodyFootsteps > 0)
	{
		// First create it
		if (!m_bloodyFootstep)
		{
			m_bloodyFootstep = g_footsteps.CreateEmitter(0);
			if (m_bloodyFootstep)
			{
				m_bloodyFootstep->SetScaling(m_params->bloodyPrintWidth, m_params->bloodyPrintLength);
				m_bloodyFootstep->SetAngleJitter(m_params->bloodyAngleJitter);
			}
		}

		if (m_bloodyFootstep)
		{
			// What alpha should we use?
			const int stepFade = m_params->bloodyStepFade;
			float alpha = (m_bloodyFootsteps > stepFade) ? 1.0f : ((float)m_bloodyFootsteps / (float)stepFade);
			m_bloodyFootstep->SetBaseAlpha(Math::Clamp(alpha, m_params->bloodyStepMinAlpha, 1.0f));

			// Setup the offset
			Vector3 offset;
			const float stance = m_params->bloodyStanceWidth;
			if (foot == FOOTFALL_RIGHT)
				offset.Set(-stance, 0.0f, 0.0f);
			else
				offset.Set(stance, 0.0f, 0.0f);
			m_bloodyFootstep->Place(actor.Instance()->Matrix(), offset, foot == FOOTFALL_LEFT);
			--m_bloodyFootsteps;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Footfall::Landed(
const char *type)
{
	u8 useMaterial = 0;
	bool foundGround = false;
	if (strcmp(type, "vault") == 0)
	{
		BoneID boneID = INVALID_BONEID;
		int ret = boneGetBoneID(actor.Instance()->Body(), &boneID, (char *)"hips");
		ASSERT(ret >= 0);

		if (boneID != INVALID_BONEID)
		{
			Vector3 start;
			boneGetStartPosition(actor.Instance()->Body(), boneID, &start);
			start = start * actor.Instance()->Matrix();
			float range = 2.0f;

			// What are we landing on...
			CLineEntity line;
			const Vector3 down(0.0f, -1.0f, 0.0f);
			line.Line().SetOriginBasis(start, down);
			line.Line().SetMaxBoundary(range);
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
			line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			line.SetIgnoreEntity(&actor);

			// Now cast it
			CLineQuery test;
			foundGround = test.Test(line);

#ifdef DEBUGLAND
			hitLand = foundGround;
			startLand = start;
			endLand = startLand + down * range;
			hitLandAt = test.intersection;
#endif

			// Find the sound
			if (foundGround)
				useMaterial = test.material;
		}
	}

	// If the material under the feet has not been found, find it under the actor
	if (!foundGround)
		useMaterial = FindMaterial(FOOTFALL_LANDING);

	// This is the simple, fast, and cheap way but can be wrong
	const char *soundPrefix = StepSoundPrefix(useMaterial);
	if (soundPrefix)
	{
		// Build the sound name
		char sound[48];
		strcpy(sound, soundPrefix);
		strcat(sound, "_land");

		// Play the sound
		Sfx::Stop(m_footFall);
		Sfx::Play(sound, actor, Sfx::ACTORPLAYPOS_BODY, 1.0f, 0.0f, &m_footFall);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get at ground material
/////////////////////////////////////////////////////////////////////////////
u8 Footfall::FindMaterial(Type type)
{
	// Add a distance check here!
	const Vector3 myPos = actor.GetBodyInWorld();
	SoundListener *listener = g_audio.BestListener(myPos);
	if (!listener)
		return INVALID_MATERIALINDEX;

	// Are we close enough to even hear it?
	float dSquared = listener->DistanceSquared(myPos);
	switch (type)
	{
		case FOOTFALL_WALK:
#ifndef HAVESCUFFS
		case FOOTFALL_SIDESTEP:
#endif
			if (dSquared > 15.0f * 15.0f)
				return INVALID_MATERIALINDEX;
			break;

		case FOOTFALL_LANDING:
			if (dSquared > 25.0f * 25.0f)
				return INVALID_MATERIALINDEX;
			break;

		default: //FOOTFALL_RUN,
			if (dSquared > 20.0f * 20.0f)
				return INVALID_MATERIALINDEX;
			break;
	}

	// Do it!
	CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
	if (groundHeight != NULL)
	{
		CGroundHeightIF::SCheckResults* results;
		groundHeight->Get(0.5, -0.5f, results);
		return results->d_material;
	}

	return INVALID_MATERIALINDEX;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Footfall::BloodStep::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	int steps = CMessageDispatcher::DataToInt(i_data);
	d_ref->m_bloodyFootsteps = Math::Max(d_ref->m_bloodyFootsteps, steps);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Footfall::CheckpointReset::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	d_ref->Reset();
	if (d_ref->m_bloodyFootstep)
		d_ref->m_bloodyFootstep->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Footfall::Landed::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	d_ref->Landed((const char *)i_data);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Footfall::SetupStepLookup()
{
	for (int s = 0; s < s_stepLookups; s++)
		s_stepLookup[s].material = g_surfaceType.GetIndex(s_stepLookup[s].materialName);

#ifdef FOOTFALL_SORT
	// sort it
	qsort(s_stepLookup, s_stepLookups, sizeof(StepLookup), CompareLookup);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char *Footfall::StepSoundPrefix(
u8 material)
{
	if (material == INVALID_MATERIALINDEX)
		return NULL;

#ifdef FOOTFALL_SORT
	StepLookup lookFor;
	lookFor.material = material;
	StepLookup *found = (StepLookup *)bsearch(&lookFor, s_stepLookup, s_stepLookups, sizeof(StepLookup), CompareLookup);
	ASSERTS(found, "Walking on unknown material");

	if (!found)
		return NULL;

#if 0//defined(FOOD) && defined(_DEBUG)
	TRACE("Walking on %s\n", found->materialName);
#endif

	return found->soundPrefix;
#else
	for (int s = 0; s < s_stepLookups; s++)
	{
		if (s_stepLookup[s].material == material)
		{
#if 0//defined(FOOD) && defined(_DEBUG)
			TRACE("Walking on %s\n", s_stepLookup[s].materialName);
#endif
			return s_stepLookup[s].soundPrefix;
		}
	}
	// Should we default to something?
	ASSERTS(false, "Walking on unknown material");
	return NULL;
#endif
}

#ifdef FOOTFALL_SORT
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int __cdecl Footfall::CompareLookup(const void *a, const void *b)
{
	StepLookup *surface1 = (StepLookup *)a;
	StepLookup *surface2 = (StepLookup *)b;

	return (int)surface1->material - (int)surface2->material;
}
#endif

