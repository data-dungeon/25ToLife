/***************************************************************************/
// Sound emitter for environmental stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "collide/collsys.h"

// Setup an update rate
#define UPDATE_RATE_HZ		10
#define UPDATE_RATE_MS		(1000 / UPDATE_RATE_HZ)

// Ambient sound fade-out time in seconds
#define AMBIENT_FADEIN_TIME 	1.0f
#define AMBIENT_FADEOUT_TIME 	2.0f

// Define this for some debugging
//#define ENVSOURCE_DEBUG

/***************************************************************************/
/***************************************************************************/
SoundEmitterEnvironment::SoundEmitterEnvironment(
const char *name,
SoundDriver &soundDriver,
SoundGroup &soundGroup) :
SoundEmitter3D(name, soundDriver, soundGroup)
{
	// Setup the field
	fieldMap = NULL;
	field = NULL;
	allocatedFields = 0;
	fieldID = 0;
	currentEffect = globalEffect = SoundDriver::NONE;
	currentEffectVolume = globalEffectVolume = 1.0f;

	// Initialize the zone map
	zoneMap = NULL;
	zoneID = 0;

	// Setup the zone tracking
	for (int i = 0; i < MAX_LISTENERS; i++)
		InitProcessZoneMap(zoneProcess[i]);

	// Gate ourselves a little
	nextProcess = 0;
	firstPass = true;
	paused = false;
}

/***************************************************************************/
/***************************************************************************/
SoundEmitterEnvironment::~SoundEmitterEnvironment()
{
	Reset();
}

/***************************************************************************/
// Bind in an ambient emitter
/***************************************************************************/
void SoundEmitterEnvironment::BindAmbientEmitter(
SoundEmitter2D *emitter2D)
{
	ambient.SetEmitter(emitter2D);
}

/***************************************************************************/
// Reset the emitter by removing ALL sounds
// ALL environment handles are invalid after this call, take note!
/***************************************************************************/
void SoundEmitterEnvironment::Reset()
{
	// Blow away the zone map
	delete zoneMap;
	zoneMap = NULL;
	zoneID = 0;
	for (int i = 0; i < MAX_LISTENERS; i++)
		FreeProcessZoneMap(zoneProcess[i]);

	// Clean up the triggers
	EnvSourceTrigger *trigSound = triggerSource.Head();
	while (trigSound)
	{
		triggerSource.Remove(trigSound);
		trigSound = triggerSource.Head();
	}

	// Clean up the field
	delete fieldMap;
	fieldMap = NULL;
	delete [] field;
	field = NULL;
	allocatedFields = 0;

	// No effect
	SetEffect(globalEffect, globalEffectVolume);

	// Clear out the ambient sound
	ambient.Clear();
}

/***************************************************************************/
// Register a source in the world
/***************************************************************************/
SoundEmitterEnvironment::Handle SoundEmitterEnvironment::AddSource(
const char *name,
EnvSoundSourceType type,
Vector3CRef pos,
float audibleDistance,
int minTriggerTime,
int maxTriggerTime,
float volume,
float pitch,
float ambience,
float presence)
{
	// Duh
	if (!name || name[0] == '\0')
		return NULL;

	ts_EnvSoundSource source;

	memset(&source, '\0', sizeof(source));
	strncpy(source.name, name, sizeof(source.name) - 1);
	source.flags = EnvSoundSourceFromType(type);
	source.position = pos;
	source.audibleDistance = audibleDistance;
	source.minTriggerTime = minTriggerTime;
	source.maxTriggerTime = maxTriggerTime;
	source.volume = volume;
	source.pitch = pitch;
	source.ambience = ambience;
	source.presence = presence;

	return AddSource(source);
}

/***************************************************************************/
// Add an environmental sound that gets evaulated externally
// You pass a new'd triggeer, which I delete when the sound is unregistered.
/***************************************************************************/
SoundEmitterEnvironment::Handle SoundEmitterEnvironment::AddTriggered(
const char *name,
EnvSoundSourceType type,
EnvSoundTrigger *trigger)
{
	// Duh
	if (!trigger)
		return NULL;

	// Is this valid?
	if (!EnvSourceSound::Valid(driver, name, type))
		return NULL;

	// Make the object
	EnvSourceTrigger *source = new EnvSourceTrigger(name, type, trigger);
	if (!source)
		return NULL;

	// Add it to the list
	triggerSource.AddTail(source);

	return (Handle)source;
}

