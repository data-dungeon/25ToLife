//***************************************************************************
// Utiltiy class that helps ensure an actor is only ever "saying" one thing
// at a time.  This manages both a emitter based SFX and a stream.
//***************************************************************************
#include "GameHelper/GameHelperPCH.h"

//***************************************************************************
// Passing NULL in for the actor does everything in 2D
//***************************************************************************
ActorSpeech::ActorSpeech(
CActor *owningActor)
{
	actor = owningActor;
	SetWet(1.0f);

	// Nothing playing yet
	sfx = NULL;
	inStream = false;
}

//***************************************************************************
//***************************************************************************
ActorSpeech::~ActorSpeech()
{
	Silence();
}

//***************************************************************************
// Play a sfx based speech
//***************************************************************************
bool ActorSpeech::Play(
const char *sfxName,
STYLE style,
float volume)
{
	// Preempt!
	if (!OkayToPlay(style, false))
		return false;

	// Play it
	SoundHandle newSpeech = PlaySFX(sfxName, actor, RealVolume(volume), wet);
	if (!newSpeech)
		return false;

	// Stop the old thing
	NewSpeechStarted(style);
	sfx = newSpeech;

	// We workd
	return true;
}

//***************************************************************************
// Play an SFX ignoring actor position
//***************************************************************************
bool ActorSpeech::Play2D(
const char *sfxName,
STYLE style,
float volume)
{
	// Preempt!
	if (!OkayToPlay(style, false))
		return false;

	// Play it
	SoundHandle newSpeech = PlaySFX(sfxName, NULL, RealVolume(volume), wet);
	if (!newSpeech)
		return false;

	// Stop the old thing
	NewSpeechStarted(style);
	sfx = newSpeech;

	// We workd
	return true;
}

//***************************************************************************
// Stream something
//***************************************************************************
bool ActorSpeech::PlayStream(
const char *streamName,
STYLE style,
float volume)
{
	// Preempt!
	if (!OkayToPlay(style, true))
		return false;

	// Does the stream exist?
	if (!g_audio.dialog.Exists(streamName))
		return false;

	// Stop everything
	NewSpeechStarted(style);

	// Play it
	return PlayStream(streamName, RealVolume(volume), wet);
}

//***************************************************************************
// Is the actor saying something?
//***************************************************************************
bool ActorSpeech::Talking()
{
	// is the sfx done?
	if (sfx && !Sfx::Active(sfx))
		Sfx::Stop(sfx);

	// Are we busy?
	return (sfx != NULL || inStream);
}

//***************************************************************************
//***************************************************************************
bool ActorSpeech::Silence()
{
	bool wasTalking = Talking();

	// Kill the sfx based speech
	Sfx::Stop(sfx);

	// Are we in a stream?
	if (inStream)
		g_audio.dialog.Stop();

	return wasTalking;
}

//***************************************************************************
// What is the real volume
//***************************************************************************
float ActorSpeech::RealVolume(
float volume)
{
	return volume * g_audio.GetVolume(Audio::SFX);
}

//***************************************************************************
//***************************************************************************
void ActorSpeech::StreamStarted(
void *context,
CommonStreamInterface *stream)
{
	ActorSpeech *me = (ActorSpeech *)context;

	if (me)
	{
		ASSERT(!me->inStream);
		me->inStream = true;
		me->oldDialogVolume = g_audio.dialog.GetVolume();
		g_audio.SetVolume(Audio::DIALOG, RealVolume(me->dialogVolume));
	}
}

//***************************************************************************
//***************************************************************************
void ActorSpeech::StreamDone(
void *context,
CommonStreamInterface *stream)
{
	ActorSpeech *me = (ActorSpeech *)context;
	if (me)
	{
		ASSERT(me->inStream);
		me->inStream = false;
		g_audio.SetVolume(Audio::DIALOG, me->oldDialogVolume);
	}
}

//***************************************************************************
// Is it okay to play someting
//***************************************************************************
bool ActorSpeech::OkayToPlay(
STYLE style,
bool stream)
{
	if (style == NOTBUSY)
		return !Talking();
	else if (style == PREEMPT)
	{
		// If they want to play a stream w/preempt
		// it has to be our stream playing
		if (!stream)
			return true;
		else
			return (!g_audio.dialog.Playing() || inStream);
	}
	else if (style == PREEMPTALL)
		return true;
	else
	{
		// Undefined state
		ASSERT(false);
		return false;
	}
}

//***************************************************************************
//***************************************************************************
void ActorSpeech::NewSpeechStarted(
STYLE style)
{
	// Do they want to preempt the entire dialog queue?
	if (style == PREEMPTALL && g_audio.dialog.Playing())
		g_audio.dialog.Clear();

	// Is something playing?
	Silence();
}

//***************************************************************************
// Subclass this to change play properties
//***************************************************************************
SoundHandle ActorSpeech::PlaySFX(
const char *sfxName,
CActor *actor,
float volume,
float wet)
{
	SoundHandle handle;

	// No actor, play it in 2d
	bool ret;
	if (!actor)
		ret = Sfx::Play2D(sfxName, volume, 0.0f, &handle);
	else
		ret = Sfx::Play(sfxName, *actor, Sfx::ACTORPLAYPOS_DEFAULT, volume, 0.0f, &handle);

	// Did it work?
	if (!ret)
		return NULL;

	// Make sure we set wet/dry
	Sfx::SetWetDry(handle, wet, 1.0f);

	// Give them back the handle
	return handle;
}

//***************************************************************************
//***************************************************************************
bool ActorSpeech::PlayStream(
const char *streamName,
float volume,
float wet)
{
	// We want the volume to match up
	dialogVolume = volume;
	return g_audio.dialog.QueueCallback(streamName, StreamStarted, StreamDone, this, 0.0f, false, wet);
}
