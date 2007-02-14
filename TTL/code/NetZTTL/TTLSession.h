/*
**
**  File:   TTLSession.h
**  Date:   February 13, 2004
**  By:     Bryant Collard
**  Desc:   A TTL specific session.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/TTLSession.h $
**  $Revision: #16 $
**  $DateTime: 2005/06/04 13:15:18 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef TTL_SESSION_H
#define TTL_SESSION_H

// GameLib includes
#include "Network/NetZ/NetZSession.h"

//#define DUMP_REFERENCES
#ifdef DUMP_REFERENCES

// GameLib includes
#include "GameHelper/Message.h"

#define MAX_REFERENCE_HANDLES  20

/////////////////////////// CDumpReferencesCallback ////////////////////////////

class CDumpReferencesCallback : public OperationCallback
{
  public:
	CDumpReferencesCallback();
	virtual void CallMethod(Operation* i_operation);
	void Initialize(void);
	void Update(void);

  private:
	void Dump(const char* i_label);

	uint32 d_nextDumpTime;
	int d_numHandles;
	DOHandle d_handle[MAX_REFERENCE_HANDLES];

	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CDumpReferences, CDumpReferencesCallback)
		d_dumpReferencesHandler;
};

#endif

///////////////////////// CChangeMasterStationCallback /////////////////////////

class CChangeMasterStationCallback : public OperationCallback
{
  public:
	CChangeMasterStationCallback();
	virtual void CallMethod(Operation* i_operation);
};

///////////////////////////////// CTTLSession //////////////////////////////////

class CTTLSession : public CNetZSession
{
  protected:
	// Installation
	virtual void RegisterCallbacks(void);
	virtual void UnregisterCallbacks(void);
	virtual void InstallMessageTranslators(void);
	virtual const char* GetLocalName(void);

  private:
	// Make calls required by entries in .ddl files, e.g. if RendezVous is used,
	// the host migration extension adds a well-known object that must be
	// created. The actual implementation is in the subdirectory with the
	// .ddl file.
	void RegisterDDLSpecificCallbacks(void);

	CChangeMasterStationCallback d_changeMasterStationCallback;

#ifdef DUMP_REFERENCES
  public:
	virtual void Update(void);

  private:
	CDumpReferencesCallback d_dumpReferencesCallback;
#endif

};

#endif // TTL_SESSION_H