/***************************************************************************/
// Shortcut methods for intializing sounds from .dbl records
/***************************************************************************/
SoundEmitterEnvironment::Handle SoundEmitterEnvironment::AddSource(
ts_EnvSoundSource &source)
{
	// Is this valid?
	EnvSoundSourceType type = EnvSoundSourceToType(source.flags);
	if (!EnvSourceSound::Valid(driver, source.name, type))
		return NULL;

	// This would be bad
	if (!zoneMap)
	{
		zoneMap = new ZoneMap(g_collisionSystem.WorldSize(), 5.0f, 48);
		ASSERT(zoneMap);
		if (!zoneMap)
			return NULL;
	}

	// Figure out the right audible distance
	float audibleD = !Math::Zero(source.audibleDistance) ? source.audibleDistance : params.AudibleDistance();

	// Dude, create the sound
	EnvSourceStatic *s = new EnvSourceStatic(zoneID++, source.name, type,
																source.position, audibleD,
																source.volume, source.pitch,
																source.ambience, source.presence,
																source.minTriggerTime, source.maxTriggerTime);

	// Add it to the zone map
	zoneMap->Add(s);

	// Give them back a "handle" to the sound
	return (Handle)s;
}

/***************************************************************************/
// Done adding sources
/***************************************************************************/
bool SoundEmitterEnvironment::DoneSources()
{
	if (zoneMap)
		return zoneMap->Bake();

	return true;
}

/***************************************************************************/
// Setup the global "field"
/***************************************************************************/
bool SoundEmitterEnvironment::SetGlobalEnvironmentEffect(
SoundDriver::ENVEFFECT effect,
float volume)
{
	globalEffect = effect;
	globalEffectVolume = volume;

	return true;
}

/***************************************************************************/
// Add an OBB sound field to the world
/***************************************************************************/
bool SoundEmitterEnvironment::AddField(
ts_EnvSoundField &field)
{
	return (CreateAndAddField(field) != NULL);
}

/***************************************************************************/
// Done adding fields
/***************************************************************************/
bool SoundEmitterEnvironment::DoneFields()
{
	if (fieldMap)
		fieldMap->Bake();

	// Tell the driver about all the stuff we might use
	driver.BeginEnvironmentEffectProfile();
	driver.EnvironmentEffectProfile(globalEffect);

	// Tell them about all the effect zones we have
	if (fieldMap)
	{
		for (int f = 0; f < fieldMap->TotalZones(); f++)
		{
			EnvField *field = (EnvField *)fieldMap->GetZone(f);
			ASSERT(field);

			driver.EnvironmentEffectProfile(field->Effect());
		}
	}

	// Wee
	driver.EndEnvironmentEffectProfile();

	return true;
}

/***************************************************************************/
// Stop a sound
/***************************************************************************/
void SoundEmitterEnvironment::Silence(
Handle handle)
{
	if (!handle)
		return;
	EnvSource *source = (EnvSource *)handle;
	source->Stop();
}

/***************************************************************************/
// Activate/deactivate a source
/***************************************************************************/
void SoundEmitterEnvironment::Activate(
Handle handle)
{
  if (!handle)
		return;
	EnvSource *source = (EnvSource *)handle;
	source->Activate();
}

/***************************************************************************/
// Activate/deactivate a source
/***************************************************************************/
void SoundEmitterEnvironment::Deactivate(
Handle handle)
{
  if (!handle)
		return;
	EnvSource *source = (EnvSource *)handle;
	source->Deactivate();
}

/***************************************************************************/
// Remove a sound
/***************************************************************************/
void SoundEmitterEnvironment::Remove(
Handle handle)
{
	if (!handle)
		return;
	EnvSource *source = (EnvSource *)handle;

	// Stop it
	source->Stop();

	// Remove it from the right place
	if (source->Type() == EnvSource::TRIGGERED)
		triggerSource.Remove((EnvSourceTrigger *)source);
	else
	{
		// You cannot delete these
		ASSERT(false);
	}
}

