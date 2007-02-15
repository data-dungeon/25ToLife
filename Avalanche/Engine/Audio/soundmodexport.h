//============================================================================
//==
//== Project: soundmod
//== File:    soundmodexport.h
//== Desc:    Builds a composite sound bank file from wav/aiff files and
//==          control modules files.
//== Author:  Rob Nelson
//== Date:    2/6/2003
//== (c)      2003 Avalanche Software, Inc.
//==
//== This file defines the structures used in the .BNK file from the exporter
//== and what is used by the runtime engine.  Basic file format:
//==
//== --------------------------
//== HEADERS
//==     [SoundModHeaderDef]
//==     [SourceDefs]
//==     [SoundDefs]
//==     [GroupDefs]
//==     [ascii names]
//== --------------------------
//== SOUND RAM DATA
//==     [raw binary chunk]
//== --------------------------
//==
//============================================================================

#ifndef _SOUNDMODEXPORT_H
#define _SOUNDMODEXPORT_H

// Sounds have a destructor, so we must explicitly use it's type
class Sound;

struct SourceDef
{
	s32		offset;				// offset into bank of this sound
	s32		length;				// length of this sound
	s32		flags;				// flag if this is a looping sound effect
	Sound*	engineSndPtr;		// ptr the engine can use to remember any details about this sound
};

struct SoundDef
{
	char*			name;				// name of this sound.
	SourceDef*	source;			// source for this sound
	signed char volume1;			// low volume this sound should be played at
	signed char volume2;			// high volume this sound should be played at
	signed char pitch1;			// low pitch this sound should be played at
	signed char pitch2;			// high pitch this sound should be played at
	signed char ambience;		// ambience
	signed char presence;		// presence
	signed char wet;				// wetness
	signed char dry;				// dryness
	unsigned char audibleDist;	// distance in meters this sound is audible
};

struct GroupDef
{
	char*			name;				// name of this group
	s32			type;				// type of group (see SOUND_GROUP_TYPE_xxx below)
	s32			count;			// number of sounds in this group
	u32			engineData;		// used by engine for various group methods (used bits, last fx, etc)
	SoundDef**	soundTbl;		// pointer to arr of SoundDef ptrs.
};

struct SoundModHeaderDef
{
	u32			id;				// id of file
	u32			version;			// version of file
	s32			sourceCount;	// num sources
	s32			soundCount;		// num sounds
	s32			groupCount;		// num groups
	SourceDef*	sourceTable;	// ptr to list of sources
	SoundDef*	soundTable;		// ptr to list of sounds
	GroupDef*	groupTable;		// ptr to list of groups
	s32			soundBankOffset; // offset to the sound bank data from the root
	int			soundBankSize;	// size of the sound bank data
	char			externalFile[16]; // External data name
	u32			externalFileSize; // External data size
};

#define SOUND_MOD_FILE_ID			( ( 'S' << 24 ) | ( 'M' << 16 ) | ( 'O' << 8 ) | ( 'D' << 0 ) )
#define SOUND_MOD_FILE_VERSION	( 0x1005 )

#define SOUND_MOD_PITCH_SCALE		( 4.0f )

#define SOUND_DEINIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = ( (int)(p) - (int)&(p) - 1 ); \
	} \
}

#define SOUND_INIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = (int)p + (int)&(p) + 1; \
	} \
}

enum
{
	SOUND_SOURCE_FLAG_LOOPING		= 0x0001,
	SOUND_SOURCE_FLAG_2D          = 0x0002,
};

enum
{
	SOUND_GROUP_TYPE_NONE,
	SOUND_GROUP_TYPE_RANDOM,
	SOUND_GROUP_TYPE_RANDOM_UNIQUE,
	SOUND_GROUP_TYPE_RANDOM_SKIP,
};

#endif //_SOUNDMODEXPORT_H
