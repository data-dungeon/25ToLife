/*
**
**  File:   DOActor.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the actor distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOActor.cpp $
**  $Revision: #42 $
**  $DateTime: 2005/08/29 16:53:21 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOActor.h"

// Local headers
#include "Network/Session.h"
#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib includes
#include "Environ/ActorInitializer.h"
#include "stage/stage.h"
#include "stage/CCompActor.h"
#include "EngineHelper/Hash.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/PackBuffer.h"
#include "GameHelper/DebugOutput.h"
#include "math/fastrand.h"
#include "Script/ScriptVM.h"
#include "Layers/Debug.h"

// Default values.
#define DEFAULT_LOW_MASTER_DATA_REFRESH_DELAY   4000
#define DEFAULT_HIGH_MASTER_DATA_REFRESH_DELAY  7000

// Aide in dumping debug info.
static const char* GetActorName(CActor* i_actor)
{
	if (i_actor != NULL)
	{
		if ((i_actor->InstanceName() != NULL) &&
				(*i_actor->InstanceName() != '\0'))
			return(i_actor->InstanceName());
		if ((i_actor->Name() != NULL) && (*i_actor->Name() != '\0'))
			return(i_actor->Name());
	}
	return("UNNAMED");
}

/******************************************************************************/
/******************************************************************************/
DOActor::DOActor() : d_host(INVALID_OBJECT_HANDLE)
{
	d_lowMasterDataRefreshDelay = DEFAULT_LOW_MASTER_DATA_REFRESH_DELAY;
	d_highMasterDataRefreshDelay = DEFAULT_HIGH_MASTER_DATA_REFRESH_DELAY;
	d_masterDataRefreshTime = 0;

	// Terminate config data at the front of the buffer.
	d_configData.SetSize(sizeof(int));
	PackBuffer::Pack(d_configData.GetBuffer(), (int)0);
	d_configData.SetCallback(d_callback.Set(this, &DOActor::DataReceived));
}

