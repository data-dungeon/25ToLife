/***************************************************************************/
// DBL support stuff for Environmental sound stuff
/***************************************************************************/
#ifndef ENVDBL_H
#define ENVDBL_H

#include "Math/Vector.h"
#include "collide/obb.h"

#define MAX_SOUND_NAME_SIZE	32
#define MAX_EFFECT_NAME_SIZE	32

// Environmental sound source flags
#define ENVSOUNDSOURCE_DIRECTIONAL		0x01
#define ENVSOUNDSOURCE_STREAM				0x02
#define ENVSOUNDSOURCE_STREAMLOOPED		0x04

// An environmental sound source
struct ts_EnvSoundSource
{
	u32				flags;

	char				name[MAX_SOUND_NAME_SIZE]; // effect name from MOD
	Vector3Packed	position;						// (world space)
	float				audibleDistance; 				// (meters)
	s32				minTriggerTime;				// minimum trigger time in ms
	s32				maxTriggerTime;				// maximum trigger time in ms
	float				volume;							// 0 - 1
	float				pitch;							// In semitones
	float 			ambience;						// 0 - 1
	float 			presence;						// 0 - 1

	Vector3Packed	dir;								// Direction for directional sounds
	float				minConeAngle;					// Min cone angle in radians
	float				maxConeAngle;					// Max cone angle in radians

	char				pad[32];							// Padding to bug people who hate padding
};

// some handy tests
inline bool EnvSoundSourceIsStream(const ts_EnvSoundSource &source)
	{ return (source.flags & (ENVSOUNDSOURCE_STREAM | ENVSOUNDSOURCE_STREAMLOOPED)) != 0; }
inline bool EnvSoundSourceStreamLooped(const ts_EnvSoundSource &source)
	{ return (source.flags & ENVSOUNDSOURCE_STREAMLOOPED) != 0; }

// A real type based on the flags
enum EnvSoundSourceType
{
	ESST_SFX,			// standard mod based SFX
	ESST_SFXDIR,		// standard mod based dir SFX
	ESST_STREAM,		// stream based
	ESST_STREAMLOOPED	// stream based loop
};
inline EnvSoundSourceType EnvSoundSourceToType(u32 flags)
{
	if (flags & ENVSOUNDSOURCE_STREAM)
		return ESST_STREAM;
	else if (flags & ENVSOUNDSOURCE_STREAMLOOPED)
		return ESST_STREAMLOOPED;
	else if (flags & ENVSOUNDSOURCE_DIRECTIONAL)
		return ESST_SFXDIR;
	else
		return ESST_SFX;
}
inline u32 EnvSoundSourceFromType(EnvSoundSourceType type)
{
	if (type == ESST_STREAM)
		return ENVSOUNDSOURCE_STREAM;
	else if (type == ESST_STREAMLOOPED)
		return ENVSOUNDSOURCE_STREAMLOOPED;
	else if (type == ESST_SFXDIR)
		return ENVSOUNDSOURCE_DIRECTIONAL;
	else
		return 0;
}


// Is the ambient sound in stereo?
#define ENVSOUNDFIELD_AMBIENTSTEREO		0x01

struct ts_EnvSoundField
{
	u32							flags;	  							// The flags

	// OBB that defines this field in non-OBB form (go figure)
	Vector3Packed				extents;								// Max extents
	Vector3Packed				position;							// position
	DirCos3x3					orientation;						// orientation

	char							effect[MAX_EFFECT_NAME_SIZE];	// reverb/echo effect
	float							effectVolume;						// effect volume (0 - 1)

	// Stuff for the ambient effect
	char							ambient[MAX_SOUND_NAME_SIZE]; // ambient effect name from MOD
	float							volume;								// ambient volume
	s32							minRetriggerTime;		 			// minimum trigger time in ms
	s32							maxRetriggerTime;					// maximum trigger time in ms

	float							fadeInTime;							// Fade in time
	float							fadeOutTime;						// Fade out time

	float							effectTaper;		 				// Fade in/out for the field effect

	char							pad[36];								// Don't get mad, get pad
};

#endif
