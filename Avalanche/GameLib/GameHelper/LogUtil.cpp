/*
**
**  File:   Log.h
**  Date:   November 24, 2004
**  By:     Bryant Collard
**  Desc:   Utility to support data logging.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/LogUtil.cpp $
**  $Revision: #8 $
**  $DateTime: 2005/08/24 11:00:40 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/LogUtil.h"

CLogUtil g_logUtil;

/////////////////////////////////// CLogUtil ///////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CLogUtil::CLogUtil()
{
	d_enabled = false;
	d_path[0] = '\0';
	d_dir[0] = '\0';
	d_prefix[0] = '\0';
}

/******************************************************************************/
/******************************************************************************/
bool CLogUtil::SetPath(const char* i_path)
{
	char path[MAX_LOG_NAME_LEN] = "";
	switch(Append(path, i_path, '\\'))
	{
	 case APPEND_SUCCESS:
		if (strlen(path) + strlen(d_dir) + strlen(d_prefix) <
				(MAX_LOG_NAME_LEN - 1))
		{
			strcpy(d_path, path);
			return(true);
		}
		ASSERTF(false, ("Base log path (%s%s%s) too long", i_path, d_dir,
				d_prefix));
		break;

	 case APPEND_INVALID:
		ASSERTF(false, ("Invalid log path: %s", i_path));
		break;

	 case APPEND_TOO_LONG:
		ASSERTF(false, ("Log path too long: %s", i_path));
		break;
	}

	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CLogUtil::SetDirectory(const char* i_dir)
{
	char dir[MAX_LOG_NAME_LEN] = "";
	switch(Append(dir, i_dir, '\\'))
	{
	 case APPEND_SUCCESS:
		if (strlen(d_path) + strlen(dir) + strlen(d_prefix) <
				(MAX_LOG_NAME_LEN - 1))
		{
			strcpy(d_dir, dir);
			return(true);
		}
		ASSERTF(false, ("Base log path (%s%s%s) too long", d_path, i_dir,
				d_prefix));
		break;

	 case APPEND_INVALID:
		ASSERTF(false, ("Invalid log directory: %s", i_dir));
		break;

	 case APPEND_TOO_LONG:
		ASSERTF(false, ("Log directory too long: %s", i_dir));
		break;
	}

	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CLogUtil::SetPrefix(const char* i_prefix)
{
	char prefix[MAX_LOG_NAME_LEN] = "";
	switch(Append(prefix, i_prefix, '_'))
	{
	 case APPEND_SUCCESS:
		if (strlen(d_path) + strlen(d_dir) + strlen(prefix) <
				(MAX_LOG_NAME_LEN - 1))
		{
			strcpy(d_prefix, prefix);
			return(true);
		}
		ASSERTF(false, ("Base log path (%s%s%s) too long", d_path, d_dir,
				i_prefix));
		break;

	 case APPEND_INVALID:
		ASSERTF(false, ("Invalid log prefix: %s", i_prefix));
		break;

	 case APPEND_TOO_LONG:
		ASSERTF(false, ("Log prefix too long: %s", i_prefix));
		break;
	}

	return(false);
}

/******************************************************************************/
/******************************************************************************/
const char* CLogUtil::MakeFullName(const char* i_name)
{
	static char name[MAX_LOG_NAME_LEN];
	name[0] = '\0';
#if LOG_TYPE == USE_STDIO
	strcat(name, d_path);
	strcat(name, d_dir);
#elif LOG_TYPE == USE_FILESTREAM
	strcat(name, d_dir);
#endif
	strcat(name, d_prefix);

	switch(Append(name, i_name, '\0'))
	{
	 case APPEND_SUCCESS:
#if LOG_TYPE != USE_STDIO
		if (((name[0] != '\0') && (name[1] != '\0')) &&
				(((name[0] == '\\') && (name[1] == '\\')) || (name[1] == ':')))
		{
			ASSERTF(false, ("Invalid log file name for platform: %s", i_name));
			break;
		}
#endif
		return(name);

	 case APPEND_INVALID:
		ASSERTF(false, ("Invalid log file name: %s", i_name));
		break;

	 case APPEND_TOO_LONG:
		ASSERTF(false,
				("Log path (%s), directory (%s) and name (%s) combined too long",
				d_path, d_dir, i_name));
		break;
	}

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CLogUtil::EAppendResult CLogUtil::Append(char* io_dest, const char* i_src,
		const char i_terminate)
{
	// Make sure size limits are not exceeded.
	int i = strlen(io_dest);
	if ((i + ((i_src == NULL) ? 0 : strlen(i_src)) +
			(i_terminate == '\0') ? 0 : 1) >= MAX_LOG_NAME_LEN)
		return(APPEND_TOO_LONG);

	// If anything is in io_dest at all, it has at least 2 characters, a name
	// and a terminating '\'. Also, the only place a ':' can be is at io_dest[1].
	// So, assume that if they exist io_dest[0] and io_dest[1] have already been
	// validated. Make sure that a ':' does not appear anywhere esle.
	for (int j = 2; j < i; j++)
	{
		if (io_dest[j] == ':')
			return(APPEND_INVALID);
	}

	if ((i_src != NULL) && (i_src[0] != '\0'))
	{
		const byte* p = (const byte*)i_src;
		while(i < MAX_LOG_NAME_LEN)
		{
			if (*p == '\0')
			{
				if (io_dest[i - 1] == '\\')
					i--;
				if ((i_terminate != '\0') &&
						((i_terminate != '\\') || (i > 0)))
					io_dest[i++] = i_terminate;
				io_dest[i] = '\0';
				return(APPEND_SUCCESS);
			}
			if ((*p < ' ') || (*p == '\"') || (*p == '*') || (*p == '<') ||
					(*p == '>') || (*p == '?') || (*p == '|') || (*p == 127) ||
					((*p == ':') && (i != 1)))
				return(APPEND_INVALID);
			if (*p == '/')
				io_dest[i++] = '\\';
			else if (*p > 127)
				io_dest[i++] = '_';
			else
				io_dest[i++] = (char)(*p);
			p++;
		}
		return(APPEND_TOO_LONG);
	}
	return(APPEND_SUCCESS);
}

/////////////////////////////////// CLogFile ///////////////////////////////////

#if LOG_TYPE == USE_STDIO

#define FP d_file,
#define OPEN_LOG(n) fopen(n, "w")
#define CLOSE_LOG() fclose(d_file)
#define PRINT_FIELD(a) fprintf a
#define PRINT_RECORD() fprintf(d_file, "\n"); fflush(d_file)

#elif LOG_TYPE == USE_FILESTREAM

#define FP
#define OPEN_LOG(n) FileStream::NewAndOpen(n, FileStream::WRITE, true)
#define CLOSE_LOG() d_file->Close(); delete d_file
#define PRINT_FIELD(a) d_file->Print a
#define PRINT_RECORD() d_file->Print("\n"); d_file->Flush()

#elif LOG_TYPE == USE_EMAIL

#define FP d_file,
#define OPEN_LOG(n) EmailOpen(n)
#define CLOSE_LOG() EmailClose()
#define PRINT_FIELD(a) sprintf a; EmailField()
#define PRINT_RECORD() EmailRecord()

#else

#define FP 
#define OPEN_LOG(n)
#define CLOSE_LOG()
#define PRINT_FIELD(a)
#define PRINT_RECORD()

#endif

#if LOG_TYPE != USE_NONE

/******************************************************************************/
/******************************************************************************/
CLogFile::CLogFile()
{
	d_state = LOG_CLOSED;
	d_numFields = 0;
	d_field = NULL;
	d_recordNumber = -1;
	d_recordNumberLabel = NULL;
	d_file = NULL;
}

/******************************************************************************/
/******************************************************************************/
CLogFile::~CLogFile()
{
	Close();
}

/******************************************************************************/
// Open a file and prep for logging.
/******************************************************************************/
bool CLogFile::Open(const char* i_name, uint i_numFields, bool i_incRecNum)
{
	// Make sure inputs are valid.
	if ((i_name == NULL) || (i_name[0] == '\0') || (i_numFields <= 0))
	{
		ASSERT(false);
		d_state = LOG_INVALID;
		return(false);
	}

	// Make sure we are clean.
	Close();

	// Open a file.
	d_file = OPEN_LOG(i_name);
	if (d_file == NULL)
	{
		ASSERTF(false, ("Unable to open log file: %s", i_name));
		d_state = LOG_INVALID;
		return(false);
	}

	// Allocate field array.
	d_field = new SField[i_numFields];
	if (d_field == NULL)
	{
		ASSERTF(false, ("Unable to allocate space for log file: %s", i_name));
		CLOSE_LOG();
		d_file = NULL;
		d_state = LOG_INVALID;
		return(false);
	}

	// Initialize members.
	d_state = LOG_OPEN;
	d_numFields = i_numFields;
	for (uint i = 0; i < d_numFields; i++)
	{
		d_field[i].d_type = FIELD_NONE;

		// Initialize string storage. Note that a 1 character string could only
		// hold the terminating zero character so it is not useful; therefore,
		// allocated strings will always have a size of at least 2. By setting the
		// size of an unallocated string to 1 even though it is actully 0, memory
		// can be better managed.
		d_field[i].d_stringSize = 1;
		d_field[i].d_stringValue = NULL;
	}

	// Set up record number field.
	d_recordNumber = i_incRecNum ? 0 : -1;

	return(true);
}

/******************************************************************************/
// Cleanup
/******************************************************************************/
void CLogFile::Close(void)
{
	// Clean up field array.
	if (d_field != NULL)
	{
		for (uint i = 0; i < d_numFields; i++)
		{
			delete[] d_field[i].d_stringValue;
		}
		delete[] d_field;
		d_field = NULL;
	}

	// Reset variables.
	d_state = LOG_CLOSED;
	d_numFields = 0;
	d_recordNumber = -1;
	delete[] d_recordNumberLabel;
	d_recordNumberLabel = NULL;

	// Close file.
	if (d_file != NULL)
	{
		CLOSE_LOG();
		d_file = NULL;
	}
}

/******************************************************************************/
// If not open, open a file and prep for logging.
/******************************************************************************/
bool CLogFile::Reopen(const char* i_name, uint i_numFields, bool i_incRecNum)
{
	if (d_state == LOG_CLOSED)
		return(Open(i_name, i_numFields, i_incRecNum));
	return(d_state != LOG_INVALID);
}

/******************************************************************************/
// Cleanup if not closed.
/******************************************************************************/
void CLogFile::Reclose(void)
{
	if (d_state != LOG_CLOSED)
		Close();
}

/******************************************************************************/
// Write a label in the record number field (if there is one), instead of
// the record number.
/******************************************************************************/
void CLogFile::LabelRecordNumberField(const char* i_label)
{
	if (d_recordNumber >= 0)
	{
		delete[] d_recordNumberLabel;
		if ((i_label != NULL) && (i_label[0] != '\0'))
		{
			d_recordNumberLabel = new char[strlen(i_label) + 1];
			ASSERT_PTR(d_recordNumberLabel);
			strcpy(d_recordNumberLabel, i_label);
		}
		else
			d_recordNumberLabel = NULL;
	}
}

/******************************************************************************/
// Set a field to an integer value.
/******************************************************************************/
void CLogFile::SetField(uint i_index, int i_value)
{
	if (i_index >= d_numFields)
	{
		ASSERT(false);
		return;
	}
	d_field[i_index].d_type = FIELD_INT;
	d_field[i_index].d_intValue = i_value;
}

/******************************************************************************/
// Set a field to an unsigned integer value. Output will be in hex.
/******************************************************************************/
void CLogFile::SetField(uint i_index, uint i_value)
{
	if (i_index >= d_numFields)
	{
		ASSERT(false);
		return;
	}
	d_field[i_index].d_type = FIELD_HEX_UINT;
	d_field[i_index].d_uintValue = i_value;
}

/******************************************************************************/
// Set a field to a floating point value.
/******************************************************************************/
void CLogFile::SetField(uint i_index, float i_value)
{
	if (i_index >= d_numFields)
	{
		ASSERT(false);
		return;
	}
	d_field[i_index].d_type = FIELD_FLOAT;
	d_field[i_index].d_floatValue = i_value;
}

/******************************************************************************/
// Set a field to character string.
/******************************************************************************/
void CLogFile::SetField(uint i_index, const char* i_value, int i_indentSize,
		const char i_indentChar)
{
	if (i_index >= d_numFields)
	{
		ASSERT(false);
		return;
	}
	d_field[i_index].d_type = FIELD_STRING;

	// Make sure the string is big enough to receive its value. Note that since
	// the string size can only grow and it is initialized to 1, the size of an
	// allocated string will always be at least 2.
	uint size = i_indentSize + 1;
	if (i_value != NULL)
		size += strlen(i_value);
	if (size > d_field[i_index].d_stringSize)
	{
		delete[] d_field[i_index].d_stringValue;
		d_field[i_index].d_stringValue = new char[size];
		if (d_field[i_index].d_stringValue == NULL)
		{
			ASSERT(false);
			return;
		}
		d_field[i_index].d_stringSize = size;
	}

	// Fill in the string.
	if (d_field[i_index].d_stringValue != NULL)
	{
		int i;
		for (i = 0; i < i_indentSize; i++)
			d_field[i_index].d_stringValue[i] = i_indentChar;
		d_field[i_index].d_stringValue[i] = '\0';
		if (i_value != NULL)
			strcat(d_field[i_index].d_stringValue, i_value);
	}
}

/******************************************************************************/
// Output the current field values.
/******************************************************************************/
void CLogFile::Print(void)
{
	if (d_state == LOG_OPEN)
	{
		if (d_recordNumber >= 0)
		{
			if (d_recordNumberLabel != NULL)
			{
				PRINT_FIELD((FP "%s", d_recordNumberLabel));
				delete[] d_recordNumberLabel;
				d_recordNumberLabel = NULL;
			}
			else
				PRINT_FIELD((FP "%d", d_recordNumber));
			d_recordNumber++;
			if (d_numFields > 0)
				PRINT_FIELD((FP "\t"));
		}
		for (uint i = 0; i < d_numFields; i++)
		{
			switch(d_field[i].d_type)
			{
			 case FIELD_INT:
				PRINT_FIELD((FP "%d", d_field[i].d_intValue));
				break;
			 case FIELD_HEX_UINT:
				PRINT_FIELD((FP "0x%x", d_field[i].d_uintValue));
				break;
			 case FIELD_FLOAT:
				PRINT_FIELD((FP "%f", d_field[i].d_floatValue));
				break;
			 case FIELD_STRING:
				if (d_field[i].d_stringValue != NULL)
					PRINT_FIELD((FP "%s", d_field[i].d_stringValue));
				break;
			}
			if (i < (d_numFields - 1))
				PRINT_FIELD((FP "\t"));
			d_field[i].d_type = FIELD_NONE;
		}
		PRINT_RECORD();
	}
}

#if LOG_TYPE == USE_EMAIL

/******************************************************************************/
/******************************************************************************/
char* CLogFile::EmailOpen(const char* i_name)
{
	if (d_email.Begin("ttl@avalanchesoftware.com",
			"bcollard@avalanchesoftware.com", "Log file"))
	{
		d_email.Attachment(i_name, "text/plain", false, false);

		d_record[0] = '\0';

		static char buffer[32];
		return(buffer);
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CLogFile::EmailClose(void)
{
	d_email.End();
}

/******************************************************************************/
/******************************************************************************/
void CLogFile::EmailField(void)
{
	if((strlen(d_record) + strlen(d_file) + 1) >= MAX_RECORD_SIZE)
	{
		ASSERT(false);
		return;
	}
	strcat(d_record, d_file);
}

/******************************************************************************/
/******************************************************************************/
void CLogFile::EmailRecord(void)
{
	d_email.AppendLine(d_record);
	d_record[0] = '\0';
}

#endif

#endif //LOG_TYPE != USE_NONE
