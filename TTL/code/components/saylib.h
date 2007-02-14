/////////////////////////////////////////////////////////////////////////////
// The say library
/////////////////////////////////////////////////////////////////////////////
#ifndef SAYLIB_H
#define SAYLIB_H

#include "container/dynlist.h"
#include "Components/Component.h"
#include "EngineHelper/Singleton.h"
#include "EngineHelper/StringPool.h"
#include "EngineHelper/hog.h"

// These are the defined priorities
enum SayPriority
{
	SAYPRIO_NONE = 0, // Use the category setting
	SAYPRIO_LOW = 1,
	SAYPRIO_NORMAL = 2,
	SAYPRIO_HIGH = 3,
	SAYPRIO_VERYHIGH = 4
};

class SayVoiceSet
{
public:
	SayVoiceSet(const char *name, int categories, float volume = 1.0f, float pitch = 0.0f);
	~SayVoiceSet();

	// Load it!
	static SayVoiceSet *Load(const char *name, HOGFILE hf);
	bool DumpToNewFile();

	// our name
	const char *Name() const
		{ return m_name; }

	// Get the volume and pitch
	float Volume() const
		{ return m_volume; }
	float Pitch() const
		{ return m_pitch; }

	////////////////////////////////////////
	// A category is a collection of phrases
	////////////////////////////////////////
	class Category
	{
	public:
		Category();
		~Category();

		bool Setup(const char *name, int index, bool stream, SayPriority priority, int phrases);

		const char *Name() const;
		int Index() const;
		bool IsStream() const;
		SayPriority GetPriority() const;
		int Phrases();
		bool AddPhrase(const char *phrase);
		const char *GetPhrase(int index) const;

	private:
		const char 				*m_name;
		int						m_index;
		bool						m_stream;
		SayPriority				m_priority;
		struct Phrase
		{
			const char *name;
		};
		Phrase					*m_phrase;
		int
		m_phrases, m_maxPhrases;
	};

	// Add a new category (returns < 0) if invalid
	int AddCategory(const char *category, bool isStream, int phrases, SayPriority priority = SAYPRIO_NORMAL);

	// Find a category (returns -1 if not found)
	int FindCategory(const char *category) const;

	// Get the category
	Category *GetCategory(int category) const;

	// Get the category count
	int Categories() const
		{ return	m_categories; }

	// Add a phrase to a category
	bool AddPhrase(int category, const char *phrase);

	// How many phrases are in a category?
	int Phrases(const char *category) const
		{ return Phrases(FindCategory(category)); }
	int Phrases(int category) const;

private:
	// General settings
	char								m_name[8];
	float								m_volume;
	float								m_pitch;

	// Category info
	Category							*m_category;
	int								m_categories, m_maxCategories;

	// Utilities
	static SayPriority PriorityFromString(const char *priority);
};

class SayVoiceSetLibrary
{
public:
	SayVoiceSetLibrary();
	~SayVoiceSetLibrary();

	bool Initialize();
	void Terminate();

	SayVoiceSet *FindVoiceSet(const char *voiceSet);

	// A couple of public pools for the SayVoiceSet
	StringPool						m_categoryPool;
	StringPool						m_sfxPool;

private:
	DynList<SayVoiceSet>			m_voiceSet;
};

DefineSingleton(SayVoiceSetLibrary)
#define g_sayLib GetSingleton(SayVoiceSetLibrary)

// The inlines
#include "components/saylib.hpp"

#endif
