/*
**
**  File:   DebugOutput.h
**  Date:   March 29, 2005
**  By:     Bryant Collard
**  Desc:   Buffer that holds text that can be output on some type of console.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/DebugOutput.h $
**  $Revision: #19 $
**  $DateTime: 2005/08/15 10:00:28 $
**    $Author: Ken_Harward $
**
*/

#pragma once

#ifndef DEBUG_OUTPUT_H
#define DEBUG_OUTPUT_H

// Gamelib includes
#include "EngineHelper/Singleton.h"

// Engine headers
#include "Layers/EMail/Email.h"

#ifdef WIN32
#define VARARGCDECL __cdecl
#else
#define VARARGCDECL
#endif

typedef int32 (*TimestampFunc)();

enum EDebugVerbosity
{
	DEBUG_BORE = 0,
	DEBUG_INFORM,
	DEBUG_NOTIFY,
	DEBUG_ALERT,
	DEBUG_INVALID
};

#ifndef CONSUMER_BUILD
#define DEBUG_OUTPUT_ENABLED
#endif

#define INVALID_ENTRY           ((uint32)(~0))
#define INVALID_CATEGORY        ((uint8)(~0))

#ifdef DEBUG_OUTPUT_ENABLED

#ifndef CDROM
#define LOG_DEBUG_OUTPUT
#endif

#ifdef LOG_DEBUG_OUTPUT
#include "GameHelper/LogUtil.h"
#endif

#define EMPTY_TIMESTAMP         MAXINT
#define MAX_CATEGORIES          15           // Must be <= 30
#define MAX_CATEGORY_TAG_SIZE   6

///////////////////////////////// CDebugBuffer /////////////////////////////////

class CDebugBuffer
{
  public:
	virtual ~CDebugBuffer() {}
	void Reset(void);
	uint32 AddEntry(uint8 i_category, EDebugVerbosity i_verbosity,
			int32 i_timestamp, int i_contentLength, char* &o_destBuffer,
			uint32 &o_destCount);
	uint32 AddEntry(uint8 i_category, EDebugVerbosity i_verbosity,
			int i_contentLength, char* &o_destBuffer, uint32 &o_destCount);
	uint32 AddEntry(uint8 i_category, EDebugVerbosity i_verbosity,
			int32 i_timestamp, const char* i_content);
	uint32 AddEntry(uint8 i_category, EDebugVerbosity i_verbosity,
			const char* i_content);
	uint32 GetOldestEntry(void) const;
	uint32 GetNewestEntry(void) const;
	uint32 GetNextEntry(uint32 i_start = INVALID_ENTRY) const;
	uint32 GetPrevEntry(uint32 i_start = INVALID_ENTRY) const;
	int TestEntry(uint32 i_entry) const;
	uint8 GetCategory(uint32 i_entry) const;
	EDebugVerbosity GetVerbosity(uint32 i_entry) const;
	int32 GetTimestamp(uint32 i_entry) const;
	uint8 GetLength(uint32 i_entry) const;
	const char* GetContent(uint32 i_entry) const;

  protected:
	CDebugBuffer() {}

	char* d_buffer;
	uint32 d_bufSize;

  private:
	uint32 GetIndex(uint32 i_entry) const;
	uint32 GetEntry(uint32 i_index) const;
	inline uint8 GetSize(uint32 i_index) const
		{return(*((uint8*)&d_buffer[i_index]));}
	inline uint8 GetPrevSize(uint32 i_index) const
		{return(*((uint8*)&d_buffer[i_index + 1]));}

	uint32 d_oldestIndex;
	uint32 d_newestIndex;
	uint32 d_bufferEnd;
	uint32 d_iteration;
	uint32 d_lastIteration;
};

/******************************************************************************/
/******************************************************************************/
inline uint32 CDebugBuffer::AddEntry(uint8 i_category,
		EDebugVerbosity i_verbosity, int i_contentLength, char* &o_destBuffer,
		uint32 &o_destCount)
{
	return(AddEntry(i_category, i_verbosity, EMPTY_TIMESTAMP, i_contentLength,
			o_destBuffer, o_destCount));
}

/******************************************************************************/
/******************************************************************************/
inline uint32 CDebugBuffer::AddEntry(uint8 i_category,
		EDebugVerbosity i_verbosity, int32 i_timestamp, const char* i_content)
{
	char* buffer;
	uint32 count = 0;
	if (i_content != NULL)
		count = strlen(i_content);

	uint32 entry = AddEntry(i_category, i_verbosity, i_timestamp, count, buffer,
			count);

	if (count > 0)
		strncpy(buffer, i_content, count);

	return(entry);
}

/******************************************************************************/
/******************************************************************************/
inline uint32 CDebugBuffer::AddEntry(uint8 i_category,
		EDebugVerbosity i_verbosity, const char* i_content)
{
	return(AddEntry(i_category, i_verbosity, EMPTY_TIMESTAMP, i_content));
}

