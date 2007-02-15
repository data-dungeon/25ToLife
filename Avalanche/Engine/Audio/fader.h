///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef FADER_H
#define FADER_H

#include "container/dllist.h"

class Fader
{
public:
	Fader();
	Fader(const char *name, float volume = 1.0f, Fader *parent = NULL);

	// Seutp the full monty
	void Setup(const char *name, float volume = 1.0f, Fader *parent = NULL);

	// What is our name
	const char *Name()
		{ return m_name; }

	// Who is our parent?
	Fader *Parent()
		{ return m_parent; }
	void SetParent(Fader *parent)
		{ m_parent = parent; }

	// What is our current volume
	float Volume()
		{ return m_volume; }
	void SetVolume(float targetVolume, float time = 0.0f);

	// Set a factor without affecting the main volume
	float Factor()
		{ return m_factor; }
	void SetVolumeFactor(float factor)
		{ m_factor = factor; }

	// Evaluate the current value
	float MixVolume();

	// Handy thing that handles volume
	static float MixVolume(Fader *fader)
		{ return fader ? fader->MixVolume() : 1.0f; }

	// Update (call this once per frame)
	void Update(float timeElapsed);

private:
	char 		m_name[8];
	Fader		*m_parent;

	float		m_volume;
	float		m_targetVolume;
	float		m_volumeChangeRate;

	float		m_factor;

public:
	// We appear in a list
	Fader		*prev, *next;
};

class FaderBoard
{
public:
	FaderBoard();
	~FaderBoard();

	// Add a fader (I own it now, and will call delete)
	void Add(Fader *fader);

	// Delete a fader (remove from list & destructor)
	void Delete(Fader *&fader);

	// Remove a fader (will not call delete)
	void Remove(Fader *fader);

	// Find a fader based on name
	Fader *Find(const char *name);

	// Update all the faders
	void Update(float timeElapsed);

private:
	DoublyLinkedList<Fader>		m_faders;
};

#endif
