/*
**
**  File:   TTL_RendezVousSession.cpp
**  Date:   February 13, 2004
**  By:     Bryant Collard
**  Desc:   A TTL specific session.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/RendezVous/TTL_RendezVousSession.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/06/04 13:15:18 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/TTLSession.h"

// Well-Known headers
#include <Services/Extensions/HostMigration.h>

/******************************************************************************/
/******************************************************************************/
void CTTLSession::RegisterDDLSpecificCallbacks(void)
{
	HostMigrationExtension::Register();
}
