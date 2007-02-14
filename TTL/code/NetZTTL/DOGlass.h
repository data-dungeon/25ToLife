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
**      $File: //depot/TTL/code/NetZTTL/DOGlass.h $
**  $Revision: #12 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_GLASS_H
#define DO_GLASS_H

#include <netz.h>
#include "NetZTTL/DOGlassDDL.h"

class DOGlass : public DOCLASS(DOGlass)
{
  public:
	void StateChanged(void);

  protected:
	virtual CActor* CreateProxy(const char* i_class);
	virtual void InitMaster(void);
	virtual void BecomeMaster(void);
	virtual void PropagateMaster(float i_gameTime);
};

#endif // DO_GLASS
