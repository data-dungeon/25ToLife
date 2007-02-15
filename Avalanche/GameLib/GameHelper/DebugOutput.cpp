/*
**
**  File:   DebugOutput.cpp
**  Date:   March 29, 2005
**  By:     Bryant Collard
**  Desc:   Buffer that holds text that can be output on some type of console.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/DebugOutput.cpp $
**  $Revision: #20 $
**  $DateTime: 2005/08/05 20:22:22 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/DebugOutput.h"

// GameLib headers
#include "EngineHelper/PackBuffer.h"

#ifdef DEBUG_OUTPUT_ENABLED

#define VERBOSITY_SHIFT       5
#define CATEGORY_MASK         0x1f
#define VERBOSITY_MASK        0x03
#define TIMESTAMP_MASK        0x80
#define TIMESTAMP_WIDTH       9
#define ITERATION_LIMIT       ((uint32)(~((uint32)(~0)) >> 1))
#define DISPLAY_NEWEST        ((uint32)(~0))
#define DISPLAY_OLDEST        (DISPLAY_NEWEST - 1)
#define NO_CATEGORY           MAX_CATEGORIES
#define UNKNOWN_CATEGORY      (MAX_CATEGORIES + 1)

// Setup the singleton
DeclareSingleton(CDebugOutput);

///////////////////////////////// CDebugBuffer /////////////////////////////////

// The buffer contains a collection of entries. Each entry consists of a 3 or 7
// byte header followed by the contents which is a null terminated string. The
// maximum size of an entry (header + contents) is 256 bytes. The header format
// is:
//    Byte 1: The size of the entry.
//    Byte 2: An offset from the start of the entry to the start of the previous
//            entry or, if the entry is at the beginning of the buffer, from the
//            byte following the entry at the end of the buffer to the start of
//            that entry.
//    Byte 3: The options (category (5 bits), verbosity (2 bits), and timestamp
//            (1 bit)).
// Bytes 4-7: (Optional) If the timestamp flag is set, the timestamp.
//
// The buffer is a ring buffer. When a new entry is added, 0 or more of the
// oldest entries will be removed to make space. Generally, the newest entry
// will be located immediately before the oldest entry. An entry is never split
// at the end of the buffer, that is, if there is not enough room at the end of
// the buffer to fit an entry, it is added at the beginning of the buffer. As a
// result, the only time the newest entry does not preceed the oldest entry in
// the buffer is when newest entry is the closest to the end of the buffer and,
// in that case, the oldest entry will be located at the beginning of the
// buffer.

/******************************************************************************/
/******************************************************************************/
void CDebugBuffer::Reset(void)
{
	d_oldestIndex = INVALID_ENTRY;
	d_newestIndex = INVALID_ENTRY;
	d_bufferEnd = INVALID_ENTRY;
	d_iteration = 0;
	d_lastIteration = INVALID_ENTRY;
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugBuffer::AddEntry(uint8 i_category, EDebugVerbosity i_verbosity,
		int32 i_timestamp, int i_contentLength, char* &o_destBuffer,
		uint32 &o_destCount)
{
	// Can only add an entry when a buffer exists.
	if (d_buffer == NULL)
	{
		o_destBuffer = NULL;
		o_destCount = 0;
		return(INVALID_ENTRY);
	}

	// Build up the options.
	ASSERT(i_category == (i_category & CATEGORY_MASK));
	ASSERT(i_verbosity < DEBUG_INVALID);
	uint8 options = (i_category & CATEGORY_MASK) |
			((uint8)i_verbosity << VERBOSITY_SHIFT);

	// Find the size of the entry.
	o_destCount = 4;  // Prev, next, options, terminating NULL.
	if (i_timestamp != EMPTY_TIMESTAMP)
	{
		o_destCount += 4;
		options |= TIMESTAMP_MASK;
	}
	o_destCount += i_contentLength;

	// Since the buffer is traversed using offsets and they are limited to 1
	// byte, the content may need to be truncated to fit in the allowed space.
	ASSERTF(o_destCount <= 0xff,
			("Debug buffer entry too long (%d)", o_destCount));
	if (o_destCount > 0xff)
		o_destCount = 0xff;

	// If there are no entries, start at the front.
	uint8 prevSize;
	if (d_newestIndex == INVALID_ENTRY)
	{
		d_oldestIndex = 0;
		d_newestIndex = 0;
		d_bufferEnd = o_destCount;
		d_iteration = 0;
		d_lastIteration = INVALID_ENTRY;
		prevSize = 0;
	}

	// There are entries.
	else
	{
		// Grab the size of the most recent entry.
		prevSize = GetSize(d_newestIndex);

		// If the oldest entry is at the front of the buffer, the new entry will
		// try to be added near the end of the buffer. If there is not enough
		// space, the new entry will be added at the beginning of the buffer.
		uint32 available;
		if (d_oldestIndex == 0)
		{
			available = d_bufSize - d_bufferEnd;
			if (available >= o_destCount)
			{
				d_newestIndex = d_bufferEnd;
				d_bufferEnd += o_destCount;
			}
			else
			{
				d_newestIndex = 0;
				d_lastIteration = d_iteration;
				d_iteration += d_bufSize;
				if ((d_iteration & ITERATION_LIMIT) != 0)
					d_iteration = 0;
				available = 0;
			}
		}

		// The new entry will be added in the middle of the buffer.
		else
		{
			d_newestIndex += GetSize(d_newestIndex);
			available = d_oldestIndex - d_newestIndex;
		}

		// If there is not room for the new entry, reassign the oldest location
		// to make room.
		while (available < o_destCount)
		{
			d_oldestIndex += GetSize(d_oldestIndex);
			if (d_oldestIndex == d_bufferEnd)
			{
				d_oldestIndex = 0;
				available = d_bufSize - d_newestIndex;
				if (available >= o_destCount)
					d_bufferEnd = d_newestIndex + o_destCount;
				else
				{
					d_bufferEnd = d_newestIndex;
					d_newestIndex = 0;
					d_lastIteration = d_iteration;
					d_iteration += d_bufSize;
					if ((d_iteration & ITERATION_LIMIT) != 0)
						d_iteration = 0;
					available = 0;
				}
			}
			else
				available = d_oldestIndex - d_newestIndex;
		}
	}

	// Fill in the entry.
	void* entry = PackBuffer::Pack((void*)&d_buffer[d_newestIndex],
			(uint8)o_destCount);
	entry = PackBuffer::Pack(entry, prevSize);
	entry = PackBuffer::Pack(entry, options);
	if ((options & TIMESTAMP_MASK) != 0)
	{
		entry = PackBuffer::Pack(entry, i_timestamp);
		o_destCount -= 4;
	}
	o_destCount -= 4;  // Prev, next, options, terminating NULL.
	o_destBuffer = (char*)entry;
	*((char*)entry + o_destCount) = '\0';

	return(GetNewestEntry());
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugBuffer::GetNextEntry(uint32 i_start) const
{
	if (d_buffer == NULL)
		return(INVALID_ENTRY);

	if (i_start == INVALID_ENTRY)
		return(GetOldestEntry());

	uint32 index = GetIndex(i_start);
	ASSERT(index != INVALID_ENTRY);

	if (index == d_newestIndex)
		return(INVALID_ENTRY);

	uint32 next = index + GetSize(index);
	ASSERT(next <= d_bufferEnd);
	if (next == d_bufferEnd)
		return(GetEntry(0));

	return(GetEntry(next));
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugBuffer::GetPrevEntry(uint32 i_start) const
{
	if (d_buffer == NULL)
		return(INVALID_ENTRY);

	if (i_start == INVALID_ENTRY)
		return(GetNewestEntry());

	uint32 index = GetIndex(i_start);

	if (index == d_oldestIndex)
		return(INVALID_ENTRY);

	uint32 offset = GetPrevSize(index);
	if (index == 0)
	{
		ASSERT(d_bufferEnd >= offset);
		return(GetEntry(d_bufferEnd - offset));
	}

	ASSERT(index >= offset);
	return(GetEntry(index - offset));
}

/******************************************************************************/
/******************************************************************************/
uint8 CDebugBuffer::GetCategory(uint32 i_entry) const
{
	if (d_buffer != NULL)
	{
		uint32 index = GetIndex(i_entry);

		if (index != INVALID_ENTRY)
			return((*((uint8*)&d_buffer[index + 2])) & CATEGORY_MASK);
	}
	return(INVALID_CATEGORY);
}

/******************************************************************************/
/******************************************************************************/
EDebugVerbosity CDebugBuffer::GetVerbosity(uint32 i_entry) const
{
	if (d_buffer != NULL)
	{
		uint32 index = GetIndex(i_entry);

		if (index != INVALID_ENTRY)
		{
			return((EDebugVerbosity)(((*((uint8*)&d_buffer[index + 2])) >>
					VERBOSITY_SHIFT) & VERBOSITY_MASK));
		}
	}
	return(DEBUG_INVALID);
}

/******************************************************************************/
/******************************************************************************/
int32 CDebugBuffer::GetTimestamp(uint32 i_entry) const
{
	if (d_buffer != NULL)
	{
		uint32 index = GetIndex(i_entry);

		if ((index != INVALID_ENTRY) &&
				(((*((uint8*)&d_buffer[index + 2])) & TIMESTAMP_MASK) != 0))
		{
			int32 timestamp;
			PackBuffer::Unpack((void*)&d_buffer[index + 3], timestamp);
			return(timestamp);
		}
	}
	return(EMPTY_TIMESTAMP);
}

/******************************************************************************/
/******************************************************************************/
uint8 CDebugBuffer::GetLength(uint32 i_entry) const
{
	if (d_buffer != NULL)
	{
		uint32 index = GetIndex(i_entry);

		if (index != INVALID_ENTRY)
		{
			char* content;
			if (((*((uint8*)&d_buffer[index + 2])) & TIMESTAMP_MASK) != 0)
				content = &d_buffer[index + 7];
			else
				content = &d_buffer[index + 3];
			return(strlen(content));
		}
	}
	return(0);
}

/******************************************************************************/
/******************************************************************************/
const char* CDebugBuffer::GetContent(uint32 i_entry) const
{
	if (d_buffer != NULL)
	{
		uint32 index = GetIndex(i_entry);

		if (index != INVALID_ENTRY)
		{
			if (((*((uint8*)&d_buffer[index + 2])) & TIMESTAMP_MASK) != 0)
				return(&d_buffer[index + 7]);
			else
				return(&d_buffer[index + 3]);
		}
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugBuffer::GetIndex(uint32 i_entry) const
{
	if (i_entry == INVALID_ENTRY)
		return(INVALID_ENTRY);

	uint32 index = i_entry % d_bufSize;
	uint32 iteration = i_entry - index;

	if (d_newestIndex >= d_oldestIndex)
	{
		if ((iteration != d_iteration) || (index < d_oldestIndex) ||
				(index > d_newestIndex))
			return(INVALID_ENTRY);
	}
	else if (index >= d_oldestIndex)
	{
		if (iteration != d_lastIteration)
			return(INVALID_ENTRY);
	}
	else if ((index > d_newestIndex) || (iteration != d_iteration))
		return(INVALID_ENTRY);

	ASSERT(((index == 0) || (d_buffer[index - 1] == '\0')) &&
			(GetSize(index) > 4) && ((index + GetSize(index)) <= d_bufSize) &&
			(d_buffer[index + GetSize(index) - 1] == '\0'));

	return(index);
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugBuffer::GetEntry(uint32 i_index) const
{
	if (d_newestIndex >= d_oldestIndex)
	{
		if ((i_index >= d_oldestIndex) && (i_index <= d_newestIndex) &&
				(i_index != INVALID_ENTRY))
			return(i_index + d_iteration);
	}

	else if (i_index <= d_newestIndex)
		return(i_index + d_iteration);

	else if ((i_index >= d_oldestIndex) && (i_index < d_bufferEnd))
		return(i_index + d_lastIteration);

	return(INVALID_ENTRY);
}

///////////////////////////// CVariableDebugBuffer /////////////////////////////

/******************************************************************************/
/******************************************************************************/
CVariableDebugBuffer::CVariableDebugBuffer()
{
	d_buffer = NULL;
	d_bufSize = 0;
	Reset();
}

/******************************************************************************/
/******************************************************************************/
CVariableDebugBuffer::~CVariableDebugBuffer()
{
	if (d_buffer != NULL)
		memFree(d_buffer);
}

/******************************************************************************/
/******************************************************************************/
bool CVariableDebugBuffer::Configure(uint32 i_size)
{
	bool result = true;
	if (i_size != d_bufSize)
	{
		if (d_buffer != NULL)
			memFree(d_buffer);
		if (i_size < 0xff)
		{
			ASSERT(i_size == 0);
			d_buffer = NULL;
			d_bufSize = 0;
		}
		else
		{
			d_buffer = (char*)memAlloc(d_bufSize);
			if (d_buffer == NULL)
			{
				ASSERT(false);
				d_bufSize = 0;
				result = false;
			}
			d_bufSize = i_size;
		}
		Reset();
	}
	return(result);
}

///////////////////////////////// CDebugOutput /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CDebugOutput::CDebugOutput()
{
	Construct();
}

/******************************************************************************/
/******************************************************************************/
CDebugOutput::CDebugOutput(CDebugBuffer* i_buffer)
{
	Construct();
	d_buffer = i_buffer;;
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::Construct(void)
{
	d_buffer = NULL;
	d_timestampFunc = NULL;
	d_nextCategory = 0;
	ClearCategories();
	d_lockCount = 0;
#ifdef LOG_DEBUG_OUTPUT
	d_logEnabled = false;
#endif
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::SetBuffer(CDebugBuffer* i_buffer)
{
	if (i_buffer != d_buffer)
	{
		if (d_lockCount != 0)
		{
			ASSERTS(false, "Trying to change debug output buffer while the buffer "
					"is locked.");
			return;
		}
		d_buffer = i_buffer;
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::ClearBuffer(void)
{
	if (d_buffer != NULL)
	{
		if (d_lockCount != 0)
		{
			ASSERTS(false, "Trying to clear the debug output buffer while the "
					"buffer is locked.");
			return;
		}
		d_buffer->Reset();
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::ClearCategories(void)
{
	if (d_lockCount != 0)
	{
		ASSERTS(false, "Trying to clear the debug output categories while the "
				"buffer is locked.");
		return;
	}

	d_nextCategory = 0;
	strcpy(d_category[NO_CATEGORY].d_tag, " ");       // Tags must have same
	strcpy(d_category[UNKNOWN_CATEGORY].d_tag, "?");  // length.
	d_category[NO_CATEGORY].d_name = "No Category";
	d_category[UNKNOWN_CATEGORY].d_name = "Unknown Category";
	d_tagLength = strlen(d_category[UNKNOWN_CATEGORY].d_tag);
	d_nameLength = strlen(d_category[UNKNOWN_CATEGORY].d_name);
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::ClearAll(void)
{
	ClearCategories();
	ClearBuffer();
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::SetTimestampFunction(TimestampFunc i_timestampFunc)
{
	if ((d_lockCount != 0) &&
			(((i_timestampFunc != NULL) && (d_timestampFunc == NULL)) ||
			((i_timestampFunc == NULL) && (d_timestampFunc != NULL))))
	{
		ASSERTS(false, "Trying to change debug output timestamp function "
				"while the buffer is locked.");
		return;
	}
	d_timestampFunc = i_timestampFunc;
}

/******************************************************************************/
/******************************************************************************/
int32 CDebugOutput::GetTimestamp(void)
{
	if (d_timestampFunc != NULL)
		return((*d_timestampFunc)());
	return(EMPTY_TIMESTAMP);
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::Lock(void)
{
	d_lockCount++;
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::Unlock(void)
{
	ASSERT(d_lockCount > 0);
	if (d_lockCount > 0)
		d_lockCount--;
}

/******************************************************************************/
/******************************************************************************/
const char* CDebugOutput::GetCategoryName(uint8 i_id) const
{
	if (i_id < d_nextCategory)
		return(d_category[i_id].d_name);
	if ((i_id == d_nextCategory) || (i_id == NO_CATEGORY))
		return(d_category[NO_CATEGORY].d_name);
	if ((i_id == d_nextCategory + 1) || (i_id == UNKNOWN_CATEGORY))
		return(d_category[UNKNOWN_CATEGORY].d_name);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CDebugOutput::GetCategoryTag(uint8 i_id) const
{
	if (i_id < d_nextCategory)
		return(d_category[i_id].d_tag);
	if ((i_id == d_nextCategory) || (i_id == NO_CATEGORY))
		return(d_category[NO_CATEGORY].d_tag);
	if ((i_id == d_nextCategory + 1) || (i_id == UNKNOWN_CATEGORY))
		return(d_category[UNKNOWN_CATEGORY].d_tag);
	return(NULL);
}

/******************************************************************************/
// Register a category. NOTE: only a pointer to the name is stored. The caller
// is responsible for ensuring that the pointer remains valid. This is done to
// avoid any dynamic memory allocation and avoid problems with transitions
// between loops. Simply using a quoted string will work. The tag string is
// stored.
/******************************************************************************/
uint8 CDebugOutput::RegisterCategory(const char* i_name, const char* i_tag)
{
	// Cannot register categories while the buffer is locked.
	if (d_lockCount > 0)
	{
		ASSERTS(false, "Trying to register debug buffer category while the "
				"buffer is locked");
		return(INVALID_CATEGORY);
	}

	// Make sure there is a category available.
	if (d_nextCategory >= MAX_CATEGORIES)
	{
		ASSERTS(false, "Ran out of debug buffer categories");
		return(INVALID_CATEGORY);
	}

	// A name and tag must be provided.
	ASSERTS((i_name != NULL) && (i_name[0] != '\0'),
			"No name when registering debug buffer category");
	ASSERTS((i_tag != NULL) && (i_tag[0] != '\0'),
			"No tag when registering debug buffer category");
	if ((i_name == NULL) || (i_name[0] == '\0') ||
			(i_tag == NULL) || (i_tag[0] == '\0'))
		return(INVALID_CATEGORY);

	// Get the length of the tag. It cannot be too long.
	int len = strlen(i_tag);
	if (len >= MAX_CATEGORY_TAG_SIZE)
	{
		ASSERTF(false, ("Debug buffer tag (%s) too long (%d >= %d)", i_tag, len,
				MAX_CATEGORY_TAG_SIZE - 1));
		return(INVALID_CATEGORY);
	}

	// Adjust padding.
	int i, j;
	char tag[MAX_CATEGORY_TAG_SIZE];

	// If the new tag is shorter than the longest tag, create a padded version.
	if (len < d_tagLength)
	{
		for (i = 0; i < d_tagLength - len; i++)
			tag[i] = ' ';
		strcpy(&tag[i], i_tag);
		i_tag = tag;
	}

	// If the new tag is longer than the longest tag, pad out the existing tags
	// to match.
	else if (len > d_tagLength)
	{
		for (i = 0; i < d_nextCategory + 2; i++)
		{
			j = i;
			if (j == d_nextCategory)
				j = NO_CATEGORY;
			else if (j > d_nextCategory)
				j = UNKNOWN_CATEGORY;

			int m, n;
			for (m = len + 1, n = d_tagLength + 1; n >= 0; m--, n--)
				d_category[j].d_tag[m] = d_category[j].d_tag[n];
			for (; m >= 0; m--)
				d_category[j].d_tag[m] = ' ';
		}
		d_tagLength = len;
	}

	// See if the category has already been registered.
	for (i = 0; i < d_nextCategory + 2; i++)
	{
		j = i;
		if (j == d_nextCategory)
			j = NO_CATEGORY;
		else if (j > d_nextCategory)
			j = UNKNOWN_CATEGORY;

		if (!strcmp(i_tag, d_category[j].d_tag))
		{
			ASSERTF(!strcmp(i_name, d_category[j].d_name), ("Trying to reregister "
					"debug buffer tag '%s' with different name (%s v. %s)",
					i_tag, i_name, d_category[j].d_name));
			return((uint8)j);
		}
	}

	// Make sure the name is unique.
	for (i = 0; i < d_nextCategory + 2; i++)
	{
		j = i;
		if (j == d_nextCategory)
			j = NO_CATEGORY;
		else if (j > d_nextCategory)
			j = UNKNOWN_CATEGORY;

		if (!strcmp(i_name, d_category[j].d_name))
		{
			ASSERTF(false, ("Trying to reregister debug buffer name '%s' with "
					"different tag (%s v. %s)", i_name, i_tag, d_category[j].d_tag));
			return(INVALID_CATEGORY);
		}
	}

	// Adjust the longest name length if necessary.
	len = strlen(i_name);
	if (len > d_nameLength)
		d_nameLength = len;

	// Add the category.
	strcpy(d_category[d_nextCategory].d_tag, i_tag);
	d_category[d_nextCategory].d_name = i_name;

	// Return.
	return(d_nextCategory++);
}

/******************************************************************************/
/******************************************************************************/
uint8 CDebugOutput::Category(const char* i_tag) const
{
	if ((i_tag == NULL) || (i_tag[0] == '\0'))
		return(NO_CATEGORY);

	int len = strlen(i_tag);
	if (len <= d_tagLength)
	{
		int i;
		char tag[MAX_CATEGORY_TAG_SIZE];
		for (i = 0; i < d_tagLength - len; i++)
			tag[i] = ' ';
		strcpy(&tag[i], i_tag);
		for (i = 0; i < d_nextCategory; i++)
		{
			if (!strcmp(tag, d_category[i].d_tag))
				return((uint8)i);
		}
	}
	return(UNKNOWN_CATEGORY);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, DEBUG_ALERT, false, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(uint8 i_category,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), DEBUG_ALERT, false,
			i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(EDebugVerbosity i_verbosity,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, i_verbosity, false, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(uint8 i_category,
		EDebugVerbosity i_verbosity, const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), i_verbosity, false,
			i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(bool i_timestamp,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, DEBUG_ALERT, i_timestamp, i_format,
			args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(bool i_timestamp, uint8 i_category,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), DEBUG_ALERT,
			i_timestamp, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(bool i_timestamp,
		EDebugVerbosity i_verbosity, const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, i_verbosity, i_timestamp, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::Print(bool i_timestamp, uint8 i_category,
		EDebugVerbosity i_verbosity, const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), i_verbosity, i_timestamp,
			i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::TSPrint(const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, DEBUG_ALERT, true, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::TSPrint(uint8 i_category,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), DEBUG_ALERT, true,
			i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::TSPrint(EDebugVerbosity i_verbosity,
		const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(NO_CATEGORY, i_verbosity, true, i_format, args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint32 VARARGCDECL CDebugOutput::TSPrint(uint8 i_category,
		EDebugVerbosity i_verbosity, const char* i_format, ...)
{
	va_list args;
	va_start(args, i_format);
	uint32 entry = Add(ValidateCategory(i_category), i_verbosity, true, i_format,
			args);
	va_end(args);
	return(entry);
}

/******************************************************************************/
/******************************************************************************/
uint8 CDebugOutput::ValidateCategory(uint8 i_category) const
{
	if (i_category < d_nextCategory)
		return(i_category);
	if ((i_category == d_nextCategory) || (i_category == NO_CATEGORY))
		return(NO_CATEGORY);
	return(UNKNOWN_CATEGORY);
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugOutput::Add(int i_category, EDebugVerbosity i_verbosity,
		bool i_timestamp, const char* i_format, va_list i_args)
{
	if ((d_buffer != NULL) && (d_lockCount == 0))
				 
	{
		char content[256];  // Longer than can be added to buffer.
		_vsnprintf(content, 255, i_format, i_args);
		content[255] = '\0';
		uint32 entry;
		if (i_timestamp && (d_timestampFunc != NULL))
			entry = d_buffer->AddEntry(i_category, i_verbosity,
					(*d_timestampFunc)(), content);
		else
			entry = d_buffer->AddEntry(i_category, i_verbosity, content);

#ifdef LOG_DEBUG_OUTPUT
		LogEntry(entry);
#endif

		return(entry);
	}
	return(INVALID_ENTRY);
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::AppendBufferDescriptionToEmail(EMail* email) const
{
	if ((d_buffer == NULL) || (d_buffer->GetNextEntry() == INVALID_ENTRY))
	{
		if (d_timestampFunc != NULL)
		{
			char body[64];
			sprintf(body, "Debug output buffer empty at %f sec.",
					(*d_timestampFunc)() / 1000.0f);
			email->AppendLine(body);
		}
		else
			email->AppendLine("Debug output buffer empty.");
	}
	else
	{
		// Construct the message body.
		if (d_timestampFunc != NULL)
		{
			char body[64];
			sprintf(body, "Contents of debug output buffer at %f sec.",
					(*d_timestampFunc)() / 1000.0f);
			email->AppendLine(body);
		}
		else
			email->AppendLine("Contents of debug output buffer.");
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::AttachBufferToEmail(EMail* email, bool descriptiveLine /*= true*/) const
{
	// Send the buffer as an attachment.
	email->Attachment("DebugOutput.txt", "text/plain", false, false);

	if(descriptiveLine)
	{
		AppendBufferDescriptionToEmail( email );
		email->AppendLine("");
	}

	if(d_buffer == NULL)
		return;

	// Append each line.
	uint32 entry = INVALID_ENTRY;
	while((entry = d_buffer->GetNextEntry(entry)) != INVALID_ENTRY)
	{
		// Construct the line.
		char line[255 + TIMESTAMP_WIDTH + MAX_CATEGORY_TAG_SIZE];
		line[0] = '\0';
		if (d_timestampFunc != NULL)
		{
			int32 timestamp = d_buffer->GetTimestamp(entry);
			if (timestamp != EMPTY_TIMESTAMP)
			{
				sprintf(line, "%*.3f ", TIMESTAMP_WIDTH,
						(float)timestamp * 0.001f);
			}
			else
				sprintf(line, "%*s ", TIMESTAMP_WIDTH, " ");
		}
		strcat(line, GetCategoryTag(d_buffer->GetCategory(entry)));
		strcat(line, ">");
		strcat(line, d_buffer->GetContent(entry));

		// Append the line.
		email->AppendLine(line);
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::EmailBuffer(const char* i_sender,
		const char* i_recipient, const char* i_subject) const
{
	// Try to connect to the email server and begin a message.
	EMail email;
	if (email.Begin(i_sender, i_recipient, i_subject))
	{
		// put the descriptive line in the body of the email...
		AppendBufferDescriptionToEmail( &email );

		// attach buffer, without embedded descriptive line
		if ((d_buffer != NULL) && (d_buffer->GetNextEntry() != INVALID_ENTRY))
		{
			AttachBufferToEmail( &email, false );
		}

		// End the message and close the connection.
		email.End();
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::EnableLog(bool i_enable)
{
#ifdef LOG_DEBUG_OUTPUT
	d_logEnabled = i_enable;
#endif
}

/******************************************************************************/
/******************************************************************************/
void CDebugOutput::ToggleLog(void)
{
#ifdef LOG_DEBUG_OUTPUT
	d_logEnabled = !d_logEnabled;
#endif
}

#ifdef LOG_DEBUG_OUTPUT
/******************************************************************************/
/******************************************************************************/
void CDebugOutput::LogEntry(uint32 i_entry)
{
	if (d_logEnabled && !d_logFile.IsInvalid() &&
			(d_buffer != NULL) && (d_buffer->TestEntry(i_entry) == 0))
	{
		if (!d_logFile.IsOpen())
		{
			char rootName[] = "Output.txt";
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_logFile.Open(name, 6))
				return;

			d_logFile.SetField(0, "Entry");
			d_logFile.SetField(1, "Timestamp");
			d_logFile.SetField(2, "Category");
			d_logFile.SetField(3, "Tag");
			d_logFile.SetField(4, "Verbosity");
			d_logFile.SetField(5, "Content");
			d_logFile.Print();
		}

		d_logFile.SetField(0, (int)i_entry);

		int32 timestamp = d_buffer->GetTimestamp(i_entry);
		if (timestamp != EMPTY_TIMESTAMP)
			d_logFile.SetField(1, (float)timestamp * 0.001f);

		uint8 category = d_buffer->GetCategory(i_entry);
		if (category != INVALID_CATEGORY)
		{
			d_logFile.SetField(2, GetCategoryName(category));
			d_logFile.SetField(3, GetCategoryTag(category));
		}

		switch(d_buffer->GetVerbosity(i_entry))
		{
		 case DEBUG_BORE:
			d_logFile.SetField(4, "Bore");
			break;
		 case DEBUG_INFORM:
			d_logFile.SetField(4, "Inform");
			break;
		 case DEBUG_NOTIFY:
			d_logFile.SetField(4, "Notify");
			break;
		 case DEBUG_ALERT:
			d_logFile.SetField(4, "Alert");
			break;
		}

		d_logFile.SetField(5, d_buffer->GetContent(i_entry));

		d_logFile.Print();
	}
}
#endif

//////////////////////////////// CDebugDisplay /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CDebugDisplay::CDebugDisplay()
{
	Construct();
}

/******************************************************************************/
/******************************************************************************/
CDebugDisplay::CDebugDisplay(CDebugOutput* i_output)
{
	Construct();
	d_output = i_output;;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::Construct(void)
{
	d_output = NULL;
	d_dispLines = 10;
	d_dispCols = 80;
	d_wrap = true;
	d_showCategories = false;
	d_showTimestamps = false;
	d_dispAnchorEntry = DISPLAY_NEWEST;
	d_dispAnchorRow = 0;
	d_dispOffset = 0;
	d_verbosity = DEBUG_BORE;
	d_locked = false;
	SelectAllCategories();
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::SetOutput(CDebugOutput* i_output)
{
	if (i_output != d_output)
	{
		if (d_locked)
		{
			ASSERTS(false, "Trying to change debug output display while the "
					"display is locked.");
			return;
		}
		d_output = i_output;
		d_dispAnchorEntry = DISPLAY_NEWEST;
		d_dispAnchorRow = 0;
		d_dispOffset = 0;
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DescribeDisplay(int i_lines, int i_cols)
{
	if ((i_lines != d_dispLines) || (i_cols != d_dispCols))
	{
		if (d_locked)
		{
			ASSERTS(false, "Trying to change debug output display description "
				"while the display is locked.");
			return;
		}
		ASSERTF((i_lines > 0) && (i_cols > 1),
				("Invalid debug output display description (lines = %d, cols = %d)",
				i_lines, i_cols));
		d_dispLines = i_lines;
		d_dispCols = i_cols;
	}
}

/******************************************************************************/
/******************************************************************************/
bool CDebugDisplay::ToggleShowCategories(void)
{
	if (!d_locked)
		d_showCategories = !d_showCategories;
	return(d_showCategories);
}

/******************************************************************************/
/******************************************************************************/
bool CDebugDisplay::ToggleShowTimestamps(void)
{
	if (!d_locked)
		d_showTimestamps = !d_showTimestamps;
	return(d_showTimestamps);
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DisplayNewest(int i_offset)
{
	d_dispAnchorEntry = DISPLAY_NEWEST;
	d_dispAnchorRow = 0;
	d_dispOffset = i_offset;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DisplayOldest(int i_offset)
{
	d_dispAnchorEntry = DISPLAY_OLDEST;
	d_dispAnchorRow = 0;
	d_dispOffset = i_offset;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DisplayEntry(uint32 i_entry, int i_offset)
{
	d_dispAnchorEntry = i_entry;
	d_dispAnchorRow = 0;
	d_dispOffset = i_offset;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::LockBuffer(void)
{
	// Lock the buffer if there is a buffer and it is not already locked.
	CDebugBuffer* buffer = NULL;
	if (d_output != NULL)
		buffer = d_output->GetBuffer();
	if ((buffer != NULL) && !d_locked)
	{
		d_locked = true;
		d_output->Lock();

		// Compute the size of the content.
		d_contentLength = d_dispCols - 1;
		if (d_showTimestamps && d_output->IsTimestampFunctionSet())
		{
			d_contentLength -= TIMESTAMP_WIDTH;
			if (d_showCategories)
				d_contentLength -= d_output->GetLongestTagLength() + 1;
		}
		else if (d_showCategories)
			d_contentLength -= d_output->GetLongestTagLength();

		// Validate the anchor entry, if any.
		if ((d_dispAnchorEntry != DISPLAY_NEWEST) &&
				(d_dispAnchorEntry != DISPLAY_OLDEST))
		{
			// If the anchor entry was discarded to make room for a new entry,
			// display the oldest entry.
			int entryTest = buffer->TestEntry(d_dispAnchorEntry);
			if (entryTest < 0)
				d_dispAnchorEntry = DISPLAY_OLDEST;

			// If the anchor entry has not been entered, display the newest entry.
			else if (entryTest > 0)
				d_dispAnchorEntry = DISPLAY_NEWEST;

			// If the anchor entry's category is not selected, move the anchor
			// to the closest previous entry that is selected.
			else
			{
				uint8 category = buffer->GetCategory(d_dispAnchorEntry);
				ASSERT((category < (d_output->GetNumCategories() - 2)) ||
						(category == NO_CATEGORY) || (category == UNKNOWN_CATEGORY));
				if (!d_categorySelected[category])
				{
					d_dispAnchorEntry = GetPrevSelectedEntry(d_dispAnchorEntry);
					if (d_dispAnchorEntry == INVALID_ENTRY)
						d_dispAnchorEntry = DISPLAY_OLDEST;
					else
						d_dispAnchorRow = GetRows(d_dispAnchorEntry) - 1;
				}
			}
		}

		// Process any scroll up instruction.
		if (d_dispOffset < 0)
		{
			// Cannot scroll up past the oldest entry.
			if (d_dispAnchorEntry == DISPLAY_OLDEST)
				d_dispOffset = 0;

			// When the newest entry is to be displayed, the bottom line is
			// actually a line lead with no content. Move the anchor up to the last
			// row of the most recent entry. If there is no entry, still display
			// newest.
			else if (d_dispAnchorEntry == DISPLAY_NEWEST)
			{
				d_dispAnchorEntry = GetPrevSelectedEntry(INVALID_ENTRY);
				if (d_dispAnchorEntry == INVALID_ENTRY)
				{
					d_dispAnchorEntry = DISPLAY_NEWEST;
					d_dispOffset = 0;
				}
				else
				{
					d_dispAnchorRow = GetRows(d_dispAnchorEntry) - 1;
					d_dispOffset += 1;
				}
			}

			// At this point, if d_dispOffset < 0 then the anchor entry is valid.
			// Keep moving the anchor point up until scrolling is completed.
			while(d_dispOffset < 0)
			{
				// If there are fewer lines to scroll than there are in the current
				// entry, set the anchor row.
				if (d_dispAnchorRow >= -d_dispOffset)
				{
					d_dispAnchorRow += d_dispOffset;
					d_dispOffset = 0;
				}

				// Scroll up to the bottom of the previous entry. If there is no
				// previous entry, display the oldest entry.
				else
				{
					d_dispAnchorEntry = GetPrevSelectedEntry(d_dispAnchorEntry);
					if (d_dispAnchorEntry == INVALID_ENTRY)
					{
						d_dispAnchorEntry = DISPLAY_OLDEST;
						d_dispOffset = 0;
					}
					else
					{
						d_dispOffset += d_dispAnchorRow + 1;
						d_dispAnchorRow = GetRows(d_dispAnchorEntry) - 1;
					}
				}
			}
		}

		// Make the bottom line invalid (which will imply DISPLAY_NEWEST).
		d_bottomEntry = INVALID_ENTRY;
		d_bottomRow = 0;

		// If the oldest entry is to be displayed (barring application of scroll
		// down instructions), find the bottom line.
		if (d_dispAnchorEntry == DISPLAY_OLDEST)
		{
			// Try to find the bottom line.
			int lines = d_dispLines;
			while((d_bottomEntry = GetNextSelectedEntry(d_bottomEntry)) !=
					INVALID_ENTRY)
			{
				int rows = GetRows(d_bottomEntry);
				if (lines > rows)
					lines -= rows;
				else
				{
					d_bottomRow = lines - 1;
					break;
				}
			}
		}

		// If a valid entry is to be displayed, the desired bottom line is the
		// anchor line (barring application of scroll down instructions).
		else if (d_dispAnchorEntry != DISPLAY_NEWEST)
		{
			d_bottomEntry = d_dispAnchorEntry;
			d_bottomRow = d_dispAnchorRow;
		}

		// Now process any scroll down instruction. Apply it to the bottom row.
		// Note that the bottom entry cannot be DISPLAY_OLDEST.
		if (d_dispOffset > 0)
		{
			// A scroll down instruction will discontinue the display of the oldest
			// entry (this really just clears the display mode).
			if (d_dispAnchorEntry == DISPLAY_OLDEST)
				d_dispAnchorEntry = DISPLAY_NEWEST;

			// Cannot scroll down past the newest entry.
			if (d_bottomEntry == INVALID_ENTRY)
				d_dispOffset = 0;

			// At this point, if d_dispOffset > 0 then the bottom entry is valid.
			// Keep moving the bottom point down until scrolling is completed.
			while(d_dispOffset > 0)
			{
				// Find the number of rows left in the bottom entry.
				int rows = GetRows(d_bottomEntry) - d_bottomRow - 1;

				// If there are enough rows left in the bottom entry to accomodate
				// all scrolling, just move to a lower row in that entry.
				if (d_dispOffset <= rows)
				{
					d_bottomRow += d_dispOffset;
					d_dispOffset = 0;
				}

				// There are more rows to scroll than are left in the bottom
				// entry.
				else
				{
					// Scroll past the current bottom entry/
					d_dispOffset -= rows + 1;

					// Make the first row of the next entry the new bottom line.
					d_bottomEntry = GetNextSelectedEntry(d_bottomEntry);
					d_bottomRow = 0;

					// If there is no next entry, the bottom line will be an
					// empty line.
					if (d_bottomEntry == INVALID_ENTRY)
						break;
				}
			}
		}

		// Set the top line at the bottom line.
		d_topEntry = d_bottomEntry;
		d_topRow = d_bottomRow;

		// Find the number of lines left to fill. Note that one is already filled
		// by the bottom line.
		int lines = d_dispLines - 1;

		// Move the top line up to fill lines.
		while(lines > 0)
		{
			// If the top entry has enough rows above the current top row to fill
			// all the remaining lines, adjust the top row and indicate that all
			// lines are filled.
			if (lines <= d_topRow)
			{
				d_topRow -= lines;
				lines = 0;
			}

			// The current top entry cannot fill all the remaining lines.
			else
			{
				// Use the rows of the top entry above the current row to fill
				// lines. This makes the first row of the current top entry the top
				// line.
				lines -= d_topRow;
				d_topRow = 0;

				// Find the next entry up. If there isn't one, there are no older
				// entries left to fill lines.
				uint32 entry = GetPrevSelectedEntry(d_topEntry);
				if (entry == INVALID_ENTRY)
					break;

				// Make the bottom row of the new top entry the new top line and use
				// it to fill an empty line.
				d_topEntry = entry;
				d_topRow = GetRows(d_topEntry) - 1;
				lines -= 1;
			}
		}

		// The only way for the top entry to be invalid is for there to be no
		// entries at all.
		if (d_topEntry == INVALID_ENTRY)
		{
			d_topEntry = DISPLAY_NEWEST;
			d_topRow = 0;
		}

		// Try to fill empty lines by moving the bottom line down.
		while((lines > 0) && (d_bottomEntry != INVALID_ENTRY))
		{
			// Find the number of rows the bottom entry below the bottom row.
			int rows = GetRows(d_bottomEntry) - d_bottomRow - 1;

			// If there are enough rows available to fill all the remaining lines,
			// move the bottom row down and indicate that all lines have been
			// filled.
			if (lines <= rows)
			{
				d_bottomRow += lines;
				lines = 0;
			}

			// There are not enough rows left in the bottom entry below the
			// bottom row to fill all the remaining lines.
			else
			{
				// The new bottom row will be the first row of either the next entry
				// down or the empty line indicating the newest entry is being
				// displayed. Use the remaining rows of the bottom entry to fill
				// lines and set the bottom row.
				lines -= rows + 1;
				d_bottomRow = 0;

				// Find the next entry down.
				d_bottomEntry = GetNextSelectedEntry(d_bottomEntry);
			}
		}

		// If the bottom entry is invalid, indicate that the empty "newest" line
		// is to be displayed, store the number of blank lines below that in
		// d_bottomRow, and set the display mode to DISPLAY_NEWEST.
		if (d_bottomEntry == INVALID_ENTRY)
		{
			d_bottomEntry = DISPLAY_NEWEST;
			d_bottomRow = lines;
			d_dispAnchorEntry = DISPLAY_NEWEST;
			d_dispAnchorRow = 0;
		}

		// If the display mode is not DISPLAY_OLDEST, set the anchor to the bottom
		// line.
		else if (d_dispAnchorEntry != DISPLAY_OLDEST)
		{
			d_dispAnchorEntry = d_bottomEntry;
			d_dispAnchorRow = d_bottomRow;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::UnlockBuffer(void)
{
	if (d_locked)
	{
		d_locked = false;
		d_output->Unlock();
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::GotoTop(void)
{
	ASSERT(d_locked);
	if (d_locked)
	{
		d_currentEntry = d_topEntry;
		d_currentRow = d_topRow;
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::GotoBottom(void)
{
	ASSERT(d_locked);
	if (d_locked)
	{
		d_currentEntry = d_bottomEntry;
		d_currentRow = d_bottomRow;
	}
}

// Normally, d_topEntry/d_topRow and d_bottomEntry/d_bottomRow are the entry and
// row in that entry to be displayed at the top and bottom of the display. If
// d_bottomEntry is DISPLAY_NEWEST then a line with a lead and no content will
// be shown. If there are not enough lines available to fill the entire display,
// d_bottomRow will contain the number of empty rows. The top line will be
// either a valid line or the empty "newest" line. So, d_topEntry/d_topRow will
// be point to the valid to line or d_topEntry will be DISPLAY_NEWEST and
// d_topRow will be zero. When GetLine() is called, d_currentEntry and
// d_currentRow are used to build the line. When the current line is past the
// top of the display, d_currentEntry will be DISPLAY_OLDEST. When the current
// line is past the bottom of the display, d_currentEntry will be d_bottomEntry
// and d_currentRow will be greater than d_bottomRow.

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::operator--()
{
	ASSERT(d_locked);
	if (d_locked && (d_currentEntry != DISPLAY_OLDEST))
	{
		if ((d_currentEntry == d_topEntry) && (d_currentRow <= d_topRow))
		{
			d_currentEntry = DISPLAY_OLDEST;
			d_currentRow = 0;
		}
		else if (d_currentRow > 0)
			d_currentRow--;
		else
		{
			if (d_currentEntry == DISPLAY_NEWEST)
				d_currentEntry = INVALID_ENTRY;
			d_currentEntry = GetPrevSelectedEntry(d_currentEntry);
			if (d_currentEntry == INVALID_ENTRY)
			{
				d_currentEntry = DISPLAY_OLDEST;
				d_currentRow = 0;
			}
			else
				d_currentRow = GetRows(d_currentEntry) - 1;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::operator++()
{
	ASSERT(d_locked);
	if (d_locked)
	{
		if (d_currentEntry == d_bottomEntry)
		{
			if (d_currentRow <= d_bottomRow)
				d_currentRow++;
		}
		else if (d_currentEntry == DISPLAY_OLDEST)
		{
			d_currentEntry = d_topEntry;
			d_currentRow = d_topRow;
		}
		else if (++d_currentRow >= GetRows(d_currentEntry))
		{
			d_currentEntry = GetNextSelectedEntry(d_currentEntry);
			if (d_currentEntry == INVALID_ENTRY)
				d_currentEntry = DISPLAY_NEWEST;
			d_currentRow = 0;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
bool CDebugDisplay::PastEnds(void)
{
	ASSERT(d_locked);
	if (d_locked)
	{
		if (d_currentEntry == DISPLAY_OLDEST)
			return(true);
		else if (d_currentEntry == d_bottomEntry)
			return(d_currentRow > d_bottomRow);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
const char* CDebugDisplay::GetLine(void)
{
	ASSERT(d_locked);
	if (!d_locked || (d_currentEntry == DISPLAY_OLDEST) ||
			((d_currentEntry == d_bottomEntry) && (d_currentRow > d_bottomRow)))
		return(NULL);

	// Establish a chunk of memory for the line. Its size is the maximum size of
	// an entry (256) minus the header (3) plus room for a timestamp
	// (TIMESTAMP_WIDTH), a space (1), a category tag (MAX_CATEGORY_TAG_SIZE),
	// and a lead character (1).
	static char line[255 + TIMESTAMP_WIDTH + MAX_CATEGORY_TAG_SIZE];

	line[0] = '\0';
	if (d_currentEntry != DISPLAY_NEWEST)
	{
		CDebugBuffer* buffer = d_output->GetBuffer();
		if (d_currentRow == 0)
		{
			if (d_showTimestamps && d_output->IsTimestampFunctionSet())
			{
				int32 timestamp = buffer->GetTimestamp(d_currentEntry);
				if (timestamp != EMPTY_TIMESTAMP)
				{
					sprintf(line, "%*.3f", TIMESTAMP_WIDTH,
							(float)timestamp * 0.001f);
				}
				else
					sprintf(line, "%*s", TIMESTAMP_WIDTH, " ");
				if (d_showCategories)
					strcat(line, " ");
			}
			if (d_showCategories)
			{
				strcat(line,
						d_output->GetCategoryTag(buffer->GetCategory(d_currentEntry)));
			}
			strcat(line, ">");
		}
		else
		{
			if (d_showTimestamps && d_output->IsTimestampFunctionSet())
			{
				sprintf(line, "%*s", TIMESTAMP_WIDTH, " ");
				if (d_showCategories)
					strcat(line, " ");
			}
			if (d_showCategories)
			{
				strcat(line, d_output->GetCategoryTag(NO_CATEGORY));
			}
			strcat(line, " ");
		}
		int lead = strlen(line);
		const char* content = buffer->GetContent(d_currentEntry);
		strncat(line, content + d_currentRow * d_contentLength, d_contentLength);
		line[lead + d_contentLength] = '\0';
	}
	else if (d_currentRow == 0)
	{
		if (d_showTimestamps && d_output->IsTimestampFunctionSet())
		{
			int32 timestamp = d_output->GetTimestamp();
			sprintf(line, "%*.3f", TIMESTAMP_WIDTH, (float)timestamp * 0.001f);
			if (d_showCategories)
				strcat(line, " ");
		}
		if (d_showCategories)
			strcat(line, d_output->GetCategoryTag(NO_CATEGORY));
		strcat(line, ">");
	}
	return(line);
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::SetVerbosity(EDebugVerbosity i_verbosity)
{
	if (!d_locked)
		d_verbosity = i_verbosity;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::SelectCategory(uint8 i_id)
{
	if (d_locked || (d_output == NULL))
		return;

	int numCategories = d_output->GetNumCategories() - 2;
	if (i_id < numCategories)
		d_categorySelected[i_id] = true;
	else if ((i_id == numCategories) || (i_id == NO_CATEGORY))
		d_categorySelected[NO_CATEGORY] = true;
	else if ((i_id == numCategories + 1) || (i_id == UNKNOWN_CATEGORY))
		d_categorySelected[UNKNOWN_CATEGORY] = true;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DeselectCategory(uint8 i_id)
{
	if (d_locked || (d_output == NULL))
		return;

	int numCategories = d_output->GetNumCategories() - 2;
	if (d_locked)
		return;

	if (i_id < numCategories)
		d_categorySelected[i_id] = false;
	else if ((i_id == numCategories) || (i_id == NO_CATEGORY))
		d_categorySelected[NO_CATEGORY] = false;
	else if ((i_id == numCategories + 1) || (i_id == UNKNOWN_CATEGORY))
		d_categorySelected[UNKNOWN_CATEGORY] = false;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::SelectAllCategories(void)
{
	if (d_locked)
		return;

	for (int i = 0; i < MAX_CATEGORIES; i++)
		d_categorySelected[i] = true;
	d_categorySelected[NO_CATEGORY] = true;
	d_categorySelected[UNKNOWN_CATEGORY] = true;
}

/******************************************************************************/
/******************************************************************************/
void CDebugDisplay::DeselectAllCategories(void)
{
	if (d_locked)
		return;

	for (int i = 0; i < MAX_CATEGORIES; i++)
		d_categorySelected[i] = false;
	d_categorySelected[NO_CATEGORY] = false;
	d_categorySelected[UNKNOWN_CATEGORY] = false;
}

/******************************************************************************/
/******************************************************************************/
bool CDebugDisplay::IsCategorySelected(uint8 i_id)
{
	if (d_output != NULL)
	{
		int numCategories = d_output->GetNumCategories() - 2;
		if (i_id < numCategories)
			return(d_categorySelected[i_id]);
		if ((i_id == numCategories) || (i_id == NO_CATEGORY))
			return(d_categorySelected[NO_CATEGORY]);
		if ((i_id == numCategories + 1) || (i_id == UNKNOWN_CATEGORY))
			return(d_categorySelected[UNKNOWN_CATEGORY]);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugDisplay::GetNextSelectedEntry(uint32 i_start)
{
	ASSERT((d_output != NULL) && (d_output->GetBuffer() != NULL));
	CDebugBuffer* buffer = d_output->GetBuffer();
	while((i_start = buffer->GetNextEntry(i_start)) != INVALID_ENTRY)
	{
		uint8 category = buffer->GetCategory(i_start);
		EDebugVerbosity verbosity = buffer->GetVerbosity(i_start);
		ASSERT((category < (d_output->GetNumCategories() - 2)) ||
				(category == NO_CATEGORY) || (category == UNKNOWN_CATEGORY));
		ASSERT(verbosity < DEBUG_INVALID);
		if (d_categorySelected[category] && (verbosity >= d_verbosity))
			return(i_start);
	}
	return(INVALID_ENTRY);
}

/******************************************************************************/
/******************************************************************************/
uint32 CDebugDisplay::GetPrevSelectedEntry(uint32 i_start)
{
	ASSERT((d_output != NULL) && (d_output->GetBuffer() != NULL));
	CDebugBuffer* buffer = d_output->GetBuffer();
	while((i_start = buffer->GetPrevEntry(i_start)) != INVALID_ENTRY)
	{
		uint8 category = buffer->GetCategory(i_start);
		EDebugVerbosity verbosity = buffer->GetVerbosity(i_start);
		ASSERT((category < (d_output->GetNumCategories() - 2)) ||
				(category == NO_CATEGORY) || (category == UNKNOWN_CATEGORY));
		ASSERT(verbosity < DEBUG_INVALID);
		if (d_categorySelected[category] && (verbosity >= d_verbosity))
			return(i_start);
	}
	return(INVALID_ENTRY);
}

#else  // !DEBUG_OUTPUT_ENABLED

CDebugOutput g_debugOutput;

#endif // DEBUG_OUTPUT_ENABLED
