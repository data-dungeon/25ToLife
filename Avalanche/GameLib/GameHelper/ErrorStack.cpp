/*
**
**  File:   ErrorStack.cpp
**  Date:   February 3, 2005
**  By:     Bryant Collard
**  Desc:   Maintains a "stack" of error messages.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/ErrorStack.h"

// Gamelib includes
#include "EngineHelper/timer.h"
#include "GameHelper/DebugOutput.h"

// A stack is always around.
CErrorStack g_errorStack;

// Invalid handle.
const ErrorHandle ERROR_HANDLE_INVALID = (ErrorHandle)0;

// Static functions for the game loops to use.
bool IsErrorStackEmpty(void)
{
	return(g_errorStack.IsEmpty());
}

bool IsErrorStackEmptyOfExpiring(void)
{
	return(g_errorStack.IsEmptyOfExpiring());
}

// An entry.
#define MAX_ACTIONS 3
struct SErrorStackEntry
{
	ErrorHandle d_handle;
	char* d_title;
	char* d_msg;
	char* d_dismissText;
	char* d_action[MAX_ACTIONS];
	char* d_actionMsg[MAX_ACTIONS];
	CErrorStack::ESeverity d_severity;
	uint32 d_expiration;
	SErrorStackEntry* d_prev;
	SErrorStackEntry* d_next;
};

/******************************************************************************/
/******************************************************************************/
CErrorStack::CErrorStack()
{
	d_currHandle = ERROR_HANDLE_INVALID;
	d_head = NULL;
}

/******************************************************************************/
/******************************************************************************/
CErrorStack::~CErrorStack()
{
	ClearAllErrors();
}

/******************************************************************************/
/******************************************************************************/
ErrorHandle CErrorStack::SetError(const char* i_title, const char* i_msg,
		ESeverity i_severity, uint32 i_lifetime, const char* i_dismissText)
{
	// Make sure a valid message is provided.
	if ((i_title == NULL) || (i_title[0] == '\0') || (i_msg == NULL) ||
			(i_msg[0] == '\0') || (i_lifetime < ERROR_MIN_LIFETIME))
		return(ERROR_HANDLE_INVALID);

	// See if an entry already exists.
	SErrorStackEntry* entry = Find(i_title, i_msg);
	if (entry != NULL)
	{
		// Reset the lifetime.
		SetLifetime(entry, i_lifetime);

		// Move it if necessary.
		if (entry->d_severity != i_severity)
		{
			entry->d_severity = i_severity;
			RemoveEntry(entry, false);
			AddEntry(entry);
		}

		// Return the existing entry.
		return(entry->d_handle);
	}

	// Try to allocate a new entry.
	entry = new SErrorStackEntry;
	if (entry == NULL)
		return(ERROR_HANDLE_INVALID);

	// Wipe it clean
	memset(entry, 0, sizeof( SErrorStackEntry));

	// Try to allocate and set the title string.
	entry->d_title = new char[strlen(i_title) + 1];
	if (entry->d_title == NULL)
	{
		delete entry;
		return(ERROR_HANDLE_INVALID);
	}
	strcpy(entry->d_title, i_title);

	// Try to allocate and set the message string.
	entry->d_msg = new char[strlen(i_msg) + 1];
	if (entry->d_msg == NULL)
	{
		delete[] entry->d_title;
		delete entry;
		return(ERROR_HANDLE_INVALID);
	}
	strcpy(entry->d_msg, i_msg);

	// Try to allocate and set the dismiss text, if any.
	if ((i_dismissText != NULL) && (i_dismissText[0] != '\0'))
	{
		entry->d_dismissText = new char[strlen(i_dismissText) + 1];
		if (entry->d_dismissText == NULL)
		{
			delete[] entry->d_title;
			delete[] entry->d_msg;
			delete entry;
			return(ERROR_HANDLE_INVALID);
		}
		strcpy(entry->d_dismissText, i_dismissText);
	}
	else
		entry->d_dismissText = NULL;

	// Finish initialization.
	entry->d_handle = ++d_currHandle;
	entry->d_severity = i_severity;
	SetLifetime(entry, i_lifetime);
	AddEntry(entry);

	// Dump the entry to the debug output just for fun.
	uint8 cat = g_debugOutput.RegisterCategory("Error Stack", "ERR");
	char msg[512];
	strncpy(msg, i_msg, 511);
	msg[511] = '\0';
	char* line = strtok(msg, "\n");
	if (line == NULL)
		g_debugOutput.TSPrint(cat, "Error Stack: <no message>");
	else
	{
		g_debugOutput.TSPrint(cat, "Error Stack: %s", line);
		while((line = strtok(NULL, "\n")) != NULL)
			g_debugOutput.TSPrint(cat, "             %s", line);
	}

	// Use the pointer to the entry as the handle.
	return(entry->d_handle);
}