/******************************************************************************/
/******************************************************************************/
inline uint32 CDebugBuffer::GetOldestEntry(void) const
{
	if (d_newestIndex >= d_oldestIndex)
		return(d_oldestIndex + d_iteration);
	return(d_oldestIndex + d_lastIteration);
}

/******************************************************************************/
/******************************************************************************/
inline uint32 CDebugBuffer::GetNewestEntry(void) const
{
	return(d_newestIndex + d_iteration);
}

/******************************************************************************/
/******************************************************************************/
inline int CDebugBuffer::TestEntry(uint32 i_entry) const
{
	if ((d_buffer == NULL) || (d_newestIndex == INVALID_ENTRY) ||
			(i_entry == INVALID_ENTRY) || (i_entry < GetOldestEntry()))
		return(-1);
	return((i_entry > GetNewestEntry()) ? 1 : 0);
}

/////////////////////////////// CFixedDebugBuffer //////////////////////////////

template<uint32 BUF_SIZE>
class CFixedDebugBuffer : public CDebugBuffer
{
  public:
	CFixedDebugBuffer()
	{
		ASSERT(BUF_SIZE >= 0xff);
		d_buffer = d_fixedBuffer;
		d_bufSize = BUF_SIZE;
		Reset();
	}

  private:
	char d_fixedBuffer[BUF_SIZE];
};

///////////////////////////// CVariableDebugBuffer /////////////////////////////

class CVariableDebugBuffer : public CDebugBuffer
{
  public:
	CVariableDebugBuffer();
	virtual ~CVariableDebugBuffer();
	bool Configure(uint32 i_size);
};

///////////////////////////////// CDebugOutput /////////////////////////////////

class CDebugOutput
{
  public:
	// Construct and configure.
	CDebugOutput();
	CDebugOutput(CDebugBuffer* i_buffer);
	void SetBuffer(CDebugBuffer* i_buffer);
	void ClearBuffer(void);
	void ClearCategories(void);
	void ClearAll(void);
	void SetTimestampFunction(TimestampFunc i_timestampFunc);

	// Get configuration.
	CDebugBuffer* GetBuffer(void) {return(d_buffer);}
	bool IsTimestampFunctionSet(void) {return(d_timestampFunc != NULL);}
	int32 GetTimestamp(void);

	// Add/remove lock.
	void Lock(void);
	void Unlock(void);

	// Get category info.
	int GetMaximumTagLength(void) const {return(MAX_CATEGORY_TAG_SIZE - 1);}
	int GetLongestTagLength(void) const {return(d_tagLength);}
	int GetLongestNameLength(void) const {return(d_nameLength);}
	int GetNumCategories(void) const {return(d_nextCategory + 2);}
	const char* GetCategoryName(uint8 i_id) const;
	const char* GetCategoryTag(uint8 i_id) const;

	// Register a category. NOTE: only a pointer to the name is stored. The
	// caller is responsible for ensuring that the pointer remains valid. This
	// is done to avoid any dynamic memory allocation and avoid problems with
	// transitions between loops. Simply using a quoted string will work. The
	// tag string is stored.
	uint8 RegisterCategory(const char* i_name, const char* i_tag);
	uint8 Category(const char* i_tag) const;

	// Print to the buffer.
	uint32 VARARGCDECL Print(const char* i_format, ...);
	uint32 VARARGCDECL Print(uint8 i_category, const char* i_format, ...);
	uint32 VARARGCDECL Print(EDebugVerbosity i_verbosity,
			const char* i_format, ...);
	uint32 VARARGCDECL Print(uint8 i_category, EDebugVerbosity i_verbosity,
			const char* i_format, ...);
	uint32 VARARGCDECL Print(bool i_timestamp, const char* i_format, ...);
	uint32 VARARGCDECL Print(bool i_timestamp, uint8 i_category,
			const char* i_format, ...);
	uint32 VARARGCDECL Print(bool i_timestamp, EDebugVerbosity i_verbosity,
			const char* i_format, ...);
	uint32 VARARGCDECL Print(bool i_timestamp, uint8 i_category,
			EDebugVerbosity i_verbosity, const char* i_format, ...);
	uint32 VARARGCDECL TSPrint(const char* i_format, ...);
	uint32 VARARGCDECL TSPrint(uint8 i_category, const char* i_format, ...);
	uint32 VARARGCDECL TSPrint(EDebugVerbosity i_verbosity,
			const char* i_format, ...);
	uint32 VARARGCDECL TSPrint(uint8 i_category, EDebugVerbosity i_verbosity,
			const char* i_format, ...);

