/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "enginehelper/sfparse.h"
#include "components/saylib.h"

DeclareSingleton(SayVoiceSetLibrary);

#ifndef CDROM
static int g_voiceSets = 0;
static int g_categories = 0;
static int g_phrases = 0;
#endif

/////////////////////////////////////////////////////////////////////////////
// Create the entire category/phrase set from a .var
/////////////////////////////////////////////////////////////////////////////
SayVoiceSet *SayVoiceSet::Load(
const char *name,
HOGFILE hf)
{
	// Get the memory pointer
	int size;
	void *data = Hog::FilePtr(hf, size);
	ASSERT(data && size > 0);
	if (!data || size <= 0)
		return NULL;

	// No voiceset yet
	SayVoiceSet *voiceSet = NULL;// = new SayVoiceSet(name, categories, volume, pitch);

	// Pick it apart
	SimpleFileParser parser((char *)data, size);

	// Assume default volume and pitch
	float volume = 1.0f;
	float pitch = 0.0f;

	// The current categories
	char category[32];
	category[0] = '\0';
	int categories = parser.CountSections();

	// Current priority
	SayPriority priority = SAYPRIO_NORMAL;
	bool isStream = true;

	// Current phrases
	const int maxPhrases = 64;
	char phrase[maxPhrases][16];
	int phrases = 0;

	while (true)
	{
		SimpleFileParser::TokenType tokenType;
		char token[32];
		const char *restOfLine = parser.GetNextToken(tokenType, token, sizeof(token) - 1);

		// Are we starting a category?	
		bool doneFile = (tokenType == SimpleFileParser::TOKEN_EOF);
		if (doneFile || tokenType == SimpleFileParser::TOKEN_SECTION)
		{
			// Have we even created the voice set?
			if (category[0] == '\0')
			{
				if (!voiceSet && !doneFile)
				{
					voiceSet = new SayVoiceSet(name, categories, volume, pitch);
					ASSERT(voiceSet);
					if (!voiceSet)
						break;
				}
			}
			else
			{
				// Duh!
				ASSERT(phrases > 0);

				// Create the category
				const int catIndex = voiceSet->AddCategory(category, isStream, phrases, priority);
				ASSERT(catIndex >= 0);

				// Add the phrases
				for (int p = 0; p < phrases; p++)
				{
					bool ret = voiceSet->AddPhrase(catIndex, phrase[p]);
					ASSERT(ret);
				}
			}

			// Setup the new category
			strcpy(category, token);
			priority = SAYPRIO_NORMAL;
			isStream = true;
			phrases = 0;

			// Are we done?
			if (doneFile)
				break;
		}

		// Handle the token
		switch (tokenType)
		{
			case SimpleFileParser::TOKEN_EQUATE:
				if (strcmpi(token, "volume") == 0)
				{
					ASSERT(category[0] == '\0');
					char value[32];
					SimpleFileParser::ParseRestOfLine(restOfLine, value, sizeof(value) - 1);
					sscanf(value, "%f", &volume);
				}
				else if (strcmpi(token, "pitch") == 0)
				{
					ASSERT(category[0] == '\0');
					char value[32];
					SimpleFileParser::ParseRestOfLine(restOfLine, value, sizeof(value) - 1);
					sscanf(value, "%f", &pitch);
				}
				else if (strcmpi(token, "prio") == 0)
				{
					ASSERT(category[0] != '\0');
					char value[32];
					SimpleFileParser::ParseRestOfLine(restOfLine, value, sizeof(value) - 1);
					priority = PriorityFromString(value);
				}
				else if (strcmpi(token, "phrase") == 0)
				{
					ASSERT(category[0] != '\0');
					// Pick off the phrases
					while (restOfLine && *restOfLine)
					{
						char value[32];
						restOfLine = SimpleFileParser::ParseRestOfLine(restOfLine, value, sizeof(value) - 1, ',');
						if (*value)
						{
							ASSERT(phrases < maxPhrases);
							if (phrases < maxPhrases)
								strcpy(phrase[phrases++], value);	
						}
					}
				}
				break;

			case SimpleFileParser::TOKEN_SINGLE:
				// Single tokens
				if (strcmpi(token, "stream") == 0)
				{
					ASSERT(category[0] != '\0');
					isStream = true;
				}
				else if (strcmpi(token, "sfx") == 0)
				{
					ASSERT(category[0] != '\0');
					isStream = false;
				}
				break;
		}
	}

	// Here's johnny!
	return voiceSet;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SayVoiceSet::DumpToNewFile()
{
#ifndef _DEBUG
	return false;
#else
	char outputFile[64];
	sprintf(outputFile, "say\\%s.say", m_name);

	FileStream fs;
	if (!fs.Open(outputFile, FileStream::WRITE, true))
		return false;

	fs.Print("# Generated say file for %s\r\n\r\n", m_name);

	// Wtite the basic stuff
	fs.Print("volume=%.1f\r\n", m_volume);
	fs.Print("pitch=%.1f\r\n\r\n", m_pitch);

	for (int c = 0; c < m_maxCategories; c++)
	{
		if (m_category[c].Phrases() > 0)
		{
			if (strcmpi(m_category[c].Name(), "fsp") == 0)
				fs.Print("# Category first see player\r\n");
			else if (strcmpi(m_category[c].Name(), "cl") == 0)
				fs.Print("# Category contact lost\r\n");
			else if (strcmpi(m_category[c].Name(), "pi") == 0)
				fs.Print("# Category precontact idle\r\n");
			else if (strcmpi(m_category[c].Name(), "bt") == 0)
				fs.Print("# Category battle talk\r\n");
			else if (strcmpi(m_category[c].Name(), "kp") == 0)
				fs.Print("# Category killed player\r\n");
			else if (strcmpi(m_category[c].Name(), "df") == 0)
				fs.Print("# Category deafened\r\n");
			else if (strcmpi(m_category[c].Name(), "bld") == 0)
				fs.Print("# Category blinded\r\n");
			else if (strcmpi(m_category[c].Name(), "pld") == 0)
				fs.Print("# Category pleed\r\n");
			else if (strcmpi(m_category[c].Name(), "fire") == 0)
				fs.Print("# Category on fire\r\n");
			else if (strcmpi(m_category[c].Name(), "ht") == 0)
				fs.Print("# Category hostage taken\r\n");
			else if (strcmpi(m_category[c].Name(), "hs") == 0)
				fs.Print("# Category hostage saved\r\n");
			else if (strcmpi(m_category[c].Name(), "hr") == 0)
				fs.Print("# Category hostage released\r\n");
			else if (strcmpi(m_category[c].Name(), "hk") == 0)
				fs.Print("# Category hostage killed\r\n");
			else if (strcmpi(m_category[c].Name(), "sht") == 0)
				fs.Print("# Category shot\r\n");
			else if (strcmpi(m_category[c].Name(), "ap") == 0)
				fs.Print("# Category apprehension\r\n");
			else if (strcmpi(m_category[c].Name(), "hu") == 0)
				fs.Print("# Category hurt\r\n");
			else if (strcmpi(m_category[c].Name(), "dg") == 0)
				fs.Print("# Category death\r\n");
			else if (strcmpi(m_category[c].Name(), "de") == 0)
				fs.Print("# Category death sililoquy\r\n");
			else if (strcmpi(m_category[c].Name(), "decap") == 0)
				fs.Print("# Category death decap\r\n");
			else if (strcmpi(m_category[c].Name(), "md") == 0)
				fs.Print("# Category death melee\r\n");
			else if (strcmpi(m_category[c].Name(), "ef") == 0)
				fs.Print("# Category sfx effort\r\n");
			else if (strcmpi(m_category[c].Name(), "re") == 0)
				fs.Print("# Category sfx hurt\r\n");
			else if (strcmpi(m_category[c].Name(), "mr") == 0)
				fs.Print("# Category sfx melee hurt\r\n");
			else if (strcmpi(m_category[c].Name(), "bd") == 0)
				fs.Print("# Category buddy died\r\n");
			else if (strcmpi(m_category[c].Name(), "tnt") == 0)
				fs.Print("# Category taunt\r\n");
			else
				fs.Print("# Unknown category!!!!!\r\n");

			fs.Print("[%s]\r\n", m_category[c].Name());
			if (m_category[c].IsStream())
				fs.Print("stream\r\n");
			else
				fs.Print("sfx\r\n");
			switch (m_category[c].GetPriority())
			{
				case SAYPRIO_LOW:
					fs.Print("prio=low");
					break;
				case SAYPRIO_HIGH:
					fs.Print("prio=high");
					break;
				case SAYPRIO_VERYHIGH:
					fs.Print("prio=veryhigh");
					break;
				default:
					fs.Print("prio=normal");
					break;
			}
			fs.Print("\r\n\r\n");

			// Do the phrases
			fs.Print("phrase=");
			int foundPhrases = 0;
			for (int p = 0; p < m_category[c].Phrases(); p++)
			{
				const char *phrase = m_category[c].GetPhrase(p);
				if (phrase)
				{
					if (foundPhrases++ > 0)
						fs.Print(",");
					fs.Print(phrase);
				}
			}
			fs.Print("\r\n\r\n");
		}
	}

	fs.Print("# eof\r\n");
	fs.Close();
	return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
SayVoiceSet::SayVoiceSet(
const char *name,
int categories,
float volume,
float pitch) :
m_volume(volume),
m_pitch(pitch)
{
	ASSERT(name && strlen(name) < sizeof(m_name));
	strcpy(m_name, name);

	// create the array
	m_category = new Category[categories];
	ASSERT(m_category);
	m_maxCategories = m_category ? categories : 0;
	m_categories = 0;

#ifndef CDROM
	++g_voiceSets;
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayVoiceSet::~SayVoiceSet()
{
	delete [] m_category;
}

/////////////////////////////////////////////////////////////////////////////
// Add a new category
/////////////////////////////////////////////////////////////////////////////
int SayVoiceSet::AddCategory(
const char *category,
bool isStream,
int phrases,
SayPriority priority)
{
	ASSERT(category && category[0]);

	// Will we fit?
	ASSERT(m_categories < m_maxCategories);
	if (m_categories == m_maxCategories)
		return false;

	ASSERT(phrases > 0);
	if (!m_category[m_categories].Setup(category, m_categories, isStream, priority, phrases))
		return -1;

#ifndef CDROM
	++g_categories;
#endif

	return m_categories++;
}

/////////////////////////////////////////////////////////////////////////////
// Find a category (returns -1 if not found)
/////////////////////////////////////////////////////////////////////////////
int SayVoiceSet::FindCategory(
const char *category) const
{
	ASSERT(category && *category);

	for (int c = 0; c < m_categories; c++)
	{
		if (strcmpi(category, m_category[c].Name()) == 0)
			return c;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayVoiceSet::Category *SayVoiceSet::GetCategory(int category) const
{
	// Make sure this is valid
	ASSERT(category < m_categories);
	if (category < 0 || category >= m_categories)
		return NULL;

	// Get our category
	return &m_category[category];
}

/////////////////////////////////////////////////////////////////////////////
// Add a phrase to a category
/////////////////////////////////////////////////////////////////////////////
bool SayVoiceSet::AddPhrase(
int category,
const char *phrase)
{
	// Get our category
	Category *cat = GetCategory(category);
	if (!cat)
		return false;

#if defined(FOOD) && defined(PS2) && defined(_DEBUG)
	bool found = !cat->IsStream() || g_audio.StreamExists(phrase);
	ASSERT(found);
	if (!found)
	{
		TRACE("Could not find phrase stream \"%s\"\n", phrase);
	}
#endif

#ifndef CDROM
	++g_phrases;
#endif

	// Add it
	return cat->AddPhrase(phrase);
}

/////////////////////////////////////////////////////////////////////////////
// How many phrases are in a category?
/////////////////////////////////////////////////////////////////////////////
int SayVoiceSet::Phrases(int category) const
{
	// Get our category
	Category *cat = GetCategory(category);
	if (!cat)
		return 0;

	return cat->Phrases();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayPriority SayVoiceSet::PriorityFromString(
const char *priority)
{
	if (strcmpi(priority, "low") == 0)
		return SAYPRIO_LOW;
	else if (strcmpi(priority, "normal") == 0)
		return SAYPRIO_NORMAL;
	else if (strcmpi(priority, "high") == 0)
		return SAYPRIO_HIGH;
	else if (strcmpi(priority, "veryhigh") == 0)
		return SAYPRIO_VERYHIGH;
	else
		return SAYPRIO_NORMAL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayVoiceSetLibrary::SayVoiceSetLibrary() :
m_voiceSet(8, 4, true),
m_categoryPool(32, 4, 100, 100),
m_sfxPool(16, 32, 200, 200)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayVoiceSetLibrary::~SayVoiceSetLibrary()
{
	Terminate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SayVoiceSetLibrary::Initialize()
{
#ifndef CDROM
	int freeBytes, freeHandles, largestFree, allocBytes, allocHandles;
	Layers__GetMemoryStats(&freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles);
	int startBytes = allocBytes;
	int startHandles = allocHandles;
#endif

	// Load the hog into memory
	Hog sayHog;
	if (!sayHog.Mount("say.hog", true, true))
	{
		// Mount the hog!
		ASSERT(false);
		return false;
	}

	// For each file, create a voice set
	for (int f = 0; f < sayHog.Files(); f++)
	{
		// Open the file
		const char *hogFile = sayHog.FileName(f);
		HOGFILE hf = sayHog.FileOpen(hogFile, true);
		ASSERT(hf);
		if (!hf)
			continue;

		// Pick off the name
		char name[32];
		int len = 0;
		while (hogFile[len] && hogFile[len] != '.')
		{
			name[len] = hogFile[len];
			if (++len == sizeof(name) - 1)
				break;
		}
		name[len] = '\0';
		strlwr(name);

		// Create the voiceset
		SayVoiceSet *voiceSet = SayVoiceSet::Load(name, hf);
		ASSERT(voiceSet);

		sayHog.FileClose(hf);

		// Add it the list
		if (voiceSet)
		{
 			m_voiceSet.Add(voiceSet);
#ifdef DUMPOUTSAY
			bool ret = voiceSet->DumpToNewFile();
			ASSERT(ret);
#endif
		}
	}

	// Done with this bad boy
	sayHog.Unmount();

#ifndef CDROM
	Layers__GetMemoryStats(&freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles);
	int finalBytes = allocBytes - startBytes;
	int finalHandles = allocHandles - startHandles;

	TRACE("Voice sets used %dk and %d handles ", finalBytes / 1024, finalHandles);
	TRACE("for %d voice sets, %d categories, %d phrases\n", g_voiceSets, g_categories, g_phrases);
	//m_categoryPool.DebugDump("Category");
	//m_sfxPool.DebugDump("Sfx");
#endif

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SayVoiceSetLibrary::Terminate()
{
	m_voiceSet.Clear();
	m_categoryPool.Clear();
	m_sfxPool.Clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SayVoiceSet *SayVoiceSetLibrary::FindVoiceSet(const char *voiceSet)
{
	int count = m_voiceSet.Count();
	for (int i = 0; i < count; i++)
	{
		if (strcmpi(m_voiceSet[i]->Name(), voiceSet) == 0)
			return m_voiceSet[i];
	}

	ASSERT(false);
	return NULL;
}

