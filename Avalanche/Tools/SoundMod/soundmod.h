//============================================================================
//==
//== Project: soundmod
//== File:    soundmod.h
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

#ifndef _SOUNDMOD_H
#define _SOUNDMOD_H

typedef signed long   s32;
typedef unsigned long u32;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>

#include "soundmod.h"
#include "expandablearray.h"
#include <audio\soundmodexport.h>		// this lives inside the engine...

class Source
{
public:
	char*	name;
	bool	looping;
	bool	is2D;
	int	fileChangedTime;
	char* targetName;
	int   targetSize;
	char* targetData;
	Source() { memset( this, 0, sizeof(*this) ); }
	bool operator <  ( const Source& a ) const { return stricmp( name, a.name ) <  0; }
	bool operator >  ( const Source& a ) const { return stricmp( name, a.name ) >  0; }
};

class Sound
{
public:
	char*	name;
	char*	sourceName;
	float	volume1;
	float volume2;
	float	pitch1;
	float pitch2;
	float	wet;
	float	dry;
	float	ambience;
	float	presence;
	float audibleDist;
	Sound() { memset( this, 0, sizeof(*this) ); }
	bool operator <  ( const Sound& a ) const { return stricmp( name, a.name ) < 0; }
	bool operator >  ( const Sound& a ) const { return stricmp( name, a.name ) > 0; }
};

class Group
{
public:
	char*	name;
	int		type;
	ExpandableArray<char*> soundList;
	Group() { memset( this, 0, sizeof(*this) ); }
	bool operator <  ( const Group& a ) const { return stricmp( name, a.name ) < 0; }
	bool operator >  ( const Group& a ) const { return stricmp( name, a.name ) > 0; }
};

class Target
{
public:
	char* destPath;
	char* destExtension;
	char* loopArgument;
	char* noloopArgument;
	bool	bigEndian;
	int	maxBankSize;
	int	headerSize;
	int	pad;
	char* external;
	Target() { memset( this, 0, sizeof(*this) ); }
	ExpandableArray<char*> buildCommands;
};

//-- prototypes

void exportBankFile(
	char*										filename,
	Target									&target,
	ExpandableArray<Source>				&sourceArr,
	ExpandableArray<Sound>				&soundArr,
	ExpandableArray<Group>				&groupArr
);

extern bool exportDBL;

#endif //_SOUNDMOD_H
