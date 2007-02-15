/*
**
**  File:   DOGeneric.h
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the generic distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOGeneric.h $
**  $Revision: #13 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_GENERIC_H
#define DO_GENERIC_H

#include <netz.h>
#include "Network/NetZ/DOGenericDDL.h"
#include "GameHelper/Callback.h"
#include "Components/Component.h"

class DOGeneric : public DOCLASS(DOGeneric)
{
  public:
	DOGeneric();
	virtual ~DOGeneric();
	void SetBodyAcceleration(Vector3CRef i_acc);

  protected:
	virtual CActor* CreateProxy(const char* i_class);
	virtual void InitMaster(void);
	virtual void InitProxy(void);
	virtual void BecomeMaster(void);
	virtual void BecomeProxy(void);
	virtual void PropagateMaster(float i_gameTime);
	virtual void Terminate(void);

	REF_SEND_MESSAGE_HANDLER(CToggleDebugHandler, DOGeneric) d_toggleDebugHandler;
	REF_SEND_MESSAGE_HANDLER(CClearDebugHandler, DOGeneric) d_clearDebugHandler;
	bool d_debug;

	CCallback_1<DSSixDOF, Vector3CRef> d_callback;
};

class DOGenericComponent : public CActorComponent
{
  public:
	DOGenericComponent(CCompActor &i_actor);
	virtual const char* ComponentName(void) {return("GenericNetActor");}
	virtual CNetActor* CreateNetActor(void);
	virtual void EndFrame(void);

  private:
	DOGeneric* d_genericNetActor;
};

#endif // DO_GENERIC