/***************************************************************************/
// Run every frame
/***************************************************************************/
void SoundEmitterEnvironment::Daemon()
{
	// The first frame is going to be a problem, because the camera's position hasn't been set yet
	if (firstPass)
	{
		firstPass = false;
		return;
	}

	// don't run anything if we're paused.  Pray to the programming gods that there
	// isn't anything below that should be run while paused, and that I'm not creating
	// a host of new bugs by doing this.  --TN
	if( paused )
		return;

	// Let them run
	SoundEmitter3D::Daemon();

	// What time is it?
	uint now = driver.Time();

	// Update the ambient sound very rapidly
	ambient.Daemon(now);

	// Do a simple gate here
	if (now < nextProcess)
		return;
	nextProcess = now + UPDATE_RATE_MS;

	// Update the sound field
	if (driver.Listeners() == 1)
		ProcessField(driver.Listener(0)->Position(), now);

	// Do the work for the zone map
	for (int i = 0; i < driver.Listeners(); i++)
	{
		SoundListener *listener = driver.Listener(i);
		ASSERT(listener);

		ProcessZoneMap(listener->Position(), zoneProcess[i], now);
	}

	// Let the triggers run
	EnvSourceTrigger *trigger = triggerSource.Head();
	while (trigger)
	{
		trigger->Update(driver, *this, now);
		trigger = trigger->next;
	}
}

void SoundEmitterEnvironment::Pause( void )
{
	paused = true;
}

void SoundEmitterEnvironment::Unpause( void )
{
	paused = false;
}

/***************************************************************************/
// Debugging
/***************************************************************************/
void SoundEmitterEnvironment::Debug(
const Vector3 *cameraPos)
{
	if (zoneMap)
		zoneMap->Draw(cameraPos);
	if (fieldMap)
		fieldMap->Draw(cameraPos);
}

/***************************************************************************/
/***************************************************************************/
EnvField	*SoundEmitterEnvironment::CreateAndAddField(
ts_EnvSoundField &field)
{
	// This would be bad
	if (!fieldMap)
	{
		fieldMap = new ZoneMap(g_collisionSystem.WorldSize(), 8.0f, 32);
		ASSERT(fieldMap);
		if (!fieldMap)
			return NULL;
	}

	// Resolve the effect ID
	SoundDriver::ENVEFFECT effectID = SoundDriver::ResolveEnvironmentEffectName(field.effect);
	const float effectVolume = field.effectVolume;

	// Create the sound
	AmbientSound ambientSound(field);

	// Make the obb
	OBBInstance obb;
	AABB aabb;
	aabb.Set(-field.extents, field.extents);
	obb.SetBoundingVolume(&aabb);
	obb.SetTransform(field.orientation, field.position);

	// Create the sound field
	EnvField *f = new EnvField(fieldID++, obb,
										effectID, effectVolume, field.effectTaper,
										ambientSound);
	if (!f)
		return NULL;

	// Add it to the map
	fieldMap->Add(f);

	// Everything worked
	return f;
}

