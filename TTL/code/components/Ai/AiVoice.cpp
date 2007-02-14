#include "TTLPCH.h"
#include "components/Ai/AiVoice.h"
#include "Components/ComponentCreator.h"
#include "main/PlayerDB.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiVoice, AiVoice, COMPONENT_NEUTRAL_PRIORITY);

#define SOUND_RANGE_SQRD 1000.0f

//============================================================================
AiVoice::AiVoice( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	voice = NULL;
	//SetVoiceSet(0);
}
//============================================================================
AiVoice::~AiVoice()
{
}
//============================================================================
void AiVoice::Initialize( void )
{
	//actor.AttachComponent("Say");
	//voice = (Say*)( actor.GetComponentByName( "Say" ) );
	ASSERT(voice);
	Reset();
}
//============================================================================
void AiVoice::Reset( void )
{
	soundPending = VOICETYPE_NONE;
	soundDelay = 0.0f;
	SetVoiceSet(0);
}
//============================================================================
void AiVoice::BeginFrame()
{
	if(soundPending == VOICETYPE_NONE)
		return;

	Math::MoveToZero(&soundDelay,g_timer.GetFrameSec());
	if(soundDelay > 0.0f)
		return;
	SayIt(soundPending);
	soundPending = VOICETYPE_NONE;
}
//============================================================================
void AiVoice::SetVoiceSet(int i_voiceSet)
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
		voice->UseVoiceSet(m_voiceSetName);
		return;
	}
}
//============================================================================
void AiVoice::PlayVoiceType(int i_voiceType, float minDelay, float maxDelay, bool trumpPending)
{
	if(!trumpPending && soundPending != VOICETYPE_NONE)
		return;
	soundPending = i_voiceType;
	soundDelay = g_random.InRange(minDelay,maxDelay);
}
//============================================================================
void AiVoice::SayIt(int i_voiceType)
{
	if (g_players.GetPlayer(0) == NULL)
		return;

	//range
	Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - actor.GetBodyInWorld();
	if(toAvatar.LengthSquared() > SOUND_RANGE_SQRD)
		return;

	float volume  = 1.0f;

	switch(i_voiceType)
	{
	case VOICETYPE_FIRST_SEE_PLAYER:
		voice->SpeakRandom("fsp");
		break;
	case VOICETYPE_CONTACT_LOST:
		voice->SpeakRandom("cl");
		break;
	case VOICETYPE_PRECONTACT_IDLE:
		voice->SpeakRandom("pi");
		break;
	case VOICETYPE_BATTLE_TALK:
		voice->SpeakRandom("bt");
		break;
	case VOICETYPE_KILLED_PLAYER:
		voice->SpeakRandom("kp");
		break;
	case VOICETYPE_DEAFENED:
		voice->SpeakRandom("df");
		break;
	case VOICETYPE_BLINDED:
		voice->SpeakRandom("bld");
		break;
	case VOICETYPE_PLEED:
		voice->SpeakRandom("pld");
		break;
	case VOICETYPE_ON_FIRE:
		voice->SpeakRandom("fire");
		break;
	case VOICETYPE_HOSTAGE_TAKEN:
		voice->SpeakRandom("ht");
		break;
	case VOICETYPE_HOSTAGE_SAVED:
		voice->SpeakRandom("hs");
		break;
	case VOICETYPE_HOSTAGE_RELEASED:
		voice->SpeakRandom("hr");
		break;
	case VOICETYPE_HOSTAGE_KILLED:
		voice->SpeakRandom("hk");
		break;
	case VOICETYPE_SHOT:
		voice->SpeakRandom("sht");
		break;
	case VOICETYPE_APPREHENSION:
		voice->SpeakRandom("ap");
		break;
	case VOICETYPE_DEATH:
		voice->SpeakRandom("de");
		break;
	case VOICETYPE_HURT:
		voice->SpeakRandom("hu");
		break;
	case VOICETYPE_SFX_DEATH:
		voice->SpeakRandom("dg");
		break;
	case VOICETYPE_SFX_EFFORT:
		voice->SpeakRandom("ef");
		break;
	case VOICETYPE_SFX_HURT:
		voice->SpeakRandom("re");
		break;
	case VOICETYPE_SFX_MELEE_DEATH:
		voice->SpeakRandom("md");
		break;
	case VOICETYPE_SFX_MELEE_HURT:
		voice->SpeakRandom("mr");
		break;
	}
}
//============================================================================
//============================================================================
//============================================================================
//============================================================================
