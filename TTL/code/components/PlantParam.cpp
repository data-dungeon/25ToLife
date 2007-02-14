/*
**
**  File:   PlantParam.cpp
**  Date:   November 1, 2001
**  By:     Bryant Collard
**  Desc:   Interactive environmental plant parameters.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /TTL/environ/PlantParam.cpp $
**  $Revision: #3 $
**      $Date: 2004/08/19 $
**    $Author: Food $
**
*/

/******************* includes ****************************/

#include "TTLPCH.h"


#include "cmdcon/cmdcon.h"
#include "components/Plant.h"
#include "components/PlantParam.h"

CPlantParam* CPlantParam::d_paramHead = NULL;
uint32 CPlantParam::d_lastFrame = 0;

CPlantInstance::CPlantInstance()
{
	d_plant = NULL;
	d_instancePrev = NULL;
	d_instanceNext = NULL;
}

CPlantParam::CPlantParam()
{
	d_paramPrev = NULL;
	d_paramNext = NULL;
	d_instanceHead = NULL;
	d_context[0] = '\0';
	d_refCount = 0;
}

CPlantParam::~CPlantParam()
{
	if (d_paramPrev != NULL)
		d_paramPrev->d_paramNext = d_paramNext;
	if (d_paramNext != NULL)
		d_paramNext->d_paramPrev = d_paramPrev;

	CPlantInstance* next;
	CPlantInstance* curr = d_instanceHead;
	while (curr)
	{
		next = curr->d_instanceNext;
		delete curr;
		curr = next;
	}
}

CPlantParam* CPlantParam::Register(CPlant* i_plant, const char* i_instanceName,
		char* i_scriptPath /*= NULL*/)
{
	CPlantParam* curr = d_paramHead;
	while (curr)
	{
		if (curr->HasPlant(i_plant))
			return(curr);
		curr = curr->d_paramNext;
	}

	char context[9];
	GetContext(i_instanceName, context);

	curr = d_paramHead;
	while (curr)
	{
		if (!strcmp(curr->d_context, context))
		{
			curr->AddPlant(i_plant);
			return(curr);
		}
		curr = curr->d_paramNext;
	}

	MEM_SET_ONE_SHOT_NEW_NAME("CPlantPar::Register");
	curr = new CPlantParam;
	curr->d_paramPrev = NULL;
	curr->d_paramNext = d_paramHead;
	d_paramHead = curr;
	curr->CreateContext(context, i_scriptPath);
	curr->AddPlant(i_plant);
	return(curr);
}

void CPlantParam::Unregister(CPlant* i_plant)
{
	CPlantParam* curr = d_paramHead;
	while (curr)
	{
		if (curr->HasPlant(i_plant))
		{
			curr->RemovePlant(i_plant);
			if (curr->d_refCount <= 0)
			{
				if (curr == d_paramHead)
					d_paramHead = curr->d_paramNext;
				delete curr;
			}
			return;
		}
		curr = curr->d_paramNext;
	}
}

void CPlantParam::Update(void)
{
	uint32 frame = g_timer.GetFrameCount();
	if (frame != d_lastFrame)
	{
		d_lastFrame = frame;
		CPlantParam* curr = d_paramHead;
		while (curr)
		{
			curr->UpdateContext();
			curr = curr->d_paramNext;
		}
	}
}

void CPlantParam::GetContext(const char* i_instanceName, char* i_context)
{
	const char* start = i_instanceName;
	while(true)
	{
		if ((*start == ' ') || (*start == '_'))
		{
			start++;
			continue;
		}

		if (!strnicmp("env", start, 3) || !strnicmp("abt", start, 3))
		{
			start += 3;
			continue;
		}

		break;
	}

	int i;
	for (i = 0; i < 8; i++)
	{
		if ((*start == '\0') || (*start == ' ') || (*start == '('))
			break;
		i_context[i] = *start;
		start++;
	}
	i_context[i] = '\0';
}

