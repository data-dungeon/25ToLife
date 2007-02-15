/////////////////////////////////////////////////////////////////////////////
// Simple class to track the loaded data
/////////////////////////////////////////////////////////////////////////////
#ifndef LOADPROFILE_H
#define LOADPROFILE_H

// No profiling for a CDRom
#ifndef CDROM
#define WANT_LOADPROFILE
#endif

namespace LoadProfile
{
#ifdef WANT_LOADPROFILE
	// Initialize everything
	void Start();

	// Set the permanent state
	void DonePermanent();

	// Are we done with the profile
	void Done();

	// Setup a path to "infer" the category from
	// do these in specific to general order, the first match
	// is the match....
	void AddPathCategory(const char *path, const char *category);
	void AddExtSubcategory(const char *ext, const char *subcategory);
	void AddChunkSubcategory(uint chunkKey, const char *subcategory);

	// Notifys (internal)
	void NotifyFileLoad(const char *pathFile);
	void NotifyChunk(uint chunkKey, int size);

	// Dump out the results
	void Report(const char *title = NULL, bool showPermanent = true, bool brief = true);
	void Summary(const char *title = NULL, bool showPermanent = true);

	// Reset the state
	void Reset(bool includingPermanent = false);
#else
	// Initialize everything
	inline void Start() {}

	// Set the permanent state
	inline void DonePermanent() {}

	// Are we done with the profile
	inline void Done() {}

	// Setup a path to "infer" the category from
	// do these in specific to general order, the first match
	// is the match....
	inline void AddPathCategory(const char *path, const char *category) {}
	inline void AddExtSubcategory(const char *ext, const char *subcategory) {}
	inline void AddChunkSubcategory(uint chunkKey, const char *subcategory) {}

	// Notifys (internal)
	inline void NotifyFileLoad(const char *pathFile) {}
	inline void NotifyChunk(uint chunkKey, int size) {}

	// Dump out the results
	inline void Report(const char *title = NULL, bool showPermanent = true, bool brief = true) {}
	inline void Summary(const char *title = NULL, bool showPermanent = true) {}

	// Reset the state
	inline void Reset(bool includingPermanent = false) {}
#endif
};

#endif
