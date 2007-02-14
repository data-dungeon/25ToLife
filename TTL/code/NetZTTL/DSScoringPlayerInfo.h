/*
**
**  File:   DSScoringPlayerInfo.h
**  Date:   July 15, 2005
**  By:     Bryce Thomsen
**  Desc:   Dataset holding the player info stored in the scoring model 
**				that is obtained at the beginning of every round.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DSScoringPlayerInfo.h $
**  $Revision: #1 $
**  $DateTime: 2005/07/15 18:28:14 $
**    $Author: Bryce $
**
*/

#pragma once

#ifndef DS_SCORING_PLAYER_INFO_H
#define DS_SCORING_PLAYER_INFO_H

#include <netz.h>
#include "NetZTTL/DSScoringPlayerInfoDDL.h"

class DSScoringPlayerInfo : public DATASET(DSScoringPlayerInfo)
{
  public:
};

#endif // DS_SCORING_PLAYER_INFO_H
