/*
**
**  File:   DRModel.hpp
**  Date:   June 15, 2004
**  By:     Bryant Collard
**  Desc:   Dead reckoning model base behavior.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRModel.hpp $
**  $Revision: #4 $
**  $DateTime: 2005/06/24 20:40:40 $
**    $Author: Bryant_Collard $
**
*/

#ifdef DR_MODEL_HPP
#undef DR_MODEL_HPP

// Local includes
#include "Network/NetZ/DOActor.h"

//////////////////////////////// DRCommonModel /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
inline void DRCommonModel::SetAssociatedDO(DuplicatedObject* i_do)
{
	if ((i_do != NULL) && i_do->IsAKindOf(DOCLASSID(DOActor)))
		d_actor = (DOActor*)i_do;
	else
		d_actor = NULL;
}

/////////////////////////////////// DRModel ////////////////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void DRModel<DS>::Configure(DuplicatedObject* i_do, DS* i_dataset)
{
	SetAssociatedDO(i_do);
	Initialize(i_do, i_dataset);
}

//////////////////////////////// DRStartupModel ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline DRStartupModel<DS>::DRStartupModel(DRModel<DS> &i_shared) :
	d_shared(i_shared),
	d_stationMap(8, 4)
{
	d_active = false;
	d_sharedInitialized = false;
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline bool DRStartupModel<DS>::AddStation(const Station* i_station)
{
	if (i_station != NULL)
	{
		bool newEntry;
		bool &updated = d_stationMap.Get(i_station->GetHandle(), &newEntry);
		if (newEntry)
		{
			updated = false;
			d_active = true;
		}
		return(!updated);
	}
	ASSERT(false);
	return(false);
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void DRStartupModel<DS>::RemoveStation(const Station* i_station)
{
	if (i_station != NULL)
		d_stationMap.Remove(i_station->GetHandle());
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline bool DRStartupModel<DS>::IsActive(void) const
{
	return(d_active && d_sharedInitialized);
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline qBool DRStartupModel<DS>::IsAccurateEnough(const DS &i_master,
		Time i_time, DuplicatedObject* i_masterDO, Station* i_duplicaStation)
{
	return(false);
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void DRStartupModel<DS>::ComputeValue(DS* o_deadReckoned, Time i_time)
{
	ASSERT(false);
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void DRStartupModel<DS>::UpdateModelOnMaster(const DS &i_master,
		Time i_time)
{
	if (d_active)
	{
		if (!d_sharedInitialized)
		{
			d_shared.UpdateModelOnMaster(i_master, i_time);
			d_sharedInitialized = true;
		}
		CMap<DOHandle, bool>::CIterator iterator = d_stationMap.First();
		while(!iterator.Out())
		{
			*iterator.Value() = true;
			++iterator;
		}
		d_active = false;
	}
}

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void DRStartupModel<DS>::UpdateModelOnDuplica(const DS &i_master)
{
	ASSERT(false);
}

////////////////////////////// CommonModelPolicy ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class DS>
inline void CommonModelPolicy<DS>::Configure(DuplicatedObject* i_do,
		DS* i_dataset)
{
	d_do = i_do;
	d_dataset = i_dataset;
}

/////////////////////////// StartupSharedModelPolicy ///////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class DS, class MODEL>
inline void StartupSharedModelPolicy<DS, MODEL>::Configure(
		DuplicatedObject* i_do, DS* i_dataset)
{
	CommonModelPolicy<DS>::Configure(i_do, i_dataset);
	d_shared.Configure(i_do, i_dataset);
}

/******************************************************************************/
/******************************************************************************/
template<class DS, class MODEL>
inline DRModel<DS>* StartupSharedModelPolicy<DS, MODEL
		>::GetRemoteDeadReckoningModel(const Station* i_station)
{
	if (d_startup.AddStation(i_station))
		return(&d_startup);
	return(&d_shared);
}

/******************************************************************************/
/******************************************************************************/
template<class DS, class MODEL>
inline DRModel<DS>* StartupSharedModelPolicy<DS, MODEL
		>::GetLocalDeadReckoningModel()
{
	return(&d_shared);
}

/******************************************************************************/
/******************************************************************************/
template<class DS, class MODEL>
inline void StartupSharedModelPolicy<DS, MODEL
		>::ClearRemoteDeadReckoningModel(const Station* i_station)
{
	d_startup.RemoveStation(i_station);
}

/******************************************************************************/
/******************************************************************************/
template<class DS, class MODEL>
inline qUnsignedInt32 StartupSharedModelPolicy<DS, MODEL>::GetNbModels() const
{
	return(d_startup.IsActive() ? 2 : 1);
}

/////////////////////////////// Dataset Members ////////////////////////////////

#define DeclareDRMembers(DS, DRM, DRMP)                                                           \
	DRM* GetLocalDRModel(void)                                                                     \
	{                                                                                              \
		using namespace Quazal;                                                                     \
		return((DRM*)(static_cast< _DSDECL_##DS* >(this))->GetLocalDeadReckoningModel());           \
	}                                                                                              \
                                                                                                  \
	DRM* GetRemoteDRModel(const Station* i_station)                                                \
	{                                                                                              \
		using namespace Quazal;                                                                     \
		return((DRM*)(static_cast< _DSDECL_##DS* >(this))->GetRemoteDeadReckoningModel(i_station)); \
	}                                                                                              \
                                                                                                  \
	uint32 GetNumDRModels(void)                                                                    \
	{                                                                                              \
		using namespace Quazal;                                                                     \
		return((uint32)((static_cast< _DSDECL_##DS* >(this))->GetNbModels()));                      \
	}                                                                                              \
                                                                                                  \
	DRMP &GetDRModelPolicy(void)                                                                   \
	{                                                                                              \
		using namespace Quazal;                                                                     \
		return((DRMP &)(static_cast< _DSDECL_##DS* >(this))->GetDeadReckoningModelPolicy());        \
	}                                                                                              \
                                                                                                  \
	void ConfigureModelPolicy(DuplicatedObject* i_do, DS* i_dataset)                               \
	{                                                                                              \
		GetDRModelPolicy().Configure(i_do, i_dataset);                                              \
	}

#define DROperationBegin                                                                          \
	void OperationBegin(DOOperation* i_operation)                                                  \
	{                                                                                              \
		if (i_operation->GetType() == Operation::AddToStore)                                        \
			ConfigureModelPolicy(i_operation->GetAssociatedDO(), this);                              \
	}

#endif