void CPlantParam::CreateContext(char* i_context, char* i_scriptPath)
{
	strcpy(d_context, i_context);

	d_iterationRate[0] = 300.0f;
	d_globalDamping[0] = 1.0f;
	d_freeSwinging[0] = false;
	d_baseMass[0] = 1.0f;
	d_tipMass[0] = 1.0f;
	d_baseNaturalFrequency[0] = 10.0f;
	d_tipNaturalFrequency[0] = 10.0f;
	d_baseDampingFactor[0] = 1.0f;
	d_tipDampingFactor[0] = 1.0f;
	d_baseDragCoefficient[0] = 0.1f;
	d_tipDragCoefficient[0] = 0.1f;
	d_boxActive[0] = false;
	d_boxDisplay[0] = false;
	d_boxLocation[0].Clear();
	d_boxRotation[0].Set(0.0f, 0.0f, 0.0f);
	d_boxExtent[0].Set(0.5f, 0.5f, 0.5f);
	d_obbInflateTime[0] = 0.5f;
	d_constraintScale[0] = 1.0f;
	strncpy(d_touchedSound, "plants", sizeof(d_touchedSound)-1);	// default sound handle

	char script[128];
	if ((i_scriptPath != NULL) && (i_scriptPath[0] != '\0'))
	{
		strcpy (script, i_scriptPath);
		strcat (script, d_context);
	}
	else
		strcpy (script, d_context);

	g_console.DeleteContext(d_context);
	g_console.BindScript(d_context, script);

	g_console.CreateVar(d_context, "IterationRate", d_iterationRate[0]);
	g_console.CreateVar(d_context, "GlobalDamping", d_globalDamping[0]);
	g_console.CreateVar(d_context, "FreeSwinging", d_freeSwinging[0]);
	g_console.CreateVar(d_context, "BaseMass", d_baseMass[0]);
	g_console.CreateVar(d_context, "TipMass", d_tipMass[0]);
	g_console.CreateVar(d_context, "BaseNatFreq", d_baseNaturalFrequency[0]);
	g_console.CreateVar(d_context, "TipNatFreq", d_tipNaturalFrequency[0]);
	g_console.CreateVar(d_context, "BaseDampFact", d_baseDampingFactor[0]);
	g_console.CreateVar(d_context, "TipDampFact", d_tipDampingFactor[0]);
	g_console.CreateVar(d_context, "BaseDragCoeff", d_baseDragCoefficient[0]);
	g_console.CreateVar(d_context, "TipDragCoeff", d_tipDragCoefficient[0]);
	g_console.CreateVar(d_context, "BoxActive", d_boxActive[0]);
	g_console.CreateVar(d_context, "BoxDisplay", d_boxDisplay[0]);
	g_console.CreateVar(d_context, "BoxLocation", (Vector3Packed*)&d_boxLocation[0]);
	g_console.CreateVar(d_context, "BoxRotation", (Vector3Packed*)&d_boxRotation[0]);
	g_console.CreateVar(d_context, "BoxExtent", (Vector3Packed*)&d_boxExtent[0]);
	g_console.CreateVar(d_context, "OBBInflateTime", d_obbInflateTime[0]);
	g_console.CreateVar(d_context, "ConstraintScale", d_constraintScale[0]);
	g_console.CreateVar(d_context, "TouchedSound", d_touchedSound, sizeof(d_touchedSound)-1);

	g_console.ExecuteContext(d_context);

#ifdef ENABLE_WATCHCLIENT
	WatchClient* instance = WatchClient::Instance();
	instance->RegisterVariable(d_context, &d_iterationRate[0],
			d_iterationRate[0], 0.0f, 1500.0f, "IterationRate");
	instance->RegisterVariable(d_context, &d_globalDamping[0],
			d_globalDamping[0], 0.0f, 1.0f, "GlobalDamping");
	instance->RegisterVariable(d_context, &d_baseMass[0],
			d_baseMass[0], 0.0f, 10.0f, "BaseMass");
	instance->RegisterVariable(d_context, &d_tipMass[0],
			d_tipMass[0], 0.0f, 10.0f, "TipMass");
	instance->RegisterVariable(d_context, &d_baseNaturalFrequency[0],
			d_baseNaturalFrequency[0], 0.1f, 30.0f, "BaseNatFreq");
	instance->RegisterVariable(d_context, &d_tipNaturalFrequency[0],
			d_tipNaturalFrequency[0], 0.1f, 30.0f, "TipNatFreq");
	instance->RegisterVariable(d_context, &d_baseDampingFactor[0],
			d_baseDampingFactor[0], 0.1f, 2.0f, "BaseDampFact");
	instance->RegisterVariable(d_context, &d_tipDampingFactor[0],
			d_tipDampingFactor[0], 0.1f, 2.0f, "TipDampFact");
	instance->RegisterVariable(d_context, &d_baseDragCoefficient[0],
			d_baseDragCoefficient[0], 0.0f, 0.5f, "BaseDragCoeff");
	instance->RegisterVariable(d_context, &d_tipDragCoefficient[0],
			d_tipDragCoefficient[0], 0.0f, 0.5f, "TipDragCoeff");
	instance->RegisterVariable(d_context, &d_boxLocation[0]._x,
			d_boxLocation[0].x(), -2.0f, 2.0f, "BoxLocationX");
	instance->RegisterVariable(d_context, &d_boxLocation[0]._y,
			d_boxLocation[0].y(), -2.0f, 2.0f, "BoxLocationY");
	instance->RegisterVariable(d_context, &d_boxLocation[0]._z,
			d_boxLocation[0].z(), -2.0f, 2.0f, "BoxLocationZ");
	instance->RegisterVariable(d_context, &d_boxRotation[0]._z,
			d_boxRotation[0].z(), -1.58f, 1.58f, "BoxRotationYaw");
	instance->RegisterVariable(d_context, &d_boxRotation[0]._x,
			d_boxRotation[0].x(), -1.58f, 1.58f, "BoxRotationPitch");
	instance->RegisterVariable(d_context, &d_boxRotation[0]._y,
			d_boxRotation[0].y(), -1.58f, 1.58f, "BoxRotationRoll");
	instance->RegisterVariable(d_context, &d_boxExtent[0]._x,
			d_boxExtent[0].x(), 0.0f, 2.0f, "BoxExtentX");
	instance->RegisterVariable(d_context, &d_boxExtent[0]._y,
			d_boxExtent[0].y(), 0.0f, 2.0f, "BoxExtentY");
	instance->RegisterVariable(d_context, &d_boxExtent[0]._z,
			d_boxExtent[0].z(), 0.0f, 2.0f, "BoxExtentZ");
	instance->RegisterVariable(d_context, &d_obbInflateTime[0],
			d_obbInflateTime[0], 0.0f, 1.0f, "OBBInflateTime");
	instance->RegisterVariable(d_context, &d_constraintScale[0],
			d_constraintScale[0], 0.0f, 2.0f, "ConstraintScale");
#endif //ENABLE_WATCHCLIENT

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(d_context);
#endif //INTERACTIVE_CONSOLE

	d_iterationRate[1] = d_iterationRate[0];
	d_globalDamping[1] = d_globalDamping[0];
	d_freeSwinging[1] = d_freeSwinging[0];
	d_baseMass[1] = d_baseMass[0];
	d_tipMass[1] = d_tipMass[0];
	d_baseNaturalFrequency[1] = d_baseNaturalFrequency[0];
	d_tipNaturalFrequency[1] = d_tipNaturalFrequency[0];
	d_baseDampingFactor[1] = d_baseDampingFactor[0];
	d_tipDampingFactor[1] = d_tipDampingFactor[0];
	d_baseDragCoefficient[1] = d_baseDragCoefficient[0];
	d_tipDragCoefficient[1] = d_tipDragCoefficient[0];
	d_boxActive[1] = d_boxActive[0];
	d_boxDisplay[1] = d_boxDisplay[0];
	d_boxLocation[1] = d_boxLocation[0];
	d_boxRotation[1] = d_boxRotation[0];
	d_boxExtent[1] = d_boxExtent[0];
	d_obbInflateTime[1] = d_obbInflateTime[0];
	d_constraintScale[1] = d_constraintScale[0];
}