/***************************************************************************/
// Process the field
/***************************************************************************/
void SoundEmitterEnvironment::ProcessField(
Vector3CRef listenerPos,
uint now)
{
	// Any work to do?
	if (!fieldMap || fieldMap->TotalZones() == 0)
	{
		SetEffect(globalEffect, globalEffectVolume);
		return;
	}

	// Allocate the array
	AllocateFieldList(fieldMap->TotalZones());
	if (!field)
	{
		SetEffect(globalEffect, globalEffectVolume);
		return;
	}

	// Read it in
	int fields = fieldMap->Touches(listenerPos, field, allocatedFields, false);

	// Which is the best field?
	EnvField *bestField = NULL;
	for (int f = 0; f < fields; f++)
	{
		EnvField *checkField = (EnvField *)field[f];

		// Is this field active?
		if (!field[f]->Active())
			continue;

		// Is it better?
		if (bestField == NULL || checkField->Size() < bestField->Size())
			bestField = checkField;
	}

	// No best field?
	if (!bestField)
	{
		SetEffect(globalEffect, globalEffectVolume);
		return;
	}

	// Cool, set the environmental effect
	SetEffect(bestField->Effect(), bestField->Volume(listenerPos));
	ambient.Queue(bestField->Ambient());
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::AllocateFieldList(
int maxFields)
{
	if (maxFields > allocatedFields)
	{
		delete [] field;
		field = new Zone *[maxFields];
		allocatedFields = maxFields;
		if (!field)
		{
			field = NULL;
			allocatedFields = 0;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::SetEffect(
SoundDriver::ENVEFFECT effect,
float volume)
{
	if (effect != currentEffect || !Math::Equal(volume, currentEffectVolume))
	{
		driver.SetEnvironmentEffect(effect, volume);
		currentEffect = effect;
		currentEffectVolume = volume;
	}
}

/***************************************************************************/
// Do the work for the zone map
/***************************************************************************/
void SoundEmitterEnvironment::ProcessZoneMap(
Vector3CRef pos,
ZoneProcess &process,
uint now)
{
	if (!zoneMap || zoneMap->TotalZones() == 0)
		return;

	// Make sure we have allocated this
	AllocateProcessZoneMap(process, zoneMap->MaxOverlappingZones());

	// Get the current active stuff
	process.current->zones = zoneMap->Touches(pos, process.current->zone, process.zonesAllocated, true);

	// Debugging
#ifdef ENVSOURCE_DEBUG
	if (process.current->zones != 0 || process.last->zones != 0)
	{
		TRACE("Processing sound sources (current %d last %d):\n", process.current->zones, process.last->zones);
	}
#endif

	// Go through each thing in the list
	int last = 0;
	EnvSourceStatic *lastSource = GetStaticSource(process.last, last++);
	for (int current = 0; current < process.current->zones; current++)
	{
		// Anal assert
		ASSERT(current == 0 ||
					process.current->zone[current]->ID() > process.current->zone[current - 1]->ID());

		// Setup some nice aliases
		EnvSourceStatic *currentSource = (EnvSourceStatic *)process.current->zone[current];

		// Stop all the sounds that are less than this sound
		while (lastSource && lastSource->ID() < currentSource->ID())
		{
#ifdef ENVSOURCE_DEBUG
			TRACE("Stopping source %s %d\n", lastSource->Name(), lastSource->ID());
#endif
			lastSource->Stop();
			lastSource = GetStaticSource(process.last, last++);
		}

		bool readyToPlay = currentSource->ReadyToPlay(now);
		bool playing = currentSource->Playing();

#ifdef ENVSOURCE_DEBUG
		if (playing)
			TRACE("Source %s %d playing\n", currentSource->Name(), currentSource->ID());
		else
		{
			if (readyToPlay)
				TRACE("Starting source %s %d\n", currentSource->Name(), currentSource->ID());
			else
				TRACE("Source %s %d waiting\n", currentSource->Name(), currentSource->ID());
		}
#endif

		// Start this sound playing
		if (!currentSource->Active())
			currentSource->Stop();
		else if (readyToPlay && !playing)
			currentSource->Play(driver, *this, now);

		// If these are the same, next
		if (lastSource && lastSource->ID() == currentSource->ID())
			lastSource = GetStaticSource(process.last, last++);
	}

	// Shut off all last zones we didn't touch
	while (lastSource)
	{
#ifdef ENVSOURCE_DEBUG
		TRACE("Stopping source %s %d\n", lastSource->Name(), lastSource->ID());
#endif
		lastSource->Stop();
		lastSource = GetStaticSource(process.last, last++);
	}

	// Advance the double buffer
	FlipProcessZoneMap(process);
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::InitProcessZoneMap(
ZoneProcess &process)
{
	memset(&process.touches[0], '\0', sizeof(process.touches[0]));
	memset(&process.touches[1], '\0', sizeof(process.touches[1]));
	process.current = &process.touches[0];
	process.last = &process.touches[1];
	process.zonesAllocated = 0;
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::AllocateProcessZoneMap(
ZoneProcess &process,
int maxZones)
{
	// Create new lists?
	if (process.zonesAllocated == 0 && maxZones != 0)
	{
		process.touches[0].zone = new Zone *[maxZones];
		process.touches[1].zone = new Zone *[maxZones];
		process.zonesAllocated = maxZones;
	}
	else if (maxZones <= process.zonesAllocated)
	{
		// Nothing to do here, it's fine
	}
	else
	{
		// Zones have been added after run-time,
		// assert false for now, we need to resize
		// the lists and copy stuff
		ASSERT(false);
	}
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::FreeProcessZoneMap(
ZoneProcess &process)
{
	delete [] process.touches[0].zone;
	delete [] process.touches[1].zone;
	InitProcessZoneMap(process);
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitterEnvironment::FlipProcessZoneMap(
ZoneProcess &process)
{
	// Swap the buffers
  	ZoneTouches *temp = process.current;
  	process.current = process.last;
  	process.last = temp;
}
