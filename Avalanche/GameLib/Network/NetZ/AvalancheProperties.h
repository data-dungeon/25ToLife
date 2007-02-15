/*
**
**  File:   AvalancheProperties.h
**  Date:   April 28, 2004
**  By:     Bryant Collard
**  Desc:   Avalanche custom properties.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/AvalancheProperties.h $
**  $Revision: #6 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef AVALANCHE_PROPERTIES_H
#define AVALANCHE_PROPERTIES_H

#include <extensions/deadreckoning.h>
#include "Network/NetZ/DRSixDOF.h"
#include "Network/NetZ/DRGroundFollower.h"
#include "Network/NetZ/DRPathFollower.h"

// 6 degree of freedom dead reckoning.
#define _PR_DRSixDOF_model(DS) DRModel<DS>
#define _PR_DRSixDOF_modelpolicy(MODEL) DRSixDOFModelPolicy

// Ground follower dead reckoning.
#define _PR_DRGroundFollower_model(DS) DRModel<DS>
#define _PR_DRGroundFollower_modelpolicy(MODEL) DRGroundFollowerModelPolicy

// Path follower dead reckoning.
#define _PR_DRPathFollower_model(DS) DRModel<DS>
#define _PR_DRPathFollower_modelpolicy(MODEL) DRPathFollowerModelPolicy

#endif // AVALANCHE_PROPERTIES
