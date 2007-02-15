/*
**
**  File:   Translators.cpp
**  Date:   March 3, 2004
**  By:     Bryant Collard
**  Desc:   Install generic message translators. This is simply a place to
**          collect and install some translators. There is nothing to stop
**          anyone from installing translators somewhere else.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/Translators.cpp $
**  $Revision: #50 $
**  $DateTime: 2005/07/20 01:20:35 $
**    $Author: Food $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/Translators.h"

// NetZ headers
#include <netz.h>
#include "Network/NetZ/DOActor.h"
#include "Network/NetZ/DOHost.h"

// Gamelib includes
#include "GameHelper/Message.h"
#include "Environ/World.h"

// Message includes
#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "Environ/partmsg.h"
#include "Components/CollisionInfo.h"
#include "EngineHelper/timer.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "GameHelper/TextChat.h"

#define MAX_OWNED_POS       50.0f
#define MAX_VOLUME          1.0f
#define MAX_PITCH           1.0f
#define MAX_PARTICLE_SCALE  4.0f
#define MAX_FLASH_INTENSITY 128.0f

////////////////////////////// SStationarySound /////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedStationarySound
{
	int16				d_posX, d_posY, d_posZ;
	uint16			d_volume, d_pitch;
	char				d_name[1];
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static uint32 StationarySoundSize(void* i_data)
{
	SStationarySound* dataIn = (SStationarySound*)i_data;
	ASSERT(dataIn->d_name != NULL);
	return(sizeof(uint16) * 5 + strlen(dataIn->d_name) + 1);
}

/***************************************************************************/
/***************************************************************************/
static void PackStationarySound(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	SStationarySound* dataIn = (SStationarySound*)i_dataIn;
	SPackedStationarySound* dataOut = (SPackedStationarySound*)i_dataOut;

	g_world.CompressPosition(dataIn->d_pos, dataOut->d_posX, dataOut->d_posY,
			dataOut->d_posZ);
	dataOut->d_volume = Math::UnsignedCompressToWord(dataIn->d_volume, MAX_VOLUME);
	dataOut->d_pitch = Math::UnsignedCompressToWord(dataIn->d_pitch, MAX_PITCH);
	strcpy(dataOut->d_name, dataIn->d_name);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackStationarySound(void* i_data, CMessageOwner &i_owner)
{
	static SStationarySound dataOut;
	SPackedStationarySound* dataIn = (SPackedStationarySound*)i_data;

	g_world.DecompressPosition(dataIn->d_posX, dataIn->d_posY, dataIn->d_posZ,
			dataOut.d_pos);
	dataOut.d_volume = Math::UnsignedDecompress(dataIn->d_volume, MAX_VOLUME);
	dataOut.d_pitch = Math::UnsignedDecompress(dataIn->d_pitch, MAX_PITCH);
	dataOut.d_name = dataIn->d_name;

	return(&dataOut);
}

////////////////////////////// SAttachedSound /////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedAttachedSound
{
	uint32			d_actor;
	uint16			d_volume, d_pitch;
	char				d_name[1];
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static uint32 AttachedSoundSize(void* i_data)
{
	SAttachedSound* dataIn = (SAttachedSound*)i_data;
	ASSERT(dataIn->d_name != NULL);
	return(sizeof(uint32) + sizeof(uint16) * 2 + strlen(dataIn->d_name) + 1);
}

/***************************************************************************/
/***************************************************************************/
static void PackAttachedSound(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	SAttachedSound* dataIn = (SAttachedSound*)i_dataIn;
	SPackedAttachedSound* dataOut = (SPackedAttachedSound*)i_dataOut;

	dataOut->d_actor = CTranslators::PackActor(dataIn->d_actor);
	dataOut->d_volume = Math::UnsignedCompressToWord(dataIn->d_volume, MAX_VOLUME);
	dataOut->d_pitch = Math::UnsignedCompressToWord(dataIn->d_pitch, MAX_PITCH);
	strcpy(dataOut->d_name, dataIn->d_name);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackAttachedSound(void* i_data, CMessageOwner &i_owner)
{
	static SAttachedSound dataOut;
	SPackedAttachedSound* dataIn = (SPackedAttachedSound*)i_data;

	dataOut.d_actor = CTranslators::UnpackActor(dataIn->d_actor);
	dataOut.d_volume = Math::UnsignedDecompress(dataIn->d_volume, MAX_VOLUME);
	dataOut.d_pitch = Math::UnsignedDecompress(dataIn->d_pitch, MAX_PITCH);
	dataOut.d_name = dataIn->d_name;

	return(&dataOut);
}

////////////////////////////// SParticleEffect /////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedParticleEffect
{
	int16				d_posX, d_posY, d_posZ;
	int16				d_normalX, d_normalY, d_normalZ;
	int16				d_scale;
	char				d_name[1];
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static uint32 ParticleEffectSize(void* i_data)
{
	SParticleEffect* dataIn = (SParticleEffect*)i_data;
	ASSERT(dataIn->d_name != NULL);
	return(sizeof(int16) * 7 + strlen(dataIn->d_name) + 1);
}

/***************************************************************************/
/***************************************************************************/
static void PackParticleEffect(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	SParticleEffect* dataIn = (SParticleEffect*)i_dataIn;
	SPackedParticleEffect* dataOut = (SPackedParticleEffect*)i_dataOut;

	g_world.CompressPosition(dataIn->d_pos, dataOut->d_posX, dataOut->d_posY,
			dataOut->d_posZ);
	dataOut->d_normalX = Math::SignedCompressToWord(dataIn->d_normal.X(), 1.0f);
	dataOut->d_normalY = Math::SignedCompressToWord(dataIn->d_normal.Y(), 1.0f);
	dataOut->d_normalZ = Math::SignedCompressToWord(dataIn->d_normal.Z(), 1.0f);
	dataOut->d_scale = Math::SignedCompressToWord(dataIn->d_scale,
			MAX_PARTICLE_SCALE);
	strcpy(dataOut->d_name, dataIn->d_name);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackParticleEffect(void* i_data, CMessageOwner &i_owner)
{
	static SParticleEffect dataOut;
	SPackedParticleEffect* dataIn = (SPackedParticleEffect*)i_data;

	g_world.DecompressPosition(dataIn->d_posX, dataIn->d_posY, dataIn->d_posZ,
			dataOut.d_pos);
	dataOut.d_normal.Set(Math::SignedDecompress(dataIn->d_normalX, 1.0f),
			Math::SignedDecompress(dataIn->d_normalY, 1.0f),
			Math::SignedDecompress(dataIn->d_normalZ, 1.0f));
	dataOut.d_scale = Math::SignedDecompress(dataIn->d_scale, MAX_PARTICLE_SCALE);
	dataOut.d_name = dataIn->d_name;

	return(&dataOut);
}

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
class CPackedMunitionImpactSurface
{
public:
	int8		d_weaponID;
	int8		d_munitionID;
	uint8		d_surfaceMaterial;
	uint8		d_surfaceShade;

	uint32	d_victim;

	int16		d_shotOriginX, d_shotOriginY, d_shotOriginZ;
	int16		d_intersectionX, d_intersectionY, d_intersectionZ;

	uint16	d_energy;
	uint8		d_damage;

	int16		d_surfaceNormalX, d_surfaceNormalY, d_surfaceNormalZ;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackMunitionImpactSurface(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	CMunitionImpactSurface* dataIn = (CMunitionImpactSurface*)i_dataIn;
	CPackedMunitionImpactSurface* dataOut = (CPackedMunitionImpactSurface*)i_dataOut;

	// Weapon ID
	ASSERTS(dataIn->d_weaponID < 128, "munition type out of range, contact Dwain");
	dataOut->d_weaponID = (uint8)dataIn->d_weaponID;
	if (dataIn->d_firstShot)
		dataOut->d_weaponID |= 0x80;

	// Munition ID
	ASSERTS(dataIn->d_munitionID < 128, "munition type out of range, contact Dwain");
	dataOut->d_munitionID = (uint8)dataIn->d_munitionID;
	if (dataIn->d_deflected)
		dataOut->d_munitionID |= 0x80;

	// Surface material and decalOkay
	dataOut->d_surfaceMaterial = dataIn->d_surfaceMaterial;
	if (dataIn->d_decalOkay)
		dataOut->d_surfaceMaterial |= 0x80;

	// Surface shade
	dataOut->d_surfaceShade = dataIn->d_surfaceShade;

	// The victim!
	dataOut->d_victim = CTranslators::PackActor(dataIn->d_victim);

	// Use owned versions if there is a victim.
	if (dataIn->d_victim != NULL)
	{
		CMessageOwner &packRelativeTo = dataIn->d_victim->GetMessageOwner();

		// The shot origin
		Vector3Packed vecIn;
		CTranslators::PackOwnedPosition(dataIn->d_shotOrigin, vecIn, packRelativeTo);
		dataOut->d_shotOriginX = Math::SignedCompressToWord(vecIn.X(), MAX_OWNED_POS);
		dataOut->d_shotOriginY = Math::SignedCompressToWord(vecIn.Y(), MAX_OWNED_POS);
		dataOut->d_shotOriginZ = Math::SignedCompressToWord(vecIn.Z(), MAX_OWNED_POS);

		// The intersection
		CTranslators::PackOwnedPosition(dataIn->d_intersection, vecIn, packRelativeTo);
		dataOut->d_intersectionX = Math::SignedCompressToWord(vecIn.X(), MAX_OWNED_POS);
		dataOut->d_intersectionY = Math::SignedCompressToWord(vecIn.Y(), MAX_OWNED_POS);
		dataOut->d_intersectionZ = Math::SignedCompressToWord(vecIn.Z(), MAX_OWNED_POS);

		// Normal
		CTranslators::PackOwnedVector3(dataIn->d_surfaceNormal, vecIn, packRelativeTo);
		dataOut->d_surfaceNormalX = Math::SignedCompressToWord(vecIn.X(), 1.0f);
		dataOut->d_surfaceNormalY = Math::SignedCompressToWord(vecIn.Y(), 1.0f);
		dataOut->d_surfaceNormalZ = Math::SignedCompressToWord(vecIn.Z(), 1.0f);
	}

	// Pack relative to the world.
	else
	{
		// The shot origin
		g_world.CompressPosition(dataIn->d_shotOrigin, dataOut->d_shotOriginX, dataOut->d_shotOriginY, dataOut->d_shotOriginZ);

		// The intersection
		g_world.CompressPosition(dataIn->d_intersection, dataOut->d_intersectionX, dataOut->d_intersectionY, dataOut->d_intersectionZ);

		// Normal
		dataOut->d_surfaceNormalX = Math::SignedCompressToWord(dataIn->d_surfaceNormal.X(), 1.0f);
		dataOut->d_surfaceNormalY = Math::SignedCompressToWord(dataIn->d_surfaceNormal.Y(), 1.0f);
		dataOut->d_surfaceNormalZ = Math::SignedCompressToWord(dataIn->d_surfaceNormal.Z(), 1.0f);
	}

	// Energy and damage
	ASSERTS(dataIn->d_energy < 4096.0f, "energy out of range, contact Bryce");
	dataOut->d_energy = (uint16)Math::Clamp(dataIn->d_energy, 1.0f, 4095.0f);
	ASSERTS(dataIn->d_salvoSize <= 16, "salvo size out of range, contact Bryce");
	dataOut->d_energy |= ((dataIn->d_salvoSize - 1) << 12);
	ASSERTS(dataIn->d_damage < 256, "damage out of range, contact Bryce");
	dataOut->d_damage = (uint8)dataIn->d_damage;
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackMunitionImpactSurface(void* i_data, CMessageOwner &i_owner)
{
	static CMunitionImpactSurface dataOut;
	CPackedMunitionImpactSurface* dataIn = (CPackedMunitionImpactSurface*)i_data;

	dataOut.d_firstShot = ((dataIn->d_weaponID & 0x80) != 0);
	dataOut.d_deflected = ((dataIn->d_munitionID & 0x80) != 0);
	dataOut.d_weaponID = (int)(dataIn->d_weaponID & 0x7f);
	dataOut.d_munitionID = (int)(dataIn->d_munitionID & 0x7f);
	dataOut.d_surfaceMaterial = (dataIn->d_surfaceMaterial & 0x7f);
	dataOut.d_surfaceShade = dataIn->d_surfaceShade;

	dataOut.d_decalOkay = (dataIn->d_surfaceMaterial & 0x80) != 0;

	dataOut.d_victim = CTranslators::UnpackActor(dataIn->d_victim);

	// Use owned versions if there is a victim.
	if (dataOut.d_victim != NULL)
	{
		CMessageOwner &packedRelativeTo = dataOut.d_victim->GetMessageOwner();

		// The shot origin
		Vector3Packed vecIn;
		vecIn.Set(Math::SignedDecompress(dataIn->d_shotOriginX, MAX_OWNED_POS),
				Math::SignedDecompress(dataIn->d_shotOriginY, MAX_OWNED_POS),
				Math::SignedDecompress(dataIn->d_shotOriginZ, MAX_OWNED_POS));
		CTranslators::UnpackOwnedPosition(vecIn, dataOut.d_shotOrigin, packedRelativeTo);

		// The intersection
		vecIn.Set(Math::SignedDecompress(dataIn->d_intersectionX, MAX_OWNED_POS),
				Math::SignedDecompress(dataIn->d_intersectionY, MAX_OWNED_POS),
				Math::SignedDecompress(dataIn->d_intersectionZ, MAX_OWNED_POS));
		CTranslators::UnpackOwnedPosition(vecIn, dataOut.d_intersection, packedRelativeTo);

		// Normal
		vecIn.Set(Math::SignedDecompress(dataIn->d_surfaceNormalX, 1.0f),
				Math::SignedDecompress(dataIn->d_surfaceNormalY, 1.0f),
				Math::SignedDecompress(dataIn->d_surfaceNormalZ, 1.0f));
		CTranslators::UnpackOwnedPosition(vecIn, dataOut.d_surfaceNormal, packedRelativeTo);
	}

	// Unpack relative to the world.
	else
	{
		// The shot origin
		g_world.DecompressPosition(dataIn->d_shotOriginX, dataIn->d_shotOriginY, dataIn->d_shotOriginZ, dataOut.d_shotOrigin);

		// The intersection
		g_world.DecompressPosition(dataIn->d_intersectionX, dataIn->d_intersectionY, dataIn->d_intersectionZ, dataOut.d_intersection);

		// Normal
		dataOut.d_surfaceNormal.Set(Math::SignedDecompress(dataIn->d_surfaceNormalX, 1.0f),
				Math::SignedDecompress(dataIn->d_surfaceNormalY, 1.0f),
				Math::SignedDecompress(dataIn->d_surfaceNormalZ, 1.0f));
	}

	dataOut.d_energy = (float)(dataIn->d_energy & 0xfff);
	dataOut.d_salvoSize = (dataIn->d_energy >> 12) + 1;
	dataOut.d_damage = (float)dataIn->d_damage;

	return(&dataOut);
}

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedMunitionFlash
{
	u8					d_munitionID;
	s16				d_posX, d_posY, d_posZ;
	s16				d_intensity;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackMunitionFlash(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	SMunitionFlash* dataIn = (SMunitionFlash*)i_dataIn;
	SPackedMunitionFlash* dataOut = (SPackedMunitionFlash*)i_dataOut;

	//g_console.Echo( "packing munition fired time->%d\n", g_timer.GetOSTime() );
	dataOut->d_munitionID = (u8)dataIn->d_munitionID;
	g_world.CompressPosition(dataIn->d_pos, dataOut->d_posX, dataOut->d_posY,
			dataOut->d_posZ);
	dataOut->d_intensity = Math::SignedCompressToWord(dataIn->d_intensity,
			MAX_FLASH_INTENSITY);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackMunitionFlash(void* i_data, CMessageOwner &i_owner)
{
	static SMunitionFlash dataOut;
	SPackedMunitionFlash* dataIn = (SPackedMunitionFlash*)i_data;

	dataOut.d_munitionID = dataIn->d_munitionID;
	g_world.DecompressPosition(dataIn->d_posX, dataIn->d_posY, dataIn->d_posZ,
			dataOut.d_pos);
	dataOut.d_intensity = Math::SignedDecompress(dataIn->d_intensity,
			MAX_FLASH_INTENSITY);

	return(&dataOut);
}

////////////////////////////// SMuntionExplodeFX /////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedMunitionExplodeFX
{
	u8					d_id, d_munitionID;
	int16				d_posX, d_posY, d_posZ;
	int16				d_dirX, d_dirY, d_dirZ;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackMunitionExplodeFX(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	SMunitionExplodeFX* dataIn = (SMunitionExplodeFX *)i_dataIn;
	SPackedMunitionExplodeFX* dataOut = (SPackedMunitionExplodeFX *)i_dataOut;

	dataOut->d_id = (uint8)dataIn->d_id;
	dataOut->d_munitionID = (uint8)dataIn->d_munitionID;
	g_world.CompressPosition(dataIn->d_pos, dataOut->d_posX, dataOut->d_posY, dataOut->d_posZ);
	dataOut->d_dirX = Math::SignedCompressToWord(dataIn->d_dir.X(), 1.0f);
	dataOut->d_dirY = Math::SignedCompressToWord(dataIn->d_dir.Y(), 1.0f);
	dataOut->d_dirZ = Math::SignedCompressToWord(dataIn->d_dir.Z(), 1.0f);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackMunitionExplodeFX(void* i_data, CMessageOwner &i_owner)
{
	static SMunitionExplodeFX dataOut;
	SPackedMunitionExplodeFX* dataIn = (SPackedMunitionExplodeFX *)i_data;

	dataOut.d_id = (int)dataIn->d_id;
	dataOut.d_munitionID = (int)dataIn->d_munitionID;
	g_world.DecompressPosition(dataIn->d_posX, dataIn->d_posY, dataIn->d_posZ,	dataOut.d_pos);
	dataOut.d_dir.Set(Math::SignedDecompress(dataIn->d_dirX, 1.0f),
								Math::SignedDecompress(dataIn->d_dirY, 1.0f),
								Math::SignedDecompress(dataIn->d_dirZ, 1.0f));

	return(&dataOut);
}

////////////////////////////// SCollisionInfo //////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedCollisionInfo
{
	Vector3Packed	d_intersection;
	Vector3Packed	d_normal;
	uint32			d_sender;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackCollisionInfo(void* i_dataIn, void* i_dataOut,
											CMessageOwner &i_owner)
{
	SCollisionInfo* dataIn = (SCollisionInfo*)i_dataIn;
	SPackedCollisionInfo* dataOut = (SPackedCollisionInfo*)i_dataOut;

	CTranslators::PackOwnedPosition(dataIn->d_intersection,
		dataOut->d_intersection, i_owner);
	CTranslators::PackOwnedVector3(dataIn->d_normal, dataOut->d_normal, i_owner);
	dataOut->d_sender = CTranslators::PackActor(dataIn->d_sender);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackCollisionInfo(void* i_data, CMessageOwner &i_owner)
{
	static SCollisionInfo dataOut;
	SPackedCollisionInfo* dataIn = (SPackedCollisionInfo*)i_data;

	CTranslators::UnpackOwnedPosition(dataIn->d_intersection,
		dataOut.d_intersection, i_owner);
	CTranslators::UnpackOwnedVector3(dataIn->d_normal, dataOut.d_normal, i_owner);
	dataOut.d_sender = CTranslators::UnpackActor((u32)dataIn->d_sender);

	return(&dataOut);
}

////////////////////////////// SMutingData //////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedMutingData
{
	u64				d_id;
	u32				d_flags;
	u32				d_host;
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static void PackMutingData(void* i_dataIn, void* i_dataOut,
										  CMessageOwner &i_owner)
{
	SMuteData* dataIn = (SMuteData*)i_dataIn;
	SPackedMutingData* dataOut = (SPackedMutingData*)i_dataOut;

	dataOut->d_id		= dataIn->id;
	dataOut->d_flags	= dataIn->flags;
	dataOut->d_host	= CTranslators::PackHost( dataIn->host );
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackMutingData(void* i_data, CMessageOwner &i_owner)
{
	static SMuteData dataOut;
	SPackedMutingData* dataIn = (SPackedMutingData*)i_data;

	dataOut.id		= dataIn->d_id;
	dataOut.flags	= dataIn->d_flags;
	dataOut.host	= CTranslators::UnpackHost( dataIn->d_host );
	return(&dataOut);
}

////////////////////////////// SMutingData //////////////////////////////

/***************************************************************************/
/***************************************************************************/
#pragma pack(4)
struct SPackedTextChatData
{
	u64				d_id;
	u32				d_flags;
	char				d_text[1];
};
#pragma pack()

/***************************************************************************/
/***************************************************************************/
static uint32 TextChatDataSize( void *i_data )
{
	STextChat *dataIn = (STextChat *)i_data;
	ASSERT(dataIn->text != NULL);
	return(sizeof(dataIn->id) + sizeof(dataIn->flags) + strlen(dataIn->text) + 1);
}

/***************************************************************************/
/***************************************************************************/
static void PackTextChatData(void* i_dataIn, void* i_dataOut,
									CMessageOwner &i_owner)
{
	STextChat* dataIn = (STextChat*)i_dataIn;
	SPackedTextChatData* dataOut = (SPackedTextChatData*)i_dataOut;

	dataOut->d_id	= dataIn->id;
	dataOut->d_flags = dataIn->flags;
	strcpy(dataOut->d_text, dataIn->text);
}

/***************************************************************************/
/***************************************************************************/
static void* UnpackTextChatData(void* i_data, CMessageOwner &i_owner)
{
	static STextChat dataOut;
	SPackedTextChatData* dataIn = (SPackedTextChatData*)i_data;

	dataOut.id		= dataIn->d_id;
	dataOut.flags	= dataIn->d_flags;
	dataOut.text	= dataIn->d_text;
	return(&dataOut);
}

//////////////////////////////// CTranslators ///////////////////////////////

/***************************************************************************/
/***************************************************************************/
void CTranslators::Install(void)
{
	g_messageDispatcher.InstallVariableSizeTranslator("PlayStationarySound",
			PackStationarySound, UnpackStationarySound, StationarySoundSize);

	g_messageDispatcher.InstallVariableSizeTranslator("PlayAttachedSound",
			PackAttachedSound, UnpackAttachedSound, AttachedSoundSize);

	g_messageDispatcher.InstallVariableSizeTranslator("ParticleEffect",
			PackParticleEffect, UnpackParticleEffect, ParticleEffectSize);

	g_messageDispatcher.InstallFixedSizeTranslator(MUNITION_IMPACT,
			PackMunitionImpactSurface, UnpackMunitionImpactSurface,
			sizeof(CPackedMunitionImpactSurface));

	g_messageDispatcher.InstallFixedSizeTranslator(MUNITION_FLASH,
			PackMunitionFlash, UnpackMunitionFlash,
			sizeof(SPackedMunitionFlash));

	g_messageDispatcher.InstallFixedSizeTranslator(MUNITION_EXPLODEFX,
			PackMunitionExplodeFX, UnpackMunitionExplodeFX,
			sizeof(SPackedMunitionExplodeFX));

	g_messageDispatcher.InstallFixedSizeTranslator("CollisionInfo",
			PackCollisionInfo, UnpackCollisionInfo,
			sizeof(SCollisionInfo));

	g_messageDispatcher.InstallFixedSizeTranslator(REQUEST_MUTING,
			PackMutingData, UnpackMutingData,
			sizeof( SPackedMutingData));

	g_messageDispatcher.InstallFixedSizeTranslator(REMOTE_MUTE,
		PackMutingData, UnpackMutingData,
		sizeof( SPackedMutingData));

	g_messageDispatcher.InstallFixedSizeTranslator(REMOTE_UNMUTE,
		PackMutingData, UnpackMutingData,
		sizeof( SPackedMutingData));

	g_messageDispatcher.InstallFixedSizeTranslator(MUTE,
		PackMutingData, UnpackMutingData,
		sizeof( SPackedMutingData));

	g_messageDispatcher.InstallFixedSizeTranslator(UNMUTE,
		PackMutingData, UnpackMutingData,
		sizeof( SPackedMutingData));

	g_messageDispatcher.InstallVariableSizeTranslator(KEYBOARD_TEXT_CHAT, PackTextChatData, UnpackTextChatData, TextChatDataSize );
}

///////////////////////////////// Utilities /////////////////////////////////

/***************************************************************************/
/***************************************************************************/
uint32 CTranslators::PackActor(CActor* i_actor)
{
	if (i_actor != NULL)
	{
		DOActor* actor = (DOActor*)i_actor->GetNetActor();
		if (actor != NULL)
			return(actor->GetHandleValue());
	}
	return(INVALID_DOHANDLE_VALUE);
}

/***************************************************************************/
/***************************************************************************/
CActor* CTranslators::UnpackActor(uint32 i_actor)
{
	DOHandle actor(i_actor);
	if (actor.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOActor::Ref refActor(actor);
		if (refActor.IsValid())
			return(refActor->GetActor());
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
uint32 CTranslators::PackActorHandle(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
	{
		DOActor* doActor = (DOActor*)actor->GetNetActor();
		if (doActor != NULL)
			return(doActor->GetHandleValue());
	}
	return(INVALID_DOHANDLE_VALUE);
}

/***************************************************************************/
/***************************************************************************/
ActorHandle CTranslators::UnpackActorHandle(uint32 i_actor)
{
	DOHandle actor(i_actor);
	if (actor.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOActor::Ref refActor(actor);
		if (refActor.IsValid())
			return(CActor::ToHandle(refActor->GetActor()));
	}
	return(INVALID_OBJECT_HANDLE);
}

/***************************************************************************/
/***************************************************************************/
uint32 CTranslators::PackHost(CHost* i_host)
{
	if (i_host != NULL)
		return(((DOHost*)i_host)->GetHandleValue());
	return(INVALID_DOHANDLE_VALUE);
}

/***************************************************************************/
/***************************************************************************/
CHost* CTranslators::UnpackHost(uint32 i_host)
{
	DOHandle host(i_host);
	if (host.IsAKindOf(DOCLASSID(DOHost)))
	{
		DOHost::Ref refHost(host);
		if (refHost.IsValid())
			return((CHost*)(*refHost));
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackOwnedPosition(Vector3CRef i_in,
		Vector3Packed &o_out, CMessageOwner &i_owner)
{
	o_out = (i_in - i_owner.OwnerInWorld()) / i_owner.OwnerToWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::UnpackOwnedPosition(const Vector3Packed &i_in,
		Vector3 &o_out, CMessageOwner &i_owner)
{
	o_out = i_in * i_owner.OwnerToWorld() + i_owner.OwnerInWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackOwnedVector3(Vector3CRef i_in, Vector3Packed &o_out,
		CMessageOwner &i_owner)
{
	o_out = i_in / i_owner.OwnerToWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::UnpackOwnedVector3(const Vector3Packed &i_in,
		Vector3 &o_out, CMessageOwner &i_owner)
{
	o_out = i_in * i_owner.OwnerToWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackOwnedOrientation(const DirCos3x3 &i_in, DirCos3x3 &o_out,
		CMessageOwner &i_owner)
{
	o_out = i_in / i_owner.OwnerToWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::UnpackOwnedOrientation(const DirCos3x3 &i_in,
		DirCos3x3 &o_out, CMessageOwner &i_owner)
{
	o_out = i_in * i_owner.OwnerToWorld();
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackOwnedTransform(const Cartesian4x4 &i_in,
		Cartesian4x4 &o_out, CMessageOwner &i_owner)
{
	Cartesian4x4 ownerToWorld(i_owner.OwnerToWorld(), i_owner.OwnerInWorld());
	o_out = i_in / ownerToWorld;
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::UnpackOwnedTransform(const Cartesian4x4 &i_in,
		Cartesian4x4 &o_out, CMessageOwner &i_owner)
{
	Cartesian4x4 ownerToWorld(i_owner.OwnerToWorld(), i_owner.OwnerInWorld());
	o_out = i_in * ownerToWorld;
}

///////////////////////////// Basic Data Types //////////////////////////////

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicValue(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((uint32*)i_dataOut) = (uint32)i_dataIn;
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicValue(void* i_data, CMessageOwner &i_owner)
{
	return((void*)(*((uint32*)i_data)));
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicActor(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((uint32*)i_dataOut) = PackActor((CActor*)i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicActor(void* i_data, CMessageOwner &i_owner)
{
	return((void*)UnpackActor(*((uint32*)i_data)));
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicActorHandle(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((uint32*)i_dataOut) = PackActorHandle(i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicActorHandle(void* i_data, CMessageOwner &i_owner)
{
	return((void*)UnpackActorHandle(*((uint32*)i_data)));
}

/***************************************************************************/
/***************************************************************************/
uint32 CTranslators::StringSize(void* i_data)
{
	ASSERT(i_data != NULL);
	return(strlen((char*)i_data) + 1);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicString(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	ASSERT(i_dataIn != NULL);
	strcpy((char*)i_dataOut, (char*)i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicString(void* i_data, CMessageOwner &i_owner)
{
	return(i_data);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicVector3(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((Vector3*)i_dataOut) = *((Vector3*)i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicVector3(void* i_data, CMessageOwner &i_owner)
{
	return(i_data);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicOwnedVector3(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	PackOwnedVector3(*((Vector3*)i_dataIn), *((Vector3Packed*)i_dataOut), i_owner);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicOwnedVector3(void* i_data,
		CMessageOwner &i_owner)
{
	static Vector3 out;
	UnpackOwnedVector3(*((Vector3*)i_data), out, i_owner);
	return(&out);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicOwnedPosition(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	PackOwnedPosition(*((Vector3*)i_dataIn), *((Vector3Packed*)i_dataOut), i_owner);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicOwnedPosition(void* i_data,
		CMessageOwner &i_owner)
{
	static Vector3 out;
	UnpackOwnedPosition(*((Vector3*)i_data), out, i_owner);
	return(&out);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicMatrix3x3(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((Matrix3x3*)i_dataOut) = *((Matrix3x3*)i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicMatrix3x3(void* i_data, CMessageOwner &i_owner)
{
	return(i_data);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicOwnedOrientation(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	PackOwnedOrientation(*((DirCos3x3*)i_dataIn), *((DirCos3x3*)i_dataOut),
			i_owner);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicOwnedOrientation(void* i_data,
		CMessageOwner &i_owner)
{
	static DirCos3x3 out;
	UnpackOwnedOrientation(*((DirCos3x3*)i_data), out, i_owner);
	return(&out);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicMatrix4x4(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	*((Matrix4x4*)i_dataOut) = *((Matrix4x4*)i_dataIn);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicMatrix4x4(void* i_data, CMessageOwner &i_owner)
{
	return(i_data);
}

/***************************************************************************/
/***************************************************************************/
void CTranslators::PackBasicOwnedTransform(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner)
{
	PackOwnedTransform(*((Cartesian4x4*)i_dataIn), *((Cartesian4x4*)i_dataOut),
			i_owner);
}

/***************************************************************************/
/***************************************************************************/
void* CTranslators::UnpackBasicOwnedTransform(void* i_data,
		CMessageOwner &i_owner)
{
	static Cartesian4x4 out;
	UnpackOwnedTransform(*((Cartesian4x4*)i_data), out, i_owner);
	return(&out);
}
