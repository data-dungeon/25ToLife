/*
**
**  File:   ErrorMonitor.h
**  Date:   February 3, 2005
**  By:     Bryant Collard
**  Desc:   Monitors errors and displays them.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

#pragma once

#ifndef ERROR_MONITOR_H
#define ERROR_MONITOR_H

#include "GameHelper/ErrorStack.h"

class CErrorMonitor
{
  public:
	void Initialize(void);
	void Update(void);
	void Terminate(void);
	void EnableFatalDismiss(bool i_enable) {d_enableFatalDismiss = i_enable;}

  protected:
   int d_popupResult;

  private:
	ErrorHandle d_lastHandle;
	bool d_enableFatalDismiss;
	bool d_dismissAdded;

 // install some message handlers for the popup dialog
	CMessageOwner d_messageOwner;
	REF_BASIC_MESSAGE_HANDLER( DialogCommand, MESSAGE_VALUE,  CErrorMonitor ) d_dialogCommandHandler;
};

extern CErrorMonitor g_errorMonitor;

#endif // ERROR_MONITOR_H
