/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/ComponentCreator.h"
#include "components/Effects.h"
#include "Audio/audiosys.h"
#include "Audio/cstreami.h"
#include "main/PlayerDB.h"
#include "effects/pain_decal.h"

#include "components/say.h"

STANDARD_COMPONENT_CREATOR(Say, Say, COMPONENT_NEUTRAL_PRIORITY);

#define MINTIME_MISC 2.0f
#define MINTIME_AFFLICTION 2.0f
#define MINTIME_DEATH 2.0f
#define MINTIME_SFX 0.1f

bool g_sayStress = false;

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
Say::Say(
CCompActor &owner) :
CActorComponent(owner)
{
	// No voice set yet
	m_voiceSet = NULL;

	// Nothing played yet
	m_lastPlayedTime = 0.0f;
	m_sfxHandle = NULL;
	m_streamHandle = NULL;
	m_priority = SAYPRIO_NONE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Say::~Say()
{
	bool inDeath = (sayState == SAY_IN_DEATH_SPEECH || sayState == SAY_IN_DEATH_NONSPEECH);
	if (!inDeath)
		Shutup();
	else
	{
		// Is there a stream playing?
		CommonStreamInterface *stream = CSIHandle::GetStream(m_streamHandle);
		if (stream)
		{
			// Release it
			if (!stream->IsPlaying())
				CSIHandle::Kill(m_streamHandle);
			else // we are playing and we are inDeath
				CSIHandle::Kill(m_streamHandle, false);
		}

		// Is there an SFX playing
		if (m_sfxHandle)
		{
			if (!Sfx::Active(m_sfxHandle))
				Sfx::Stop(m_sfxHandle);
			else
			{
				// We are dying and something is playing
				Sfx::Detach(m_sfxHandle);
				Sfx::ReleaseHandle(m_sfxHandle);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void Say::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	// Our message handler
	m_speakHandler.Initialize("Speak", MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS, actor.GetMessageOwner(), this);
	m_sayThisHandler.Initialize("SayThis", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
// Set the voice set
/////////////////////////////////////////////////////////////////////////////
bool Say::UseVoiceSet(
const char *voiceSetName)
{
	if (!ValidSingleton(SayVoiceSetLibrary))
		return false;

	// Find the voice set
	m_voiceSet = g_sayLib.FindVoiceSet(voiceSetName);
	if (!m_voiceSet)
		return false;

	Reset();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Reset!
/////////////////////////////////////////////////////////////////////////////
void Say::Reset()
{
	Shutup();

	// setup the stats
	for (int s = 0; s < MAX_SAY_CATEGORIES; s++)
		m_categoryStats[s].Reset();

	soundPending = VOICETYPE_NONE;
	soundDelay = 0.0f;
	sayState = SAY_NORMAL;
	for(int i = 0; i < VOICETYPE_COUNT; i++)
	{
		voicetypePlayedTime[i] = 0.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The perframe update
/////////////////////////////////////////////////////////////////////////////
void Say::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);

	if(sayState == SAY_IN_DEATH_SPEECH && Stumpy::Decapitated(actor))
      PlayVoiceType(VOICETYPE_DEATH_DECAP,0.0f,0.0f);

	// Do we have a stream playing?
	CommonStreamInterface *stream = CSIHandle::GetStream(m_streamHandle);
	if (stream)
	{
		// Update our position
		SoundStream3D *stream3D = stream->Stream3D();
		ASSERT(stream3D);
		Vector3 pos = actor.GetFocusInWorld();
		stream3D->SetPosition(pos);
		Vector3 vel = actor.GetVelocityInWorld();
		stream3D->SetVelocity(vel);
	}

	// Do we have an SFX playing (clean up if done)
	if (m_sfxHandle && !Sfx::Active(m_sfxHandle))
		Sfx::Stop(m_sfxHandle);

	if (g_sayStress && !m_streamHandle && !m_sfxHandle && sayState == SAY_NORMAL)
	{
		int category = g_random.UpToExcludeHigh(m_voiceSet->Categories());
		int phrase = g_random.UpToExcludeHigh(m_voiceSet->Phrases(category));
		Speak(category, phrase);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Say::Speak(
int category,
int phrase,
SayPriority priority)
{
	if (!m_voiceSet)
		return false;

	// Get our category
	const SayVoiceSet::Category *cat = m_voiceSet->GetCategory(category);
	if (!cat)
		return false;

	// Resolve the phrase
	const char *say = cat->GetPhrase(phrase);
	if (say == NULL)
		return false;

	// Use the category priority if they don't override
	if (priority == SAYPRIO_NONE)
		priority = cat->GetPriority();

	// Play it!
	if (!Speak(say, cat->IsStream(), priority))
		return false;

	// Set the last thing we played
	ASSERT(category >= 0 && category < m_voiceSet->Categories() && category < MAX_SAY_CATEGORIES);
	CategoryStats &stats = m_categoryStats[Math::Clamp(category, 0, MAX_SAY_CATEGORIES - 1)];
	stats.SetLastPlayed(phrase);
	stats.SetLastPlayedTime(g_timer.GetEndSec());

	// Send it!
	if (actor.IsNetMaster())
	{
		int data = PackData(category, phrase, priority);
		g_messageDispatcher.SendMessageToActor("Speak", CMessageDispatcher::IntToData(data), actor.GetHandle(), actor.GetHandle());
	}

	// We are out of here!
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Say::SpeakRandom(
const char *category,
SayPriority priority)
{
	if (!m_voiceSet)
		return false;

	return SpeakRandom(m_voiceSet->FindCategory(category), priority);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Say::SpeakRandom(
int category,
SayPriority priority)
{
	if (!m_voiceSet)
		return false;

	// Get our category
	SayVoiceSet::Category *cat = m_voiceSet->GetCategory(category);
	if (!cat || cat->Phrases() == 0)
		return false;

	// Get the stats
	ASSERT(category >= 0 && category < m_voiceSet->Categories() && category < MAX_SAY_CATEGORIES);
	CategoryStats &stats = m_categoryStats[Math::Clamp(category, 0, MAX_SAY_CATEGORIES - 1)];

	// pick the next phrase
   int phrase, phrases = cat->Phrases();
	if (phrases == 1)
		phrase = 0;
	else
	{
		do	{
			phrase = g_random.UpToExcludeHigh(phrases);
		} while (stats.GetLastPlayed() == phrase);
	}

	// Do it!
	return Speak(category, phrase, priority);
}

/////////////////////////////////////////////////////////////////////////////
// Direct!
/////////////////////////////////////////////////////////////////////////////
bool Say::Speak(
const char *name,
bool stream,
SayPriority priority)
{
	ASSERT(name && name[0]);
	if (!name || !name[0])
		return false;

	// Are we preempting
	bool speaking = Speaking();
	if (speaking && priority <= m_priority)
		return false;
	m_priority = priority;

	// Shut up!
	Shutup();

	// Fire it off
	bool ret;
	if (stream)
		ret = StartStream(name);
	else
		ret = StartSfx(name);

	// Save this
	if (ret)
		m_lastPlayedTime = g_timer.GetEndSec();
#ifdef _DEBUG
	else
		TRACE(">>>>>>>>>>>>>>Missing voice file %s\n",name);
#endif

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Stop taunts from playing (returns true if a taunt was playing)
/////////////////////////////////////////////////////////////////////////////
bool Say::Shutup()
{
	bool wasPlaying = false;

	// Is there a stream playing?
	CommonStreamInterface *stream = CSIHandle::GetStream(m_streamHandle);
	if (stream)
	{
		// Release it
		wasPlaying = stream->IsPlaying();
		CSIHandle::Kill(m_streamHandle);
	}

	// Is there an SFX playing
	if (m_sfxHandle)
	{
		wasPlaying = Sfx::Active(m_sfxHandle);
		Sfx::Stop(m_sfxHandle);
	}

	// Was there something playing
	return wasPlaying;
}

/////////////////////////////////////////////////////////////////////////////
// Are we speaking?
/////////////////////////////////////////////////////////////////////////////
bool Say::Speaking()
{
	// Do we have an active stream?
	CommonStreamInterface *stream = CSIHandle::GetStream(m_streamHandle);
	if (stream && stream->IsPlaying())
		return true;

	// What about an active SFX?
	if (m_sfxHandle)
	{
		if (Sfx::Active(m_sfxHandle))
			return true;
		Sfx::Stop(m_sfxHandle);
	}

	// We are not saying anythign
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Say::GetLastPlayedTime(
const char *category) const
{
	if (!m_voiceSet)
		return 0.0f;
	else
		return GetLastPlayedTime(m_voiceSet->FindCategory(category));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Say::GetLastPlayedTime(int category) const
{
	// Get our category
	SayVoiceSet::Category *cat = m_voiceSet->GetCategory(category);
	if (!cat)
		return 0.0f;

	// Get the stats
	return m_categoryStats[Math::Clamp(category, 0, MAX_SAY_CATEGORIES - 1)].GetLastPlayedTime();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Say::StartSfx(const char *sfx)
{
	ASSERT(!m_sfxHandle);
	ASSERT(m_voiceSet);

	// Play it!
	if (m_voiceSet)
		return Sfx::Play(sfx, actor, Sfx::ACTORPLAYPOS_DEFAULT, m_voiceSet->Volume(), m_voiceSet->Pitch(), &m_sfxHandle);
	else
		return Sfx::Play(sfx, actor, Sfx::ACTORPLAYPOS_DEFAULT, 1.0f, 0.0f, &m_sfxHandle);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Say::StartStream(const char *stream)
{
	// Uh, duh
	if (!stream || stream[0] == '\0')
		return false;

	// Is there a taunt playing?
	ASSERT(!m_streamHandle);

	// Start the new one
	CommonStreamInterface *csi = g_audio.CreateStream3D(stream, false, true);
	if (!csi)
		return false;

	// We made it!
	SoundStream3D *stream3D = csi->Stream3D();
	ASSERT(stream3D);
	Vector3 pos = actor.GetFocusInWorld();
	stream3D->SetPosition(pos);
	Vector3 vel = actor.GetVelocityInWorld();
	stream3D->SetVelocity(vel);
	if (m_voiceSet)
	{
		stream3D->SetVolume(m_voiceSet->Volume());
		stream3D->SetPitch(m_voiceSet->Pitch());
	}
	stream3D->Play();

	// Get a handle!
	m_streamHandle = csi->CreateHandle();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int Say::PackData(int category, int phrase, SayPriority priority)
{
	int data = phrase;
	data |= (category & 0xff) << 16;
	data |= ((int)priority & 0xff) << 24;

	return data;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Say::UnpackData(int data, int &category, int &phrase, SayPriority &priority)
{
	phrase = data & 0xffff;
	category = (data >> 16) & 0xff;
	priority = (SayPriority)((data >> 24) & 0xff);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Say::Speak::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	int category, phrase;
	SayPriority priority;
	UnpackData(CMessageDispatcher::DataToInt(i_data), category, phrase, priority);
	d_ref->Speak(category, phrase, priority);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Say::SayThis::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	ASSERT(i_data);
	if (i_data)
		d_ref->SpeakRandom((const char *)i_data);
}

//============================================================================
void Say::BeginFrame()
{
	if(m_voiceSet == NULL)
		SetVoiceSet(0);

	if(soundPending == VOICETYPE_NONE)
		return;

	Math::MoveToZero(&soundDelay,g_timer.GetFrameSec());
	if(soundDelay > 0.0f)
		return;
	SayIt(soundPending);
	soundPending = VOICETYPE_NONE;
}
//============================================================================
void Say::SetVoiceSet(int i_voiceSet)
{
	m_voiceSetEnum = i_voiceSet;

	char* vs = actor.Parm().GetString("voiceset");
	if (!vs)
	{
		char* prebuilt = actor.Parm().GetString("Geometry");
		if (prebuilt)
		{
			int prebuiltIndex = PlayerDB::CharacterIndexFromName(prebuilt);
			if (prebuiltIndex >= 0)
			{
				char* init = PlayerDB::CharacterInitString(prebuiltIndex);
				char* vskey = strstr(init, "voiceset=");
				if (vskey)
					vs = vskey + 9;
			}
		}
	}

	// copy voiceset name, stop at spaces or ";".
	char* p = m_voiceSetName;
	while (vs && *vs && !isspace(*vs) && *vs != ';' && p < &m_voiceSetName[sizeof(m_voiceSetName) - 1])
		*p++ = *vs++;
	*p = 0;

	if (m_voiceSetName[0])
	{
		UseVoiceSet(m_voiceSetName);
		return;
	}
}
//============================================================================
void Say::PlayVoiceType(int i_voiceType, float minDelay, float maxDelay, bool trumpPending)
{
	if(!trumpPending && soundPending != VOICETYPE_NONE)
		return;

	if(minDelay == 0.0f && maxDelay == 0.0f)
	{
		SayIt(i_voiceType);
		return;
	}

	soundPending = i_voiceType;
	soundDelay = g_random.InRange(minDelay,maxDelay);
}
//============================================================================
bool Say::SayIt(int i_voiceType)
{
#define SOUND_RANGE_SQRD 1000.0f

	bool success = false;

	if (g_players.GetPlayer(0) == NULL)
		return success;

	//range
	Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - actor.GetBodyInWorld();
	if(toAvatar.LengthSquared() > SOUND_RANGE_SQRD)
		return success;

	if(!CheckMinSeperation(i_voiceType))
		return success;

	switch(i_voiceType)
	{
	case VOICETYPE_FIRST_SEE_PLAYER:
		success = SpeakRandom("fsp");
		break;
	case VOICETYPE_CONTACT_LOST:
		success = SpeakRandom("cl");
		break;
	case VOICETYPE_PRECONTACT_IDLE:
		success = SpeakRandom("pi",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_BATTLE_TALK:
		success = SpeakRandom("bt");
		break;
	case VOICETYPE_KILLED_PLAYER:
		success = SpeakRandom("kp");
		break;
	case VOICETYPE_DEAFENED:
		success = SpeakRandom("df");
		break;
	case VOICETYPE_BLINDED:
		success = SpeakRandom("bld");
		break;
	case VOICETYPE_PLEED:
		success = SpeakRandom("pld");
		break;
	case VOICETYPE_ON_FIRE:
		success = SpeakRandom("fire",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_HOSTAGE_TAKEN:
		success = SpeakRandom("ht");
		break;
	case VOICETYPE_HOSTAGE_SAVED:
		success = SpeakRandom("hs");
		break;
	case VOICETYPE_HOSTAGE_RELEASED:
		success = SpeakRandom("hr");
		break;
	case VOICETYPE_HOSTAGE_KILLED:
		success = SpeakRandom("hk");
		break;
	case VOICETYPE_SHOT:
		success = SpeakRandom("sht");
		break;
	case VOICETYPE_APPREHENSION:
		success = SpeakRandom("ap");
		break;
	case VOICETYPE_HURT:
		if(&actor != g_players.GetPlayer(0))
			success = SpeakRandom("hu");
		break;
	case VOICETYPE_DEATH:
		Shutup();
		sayState = SAY_IN_DEATH_SPEECH;
		success = SpeakRandom("dg",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_DEATH_SILILOQUY:
		Shutup();
		sayState = SAY_IN_DEATH_SPEECH;
		success = SpeakRandom("de",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_DEATH_DECAP:
		Shutup();
		sayState = SAY_IN_DEATH_NONSPEECH;
		success = SpeakRandom("decap",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_DEATH_MELEE:
		Shutup();
		sayState = SAY_IN_DEATH_SPEECH;
		success = SpeakRandom("md",SAYPRIO_VERYHIGH);
		break;
	case VOICETYPE_SFX_EFFORT:
		success = SpeakRandom("ef");
		break;
	case VOICETYPE_SFX_HURT:
		success = SpeakRandom("re");
		break;
	case VOICETYPE_SFX_MELEE_HURT:
		success = SpeakRandom("mr");
		break;
	case VOICETYPE_BUDDY_DIED:
		success = SpeakRandom("bd");
		break;
	case VOICETYPE_TAUNT:
		success = SpeakRandom("tnt");
		if(success)
			g_messageDispatcher.SendMessageToAll("Taunted", NULL, actor.GetHandle());
		break;
	}

	return success;
}

bool Say::CheckMinSeperation(int i_voiceType)
{
	bool success = true;

	switch(i_voiceType)
	{
	case VOICETYPE_FIRST_SEE_PLAYER:
	case VOICETYPE_CONTACT_LOST:
	case VOICETYPE_PRECONTACT_IDLE:
	case VOICETYPE_BATTLE_TALK:
	case VOICETYPE_KILLED_PLAYER:
	case VOICETYPE_PLEED:
	case VOICETYPE_HOSTAGE_TAKEN:
	case VOICETYPE_HOSTAGE_SAVED:
	case VOICETYPE_HOSTAGE_RELEASED:
	case VOICETYPE_HOSTAGE_KILLED:
	case VOICETYPE_APPREHENSION:
	case VOICETYPE_BUDDY_DIED:
	case VOICETYPE_TAUNT:
		if(g_timer.GetEndSec() - voicetypePlayedTime[i_voiceType] < MINTIME_MISC)
			success = false;
		break;
	case VOICETYPE_DEAFENED:
	case VOICETYPE_BLINDED:
	case VOICETYPE_ON_FIRE:
	case VOICETYPE_SHOT:
	case VOICETYPE_HURT:
		if(g_timer.GetEndSec() - voicetypePlayedTime[i_voiceType] < MINTIME_AFFLICTION)
		{
			//these might be getting spammed, so space it from last failure
			voicetypePlayedTime[i_voiceType] = g_timer.GetEndSec();
			success = false;
		}
		break;
	case VOICETYPE_DEATH:
	case VOICETYPE_DEATH_SILILOQUY:
	case VOICETYPE_DEATH_MELEE:
	case VOICETYPE_DEATH_DECAP:
		if(g_timer.GetEndSec() - voicetypePlayedTime[i_voiceType] < MINTIME_DEATH)
			success = false;
		break;
	case VOICETYPE_SFX_EFFORT:
	case VOICETYPE_SFX_HURT:
	case VOICETYPE_SFX_MELEE_HURT:
		if(g_timer.GetEndSec() - voicetypePlayedTime[i_voiceType] < MINTIME_SFX)
			success = false;
		break;
	}

	if(success)
		voicetypePlayedTime[i_voiceType] = g_timer.GetEndSec();

	return success;
}