	// Email buffer.
	void EmailBuffer(const char* i_sender,
			const char* i_recipient, const char* i_subject) const;
	// attach buffer to existing email (and optionally put the descriptive line within the attachment)
	void AttachBufferToEmail(EMail* email, bool descriptiveLine = true) const;
	// append buffer description line
	void AppendBufferDescriptionToEmail(EMail* email) const;

	// Enable/disable logging of debug output.
	void EnableLog(bool i_writeLog);
	void ToggleLog(void);

  private:
	void Construct(void);
	uint8 ValidateCategory(uint8 i_category) const;
	uint32 Add(int i_category, EDebugVerbosity i_verbosity, bool i_timestamp,
			const char* i_format, va_list i_args);

	CDebugBuffer* d_buffer;
	TimestampFunc d_timestampFunc;
	uint8 d_nextCategory;
	int d_tagLength;
	int d_nameLength;
	struct
	{
		char d_tag[MAX_CATEGORY_TAG_SIZE];
		const char* d_name;
	} d_category[MAX_CATEGORIES + 2];
	int d_lockCount;;

#ifdef LOG_DEBUG_OUTPUT
	void LogEntry(uint32 i_entry);
	bool d_logEnabled;
	CLogFile d_logFile;
#endif
};

DefineSingleton(CDebugOutput);
#define g_debugOutput GetSingleton(CDebugOutput)

//////////////////////////////// CDebugDisplay /////////////////////////////////

class CDebugDisplay
{
  public:
	// Construct
	CDebugDisplay();
	CDebugDisplay(CDebugOutput* i_output);
	void SetOutput(CDebugOutput* i_output);

	// Configure
	void DescribeDisplay(int i_lines, int i_cols);
	void SetWrap(bool i_wrap) {if (!d_locked) d_wrap = i_wrap;}
	void SetShowCategories(bool i_show) {if (!d_locked) d_showCategories = i_show;}
	void SetShowTimestamps(bool i_show) {if (!d_locked) d_showTimestamps = i_show;}
	bool ToggleWrap(void) {if (!d_locked) d_wrap = !d_wrap; return(d_wrap);}
	bool ToggleShowCategories(void);
	bool ToggleShowTimestamps(void);
	bool GetWrap(void) {return(d_wrap);}
	bool GetShowCategories(void) {return(d_showCategories);}
	bool GetShowTimestamps(void) {return(d_showTimestamps);}

	// Manage
	void DisplayNewest(int i_offset = 0);
	void DisplayOldest(int i_offset = 0);
	void DisplayEntry(uint32 i_entry, int i_offset = 0);
	void DisplayLineUp(void) {d_dispOffset--;}
	void DisplayLineDown(void) {d_dispOffset++;}
	void DisplayPageUp(void) {d_dispOffset -= d_dispLines;}
	void DisplayPageDown(void) {d_dispOffset += d_dispLines;}

	// Retrieve buffer contents formatted for display. When the buffer is locked,
	// current configuration and display location are used to format all output.
	// Location state can be changed while the buffer is locked but will not have
	// any affect until the buffer is unlocked and locked again.  All
	// configuration, category selection and print commands will be ignored while
	// the buffer is locked.  While locked, the class has a concept of a "current
	// line". GotoTop() and GotoBottom() will make the top and bottom lines the
	// current line respectively. The increment operators will select lines
	// closer to the top (--) or bottom (++). If you increment past the top or
	// bottom, PastEnds() will return true and GetLine() will return NULL.
	void LockBuffer(void);
	void UnlockBuffer(void);
	void GotoTop(void);
	void GotoBottom(void);
	void operator--();
	void operator++();
	bool PastEnds(void);
	const char* GetLine(void);

	// Manage verbosity.
	void SetVerbosity(EDebugVerbosity i_verbosity);
	EDebugVerbosity GetVerbosity(void) {return(d_verbosity);}

	// Manage categories.
	void SelectCategory(uint8 i_id);
	void DeselectCategory(uint8 i_id);
	void SelectAllCategories(void);
	void DeselectAllCategories(void);
	bool IsCategorySelected(uint8 i_id);

  private:
	void Construct(void);
	uint32 GetNextSelectedEntry(uint32 i_start);
	uint32 GetPrevSelectedEntry(uint32 i_start);
	int GetRows(uint32 i_entry)
	{
		uint8 length = d_output->GetBuffer()->GetLength(i_entry);
		if (d_wrap && (length > 0))
			return(((length - 1) / d_contentLength) + 1);
		return(1);
	}

	CDebugOutput* d_output;
	bool d_locked;
	int d_dispLines;
	int d_dispCols;
	bool d_wrap;
	bool d_showCategories;
	bool d_showTimestamps;
	uint32 d_dispAnchorEntry;
	uint8 d_dispAnchorRow;
	int d_dispOffset;
	EDebugVerbosity d_verbosity;
	bool d_categorySelected[MAX_CATEGORIES + 2];
	int d_contentLength;
	uint32 d_topEntry;
	uint8 d_topRow;
	uint32 d_bottomEntry;
	uint8 d_bottomRow;
	uint32 d_currentEntry;
	uint8 d_currentRow;
};

