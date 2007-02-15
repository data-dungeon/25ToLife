/*
**
**  File:   MacroChain.h
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   A dynamic chain run directly by the actor.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/MacroChain.h $
**  $Revision: 7 $
**      $Date: 10/17/01 11:52a $
**    $Author: Bcollard $
**
*/

#ifndef MACRO_CHAIN_H
#define MACRO_CHAIN_H

#include "stage/DynamicChain.h"

class Graphics4x4;

class CMacroChain : public CDynamicChain
{
  public:
    bool Initialize(CInstance* i_inst, char* i_name,
            Graphics4x4* i_modelToWorld = NULL);
};

#endif // MACRO_CHAIN_H
