/*
**
**  File:   Translators.h
**  Date:   March 3, 2004
**  By:     Bryant Collard
**  Desc:   Install generic message translators. This is simply a place to
**          collect and install some translators. There is nothing to stop
**          anyone from installing translators somewhere else.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/Translators.h $
**  $Revision: #9 $
**  $DateTime: 2005/07/13 13:59:38 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef TRANSLATORS_H
#define TRANSLATORS_H

#include "platform/BaseType.h"
#include "GameHelper/Handle.h"
#include "Math/Vector.h"

handledef(CActor) ActorHandle;
class CActor;
class CHost;
class CMessageOwner;
class DirCos3x3;
class Cartesian4x4;

class CTranslators
{
  public:
	// Install the "standard" translators.
	static void Install(void);

	// Utilities
	static uint32 PackActor(CActor* i_actor);
	static CActor* UnpackActor(uint32 i_actor);
	static uint32 PackActorHandle(ActorHandle i_handle);
	static ActorHandle UnpackActorHandle(uint32 i_actor);
	static uint32 PackHost(CHost* i_host);
	static CHost* UnpackHost(uint32 i_host);
	static void PackOwnedVector3(Vector3CRef i_in, Vector3Packed &o_out,
			CMessageOwner &i_owner);
	static void UnpackOwnedVector3(const Vector3Packed &i_in, Vector3 &o_out,
			CMessageOwner &i_owner);
	static void PackOwnedPosition(Vector3CRef i_in, Vector3Packed &o_out,
			CMessageOwner &i_owner);
	static void UnpackOwnedPosition(const Vector3Packed &i_in, Vector3 &o_out,
			CMessageOwner &i_owner);
	static void PackOwnedOrientation(const DirCos3x3 &i_in, DirCos3x3 &o_out,
			CMessageOwner &i_owner);
	static void UnpackOwnedOrientation(const DirCos3x3 &i_in, DirCos3x3 &o_out,
			CMessageOwner &i_owner);
	static void PackOwnedTransform(const Cartesian4x4 &i_in, Cartesian4x4 &o_out,
			CMessageOwner &i_owner);
	static void UnpackOwnedTransform(const Cartesian4x4 &i_in,
			Cartesian4x4 &o_out, CMessageOwner &i_owner);

	// Translators for basic data types.
	static void PackBasicValue(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicValue(void* i_data, CMessageOwner &i_owner);
	static void PackBasicActor(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicActor(void* i_data, CMessageOwner &i_owner);
	static void PackBasicActorHandle(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicActorHandle(void* i_data, CMessageOwner &i_owner);
	static uint32 StringSize(void* i_data);
	static void PackBasicString(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicString(void* i_data, CMessageOwner &i_owner);
	static void PackBasicVector3(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicVector3(void* i_data, CMessageOwner &i_owner);
	static void PackBasicOwnedVector3(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicOwnedVector3(void* i_data, CMessageOwner &i_owner);
	static void PackBasicOwnedPosition(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicOwnedPosition(void* i_data, CMessageOwner &i_owner);
	static void PackBasicMatrix3x3(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicMatrix3x3(void* i_data, CMessageOwner &i_owner);
	static void PackBasicOwnedOrientation(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicOwnedOrientation(void* i_data,
			CMessageOwner &i_owner);
	static void PackBasicMatrix4x4(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicMatrix4x4(void* i_data, CMessageOwner &i_owner);
	static void PackBasicOwnedTransform(void* i_dataIn, void* i_dataOut,
			CMessageOwner &i_owner);
	static void* UnpackBasicOwnedTransform(void* i_data, CMessageOwner &i_owner);
};

#endif // TRANSLATORS_H