/******************************************************************************/
/******************************************************************************/
void CErrorStack::SetErrorSeverity(ErrorHandle i_handle, ESeverity i_severity)
{
	SErrorStackEntry* entry = Find(i_handle);
	if (entry != NULL)
	{
		entry->d_severity = i_severity;
	}
}

/******************************************************************************/
/******************************************************************************/
// Allow the popup to receive input from the user and send out a message
// when the user selects that option.
//  i_action is a string that is displayed in the popup that the user can
//      select
//  i_msg is the corresponding message that will be sent by the message
//      dispatcher
void CErrorStack::SetActionMessagePair(ErrorHandle i_handle,
		const char* i_action, const char *i_msg)
{
	SErrorStackEntry* entry = Find(i_handle);
	if (entry != NULL)
	{
		for (int i=0; i<MAX_ACTIONS; i++)
		{
			// find an empty slot
			if (entry->d_action[i] == NULL)
			{
				if (i_action)
				{
					entry->d_action[i] = new char[strlen(i_action) + 1];
					if (entry->d_action[i])
					{
						strcpy(entry->d_action[i], i_action );
					}
					else
					{
						delete [] entry->d_action[i];
						entry->d_action[i] = NULL;
					}
				}
				if (i_msg)
				{
					entry->d_actionMsg[i] = new char[strlen(i_msg) + 1];
					if (entry->d_actionMsg[i])
					{
						strcpy(entry->d_actionMsg[i], i_msg );
					}
					else
					{
						delete [] entry->d_actionMsg[i];
						entry->d_actionMsg[i] = NULL;
					}
				}
				return;
			}
		}
		ASSERTS(false, "Unable to add Action Message Pair, increase MAX_ACTIONS");
	}
}

