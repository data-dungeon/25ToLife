/***************************************************************************/
// SoundGroupAlias
//
// Allows you to attach another name to an existing sound group
/***************************************************************************/
#ifndef SGROUPALIAS_H
#define SGROUPALIAS_H

// Some forwards
class SoundGroup;

class SoundGroupAlias
{
public:
	// What is our name?
	const char *Name() const;

	// What group are we bound to
	SoundGroup *Group() const;

private:
   // Our attibutes
	char										name[64];
	SoundGroup								*group;

	// Create the clone
	SoundGroupAlias(const char *name, SoundGroup *group);

	friend class Audio;

public:
	// Has to be public for list management
	~SoundGroupAlias() {}
};

#endif
