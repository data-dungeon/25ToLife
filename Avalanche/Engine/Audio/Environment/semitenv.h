/***************************************************************************/
// Sound emitter for environmental stuff
/***************************************************************************/
#ifndef SEMITENV_H
#define SEMITENV_H

#include "container/dllist.h"
#include "Audio/Emitter/semit2d.h"
#include "Audio/Emitter/semit3d.h"
#include "Audio/Environment/envfield.h"
#include "Audio/Environment/envsource.h"
#include "Audio/Environment/envstrig.h"
#include "Audio/Environment/envdbl.h"
#include "CollideHelper/zonemap.h"
#include "Audio/Environment/ambient.h"

class SoundEmitterEnvironment : public SoundEmitter3D
{
public:
	// Bind in an ambient emitter
	void BindAmbientEmitter(SoundEmitter2D *emitter2D);

	// You can control sources after you create them
	typedef void *Handle;

	// Register a source in the world
	Handle AddSource(const char *name, EnvSoundSourceType type, 
							Vector3CRef pos, float audibleDistance = 0.0f,
							int minTriggerTime = 0, int maxTriggerTime = 0,
							float volume = 1.0f, float pitch = 0.0f,
							float ambience = 0.0f, float presence = 0.0f);

	// Add an environmental sound that gets evaulated externally
	// You pass a new'd trigger, which I delete when the sound is unregistered.
	Handle AddTriggered(const char *name, EnvSoundSourceType type, EnvSoundTrigger *trigger);

	// Shortcut methods for intializing sounds from .dbl records
	Handle AddSource(ts_EnvSoundSource &source);

	// Done adding sources
	bool DoneSources();

	// Setup the global "field"
	bool SetGlobalEnvironmentEffect(SoundDriver::ENVEFFECT effect, float volume);

	// Add an OBB sound field to the world
	bool AddField(ts_EnvSoundField &field);

	// Done adding fields
	bool DoneFields();

	// Make sure a sound is not playing
	void Silence(Handle handle);

	// Activate/deactivate a source
	void Activate(Handle handle);
	void Deactivate(Handle handle);

	// Remove a sound (can be slow if it's a static source)
	void Remove(Handle handle);

	// Run every frame
	void Daemon();

	// Remove ALL sounds from the list
	void Reset();

	// Debugging
	void Debug(const Vector3 *cameraPos);

	// this emitter has the ability to generate sounds on its own, so we
	// need to be able to pause it, since playing game sounds while the pause
	// menu is up is a TRC violation.
	void Pause( void );
	void Unpause( void );

protected:
	// Only the sound system can create us
	SoundEmitterEnvironment(const char *name,
									SoundDriver &soundDriver,
									SoundGroup &soundGroup);

	// The field management
	bool											firstPass;
	ZoneMap										*fieldMap;
	int											fieldID;
	Zone											**field;
	int											allocatedFields;

	SoundDriver::ENVEFFECT					globalEffect;
	float											globalEffectVolume;

	SoundDriver::ENVEFFECT					currentEffect;
	float											currentEffectVolume;

	EnvField	*CreateAndAddField(ts_EnvSoundField &field);
	void ProcessField(Vector3CRef listenerPos, uint now);
	void AllocateFieldList(int maxFields);
	void SetEffect(SoundDriver::ENVEFFECT effect, float volume);

	// ambient sound support
	AmbientSoundController					ambient;

	// The static source managment
	ZoneMap										*zoneMap;
	int											zoneID;
	struct ZoneTouches
	{
		Zone										**zone;
		int										zones;
	};
 	struct ZoneProcess
	{
		ZoneTouches								touches[2];
		ZoneTouches								*current;
		ZoneTouches								*last;
		int										zonesAllocated;
	};
	ZoneProcess									zoneProcess[MAX_LISTENERS];
	void InitProcessZoneMap(ZoneProcess &process);
	void AllocateProcessZoneMap(ZoneProcess &process, int maxZones);
	void FreeProcessZoneMap(ZoneProcess &process);
 	void ProcessZoneMap(Vector3CRef pos, ZoneProcess &process, uint now);
	void FlipProcessZoneMap(ZoneProcess &process);
	EnvSourceStatic *GetStaticSource(ZoneTouches *touches, int index)
		{ return (EnvSourceStatic *)((index < touches->zones) ? touches->zone[index] : NULL); }

	// The list of trigger sources (slow culling)
	DoublyLinkedList<EnvSourceTrigger> 	triggerSource;

	// Don't hog too much CPU
	uint				nextProcess;

	bool				paused;

	// The audio class creates us
	friend class Audio;

public:
	// Has to be public for list management
	~SoundEmitterEnvironment();
};

#endif
