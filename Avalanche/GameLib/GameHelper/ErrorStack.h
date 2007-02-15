/*
**
**  File:   ErrorStack.h
**  Date:   February 3, 2005
**  By:     Bryant Collard
**  Desc:   Maintains a "stack" of error messages.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

#pragma once

#ifndef ERROR_STACK_H
#define ERROR_STACK_H

#include "platform/BaseType.h"

#define ERROR_INFINITE_LIFETIME MAXUINT
#define ERROR_MIN_LIFETIME      1000

typedef uint32 ErrorHandle;
extern const ErrorHandle ERROR_HANDLE_INVALID;

struct SErrorStackEntry;

class CErrorStack
{
  public:
	enum ESeverity
	{
		ERROR_FATAL,
		ERROR_WARNING
	};

	CErrorStack();
	~CErrorStack();

	ErrorHandle SetError(const char* i_title, const char* i_msg,
			ESeverity i_severity, uint32 i_lifetime,
			const char* i_dismissText = NULL);
	void SetActionMessagePair(ErrorHandle i_handle, const char* i_action,
			const char *i_msg);

	bool ClearError(ErrorHandle i_handle);
	bool ClearError(const char* i_title, const char* i_msg);
	void ClearAllErrors(void);
	void ClearAllNonExpiringErrors(void);
	void SetErrorSeverity(ErrorHandle i_handle, ESeverity i_severity);

	ErrorHandle Update(void);

	bool IsEmpty(void) {return(d_head == NULL);}
	bool IsEmptyOfExpiring(void);
	bool IsFatal(void);
	const char* GetTitle(void);
	const char* GetMessage(void);
	const char* GetDismissText(void);
	const char* GetNextAction(const char* pCur);
	const char* GetNextActionMsg(const char* pCur);

  private:
	SErrorStackEntry* Find(ErrorHandle i_handle);
	SErrorStackEntry* Find(const char* i_title, const char* i_msg);
	void SetLifetime(SErrorStackEntry* i_entry, uint32 i_lifetime);
	void AddEntry(SErrorStackEntry* i_entry);
	void RemoveEntry(SErrorStackEntry* i_entry, bool i_delete);

	ErrorHandle d_currHandle;
	SErrorStackEntry* d_head;
};

extern CErrorStack g_errorStack;

bool IsErrorStackEmpty(void);
bool IsErrorStackEmptyOfExpiring(void);

#endif // ERROR_STACK_H
