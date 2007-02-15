/***************************************************************************/
// SoundGroup
//
// The sound group class contains a table of channels and extra information
// for the hardware voices.  Emitters allocate channels from these groups.
/***************************************************************************/
#ifndef SGROUP_H
#define SGROUP_H

#include "Audio/group/schannel.h"

// Some forwards
class AudioSystem;
class SoundDriver;
class Reclaim;

class SoundGroup
{
public:
	// What is our name
	const char *Name() const;

	// What type are we? (poor mains RTTI)
	enum TYPE
	{
		GROUP2D,
		GROUP3D
	};
	virtual TYPE Type() const = 0;

	// Setup our reclaim method
	bool SetReclaim(Reclaim &reclaimMethod);

	// How many channels are we actively using
	int UsedChannels();

	// What is the maximum number of channels we will use?
	int MaxChannels() const;

	// Adjust the number of channels (may cause voices to drop)
	bool AdjustChannels(int newChannels);

	// Allocate a channel (can return NULL)
	// doesn't put it in the list, but holds a channel (either keep or delete it)
	SoundChannel *AllocateChannel(SoundEmitter &emitter, SoundDef &def);

	// Aquire a hardware voice for the channel
	bool AquireVoice(SoundChannel *channel);

	// Free this channel
	void FreeChannel(SoundChannel *who);

	// Pause/Unpause this group
	void Pause();
	void Unpause();

	// Call this every frame
	void Daemon();

	// Enumerate the channels for an emitter
	SoundChannel *FindFirst(SoundEmitter *emitter);
	SoundChannel *FindNext(SoundEmitter *emitter, SoundChannel *current);

	// Should this group be destroyed when empty?
	void SetDestroyWhenEmpty(bool destroy = true);
	bool DestroyWhenEmpty();

	// Size down if possible
	virtual void Shrink() {}

protected:
	// Only the sound system can create us
	SoundGroup(const char *groupName,
					SoundDriver &soundDriver,
				  	int howManyChannels,
					Reclaim &reclaimMethod);

	// The subclasses need to implement these, they do not
	// need to be monkeyed with in the list
	virtual SoundChannel *NewChannel(SoundEmitter &emitter, SoundDef &def) = 0;
	virtual void DeleteChannel(SoundChannel *channel) = 0;

private:
	// The driver
	SoundDriver								&driver;

   // Our attibutes
	char										name[64];
	Reclaim									&reclaim;

	// The array of channels
	DoublyLinkedList<SoundChannel>  	channelList;
	int	  									maxChannels;

	// Tracking
	int										maxUsedVoices;
	int										reclaims;

	// Destroy the group if it has no emitters.
	bool										destroyWhenEmpty;

	// Reclaim a voice
	bool ReclaimVoice(SoundChannel *forWho);

	// Dump out stuff about stuff
	void DebugReclaim(SoundChannel *whoIsGoingToDie);

protected:
	// Clean up the channels, child classes must call this
	// As it uses the virtual DeleteChannel function
	void 										KillChannels();

public:
	// this has to be public for list management
	virtual ~SoundGroup();
};

#endif
