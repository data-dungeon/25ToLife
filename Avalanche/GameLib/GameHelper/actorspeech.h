//***************************************************************************
// Utiltiy class that helps ensure an actor is only ever "saying" one thing
// at a time.  This manages both a emitter based SFX and a stream.
//***************************************************************************
#ifndef ACTORSPEECH_H
#define ACTORSPEECH_H

#include "EngineHelper/sfx.h"

class CommonStreamInterface;

class ActorSpeech
{
public:
	// Passing NULL in for the actor does everything in 2D
	ActorSpeech(CActor *owningActor = NULL);
	virtual ~ActorSpeech();

	// How should a play request be handled?
	enum STYLE
	{
		NOTBUSY,				// Only play if no other speech is playing
		PREEMPT,				// Preempt other speech by this actor
		PREEMPTALL			// Preemt all other things in the dialog queue
	};

	// Set the wet mix for this actor
	void SetWet(float newWet)
		{ wet = Math::Clamp(newWet); }

	// Play a sfx based speech
	bool Play(const char *sfxName, STYLE style, float volume = 1.0f);

	// Play an SFX ignoring actor position
	bool Play2D(const char *sfxName, STYLE style, float volume = 1.0f);

	// Stream something
	bool PlayStream(const char *streamName, STYLE style, float volume = 1.0f);

	// Is the actor saying something?
	bool Talking();

	// Silence the actor	(returns true if they were talking)
	bool Silence();

protected:
	// Actor hte owns us
	CActor				*actor;

	// Settings
	float					wet;

	// Currently playing sfx
	SoundHandle			sfx;

	// Currently playing speech
	bool					inStream;
	float					dialogVolume, oldDialogVolume; // save the old volume

	// What is the real volume
	static float RealVolume(float volume);

	// Here is what we use to track a dialog being done
	static void StreamStarted(void *context, CommonStreamInterface *stream);
	static void StreamDone(void *context, CommonStreamInterface *stream);

	// Is it okay to play someting
	virtual bool OkayToPlay(STYLE style, bool stream);
	virtual void NewSpeechStarted(STYLE style);

	// Subclass this to change play properties
	virtual SoundHandle PlaySFX(const char *sfxName, CActor *actor, float volume, float wet);
	virtual bool PlayStream(const char *streamName, float volume, float wet);
};

#endif