#else  // !DEBUG_OUTPUT_ENABLED

class CDebugBuffer;

class CDebugOutput
{
  public:
	CDebugOutput() {}
	CDebugOutput(CDebugBuffer* i_buffer) {}
	void SetBuffer(CDebugBuffer* i_buffer) {}
	void ClearBuffer(void) {}
	void ClearCategories(void) {}
	void ClearAll(void) {}
	void SetTimestampFunction(TimestampFunc i_timestampFunc) {}
	CDebugBuffer* GetBuffer(void) {return(NULL);}
	bool IsTimestampFunctionSet(void) {return(false);}
	int32 GetTimestamp(void) {return(0);}
	void Lock(void) {}
	void Unlock(void) {}
	int GetMaximumTagLength(void) const {return(0);}
	int GetLongestTagLength(void) const {return(0);}
	int GetLongestNameLength(void) const {return(0);}
	int GetNumCategories(void) const {return(0);}
	const char* GetCategoryName(uint8 i_id) const {return(NULL);}
	const char* GetCategoryTag(uint8 i_id) const {return(NULL);}
	EDebugVerbosity GetVerbosity(uint32 i_entry) {return(DEBUG_INVALID);}
	uint8 RegisterCategory(const char* i_name, const char* i_tag)
		{return(INVALID_CATEGORY);}
	uint8 Category(const char* i_tag) const {return(INVALID_CATEGORY);}
	uint32 VARARGCDECL Print(const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(uint8 i_category, const char* i_format, ...)
		{return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(EDebugVerbosity i_verbosity,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(uint8 i_category, EDebugVerbosity i_verbosity,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(bool i_timestamp, const char* i_format, ...)
		{return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(bool i_timestamp, uint8 i_category,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(bool i_timestamp, EDebugVerbosity i_verbosity,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL Print(bool i_timestamp, uint8 i_category,
			EDebugVerbosity i_verbosity, const char* i_format, ...)
		{return(INVALID_ENTRY);}
	uint32 VARARGCDECL TSPrint(const char* i_format, ...)
		{return(INVALID_ENTRY);}
	uint32 VARARGCDECL TSPrint(uint8 i_category, const char* i_format, ...)
		{return(INVALID_ENTRY);}
	uint32 VARARGCDECL TSPrint(EDebugVerbosity i_verbosity,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	uint32 VARARGCDECL TSPrint(uint8 i_category, EDebugVerbosity i_verbosity,
			const char* i_format, ...) {return(INVALID_ENTRY);}
	void EmailBuffer(const char* i_sender, const char* i_recipient,
			const char* i_subject) const {}
	void EnableLog(bool i_writeLog) {}
	void ToggleLog(void) {}
};

class CDebugDisplay
{
  public:
	CDebugDisplay() {}
	CDebugDisplay(CDebugOutput* i_output) {}
	void SetOutput(CDebugOutput* i_output) {}
	void DescribeDisplay(int i_lines, int i_cols) {}
	void SetWrap(bool i_wrap) {}
	void SetShowCategories(bool i_show) {}
	void ShowCategories(bool i_show) {}
	void ShowTimestamps(bool i_show) {}
	bool ToggleWrap(void) {return true;}
	bool ToggleShowCategories(void) {}
	bool ToggleShowTimestamps(void) {return true;}
	void DisplayNewest(int i_offset = 0) {}
	void DisplayOldest(int i_offset = 0) {}
	void DisplayEntry(uint32 i_entry, int i_offset = 0) {}
	void DisplayLineDown(void) {}
	void DisplayPageUp(void) {}
	void DisplayPageDown(void) {}
	void DisplayLineUp(void) {}
	void LockBuffer(void) {}
	void UnlockBuffer(void) {}
	void GotoTop(void) {}
	void GotoBottom(void) {}
	void operator--() {}
	void operator++() {}
	bool PastEnds(void) {return(true);}
	const char* GetLine(void) {return(NULL);}
	void SetVerbosity(EDebugVerbosity i_verbosity) {}
	EDebugVerbosity GetVerbosity(void) {return(DEBUG_INVALID);}
	void SelectCategory(uint8 i_id) {}
	void DeselectCategory(uint8 i_id) {}
	void SelectAllCategories(void) {}
	void DeselectAllCategories(void) {}
	bool IsCategorySelected(uint8 i_id) {return(false);}
};

extern CDebugOutput g_debugOutput;

#endif // DEBUG_OUTPUT_ENABLED

#endif // DEBUG_OUTPUT_H