void CPlantParam::UpdateContext(void)
{
	bool update = false;

	if (d_iterationRate[1] != d_iterationRate[0])
	{
		d_iterationRate[1] = d_iterationRate[0];
		update = true;
	}

	if (d_globalDamping[1] != d_globalDamping[0])
	{
		d_globalDamping[1] = d_globalDamping[0];
		update = true;
	}

	if (d_freeSwinging[1] != d_freeSwinging[0])
	{
		d_freeSwinging[1] = d_freeSwinging[0];
		update = true;
	}

	if (d_baseMass[1] != d_baseMass[0])
	{
		d_baseMass[1] = d_baseMass[0];
		update = true;
	}

	if (d_tipMass[1] != d_tipMass[0])
	{
		d_tipMass[1] = d_tipMass[0];
		update = true;
	}

	if (d_baseNaturalFrequency[1] != d_baseNaturalFrequency[0])
	{
		d_baseNaturalFrequency[1] = d_baseNaturalFrequency[0];
		update = true;
	}

	if (d_tipNaturalFrequency[1] != d_tipNaturalFrequency[0])
	{
		d_tipNaturalFrequency[1] = d_tipNaturalFrequency[0];
		update = true;
	}

	if (d_baseDampingFactor[1] != d_baseDampingFactor[0])
	{
		d_baseDampingFactor[1] = d_baseDampingFactor[0];
		update = true;
	}

	if (d_tipDampingFactor[1] != d_tipDampingFactor[0])
	{
		d_tipDampingFactor[1] = d_tipDampingFactor[0];
		update = true;
	}

	if (d_baseDragCoefficient[1] != d_baseDragCoefficient[0])
	{
		d_baseDragCoefficient[1] = d_baseDragCoefficient[0];
		update = true;
	}

	if (d_tipDragCoefficient[1] != d_tipDragCoefficient[0])
	{
		d_tipDragCoefficient[1] = d_tipDragCoefficient[0];
		update = true;
	}

	if (d_boxActive[1] != d_boxActive[0])
	{
		d_boxActive[1] = d_boxActive[0];
		update = true;
	}

	if (d_boxDisplay[1] != d_boxDisplay[0])
	{
		d_boxDisplay[1] = d_boxDisplay[0];
		update = true;
	}

	if (d_boxLocation[1] != d_boxLocation[0])
	{
		d_boxLocation[1] = d_boxLocation[0];
		update = true;
	}

	if (d_boxRotation[1] != d_boxRotation[0])
	{
		d_boxRotation[1] = d_boxRotation[0];
		update = true;
	}

	if (d_boxExtent[1] != d_boxExtent[0])
	{
		d_boxExtent[1] = d_boxExtent[0];
		update = true;
	}

	if (d_obbInflateTime[1] != d_obbInflateTime[0])
	{
		d_obbInflateTime[1] = d_obbInflateTime[0];
		update = true;
	}

	if (d_constraintScale[1] != d_constraintScale[0])
	{
		d_constraintScale[1] = d_constraintScale[0];
		update = true;
	}

	if (update)
	{
		CPlantInstance* curr = d_instanceHead;
		while (curr)
		{
			curr->d_plant->SetParams();
			curr = curr->d_instanceNext;
		}
	}
}

