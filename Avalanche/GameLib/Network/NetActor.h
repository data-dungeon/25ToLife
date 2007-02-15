/*
**
**  File:   NetActor.h
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   A base for classes that manage the distributed aspects of actors.
**          An actor will point to one of these things if the game is
**          multi-player. Issues specific to the network implementation should
**          be contained here and below.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetActor.h $
**  $Revision: #12 $
**  $DateTime: 2005/08/02 15:34:31 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NET_ACTOR_H
#define NET_ACTOR_H

#include <stdarg.h>

class CActor;

class CNetActor
{
  public:
	CNetActor();
	virtual ~CNetActor();

	CActor* GetActor(void) {return(d_actor);}
	virtual void SetPersistent(bool i_persistent) {d_persistent = i_persistent;}
	virtual bool IsPersistent(void) {return(d_persistent);}
	virtual void Distribute(CActor* i_master, bool i_persistent);
	virtual void Establish(void) {}
	virtual void Unbind(bool i_kill = false);
	virtual bool Propagate(float i_gameTime) {return(false);}
	virtual void SetMasterDataMinUpdateTime(uint32 i_lowMS, uint32 i_highMS) {}
	virtual bool RequestDataSlot(const char* i_name, int i_maxSize)
		{return(true);}
	virtual bool WriteDataSlot(const char* i_name, void* i_data, int i_size)
		{return(true);}
	virtual void SendData(void) {}
	virtual void* OpenDataSlot(const char* i_name) {return(NULL);}
	virtual const void* ReadDataSlot(const char* i_name) {return(NULL);}
	virtual const char* GetHostName(void) {return(NULL);}

  protected:
	void BindProxy(CActor* i_proxy);

	CActor* d_actor;
	bool d_persistent;
};

#endif // NET_ACTOR_H
