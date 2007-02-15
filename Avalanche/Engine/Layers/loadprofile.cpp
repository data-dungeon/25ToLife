/////////////////////////////////////////////////////////////////////////////
// Simple class to track the loaded data
/////////////////////////////////////////////////////////////////////////////
#include "Layers/LayersPCH.h"

#include "Layers/loadprofile.h"
#include "container/dllist.h"
#include "Game/Database/DBFile.h"

extern u32 u32Milliseconds;

#ifdef WANT_LOADPROFILE

namespace LoadProfile
{
// Are we in the permanent phase?
bool s_inPermanent = true;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int FreeSystemMemory()
{
#if defined(_XBOX)
	MEMORYSTATUS stat;
	GlobalMemoryStatus(&stat);
	return (int)stat.dwAvailPhys;
#elif defined(GCN)
	return = (int)((u32)OSGetArenaHi() - (u32)OSGetArenaLo());
#else //PS2, PC
	return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int FreeMemory()
{
	int freeBytes, freeHandles, largestFree, allocBytes, allocHandles;
	Layers__GetMemoryStats(&freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles);
	//Layers__GetMemoryHighWaterStats(NULL, &maxBytes);
	return freeBytes + FreeSystemMemory();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int UsedMemory()
{
	int freeBytes, freeHandles, largestFree, allocBytes, allocHandles;
	Layers__GetMemoryStats(&freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles);
	return allocBytes;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uint Time()
{
	return u32Milliseconds;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uint BytesRead()
{
	return (uint)Media::GetReadTotal();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class Stats
{
public:
	Stats()
		{ Clear(); }

	void Clear()
		{ m_loadFiles = 0; m_loadBytes = 0; m_mallocBytes = 0; m_time = 0; }

	Stats &operator=(const Stats &s)
	{
		m_loadFiles = s.m_loadFiles;
		m_loadBytes = s.m_loadBytes;
		m_mallocBytes = s.m_mallocBytes;
		m_time = s.m_time;
		return *this;
	}

	Stats &operator+=(const Stats &s)
	{
		m_loadFiles += s.m_loadFiles;
		m_loadBytes += s.m_loadBytes;
		m_mallocBytes += s.m_mallocBytes;
		m_time += s.m_time;
		return *this;
	}

	int operator<(const Stats &s)
		{ return m_mallocBytes < s.m_mallocBytes; }
	int operator>(const Stats &s)
		{ return m_mallocBytes > s.m_mallocBytes; }

	// These are just public
	int		m_loadFiles;
	uint 		m_loadBytes;
	uint		m_mallocBytes;
	uint		m_time;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class Subcategory
{
public:
	// Must give us a name
	Subcategory(const char *name)
	{
		ASSERT(strlen(name) < sizeof(m_name));
		strcpy(m_name, name);
		m_chunkSize = 0;
		m_selectMemory = 0;
		m_selectLoadBytes = 0;
		m_selectTime = 0;
		m_selected = false;
	}

	// What is our name?
	const char *Name() const
		{ return m_name; }

	// Select & deselect
	void Select()
	{
		if (!m_selected)
		{
			m_selectTime = Time();
			m_selectLoadBytes = BytesRead();
			m_selectMemory = FreeMemory();
			m_selected = true;
		}
	}
	void Deselect()
	{
		if (m_selected)
		{
			m_stats.m_mallocBytes += m_selectMemory - FreeMemory();
			m_stats.m_loadBytes += BytesRead() - m_selectLoadBytes;
			m_stats.m_time += Time() - m_selectTime;
			m_selected = false;
		}
	}

	// Update stats
	void AddFile()
		{ m_stats.m_loadFiles++; }

	// Chunk size
	void AddChunkSize(int size)
		{ m_chunkSize += size; }
	int ChunkSize()
		{ return m_chunkSize; }

	// Clear
	void Clear()
		{ m_stats.Clear(); m_chunkSize = 0; m_selected = false; }

	const Stats &GetStats() const
		{ return m_stats; }

	int operator<(const Subcategory &c)
		{ return m_chunkSize < c.m_chunkSize; }
	int operator>(const Subcategory &c)
		{ return m_chunkSize > c.m_chunkSize; }

private:
	char 			m_name[32];
	Stats			m_stats;
	int			m_chunkSize;

	// Selection stuff
	bool			m_selected;
	uint			m_selectTime;
	uint			m_selectLoadBytes;
	uint			m_selectMemory;

public:
	// We appear in a list
	Subcategory	*prev, *next;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class Category
{
public:
	Category(const char *name)
	{
		ASSERT(strlen(name) < sizeof(m_name));
		strcpy(m_name, name);
		Add("Untagged");
		m_selected = NULL;
	}

	// Get at our name
	const char *Name() const
		{ return m_name; }

	// Find a subcategory (will add if it doesn't find)
	Subcategory *Find(const char *subcategory, bool add = true)
	{
		const char *name = subcategory ? subcategory : "Untagged";
		Subcategory *sub = m_subcategory.Head();
		while (sub)
		{
			if (strcmpi(sub->Name(), name) == 0)
				return sub;
			sub = sub->next;
		}

		return add ? Add(subcategory) : NULL;
	}

	// Enumerate subcategories
	Subcategory *FindFirst()
		{ return m_subcategory.Head(); }
	Subcategory *FindNext(Subcategory *current)
		{ return current ? current->next : NULL; }

	// Play with the stats
	void Select(Subcategory *subcategory)
	{
		if (subcategory != m_selected)
		{
			if (m_selected)
				m_selected->Deselect();
			m_selected = subcategory;
			if (m_selected)
				m_selected->Select();
		}
	}
	void Done()
		{ Select(NULL); }

	// add a subcategory
	Subcategory *Add(const char *subcategory)
	{
		Subcategory *sub = new Subcategory(subcategory);
		m_subcategory.AddTail(sub);
		return sub;
	}

	void AddFile()
	{
		ASSERT(m_selected);
		m_selected->AddFile();
	}

	void Reset()
	{
		Subcategory *sub = m_subcategory.Head();
		while (sub)
		{
			sub->Clear();
			sub = sub->next;
		}
	}

	Stats Total()
	{
		Stats total;
		Subcategory *sub = m_subcategory.Head();
		while (sub)
		{
			total += sub->GetStats();
			sub = sub->next;
		}

		return total;
	}

	void Sort()
		{ m_subcategory.BubbleSortDescending(); }

	int operator<(Category &c)
		{ Stats a = Total(); Stats b = c.Total(); return a < b; }
	int operator>(Category &c)
		{ Stats a = Total(); Stats b = c.Total(); return a > b; }

private:
	char										m_name[32];
	DoublyLinkedList<Subcategory>		m_subcategory;
	Subcategory								*m_selected;

public:
	// We appear in a list
	Category		*prev, *next;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class PathCategory
{
public:
	PathCategory(const char *path, const char *category)
	{
		ASSERT(strlen(path) < sizeof(m_path));
		strcpy(m_path, path);
		Clean(m_path);
		ASSERT(strlen(category) < sizeof(m_category));
		strcpy(m_category, category);
	}

	const char *GetCategory()
		{ return m_category; }

	bool Match(const char *pathFile)
	{
		char clean[128];
		strncpy(clean, pathFile, sizeof(clean) - 1);
		clean[sizeof(clean) - 1] = '\0';
		Clean(clean);

		return (strstr(clean, m_path) != 0);
	}

private:
	char			m_path[64];
	char			m_category[32];

	// Clean a path
	void Clean(char *path)
	{
		int len = strlen(path);
		for (int i = 0; i < len; i++)
		{
			path[i] = tolower(path[i]);
			if (path[i] == '/')
				path[i] = '\\';
		}
	}

public:
	// We appear in a list
	PathCategory	*prev, *next;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class ExtSubcategory
{
public:
	ExtSubcategory(const char *ext, const char *subcategory)
	{
		ASSERT(strlen(ext) < sizeof(m_ext));
		strcpy(m_ext, ext);
		ASSERT(strlen(subcategory) < sizeof(m_subcategory));
		strcpy(m_subcategory, subcategory);
	}

	const char *GetSubcategory()
		{ return m_subcategory; }

	bool Match(const char *pathFile)
	{
		if (strlen(pathFile) < strlen(m_ext))
			return false;
		else
			return (strcmp(m_ext, &pathFile[strlen(pathFile) - strlen(m_ext)]) == 0);
	}

private:
	char			m_ext[16];
	char			m_subcategory[32];

public:
	// We appear in a list
	ExtSubcategory *prev, *next;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class ChunkSubcategory
{
public:
	ChunkSubcategory(uint chunkKey, const char *subcategory)
	{
		m_chunkKey = chunkKey;
		ASSERT(strlen(subcategory) < sizeof(m_subcategory));
		strcpy(m_subcategory, subcategory);
	}

	const char *GetSubcategory()
		{ return m_subcategory; }

	bool Match(uint chunkKey)
		{ return m_chunkKey == chunkKey; }

private:
	uint			m_chunkKey;
	char			m_subcategory[32];

public:
	// We appear in a list
	ChunkSubcategory *prev, *next;
};

/////////////////////////////////////////////////////////////////////////////
// Our data
/////////////////////////////////////////////////////////////////////////////
bool s_initialized = false;
bool s_active = false;

DoublyLinkedList<Category> s_permanentCategory;
DoublyLinkedList<Category> s_dynamicCategory;

DoublyLinkedList<PathCategory> s_pathCategory;
DoublyLinkedList<ExtSubcategory> s_extSubcategory;
DoublyLinkedList<ChunkSubcategory> s_chunkSubcategory;

// Memory stats
int s_memoryAtInit = 0;
int s_memoryAtFirstPermanent = 0;
int s_memoryAtPermanent = 0;
int s_memoryAtDone = 0;
uint s_initTime = 0;
uint s_donePermanentTime = 0;
uint s_doneTime = 0;

// What is our currently selected category
Category *s_selectedCategory = NULL;

/////////////////////////////////////////////////////////////////////////////
// Some utilities
/////////////////////////////////////////////////////////////////////////////
Category *AddCategory(const char *name)
{
	Category *cat = new Category(name);
	if (s_inPermanent)
		s_permanentCategory.AddTail(cat);
	else
		s_dynamicCategory.AddTail(cat);

	return cat;
}

/////////////////////////////////////////////////////////////////////////////
// Find the category, will add it if it doesnt' exist
/////////////////////////////////////////////////////////////////////////////
Category *FindCategory(const char *name, bool add = true)
{
	Category *cat = s_inPermanent ? s_permanentCategory.Head() : s_dynamicCategory.Head();
	while (cat)
	{
		if (strcmpi(cat->Name(), name) == 0)
			return cat;
		cat = cat->next;
	}

	return add ? AddCategory(name) : NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SelectCategory(Category *category)
{
	if (s_selectedCategory == category)
		return;

	ASSERT(category);

	// Force this to deselect it's sub
	if (s_selectedCategory)
		s_selectedCategory->Select(NULL);

	s_selectedCategory = category;

	// Select the root
	s_selectedCategory->Select(NULL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SelectCategoryByName(const char *categoryName)
{
	SelectCategory(FindCategory(categoryName ? categoryName : "Untagged", true));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *FindPathCategoryName(const char *pathFile)
{
	// Is this a path we care about?
	PathCategory *pathCategory = s_pathCategory.Head();
	while (pathCategory)
	{
		// Is this the one?
		if (pathCategory->Match(pathFile))
			return pathCategory->GetCategory();

		// Next
		pathCategory = pathCategory->next;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *FindExtSubcategoryName(
const char *pathFile)
{
	// Is this a subcategory?
	ExtSubcategory *extSubcategory = s_extSubcategory.Head();
	while (extSubcategory)
	{
		// Is this the one?
		if (extSubcategory->Match(pathFile))
			return extSubcategory->GetSubcategory();

		// Next
		extSubcategory = extSubcategory->next;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *FindChunkSubcategoryName(
uint chunkKey)
{
	// Is this a path we care about?
	ChunkSubcategory *chunkSubcategory = s_chunkSubcategory.Head();
	while (chunkSubcategory)
	{
		// Is this the one?
		if (chunkSubcategory->Match(chunkKey))
			return chunkSubcategory->GetSubcategory();

		// Next
		chunkSubcategory = chunkSubcategory->next;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize everything
/////////////////////////////////////////////////////////////////////////////
void Start()
{
	if (!s_initialized)
	{
		// Add the main categories
		AddPathCategory("active\\", "Active");
		AddPathCategory("avatars\\", "Avatar");
		AddPathCategory("Blood\\", "SpecialFX");
		AddPathCategory("flares\\", "SpecialFX");
		AddPathCategory("fonts\\", "Fonts");
		AddPathCategory("footstep\\", "SpecialFX");
		AddPathCategory("levels\\", "Level");
		AddPathCategory("npc\\", "NPC");
		AddPathCategory("objects\\", "Objects");
		AddPathCategory("pain\\", "SpecialFX");
		AddPathCategory("particles\\", "Particles");
		AddPathCategory("Screens\\", "Screens");
		AddPathCategory("shadow\\", "SpecialFX");
		AddPathCategory("shatter\\", "SpecialFX");
		AddPathCategory("shell\\", "Shell");
		AddPathCategory("sound\\", "Sound");
		AddPathCategory("Tag\\", "SpecialFX");
		AddPathCategory("var\\", "Var");
		AddPathCategory("weapons\\", "Weapons");
		AddPathCategory("scripts\\", "Script");
		AddPathCategory("tables\\", "Tables");
		AddPathCategory("var\\", "Var");
		AddPathCategory("xml\\", "XML");

		// Do the subcategories
		AddExtSubcategory("dbl", "DBL");
		AddExtSubcategory("dba", "Bank");
		AddExtSubcategory("mdb", "MotionDB");
		AddExtSubcategory("var", "Var");
		AddExtSubcategory("ap", "AP");
		AddExtSubcategory("txt", "Text");
		AddExtSubcategory("tbd", "Surface");
		AddExtSubcategory("tbl", "Surface");
		AddExtSubcategory("sx", "sx");
		AddExtSubcategory("xml", "xml");
		AddExtSubcategory("fnt", "Font");
		AddExtSubcategory("sc", "7up");

		AddChunkSubcategory(DB_SNGLSOM, "Mesh");
		AddChunkSubcategory(DB_SNGLSOMBONE, "Mesh");
		AddChunkSubcategory(DB_SOMARRAY, "Mesh");
		AddChunkSubcategory(DB_SOMARRAYBONE, "Mesh");
		AddChunkSubcategory(DB_COLLISIONSOMARRAY, "Collision");
		AddChunkSubcategory(DB_AISCRIPT, "Script");
		AddChunkSubcategory(DB_ANIMSCRIPT, "Script");
		AddChunkSubcategory(DB_DMASNGLSOM, "Mesh");
		AddChunkSubcategory(DB_DMASOMARRAY, "Mesh");
		AddChunkSubcategory(DB_SOMTEXTUREIDARRAY, "Texture");
		AddChunkSubcategory(DB_INSTANCE, "Instance");
		AddChunkSubcategory(DB_GROUP, "Group");
		AddChunkSubcategory(DB_LOD, "LOD");
		AddChunkSubcategory(DB_PARTICLE_EMITTER, "Particles");
		AddChunkSubcategory(DB_PARTICLE_SOURCE, "Particles");
		AddChunkSubcategory(DB_PARTICLE_EFFECT, "Particles");
		AddChunkSubcategory(DB_TEXTURE_CONTEXT, "Texture");
		AddChunkSubcategory(DB_SCENE_TREE, "Scene");
		AddChunkSubcategory(DB_VIS_REGIONS, "Scene");
		AddChunkSubcategory(DB_SWOOSH_SOURCE, "SpecialFX");
		AddChunkSubcategory(DB_INSTANCE_GROUP, "Instance");
		AddChunkSubcategory(DB_SWOOSH_EMITTER, "SpecialFX");
		AddChunkSubcategory(DB_PALETTE_SET, "Texture");
		AddChunkSubcategory(DB_TEXTURE_SET, "Texture");
		AddChunkSubcategory(DB_TERRAIN_LIST, "Terrain");
		AddChunkSubcategory(DB_EMITTER_LIST, "Particle");
		AddChunkSubcategory(DB_EFFECT_LIST, "Particle");
		AddChunkSubcategory(DB_MERGED_TEXTURE_SET, "Texture");
		AddChunkSubcategory(DB_ANIMATED_TEXTURE_SET, "Texture");
		AddChunkSubcategory(DB_PARTICLE_LIST, "Particle");
		AddChunkSubcategory(DB_PARTICLE_SURFACE_LIST, "Particle");
		AddChunkSubcategory(DB_SWOOSH_LIST, "SpecialFX");
		AddChunkSubcategory(DB_SWOOSH_EMITTER_LIST, "SpecialFX");
		AddChunkSubcategory(DB_WORLD_EMITTERS, "Particle");
		AddChunkSubcategory(DB_WORLD_DISTORTIONS, "SpecialFX");
		AddChunkSubcategory(DB_ANIM_CURVE, "Animation");
		AddChunkSubcategory(DB_MOTION, "Animation");
		AddChunkSubcategory(DB_BONEINFO, "Animation");
		AddChunkSubcategory(DB_MOTION_NAMES, "Animation");
		AddChunkSubcategory(DB_BONE_GROUP_MIRRORS, "Animation");
		AddChunkSubcategory(DB_TEXTURE_NAMES, "Texture");
		AddChunkSubcategory(DB_LIGHTS, "Lights");
		AddChunkSubcategory(DB_NAMED_POINTS, "Named Points");
		AddChunkSubcategory(DB_SECTORS, "Sectors");
		AddChunkSubcategory(DB_PATHS, "Path");
		AddChunkSubcategory(DB_INSTANCE_ATTRIBUTES, "Instance");
		AddChunkSubcategory(DB_CURVES, "Path");
		AddChunkSubcategory(DB_CDB, "Collision");
		AddChunkSubcategory(DB_ODB, "Collision");
		AddChunkSubcategory(DB_PDB, "Collision");
		AddChunkSubcategory(DB_CPH, "Collision");
		AddChunkSubcategory(DB_NAV, "Nav");
		AddChunkSubcategory(DB_SOUNDBANK_3D, "Sound");
		AddChunkSubcategory(DB_SOUNDBANK_2D, "Sound");
		AddChunkSubcategory(DB_SOUNDBANK_MOD, "Sound");
		AddChunkSubcategory(DB_XML_SOURCE, "XML");
		AddChunkSubcategory(DB_OA_SOURCE, "Env");
		AddChunkSubcategory(DB_ENV_BINARY, "Env");
		AddChunkSubcategory(DB_CUT_BINARY, "Cutscene");
		AddChunkSubcategory(DB_SOUNDSOURCE, "Sound");
		AddChunkSubcategory(DB_SOUNDFIELD, "Sound");
		AddChunkSubcategory(DB_WATER, "SpecialFX");
		AddChunkSubcategory(DB_GLASS, "SpecialFX");
		AddChunkSubcategory(DB_HALO, "SpecialFX");
		AddChunkSubcategory(DB_BLOOM, "SpecialFX");

		// We are good to go
		s_initialized = true;
	}

	// Set everything up
	Reset(true);
}

/////////////////////////////////////////////////////////////////////////////
// Set the permanent state
/////////////////////////////////////////////////////////////////////////////
void DonePermanent()
{
	// We are already done!
	if (!s_inPermanent)
		return;

	// Set the new state
	s_inPermanent = false;
	s_memoryAtPermanent = FreeMemory();
	if (s_memoryAtFirstPermanent == 0)
		s_memoryAtFirstPermanent = s_memoryAtPermanent;

	s_donePermanentTime = Time();

	// Close them out
	Category *category = s_permanentCategory.Head();
	while (category)
	{
		category->Done();
		category = category->next;
	}

	// Get out of the permanent categories
	SelectCategoryByName(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Are we done with the profile
/////////////////////////////////////////////////////////////////////////////
void Done()
{
	// We are done
	s_active = false;

	// Get the free memory
	s_memoryAtDone = FreeMemory();
	s_doneTime = Time();

	// Tell everyone
	// Close them out
	Category *category = s_dynamicCategory.Head();
	while (category)
	{
		category->Done();
		category = category->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Setup a path to "infer" the category from
// do these in specific to general order, the first match
// is the match....
/////////////////////////////////////////////////////////////////////////////
void AddPathCategory(
const char *path,
const char *category)
{
	PathCategory *pathCategory = new PathCategory(path, category);
	ASSERT(pathCategory);
	s_pathCategory.AddTail(pathCategory);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AddExtSubcategory(
const char *ext,
const char *subcategory)
{
	ExtSubcategory *extSubcategory = new ExtSubcategory(ext, subcategory);
	ASSERT(extSubcategory);
	s_extSubcategory.AddTail(extSubcategory);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AddChunkSubcategory(
uint chunkKey,
const char *subcategory)
{
	ChunkSubcategory *chunkSubcategory = new ChunkSubcategory(chunkKey, subcategory);
	ASSERT(chunkSubcategory);
	s_chunkSubcategory.AddTail(chunkSubcategory);
}

/////////////////////////////////////////////////////////////////////////////
// Notify (internal)
/////////////////////////////////////////////////////////////////////////////
void NotifyFileLoad(
const char *pathFile)
{
	// Don't do any work, we are done
	if (!s_active)
		return;

	// Is this a path we care about?
	const char *pathCategoryName = FindPathCategoryName(pathFile);
	if (pathCategoryName)
		SelectCategoryByName(pathCategoryName);
	else
		SelectCategoryByName(pathFile);

	// Is this a subcategory?
	const char *subcategoryName = FindExtSubcategoryName(pathFile);
	s_selectedCategory->Select(s_selectedCategory->Find(subcategoryName));

	// One more file
	s_selectedCategory->AddFile();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void NotifyChunk(
uint chunkKey,
int size)
{
	// Don't do any work, we are done
	if (!s_active)
		return;

	// Is this a subcategory?
	const char *subcategoryName = FindChunkSubcategoryName(chunkKey);

	// Will be null for unkown name
	ASSERT(s_selectedCategory);
	Subcategory *subcategory = s_selectedCategory->Find(subcategoryName);
	if (subcategory)
		subcategory->AddChunkSize(size);
	s_selectedCategory->Select(subcategory);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *FormatSize(int size, char *buffer)
{
	int asize = Math::Abs(size);

	if (asize < 1024)
		sprintf(buffer, "%d b", size);
	else if (asize < 1024 * 1024)
		sprintf(buffer, "%d K", (size + 1023) / 1024);
	else
		sprintf(buffer, "%.2f MB", (float)size / (1024.0f * 1024.0f));

	return buffer;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DumpStats(const char *title, const Stats &stats, int chunkSize, int indent)
{
	// Does this have any useful information
	if (chunkSize == 0 && stats.m_loadBytes == 0 && stats.m_mallocBytes == 0)
		return;

	// Indent
	for (int i = 0; i < indent; i++)
		dbgPrint("   ");

	// Title
	if (chunkSize == 0)
		dbgPrint("%s", title);
	else
	{
		char chunk[64];
		FormatSize(chunkSize, chunk);
		dbgPrint("%s (%s)", title, chunk);
	}

	if (stats.m_loadFiles > 0)
		dbgPrint(" opened %d files", stats.m_loadFiles);

	if (stats.m_loadBytes > 0)
	{
		char loadSize[64];
		FormatSize(stats.m_loadBytes, loadSize);
		dbgPrint(" loaded %s", loadSize);
	}

	if (stats.m_mallocBytes != 0)
	{
		char mallocSize[64];
		FormatSize(stats.m_mallocBytes, mallocSize);
		dbgPrint(" used %s memory", mallocSize);
	}

	if (stats.m_time >= 100)
		dbgPrint(" in time %.2f s", (float)stats.m_time / 1000.0f);

	dbgPrint("\n");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DrawStats(const char *title, const Stats &stats, int chunkSize, int maxSize, bool indent)
{
	// Does this have any useful information
	if (chunkSize == 0 && stats.m_loadBytes == 0 && stats.m_mallocBytes == 0)
		return;

	// What size are we going to use?
	int size = indent ? chunkSize : stats.m_mallocBytes;
	char sizeStr[64];
	FormatSize(size, sizeStr);

	// Setup the bar
	char bar = (indent == 0) ? 'X' : '-';
	const int maxBar = indent ? 67 : 70;
	int barLength = (int)(((float)size / (float)maxSize) * (float)maxBar);
	if (barLength <= 0 || barLength > maxBar)
		return;

	// Dump the bar
	if (indent)
		dbgPrint("   ");
	char barTitle[64];
	sprintf(barTitle, "[%s]", title);
	dbgPrint(barTitle);
	for (int b = barLength - (int)strlen(barTitle); b > 0; b--)
		dbgPrint("%c", bar);
	dbgPrint(" %s (%d%%)\n", sizeStr, size * 100 / maxSize);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DrawHeaderBar(const char *title, int maxSize)
{
	// What size are we going to use?
	char sizeStr[64];
	FormatSize(maxSize, sizeStr);

	// Setup the bar
	char bar = 'X';
	const int maxBar = 70;

	// Dump the bar
	char barTitle[64];
	sprintf(barTitle, "[%s]", title);
	dbgPrint(barTitle);
	for (int b = 0; b < maxBar - (int)strlen(barTitle); b++)
		dbgPrint("%c", bar);
	dbgPrint(" %s (%d%%)\n", sizeStr, 100);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DumpCategory(
Category &category,
int totalMemory,
bool brief)
{
	Stats total = category.Total();
	DumpStats(category.Name(), total, 0, 0);

	if (!brief)
	{
		Subcategory *subcategory = category.FindFirst();
		while (subcategory)
		{
			DumpStats(subcategory->Name(), subcategory->GetStats(), subcategory->ChunkSize(), 1);
			subcategory = category.FindNext(subcategory);
		}

		dbgPrint("\n");
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DrawCategory(
Category &category,
int totalMemory,
bool brief)
{
	Stats total = category.Total();
	DrawStats(category.Name(), total, 0, s_memoryAtPermanent, false);
	int size = total.m_mallocBytes;

	if (!brief)
	{
		Subcategory *subcategory = category.FindFirst();
		while (subcategory)
		{
			DrawStats(subcategory->Name(), subcategory->GetStats(), subcategory->ChunkSize(), size, true);
			subcategory = category.FindNext(subcategory);
		}

		dbgPrint("\n");
	}
}

/////////////////////////////////////////////////////////////////////////////
// Dump out the results
/////////////////////////////////////////////////////////////////////////////
void Report(
const char *title,
bool showPermanent,
bool brief)
{
	// Show everything!
	dbgPrint("\n\n");
	if (title)
		dbgPrint("Load profile for %s\n\n", title);
	else
		dbgPrint("Load profile\n\n");

	// Basic stats
	dbgPrint("  Memory free at startup: %d K\n", (s_memoryAtInit + 1023) / 1024);
	dbgPrint("Memory free at permanent: %d K (%d K used)\n", (s_memoryAtPermanent + 1023) / 1024, (s_memoryAtInit - s_memoryAtPermanent + 1023) / 1024);
	if (s_memoryAtFirstPermanent != 0 && s_memoryAtFirstPermanent != s_memoryAtPermanent)
		dbgPrint("           Leaked memory: %d K\n", (s_memoryAtFirstPermanent - s_memoryAtPermanent + 1023) / 1024);
	dbgPrint("   Memory free when done: %d K (%d K used)\n", (s_memoryAtDone + 1023) / 1024, (s_memoryAtPermanent - s_memoryAtDone + 1023) / 1024);
	dbgPrint("     Permanent load time: %.1f s\n", (float)(s_donePermanentTime - s_initTime) / 1000.0f);
	dbgPrint("         Level load time: %.1f s\n", (float)(s_doneTime - s_donePermanentTime) / 1000.0f);
	dbgPrint("\n\n");

	// Permanent stats
	bool beBrief = false;
	if (showPermanent)
	{
		dbgPrint("Permanent section:\n");
		dbgPrint("---------------------------------------------------------------------\n\n");

		s_permanentCategory.BubbleSortDescending();
		Category *category = s_permanentCategory.Head();
		while (category)
		{
			category->Sort();
			DumpCategory(*category, s_memoryAtInit, beBrief);
			category = category->next;
			beBrief = brief;
		}
		dbgPrint("\n");
	}

	// Dynamic stats
	dbgPrint("Level section:\n");
	dbgPrint("---------------------------------------------------------------------\n\n");
	beBrief = false;
	s_dynamicCategory.BubbleSortDescending();
	Category *category = s_dynamicCategory.Head();
	while (category)
	{
		category->Sort();
		DumpCategory(*category, s_memoryAtInit, beBrief);
		category = category->next;
		beBrief = brief;
	}
	dbgPrint("\n");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Summary(
const char *title,
bool showPermanent)
{
	// Show everything!
	dbgPrint("\n\n");
	if (title)
		dbgPrint("Summary profile for %s\n\n", title);
	else
		dbgPrint("Summary profile\n\n");

	// Basic stats
	dbgPrint("  Memory free at startup: %d K\n", (s_memoryAtInit + 1023) / 1024);
	dbgPrint("   Memory free for level: %d K (%d K used)\n", (s_memoryAtPermanent + 1023) / 1024, (s_memoryAtInit - s_memoryAtPermanent + 1023) / 1024);
	if (s_memoryAtFirstPermanent != 0 && s_memoryAtFirstPermanent != s_memoryAtPermanent)
		dbgPrint("           Leaked memory: %d K\n", (s_memoryAtFirstPermanent - s_memoryAtPermanent + 1023) / 1024);
	dbgPrint("   Memory free for game: %d K (%d K used)\n", (s_memoryAtDone + 1023) / 1024, (s_memoryAtPermanent - s_memoryAtDone + 1023) / 1024);
	dbgPrint("         Level load time: %.1f s\n", (float)(s_doneTime - s_donePermanentTime) / 1000.0f);
	dbgPrint("\n\n");

	// Permanent stats
	bool beBrief = false;
	if (showPermanent)
	{
		dbgPrint("Permanent section:\n\n");
		DrawHeaderBar("Total Memory", s_memoryAtPermanent);

		s_permanentCategory.BubbleSortDescending();
		Category *category = s_permanentCategory.Head();
		while (category)
		{
			category->Sort();
			DrawCategory(*category, s_memoryAtInit, beBrief);
			category = category->next;
			beBrief = true;
		}
		dbgPrint("\n");
	}

	// Dynamic stats
	dbgPrint("Level section:\n\n");
	DrawHeaderBar("Total Memory", s_memoryAtPermanent);

	beBrief = false;
	s_dynamicCategory.BubbleSortDescending();
	Category *category = s_dynamicCategory.Head();
	while (category)
	{
		category->Sort();
		DrawCategory(*category, s_memoryAtInit, beBrief);
		category = category->next;
		beBrief = true;
	}
	dbgPrint("\n");
}

/////////////////////////////////////////////////////////////////////////////
// Reset the state
/////////////////////////////////////////////////////////////////////////////
void Reset(
bool includingPermanent)
{
	if (includingPermanent)
	{
		// Reset everything
		Category *category = s_permanentCategory.Head();
		while (category)
		{
			category->Reset();
			category = category->next;
		}

		s_memoryAtInit = FreeMemory();
		s_initTime = Time();

		s_memoryAtPermanent = 0;
		s_donePermanentTime = 0;

		s_inPermanent = true;
	}

	// Get the free memory
	s_memoryAtDone = 0;
	s_doneTime = 0;

	if (s_memoryAtPermanent != 0)
	{
		s_memoryAtPermanent = FreeMemory();
		s_inPermanent = false;
	}

	// Reset everything
	Category *category = s_dynamicCategory.Head();
	while (category)
	{
		category->Reset();
		category = category->next;
	}

	// Select the default category
	SelectCategoryByName(NULL);

	// we are active
	s_active = true;
}

};

#endif
