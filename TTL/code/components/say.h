/////////////////////////////////////////////////////////////////////////////
// Say class (makes sure the actor just says one thing at a time)
/////////////////////////////////////////////////////////////////////////////
#ifndef SAY_H
#define SAY_H

#include "Components/Component.h"
#include "components/saylib.h"

// Forwards
class CommonStreamInterface;
class CSIHandle;

#define MAX_SAY_CATEGORIES	32

class Say : public CActorComponent
{
public:
	// This component controls what the actor says
	Say(CCompActor &actor);
	~Say();

	// Our interface support
	const char *ComponentName()
		{ return "Say"; }

	// The initialize
	void Initialize();

	// Reset!
	void Reset();

	// Set the voice set
	bool UseVoiceSet(const char *voiceSetName);

	// The perframe update (will stop saying stuff if dead)
	void EndUpdate(float i_deltaSec);

	// Play a phrase
	bool Speak(int category, int phrase, SayPriority priority = SAYPRIO_NONE);

	// Select randomly from a category
	bool SpeakRandom(const char *category, SayPriority priority = SAYPRIO_NONE);
	bool SpeakRandom(int category, SayPriority priority = SAYPRIO_NONE);

	// Direct! (doesn't work over network...yet)
	bool Speak(const char *name, bool stream, SayPriority priority = SAYPRIO_NONE);

	// Stop any playing phrases
	bool Shutup();

	// Am I saying something?
	bool Speaking();

	// What time was the last thing said?
	float GetLastPlayedTime() const
		{ return m_lastPlayedTime; }
	float GetLastPlayedTime(const char *category) const;
	float GetLastPlayedTime(int category) const;

	//previously in AiVoice
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
		VOICETYPE_HURT,
		VOICETYPE_DEATH,
		VOICETYPE_DEATH_SILILOQUY,
		VOICETYPE_DEATH_MELEE,
		VOICETYPE_DEATH_DECAP,
		VOICETYPE_SFX_EFFORT,
		VOICETYPE_SFX_HURT,
		VOICETYPE_SFX_MELEE_HURT,
		VOICETYPE_BUDDY_DIED,
		VOICETYPE_TAUNT,
		VOICETYPE_COUNT
	};

	enum SAY_STATES
	{
		SAY_NORMAL,
		SAY_IN_DEATH_SPEECH,
		SAY_IN_DEATH_NONSPEECH
	};

private:
	// An enforcer to make sure something isn't spamming a voicetype to be played
	bool CheckMinSeperation(int i_voiceType);

	// General settings
	const SayVoiceSet 			*m_voiceSet;

	//previously in AiVoice
	bool SayIt(int i_voiceType);
	int m_voiceSetEnum;
	char m_voiceSetName[20];
	int soundPending;
	float soundDelay;
	int sayState;
	float voicetypePlayedTime[VOICETYPE_COUNT];

	// Category info
	class CategoryStats
	{
		public:
			CategoryStats() { Reset(); }
			~CategoryStats() {}

			void Reset()
				{ m_lastPlayedTime = 0.0f; m_lastPlayed = -1; }

			void SetLastPlayedTime(float now)
				{ m_lastPlayedTime = now; }
			float GetLastPlayedTime() const
				{ return m_lastPlayedTime; }

			void SetLastPlayed(int phrase)
				{ m_lastPlayed = phrase; }
			int GetLastPlayed() const
				{ return m_lastPlayed; }

		private:
			float						m_lastPlayedTime;
			int						m_lastPlayed;
	};
	CategoryStats					m_categoryStats[MAX_SAY_CATEGORIES];

	float								m_lastPlayedTime;

	// Our currently playing stuff
	CSIHandle						*m_streamHandle;
	SoundHandle						m_sfxHandle;
	SayPriority						m_priority;

	// Utilities to kickoff stuff
	bool StartSfx(const char *sfx);
	bool StartStream(const char *stream);

	// Our message handler
	static int PackData(int category, int phrase, SayPriority priority);
	static void UnpackData(int data, int &category, int &phrase, SayPriority &priority);
	REF_BASIC_MESSAGE_HANDLER(Speak, MESSAGE_VALUE, Say) m_speakHandler;
	REF_BASIC_MESSAGE_HANDLER(SayThis, MESSAGE_VALUE, Say) m_sayThisHandler;
};

#endif
