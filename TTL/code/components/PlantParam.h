/*
**
**  File:   PlantParam.h
**  Date:   November 1, 2001
**  By:     Bryant Collard
**  Desc:   Interactive environmental plant parameters.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /TTL/environ/PlantParam.h $
**  $Revision: #3 $
**      $Date: 2005/03/29 $
**    $Author: Rob $
**
*/

#ifndef PLANT_PARAM_H
#define PLANT_PARAM_H

//#include "platform/BaseType.h"

class CPlant;
class CPlantParam;

class CPlantInstance
{
  public:
	CPlantInstance();
  private:
	CPlant* d_plant;
	CPlantInstance* d_instancePrev;
	CPlantInstance* d_instanceNext;

	friend class CPlantParam;
};

class CPlantParam
{
  public:
	CPlantParam();
	~CPlantParam();
	static CPlantParam* Register(CPlant* i_plant, const char* i_instanceName, char* i_scriptPath = NULL);
	static void Unregister(CPlant* i_plant);
	static void Update(void);
  private:
	static void GetContext(const char* i_instanceName, char* i_context);
	void CreateContext(char* i_context, char* i_scriptPath);
	void UpdateContext(void);
	bool HasPlant(CPlant* i_plant);
	void AddPlant(CPlant* i_plant);
	void RemovePlant(CPlant* i_plant);

	static CPlantParam* d_paramHead;
	static uint32 d_lastFrame;
	CPlantParam* d_paramPrev;
	CPlantParam* d_paramNext;
	CPlantInstance* d_instanceHead;
	char d_context[9];
	int d_refCount;

	float d_iterationRate[2];
	float d_globalDamping[2];
	bool d_freeSwinging[2];
	float d_baseMass[2];
	float d_tipMass[2];
	float d_baseNaturalFrequency[2];
	float d_tipNaturalFrequency[2];
	float d_baseDampingFactor[2];
	float d_tipDampingFactor[2];
	float d_baseDragCoefficient[2];
	float d_tipDragCoefficient[2];
	bool d_boxActive[2];
	bool d_boxDisplay[2];
	Vector3Packed d_boxLocation[2];
	Vector3Packed d_boxRotation[2];
	Vector3Packed d_boxExtent[2];
	float d_obbInflateTime[2];
	float d_constraintScale[2];
	char d_touchedSound[16];

	friend class CPlant;
};

#endif // PLANT_PARAM_H
