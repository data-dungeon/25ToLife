/*
**
**  File:   TTLTranslators.cpp
**  Date:   March 3, 2004
**  By:     Bryant Collard
**  Desc:   Install TTL specific message translators. This is simply a place to
**          collect and install some translators. There is nothing to stop
**          anyone from installing translators somewhere else.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/TTLTranslators.cpp $
**  $Revision: #9 $
**  $DateTime: 2005/07/23 14:51:06 $
**    $Author: Bryce $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/TTLTranslators.h"

// NetZ includes
#include "Network/NetZ/Translators.h"

#include "effects/customtag.h"

////////////////////////////// SCollisionInfo //////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct PackedCustomTagInfo
{
	Vector3Packed	pos;
	Vector3Packed	normal;
	Vector3Packed	up;
	u16				color;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackCustomTagInfo(
void *i_dataIn,
void *i_dataOut,
CMessageOwner &i_owner)
{
	CustomTagInfo *dataIn = (CustomTagInfo *)i_dataIn;
	PackedCustomTagInfo *dataOut = (PackedCustomTagInfo *)i_dataOut;

	dataOut->pos = dataIn->pos;
	dataOut->normal = dataIn->normal;
	dataOut->up = dataIn->up;
	dataOut->color = dataIn->color;
}

/***************************************************************************/
/***************************************************************************/
static void *UnpackCustomTagInfo(
void *i_data,
CMessageOwner &i_owner)
{
	static CustomTagInfo dataOut;
	PackedCustomTagInfo *dataIn = (PackedCustomTagInfo*)i_data;

	dataOut.pos = dataIn->pos;
	dataOut.normal = dataIn->normal;
	dataOut.up = dataIn->up;
	dataOut.color = dataIn->color;

	return(&dataOut);
}

/***************************************************************************/
/***************************************************************************/
void CTTLTranslators::Install(void)
{
	g_messageDispatcher.InstallFixedSizeTranslator("QuickTag",
			PackCustomTagInfo, UnpackCustomTagInfo, sizeof(PackedCustomTagInfo));
}