/******************************************************************************/
/******************************************************************************/
void DOActor::SetPersistent(bool i_persistent)
{
	if (IsADuplicationMaster() && (d_migrate.d_persistent != i_persistent))
	{
		d_migrate.d_persistent = i_persistent;
		d_migrate.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOActor::IsPersistent(void)
{
	return(d_migrate.d_persistent);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::Distribute(CActor* i_master, bool i_persistent)
{
	ASSERT(d_actor == NULL);
	d_actor = i_master;
	d_migrate.d_persistent = i_persistent;
	InitMaster();
	d_actor->SetupNetData();
	Publish();
}

/******************************************************************************/
/******************************************************************************/
void DOActor::Establish(void)
{
	// Don't do anything if a proxy already exists.
	if (d_actor != NULL)
		return;

	// Get the parameter set.
	ParameterSet param(d_props.d_parm.CStr());

	// Create the proxy actor.
	CActor* proxy = CreateProxy(param.GetString("class"));
	if (proxy == NULL)
		return;

	// Load up the parameter set
	proxy->Parm().SetFromParameterSet(param);

	// Set our name
	const char* proxyName = proxy->Parm().GetString("name");
	if (proxyName && (proxyName[0] != '\0'))
		proxy->SetName(proxyName);
	else
		proxy->SetName("DOActor");

	// Set the message owner's name
	const char* instanceName = proxy->Parm().GetString("instance");
	if (instanceName && (instanceName[0] != '\0'))
		proxy->GetMessageOwner().SetOwnerName("ProxyActor", instanceName);
	else
		proxy->GetMessageOwner().SetOwnerName("ProxyActor", proxyName);

	// Set the actor as a duplica
	SET_MESSAGE_DISPATCHER_CALL_LOCATION();
	proxy->SetNetMaster(false);

	// Load geometry (for now, actors have to have a geometry)
	bool geometrySet = ActorInitializer::InitGeometry(proxy,
			GetInitGeometryFunc());
	ASSERT(geometrySet);

	// Extract scale from instance (this also grabs position and
	// orient, but we'll override those shortly).
	ActorInitializer::InitFromInstance(proxy);

	// Initialize the actor
	if (proxy->Initialize())
	{
		// Bind this net actor and the proxy actor together.
		proxy->BindNetActor(this);
		BindProxy(proxy);

		// Initialize the visual properties
		ActorInitializer::InitVisualPropertiesFromInstance(proxy);

		// Load and run any ai script
		const char* aiNet = proxy->Parm().GetString("ainet");
		if (aiNet && (aiNet[0] != '\0'))
			ActorInitializer::LoadAIScript(proxy, aiNet);
		else
			ActorInitializer::LoadAIScript(proxy, proxy->Parm().GetString("ai"));
		if (proxy->Script(AI_SCRIPT))
			proxy->Script(AI_SCRIPT)->Run(0.0f);

		// Load the runtime configured data from the DO.
		proxy->LoadNetData();

		// Load the data from the DO into the proxy. Probably override initial
		// state set in InitFromInstance or from config data.
		InitProxy();

		// Add the proxy to the stage.
		g_stage.AddActor(proxy);
		proxy->SetDeleteWhenNoCast(true);

		// If the actor migrated, become the master.
		if (IsADuplicationMaster())
			BecomeMaster();
	}
	else
		delete proxy;
}

/******************************************************************************/
/******************************************************************************/
void DOActor::Unbind(bool i_kill)
{
	Terminate();

	if (d_actor != NULL)
	{
		if (IsADuplica())
			d_actor->MarkForDeath();
		else if (i_kill)
			d_migrate.d_persistent = false;
		else if (d_migrate.d_persistent)
			RefreshMasterData();
			
		CActor* actor = d_actor;
		d_actor = NULL;
		actor->UnbindNetActor(i_kill);
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOActor::Propagate(float i_gameTime)
{
	if (IsADuplicationMaster())
	{
		PropagateMaster(i_gameTime);
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::InitDO()
{
	if (IsADuplica() && g_session.IsPopulateEnabled())
		Establish();
//	DumpDataSlots("InitDO");
}

/******************************************************************************/
/******************************************************************************/
CActor* DOActor::CreateProxy(const char* i_class)
{
	CActor* actor = NULL;
	if ((i_class == NULL) || !stricmp(i_class, "compactor"))
		actor = new CCompActor;
	else if (!stricmp(i_class, "actor") || !stricmp(i_class, "protagonist"))
		actor = new CProtagonist;
	ASSERT_PTR(actor);
	return(actor);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::InitMaster(void)
{
//	g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//			"Init : Master : %s : 0x%x : %s", GetActorName(d_actor),
//			GetHandle().GetValue(), IsPersistent() ? "Persistent" : "Terminal");

	// Fill in properties.
	const char* parmString = d_actor->Parm().GetInitializerString();
	d_props.d_parm = parmString;
	delete[] parmString;

	// Fill in interactions.
	d_interaction.d_involvement = d_actor->GetDesiredInvolvement();
	d_interaction.d_category = *(d_actor->Category().Get());
	d_interaction.d_positiveInclude = *(d_actor->PositiveInclude().Get());
	d_interaction.d_negativeInclude = *(d_actor->NegativeInclude().Get());
	d_interaction.d_positiveExclude = *(d_actor->PositiveExclude().Get());
	d_interaction.d_negativeExclude = *(d_actor->NegativeExclude().Get());

	// Setup master data.
	InitializeMasterData();
}

/******************************************************************************/
/******************************************************************************/
void DOActor::InitProxy(void)
{
//	g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//			"Init : Duplica : %s : 0x%x : %s", GetActorName(d_actor),
//			GetHandle().GetValue(), IsPersistent() ? "Persistent" : "Terminal");

	// Setup involvement.
	d_actor->RequestInvolvement(d_interaction.d_involvement);
	d_actor->Category() = d_interaction.d_category;
	d_actor->PositiveInclude() = d_interaction.d_positiveInclude;
	d_actor->NegativeInclude() = d_interaction.d_negativeInclude;
	d_actor->PositiveExclude() = d_interaction.d_positiveExclude;
	d_actor->NegativeExclude() = d_interaction.d_negativeExclude;
}

/******************************************************************************/
/******************************************************************************/
void DOActor::BecomeMaster(void)
{
//	g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//			"Become : Master : %s : 0x%x : %s", GetActorName(d_actor),
//			GetHandle().GetValue(), IsPersistent() ? "Persistent" : "Terminal");
	SET_MESSAGE_DISPATCHER_CALL_LOCATION();
	d_actor->SetNetMaster(true);

	// Set the message owner's name
	const char* name = d_actor->Parm().GetString("instance");
	if ((name == NULL) || (name[0] == '\0'))
		name = d_actor->Parm().GetString("name");
	d_actor->GetMessageOwner().SetOwnerName("MasterActor", name);

	// Load up the master data.
	void* metaDataBuffer = d_masterMetaData.GetBuffer();
	void* dataBuffer = d_masterData.GetBuffer();
	d_actor->ReadNetMasterData(metaDataBuffer, dataBuffer);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::BecomeProxy(void)
{
//	g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//			"Become : Duplica : %s : 0x%x : %s", GetActorName(d_actor),
//			GetHandle().GetValue(), IsPersistent() ? "Persistent" : "Terminal");
	SET_MESSAGE_DISPATCHER_CALL_LOCATION();
	d_actor->SetNetMaster(false);

	// Set the message owner's name
	const char* name = d_actor->Parm().GetString("instance");
	if ((name == NULL) || (name[0] == '\0'))
		name = d_actor->Parm().GetString("name");
	d_actor->GetMessageOwner().SetOwnerName("ProxyActor", name);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::PropagateMaster(float i_gameTime)
{
	if (d_actor)
	{
		if ((d_actor->PositiveInclude() != d_interaction.d_positiveInclude) ||
				(d_actor->NegativeInclude() != d_interaction.d_negativeInclude) ||
				(d_actor->PositiveExclude() != d_interaction.d_positiveExclude) ||
				(d_actor->NegativeExclude() != d_interaction.d_negativeExclude) ||
				(d_actor->Category() != d_interaction.d_category) ||
				(d_actor->GetDesiredInvolvement() != d_interaction.d_involvement))
		{
			d_interaction.d_positiveInclude = *(d_actor->PositiveInclude().Get());
			d_interaction.d_negativeInclude = *(d_actor->NegativeInclude().Get());
			d_interaction.d_positiveExclude = *(d_actor->PositiveExclude().Get());
			d_interaction.d_negativeExclude = *(d_actor->NegativeExclude().Get());
			d_interaction.d_category = *(d_actor->Category().Get());
			d_interaction.d_involvement = d_actor->GetDesiredInvolvement();
			d_interaction.RequestUpdate();
		}

		if (d_actor->HasNetMasterMetaDataChangedSinceLastWrite())
			InitializeMasterData();
		else if ((g_timer.GetOSTime() > d_masterDataRefreshTime) &&
				d_actor->HasNetMasterDataChangedSinceLastWrite())
			RefreshMasterData();
	}

	Update(g_sessionTime.ConvertTimeGameToNet(i_gameTime));
}

/******************************************************************************/
/******************************************************************************/
void DOActor::StateChanged(void)
{
	if (d_actor)
	{
		d_actor->Category() = d_interaction.d_category;
		d_actor->PositiveInclude() = d_interaction.d_positiveInclude;
		d_actor->NegativeInclude() = d_interaction.d_negativeInclude;
		d_actor->PositiveExclude() = d_interaction.d_positiveExclude;
		d_actor->NegativeExclude() = d_interaction.d_negativeExclude;

		g_stage.ChangeActor(d_actor, d_interaction.d_involvement);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOActor::OperationEnd(DOOperation* i_operation)
{
	switch(i_operation->GetType())
	{
	 case Operation::ChangeMasterStation:
	 {
		d_host = INVALID_OBJECT_HANDLE;
		if (d_actor != NULL)
		{
			if (((ChangeMasterStationOperation*)i_operation)->IsAPromotion())
				BecomeMaster();
			else if (((ChangeMasterStationOperation*)i_operation)->IsADemotion())
				BecomeProxy();
		}
		break;
	 }

	 case Operation::AddToStore:
	 {
		if (d_actor != NULL)
		{
			SET_MESSAGE_DISPATCHER_CALL_LOCATION();
			d_actor->SetNetMaster(IsADuplicationMaster());
			d_actor->GetMessageOwner().SetSessionID(GetHandleValue());
		}
		break;
	 }

	 case Operation::RemoveFromStore:
	 {
		Terminate();
		if (d_actor != NULL)
		{
			SET_MESSAGE_DISPATCHER_CALL_LOCATION();
			CActor* actor = d_actor;
			d_actor = NULL;
			actor->UnbindNetActor(false);
			actor->SetNetMaster(true);
			actor->GetMessageOwner().SetSessionID(INVALID_DOHANDLE_VALUE);
			if (IsADuplica())
				actor->MarkForDeath();
		}
		break;
	 }

	 case Operation::ChangeDupSet:
	 {
		if (d_actor != NULL)
		{
			if (((ChangeDupSetOperation*)i_operation)->IsADuplicaAddition())
			{
//				DumpDataSlots("Add Duplica");
				d_configData.RequestUpdate();
			}
		}
		break;
	 }
	}
}

/******************************************************************************/
/******************************************************************************/
DOHost* DOActor::GetHost(void)
{
	DOHost* host = DOHost::FromHandle(d_host);

	if (host == NULL)
	{
		d_host = INVALID_OBJECT_HANDLE;
		DOHandle masterStation = GetMasterStation();
		DOHost::SelectionIterator hostIt;
		hostIt.GotoStart();
		while(!hostIt.EndReached())
		{
			if (hostIt->GetMasterStation() == masterStation)
			{
				host = *hostIt;
				d_host = host->GetHostHandle();
				break;
			}
			hostIt++;
		}
	}
	return(host);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::SetHost(DOHost* i_host)
{
	d_host = DOHost::ToHandle(i_host);
}

/******************************************************************************/
/******************************************************************************/
const char* DOActor::GetHostName(void)
{
	DOHost* host = GetHost();
	if (host != NULL)
		return(host->GetName());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CMessageOwner* DOActor::MessageOwner(void)
{
	if (d_actor != NULL)
		return(&d_actor->GetMessageOwner());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::SetMasterDataMinUpdateTime(uint32 i_lowMS, uint32 i_highMS)
{
	if (i_lowMS <= i_highMS)
	{
		d_lowMasterDataRefreshDelay = i_lowMS;
		d_highMasterDataRefreshDelay = i_highMS;
	}
	else
	{
		d_lowMasterDataRefreshDelay = i_highMS;
		d_highMasterDataRefreshDelay = i_lowMS;
	}
}

/******************************************************************************/
/******************************************************************************/
void DOActor::InitializeMasterData(void)
{
	int metaDataSize = d_actor->GetNetMasterMetaDataSize();
	int dataSize = d_actor->GetNetMasterDataSize();
	d_masterMetaData.SetSize(metaDataSize);
	d_masterData.SetSize(dataSize);

	if (metaDataSize > 0)
	{
		void* metaDataBuffer = d_masterMetaData.GetBuffer();
		d_actor->WriteNetMasterMetaData(metaDataBuffer);
		d_masterMetaData.RequestUpdate();
	}

	if (dataSize > 0)
		RefreshMasterData();
	else
		d_masterDataRefreshTime = (uint32)(~0);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::RefreshMasterData(void)
{
	void* metaDataBuffer = d_masterMetaData.GetBuffer();
	void* dataBuffer = d_masterData.GetBuffer();
	d_actor->WriteNetMasterData(metaDataBuffer, dataBuffer);
	d_masterData.RequestUpdate();

	// Find a new refresh time. Randomize the delay in an effort to distribute
	// updates rather than having bursts of updates.
	uint32 currTime = g_timer.GetOSTime();
	if (((uint32)(~0) - currTime) > d_highMasterDataRefreshDelay)
		d_masterDataRefreshTime = currTime + (uint32)g_random.InRange(
				(int)d_lowMasterDataRefreshDelay, (int)d_highMasterDataRefreshDelay);
	else
		d_masterDataRefreshTime = (uint32)(~0);
}

/******************************************************************************/
/******************************************************************************/
bool DOActor::RequestDataSlot(const char* i_name, int i_maxSize)
{
	// Since only the master can write data, only the master can request a slot.
	if (!IsADuplicationMaster())
		return(false);

	// The slot must be able to contain data.
	if (i_maxSize <= 0)
	{
		ASSERT(false);
		return(false);
	}

	// Only request a slot if a valid name is given.
	ASSERT((i_name != NULL) && (i_name[0] != '\0'));
	if ((i_name != NULL) && (i_name[0] != '\0'))
	{
		// Compute a hash of the name. The hash is what is actually used to
		// identify the slot.
		unsigned int hash = Hash::CalcChecksum(i_name, strlen(i_name));

		// Try to find a slot with the given name (hash). Fail if a slot with the
		// name already exists.
		int size;
		if (GetDataSlot(hash, size) != NULL)
		{
			ASSERT(false);
			return(false);
		}

		// Expand the size of the buffer. The new size is the old size plus space
		// for the data size, the name hash and the data itself.
		size = d_configData.GetSize();
		d_configData.SetSize(size + sizeof(int) + sizeof(unsigned int) +
				i_maxSize);

		// An integer with a value of 0 is at the end of the current buffer. The
		// requested slot starts with that integer and a terminating 0 integer
		// will be appended to the end of the expanded buffer. Find the beginning
		// of the slot.
		void* buffer = PackBuffer::Skip(d_configData.GetBuffer(),
				size - sizeof(int));

		// Pack the size of the slot's data and the name hash.
		buffer = PackBuffer::Pack(buffer, i_maxSize);
		buffer = PackBuffer::Pack(buffer, hash);

		// Clear the data area and skip it.
		memset(buffer, 0, i_maxSize);
		buffer = PackBuffer::Skip(buffer, i_maxSize);

		// Write the terminating 0.
		PackBuffer::Pack(buffer, (int)0);

		// Publish the buffer to get the size and slot name out there.
//		char msg[128];
//		sprintf(msg, "Request slot %s", i_name);
//		DumpDataSlots(msg);
		d_configData.RequestUpdate();
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool DOActor::WriteDataSlot(const char* i_name, void* i_data, int i_size)
{
	if (!IsADuplicationMaster())
		return(false);

	if ((i_name != NULL) && (i_name[0] != '\0'))
	{
		int maxSize;
		void* buffer = GetDataSlot(Hash::CalcChecksum(i_name, strlen(i_name)),
				maxSize);
		if ((buffer == NULL) || (i_size > maxSize))
		{
			ASSERT(false);
			return(false);
		}

		memset(buffer, 0, maxSize);
		if ((i_data != NULL) && (i_size > 0))
			memcpy(buffer, i_data, i_size);
	}

//	char msg[128];
//	sprintf(msg, "Write slot %s", i_name);
//	DumpDataSlots(msg);
	d_configData.RequestUpdate();
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::SendData(void)
{
	if (IsADuplicationMaster())
	{
//		DumpDataSlots("Send");
		d_configData.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void* DOActor::OpenDataSlot(const char* i_name)
{
	int maxSize;
	if (IsADuplicationMaster() && (i_name != NULL) && (i_name[0] != '\0'))
		return(GetDataSlot(Hash::CalcChecksum(i_name, strlen(i_name)), maxSize));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const void* DOActor::ReadDataSlot(const char* i_name)
{
//	char msg[128];
//	sprintf(msg, "Read slot %s", i_name);
//	DumpDataSlots(msg);
	int maxSize;
	if ((i_name != NULL) && (i_name[0] != '\0'))
		return(GetDataSlot(Hash::CalcChecksum(i_name, strlen(i_name)), maxSize));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void* DOActor::GetDataSlot(unsigned int i_hash, int &o_maxSize)
{
	// The config buffer layout is:
	//   [DataSize][Id][Data] ... [0]
	if (d_configData.GetSize() >= 0)
	{
		void* buffer = PackBuffer::Unpack(d_configData.GetBuffer(), o_maxSize);
		while(o_maxSize != 0)
		{
			unsigned int hash;
			buffer = PackBuffer::Unpack(buffer, hash);
			if (hash == i_hash)
				return(buffer);
			buffer = PackBuffer::Skip(buffer, o_maxSize);
			buffer = PackBuffer::Unpack(buffer, o_maxSize);
		}
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void DOActor::DataReceived(void)
{
//	DumpDataSlots("Receive");
	if (d_actor != NULL)
		g_messageDispatcher.SendMessageToActor("NetDataReceived", NULL,
				INVALID_OBJECT_HANDLE, d_actor->GetHandle());
}

/******************************************************************************/
/******************************************************************************/
#include <ctype.h>
void DOActor::DumpDataSlots(const char* i_label)
{
	// The config buffer layout is:
	//   [DataSize][Id][Data] ... [0]

	dbgPrint("Dumping net data for %s: %s\n", GetActorName(d_actor), i_label);

	void* buffer = d_configData.GetBuffer();
	dbgPrint("  Config data buffer = 0x%x\n", buffer);

	uint32 bufferSize = d_configData.GetSize();
	if (bufferSize == 0)
	{
		dbgPrint("  No buffer\n");
		return;
	}

	int size;
	buffer = PackBuffer::Unpack(buffer, size);
	if (size == 0)
	{
		dbgPrint("  No slots (buffer size = %d)\n", bufferSize);
		return;
	}

	int i = 0;
	while(size != 0)
	{
		unsigned int hash;
		buffer = PackBuffer::Unpack(buffer, hash);
		dbgPrint("  Slot %d: Buffer = %d, Size = %d, Hash = 0x%x\n", i++,
				bufferSize, size, hash);

		while(size != 0)
		{
			dbgPrint("    ");

			int count = (size >= 8) ? 8 : size;
			size -= count;

			int j;
			uint8 data[8];
			for(j = 0; j < count; j++)
				buffer = PackBuffer::Unpack(buffer, data[j]);

			for(j = 0; j < count; j++)
				dbgPrint("%1x%1x ", ((data[j] >> 4) & 0xf), (data[j] & 0xf));

			for(; j < 9; j++)
				dbgPrint("   ");

			for(j = 0; j < count; j++)
				dbgPrint("%c", isprint(data[j]) ? data[j] : '.');

			dbgPrint("\n");
		}

		buffer = PackBuffer::Unpack(buffer, size);
	}
}