bool CPlantParam::HasPlant(CPlant* i_plant)
{
	CPlantInstance* curr = d_instanceHead;
	while (curr)
	{
		if (curr->d_plant == i_plant)
			return(true);
		curr = curr->d_instanceNext;
	}
	return(false);
}

void CPlantParam::AddPlant(CPlant* i_plant)
{
	MEM_SET_ONE_SHOT_NEW_NAME("CPlantPar::AddPlant");
	CPlantInstance* instance = new CPlantInstance;
	instance->d_instancePrev = NULL;
	instance->d_instanceNext = d_instanceHead;
	d_instanceHead = instance;
	instance->d_plant = i_plant;
	d_refCount++;
}

void CPlantParam::RemovePlant(CPlant* i_plant)
{
	CPlantInstance* curr = d_instanceHead;
	while (curr)
	{
		if (curr->d_plant == i_plant)
		{
			if (curr == d_instanceHead)
				d_instanceHead = curr->d_instanceNext;
			if (curr->d_instancePrev != NULL)
				curr->d_instancePrev->d_instanceNext = curr->d_instanceNext;
			if (curr->d_instanceNext != NULL)
				curr->d_instanceNext->d_instancePrev = curr->d_instancePrev;
			delete curr;
			d_refCount--;
			return;
		}
		curr = curr->d_instanceNext;
	}
}
