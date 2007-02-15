/***************************************************************************/
// SoundGroupAlias
//
// Allows you to attach another name to an existing sound group
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
SoundGroupAlias::SoundGroupAlias(
const char *alias,
SoundGroup *aliasGroup)
{
	strncpy(name, alias, sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';
	group = aliasGroup;
}

/***************************************************************************/
// What is our name?
/***************************************************************************/
const char *SoundGroupAlias::Name() const
{
	return name;
}

/***************************************************************************/
/***************************************************************************/
SoundGroup *SoundGroupAlias::Group() const
{
	return group;
}

