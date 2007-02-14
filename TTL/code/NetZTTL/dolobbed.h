/*
**
**  File:   DOGlass.h
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the glass distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/dolobbed.h $
**  $Revision: #8 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_LOBBED_H
#define DO_LOBBED_H

#include <netz.h>
#include "NetZTTL/DOLobbedDDL.h"
#include "GameHelper/Callback.h"

class DOLobbed : public DOCLASS(DOLobbed)
{
public:
	DOLobbed();
	~DOLobbed();

protected:
	virtual CActor* CreateProxy(const char* i_class);
	void InitMaster(void);
	void InitProxy(void);
	void PropagateMaster(float i_gameTime);
	void Terminate(void);

	CCallback_1<DSSixDOF, Vector3CRef> d_callback;
};

#endif