/******************************************************************************/
/******************************************************************************/
bool CErrorStack::ClearError(ErrorHandle i_handle)
{
	SErrorStackEntry* entry = Find(i_handle);
	if (entry != NULL)
	{
		RemoveEntry(entry, true);
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CErrorStack::ClearError(const char* i_title, const char* i_msg)
{
	SErrorStackEntry* entry = Find(i_title, i_msg);
	if (entry != NULL)
	{
		RemoveEntry(entry, true);
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CErrorStack::ClearAllErrors(void)
{
	while (d_head != NULL)
	{
		SErrorStackEntry* next = d_head->d_next;
		delete[] d_head->d_title;
		delete[] d_head->d_msg;
		delete[] d_head->d_dismissText;
		for (int i=0; i<MAX_ACTIONS; i++)
		{
			delete[] d_head->d_action[i];
			delete[] d_head->d_actionMsg[i];
		}
		delete d_head;
		d_head = next;
	}
}

/******************************************************************************/
/******************************************************************************/
void CErrorStack::ClearAllNonExpiringErrors(void)
{
	SErrorStackEntry* curr = d_head;
	while (curr != NULL)
	{
		SErrorStackEntry* next = curr->d_next;

		if (curr->d_expiration == ERROR_INFINITE_LIFETIME)
			RemoveEntry(curr, true);

		curr = next;
	}
}

/******************************************************************************/
/******************************************************************************/
ErrorHandle CErrorStack::Update(void)
{
	uint32 now = g_timer.GetOSTime();
	SErrorStackEntry* curr = d_head;
	while (curr != NULL)
	{
		SErrorStackEntry* next = curr->d_next;

		if ((curr->d_expiration != ERROR_INFINITE_LIFETIME) &&
				(now >= curr->d_expiration))
			RemoveEntry(curr, true);

		curr = next;
	}

	return((d_head != NULL) ? d_head->d_handle : ERROR_HANDLE_INVALID);
}

/******************************************************************************/
/******************************************************************************/
bool CErrorStack::IsEmptyOfExpiring(void)
{
	if (d_head != NULL)
	{
		SErrorStackEntry* entry = d_head;
		while (entry != NULL)
		{
			if (entry->d_expiration != ERROR_INFINITE_LIFETIME)
				return(false);
			entry = entry->d_next;
		}
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CErrorStack::IsFatal(void)
{
	return((d_head != NULL) && (d_head->d_severity == ERROR_FATAL));
}

/******************************************************************************/
/******************************************************************************/
const char* CErrorStack::GetTitle(void)
{
	return((d_head != NULL) ? d_head->d_title : NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CErrorStack::GetMessage(void)
{
	return((d_head != NULL) ? d_head->d_msg : NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CErrorStack::GetDismissText(void)
{
	return((d_head != NULL) ? d_head->d_dismissText : NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CErrorStack::GetNextAction( const char *pCur )
{
	if (d_head != NULL)
	{
		if ( pCur == NULL) 
			return(d_head->d_action[0]);

		for (int i=0; i<MAX_ACTIONS; i++)
		{
			if (d_head->d_action[i] == pCur)
			{
				if (i+1 < MAX_ACTIONS)
					return(d_head->d_action[i+1]);
				else
					return NULL;
			}
		}
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CErrorStack::GetNextActionMsg( const char *pCur )
{
	if (d_head != NULL)
	{
		if ( pCur == NULL) 
			return(d_head->d_actionMsg[0]);

		for (int i=0; i<MAX_ACTIONS; i++)
		{
			if (d_head->d_actionMsg[i] == pCur)
			{
				if (i+1 < MAX_ACTIONS)
					return(d_head->d_actionMsg[i+1]);
				else
					return NULL;
			}
		}
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
SErrorStackEntry* CErrorStack::Find(ErrorHandle i_handle)
{
	SErrorStackEntry* entry = d_head;
	while (entry != NULL)
	{
		if (entry->d_handle == i_handle)
			return(entry);
		entry = entry->d_next;
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
SErrorStackEntry* CErrorStack::Find(const char* i_title, const char* i_msg)
{
	SErrorStackEntry* entry = d_head;
	while (entry != NULL)
	{
		if (!strcmp(entry->d_title, i_title) && !strcmp(entry->d_msg, i_msg))
			return(entry);
		entry = entry->d_next;
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CErrorStack::SetLifetime(SErrorStackEntry* i_entry, uint32 i_lifetime)
{
	// Compute the exipration time.
	if (i_lifetime == ERROR_INFINITE_LIFETIME)
		i_entry->d_expiration = ERROR_INFINITE_LIFETIME;
	else
		i_entry->d_expiration = g_timer.GetOSTime() + i_lifetime;
}

/******************************************************************************/
// Insert the entry on top of the other entries of the same severity.
/******************************************************************************/
void CErrorStack::AddEntry(SErrorStackEntry* i_entry)
{
	if ((d_head == NULL) || (i_entry->d_severity <= d_head->d_severity))
	{
		i_entry->d_prev = NULL;
		i_entry->d_next = d_head;
		if (d_head != NULL)
			d_head->d_prev = i_entry;
		d_head = i_entry;
	}
	else
	{
		SErrorStackEntry* curr = d_head;
		while ((curr->d_next != NULL) &&
				(i_entry->d_severity > curr->d_next->d_severity))
			curr = curr->d_next;

		i_entry->d_prev = curr;
		i_entry->d_next = curr->d_next;
		if (curr->d_next != NULL)
			curr->d_next->d_prev = i_entry;
		curr->d_next = i_entry;
	}
}

/******************************************************************************/
/******************************************************************************/
void CErrorStack::RemoveEntry(SErrorStackEntry* i_entry, bool i_delete)
{
	if (i_entry->d_prev != NULL)
		i_entry->d_prev->d_next = i_entry->d_next;
	else
	{
		ASSERT(i_entry == d_head);
		d_head = i_entry->d_next;
	}

	if (i_entry->d_next != NULL)
		i_entry->d_next->d_prev = i_entry->d_prev;

	if (i_delete)
	{
		delete[] i_entry->d_title;
		delete[] i_entry->d_msg;
		delete[] i_entry->d_dismissText;
		for (int i=0; i<MAX_ACTIONS; i++)
		{
			delete[] i_entry->d_action[i];
			delete[] i_entry->d_actionMsg[i];
		}
		delete i_entry;
	}
}
