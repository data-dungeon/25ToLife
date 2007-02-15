/*
**
**  File:   DRModel.h
**  Date:   June 15, 2004
**  By:     Bryant Collard
**  Desc:   Dead reckoning model base behavior.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRModel.h $
**  $Revision: #9 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DR_MODEL_H
#define DR_MODEL_H

#include <netz.h>
#include <extensions/deadreckoning.h>
#include "container/Map.h"

class DOActor;
class DRDataset;

//////////////////////////////// DRCommonModel /////////////////////////////////

class DRCommonModel : public DeadReckoningModel
{
  public:
	DRCommonModel() {d_actor = NULL; ClearDiscontinuityFlags();}
	virtual ~DRCommonModel() {}

	// Set the associated DO (which should be the actor) so the model can
	// convert state into the time space of the associated actor. By using the
	// actor instead of the host, if the actor's master migrates, the correct
	// host will be used automatically.
	void SetAssociatedDO(DuplicatedObject* i_do);
	DOActor* GetActor(void) {return(d_actor);}

	// Deal with all bits.
	void ClearDiscontinuityFlags(void) {d_discontinuityFlags = 0x00;}
	void SetDiscontinuityFlags(uint8 i_flags) {d_discontinuityFlags = i_flags;}
	uint8 GetDiscontinuityFlags(void) {return(d_discontinuityFlags);}

	// Deal with specific bits.
	void ClearDiscontinuityBits(uint8 i_bits) {d_discontinuityFlags &= ~i_bits;}
	void SetDiscontinuityBits(uint8 i_bits) {d_discontinuityFlags |= i_bits;}
	uint8 GetDiscontinuityBits(uint8 i_bits)
		{return(d_discontinuityFlags & i_bits);}

  private:
	DOActor* d_actor;
	uint8 d_discontinuityFlags;
};

/////////////////////////////////// DRModel ////////////////////////////////////

template<class DS>
class DRModel : public DRCommonModel
{
  public:
	virtual void Initialize(DuplicatedObject* i_do, DS* i_dataset) {}
	void Configure(DuplicatedObject* i_do, DS* i_dataset);

	virtual qBool IsAccurateEnough(const DS &i_master, Time i_time, 
			DuplicatedObject* i_masterDO, Station* i_duplicaStation) = 0;
	virtual void ComputeValue(DS* o_deadReckoned, Time i_time) = 0;
	virtual void UpdateModelOnMaster(const DS &i_master, Time i_time) = 0;
	virtual void UpdateModelOnDuplica(const DS &i_master) = 0;
};

//////////////////////////////// DRStartupModel ////////////////////////////////

template<class DS>
class DRStartupModel : public DRModel<DS>
{
  public:
	DRStartupModel(DRModel<DS> &i_shared);

	bool AddStation(const Station* i_station);
	void RemoveStation(const Station* i_station);
	bool IsActive(void) const;

	virtual qBool IsAccurateEnough(const DS &i_master, Time i_time, 
			DuplicatedObject* i_masterDO, Station* i_duplicaStation);
	virtual void ComputeValue(DS* o_deadReckoned, Time i_time);
	virtual void UpdateModelOnMaster(const DS &i_master, Time i_time);
	virtual void UpdateModelOnDuplica(const DS &i_master);

  private:
	DRStartupModel();

	CMap<DOHandle, bool> d_stationMap;
	bool d_active;
	bool d_sharedInitialized;
	DRModel<DS> &d_shared;
};

////////////////////////////// CommonModelPolicy ///////////////////////////////
template<class DS>
class CommonModelPolicy : public RootObject
{
  public:
	CommonModelPolicy() {d_do = NULL; d_dataset = NULL;}
	virtual ~CommonModelPolicy() {}
	virtual void Configure(DuplicatedObject* i_do, DS* i_dataset);

  protected:
	DuplicatedObject* d_do;
	DS* d_dataset;
};

/////////////////////////// StartupSharedModelPolicy ///////////////////////////

template<class DS, class MODEL>
class StartupSharedModelPolicy : public CommonModelPolicy<DS>
{
  public:
	StartupSharedModelPolicy() : d_startup(d_shared) {}
	virtual void Configure(DuplicatedObject* i_do, DS* i_dataset);

	DRModel<DS>* GetRemoteDeadReckoningModel(const Station* i_station);
	DRModel<DS>* GetLocalDeadReckoningModel();
	void ClearRemoteDeadReckoningModel(const Station* i_station);
	qUnsignedInt32 GetNbModels() const;

  private:
	StartupSharedModelPolicy(
			const StartupSharedModelPolicy<DS, MODEL>&);
	StartupSharedModelPolicy<DS, MODEL>& operator=(
			const StartupSharedModelPolicy<DS, MODEL>&);

	DRStartupModel<DS> d_startup;
	MODEL d_shared;
};

#define DR_MODEL_HPP
#include "Network/NetZ/DRModel.hpp"

#endif // DR_MODEL_H
