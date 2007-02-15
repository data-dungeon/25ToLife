/***************************************************************************/
// Sound emitter for environmental stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "collide/collsys.h"
#include "Math/fastrand.h"

/***************************************************************************/
/***************************************************************************/
AmbientSoundController::AmbientSoundController()
{
	emitter = NULL;
	handle[0] = handle[1] = NULL;

	currentSound.Clear();
	ClearQueue();

	nextTrigger = 0;

	state = SILENT;
}

/***************************************************************************/
/***************************************************************************/
AmbientSoundController::~AmbientSoundController()
{
	Stop();
}

/***************************************************************************/
// Setup the emitter
/***************************************************************************/
void AmbientSoundController::SetEmitter(
SoundEmitter2D *parentEmitter)
{
	emitter = parentEmitter;
}

/***************************************************************************/
// Reset everything
/***************************************************************************/
void AmbientSoundController::Clear()
{
	Stop();
	ClearQueue();
}

/***************************************************************************/
// Queue an ambient sound
/***************************************************************************/
void AmbientSoundController::Queue(
const AmbientSound &queueMe)
{
	// Only queue if it isn't the same as what's playing
	if (queueMe != currentSound)
		nextSound = queueMe;
}

/***************************************************************************/
// Update
/***************************************************************************/
void AmbientSoundController::Daemon(
uint now)
{
	// Handle the current state
	if (state == SILENT)
	{
		// Is there something in the queue?
	 	if (Queued())
			AdvanceQueue(now);
	}
	else if (state == FADEIN)
	{
		float v = rampUp(now);
		if (emitter)
		{
			emitter->SetVolume(handle[0], v);
			emitter->SetVolume(handle[1], v);
		}
		if (rampUp.Done(now))
			state = PLAYING;
	}
	else if (state == FADEOUT)
	{
		float v = rampDown(now);
		if (emitter)
		{
			emitter->SetVolume(handle[0], v);
			emitter->SetVolume(handle[1], v);
		}
		if (rampDown.Done(now))
			Stop();
	}
	else if (state == PLAYING)
	{
		// Handle retriggers
		if (nextTrigger != 0 && now > nextTrigger)
		{
			Play();
			nextTrigger = NextTrigger(now);
		}

		// Is there something in the queue?
 		if (Queued())
		{
			if (currentSound.Triggered())
				Stop();
			else
				BeginFadeOut(now);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void AmbientSoundController::Stop()
{
	if (emitter)
	{
		emitter->Kill(handle[0]);
		emitter->Kill(handle[1]);
	}
	currentSound.Clear();
	state = SILENT;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSoundController::Play(float volume)
{
	if (!emitter || currentSound.Empty())
		return;

	// If there is a handle now, they are retriggering a loop...
	emitter->Kill(handle[0]);
	emitter->Kill(handle[1]);

	// Start the new sound playing
	if (!currentSound.Stereo())
	{
		handle[0] = emitter->Cue(currentSound.Name());
		emitter->SetVolume(handle[0], currentSound.Volume() * volume);
		emitter->Play(handle[0]);
	}
	else
	{
		char stereoNameL[MAX_SOUND_NAME_SIZE + 1];
		char stereoNameR[MAX_SOUND_NAME_SIZE + 1];

		strcpy(stereoNameL, currentSound.Name());
		strcat(stereoNameL, "L");
		strcpy(stereoNameR, currentSound.Name());
		strcat(stereoNameR, "R");

		handle[0] = emitter->Cue(stereoNameL);
		emitter->SetVolume(handle[0], currentSound.Volume() * volume);
		emitter->SetPan(handle[1], -1.0f);
		emitter->Play(handle[0]);

		handle[1] = emitter->Cue(stereoNameR);
		emitter->SetVolume(handle[1], currentSound.Volume() * volume);
		emitter->SetPan(handle[1], 1.0f);
		emitter->Play(handle[1]);
	}
}

/***************************************************************************/
/***************************************************************************/
void AmbientSoundController::BeginFadeIn(
uint now,
float volume)
{
	rampUp.Reset(now);
	rampUp.SetAmplitude(volume);
	rampUp.SetPeriod(currentSound.FadeInTime());
	state = FADEIN;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSoundController::BeginFadeOut(
uint now)
{
	rampDown.Reset(now);
	rampUp.SetPeriod(currentSound.FadeOutTime());
	state = FADEOUT;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSoundController::AdvanceQueue(
uint now)
{
	ASSERT(state == SILENT);
	ASSERT(Queued());

	// Copy the sound
	currentSound = nextSound;
	nextSound.Clear();

	// Triggered or not?
	if (currentSound.Triggered())
	{
		// No fade in on triggered sounds
		nextTrigger = NextTrigger(now);
		state = PLAYING;
	}
	else
	{
		// Start the sound playing and fade it in
		Play(0.0f);
		BeginFadeIn(now, currentSound.Volume());
		nextTrigger = 0;
	}
}

/***************************************************************************/
/***************************************************************************/
uint AmbientSoundController::NextTrigger(
uint now)
{
	return (now + (uint)g_random.InRange(currentSound.MinRetriggerTime(), currentSound.MaxRetriggerTime()));
}


