#ifndef AI_VOICE_H
#define AI_VOICE_H

#include "Components/Component.h"
#include "Components/Say.h"

class AiVoice : public CActorComponent
{
public:
	AiVoice( CCompActor &i_actor );
	~AiVoice();
	const char *ComponentName( void ) { return "AiVoice"; }
	void Initialize(void);
	void Reset(void);
	virtual void BeginFrame();
	void SetVoiceSet(int i_voiceSet);
	int GetVoiceSet(){return m_voiceSetEnum;}
	void PlayVoiceType(int i_voiceType, float minDelay = 0.0f, float maxDelay = 0.0f, bool trumpPending = true);
	void ClearPending(){soundPending = VOICETYPE_NONE;}

	enum VOICE_TYPES
	{
		VOICETYPE_NONE,
		VOICETYPE_FIRST_SEE_PLAYER,
		VOICETYPE_CONTACT_LOST,
		VOICETYPE_PRECONTACT_IDLE,
		VOICETYPE_BATTLE_TALK,
		VOICETYPE_KILLED_PLAYER,
		VOICETYPE_DEAFENED,
		VOICETYPE_BLINDED,
		VOICETYPE_PLEED,
		VOICETYPE_ON_FIRE,
		VOICETYPE_HOSTAGE_TAKEN,
		VOICETYPE_HOSTAGE_SAVED,
		VOICETYPE_HOSTAGE_RELEASED,
		VOICETYPE_HOSTAGE_KILLED,
		VOICETYPE_SHOT,
		VOICETYPE_APPREHENSION,
		VOICETYPE_DEATH,
		VOICETYPE_HURT,
		VOICETYPE_SFX_DEATH,
		VOICETYPE_SFX_EFFORT,
		VOICETYPE_SFX_HURT,
		VOICETYPE_SFX_MELEE_DEATH,
		VOICETYPE_SFX_MELEE_HURT,
		VOICETYPE_COUNT
	};

private:
	void SayIt(int i_voiceType);
	int m_voiceSetEnum;
	char m_voiceSetName[20];
	Say* voice;
	int soundPending;
	float soundDelay;
};

#endif // AI_VOICE_H



