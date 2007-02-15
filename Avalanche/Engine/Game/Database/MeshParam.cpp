/*
**
**  File:   MeshParam.cpp
**  Date:   May 6, 2002
**  By:     Bryant Collard
**  Desc:   Interactive mesh parameters.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshParam.cpp $
**  $Revision: 15 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

/******************* includes ****************************/

#include "Game/GamePCH.h"
#include "cmdcon/cmdcon.h"





CMeshParam* CMeshParam::d_paramHead = NULL;

// Control flag stuff.
const CMeshParam::TControlFlag CMeshParam::MESH_NO_UPDATE       = 0x00;
const CMeshParam::TControlFlag CMeshParam::MESH_UPDATE          = 0x07;
const CMeshParam::TControlFlag CMeshParam::MESH_DYNAMIC         = 0x01;
const CMeshParam::TControlFlag CMeshParam::MESH_SNAP_TO_INIT    = 0x02;
const CMeshParam::TControlFlag CMeshParam::MESH_DECAY_TO_INIT   = 0x04;
const CMeshParam::TControlFlag CMeshParam::MESH_APPLY_GRAVITY   = 0x02;
const CMeshParam::TControlFlag CMeshParam::MESH_APPLY_DRAG      = 0x04;
const CMeshParam::TControlFlag CMeshParam::MESH_CHECK_OBJECT    = 0x08;
const CMeshParam::TControlFlag CMeshParam::MESH_CONSTRAINT_DIR  = 0x10;
const CMeshParam::TControlFlag CMeshParam::MESH_COMPUTE_NORMALS = 0x20;
const CMeshParam::TControlFlag CMeshParam::MESH_UPDATE_SUB_MESH = 0x40;

/***************************************************************************/
/***************************************************************************/
CMeshParamInstance::CMeshParamInstance()
{
	d_mesh = NULL;
	d_instancePrev = NULL;
	d_instanceNext = NULL;
}

/***************************************************************************/
/***************************************************************************/
CMeshParam::CMeshParam()
{
	d_paramPrev = NULL;
	d_paramNext = NULL;
	d_instanceHead = NULL;
	d_context[0] = '\0';
	d_refCount = 0;
}

/***************************************************************************/
/***************************************************************************/
CMeshParam::~CMeshParam()
{
	if (d_paramPrev != NULL)
		d_paramPrev->d_paramNext = d_paramNext;
	if (d_paramNext != NULL)
		d_paramNext->d_paramPrev = d_paramPrev;

	CMeshParamInstance* curr = d_instanceHead;
	while (curr)
	{
		CMeshParamInstance* next = curr->d_instanceNext;
		delete curr;
		curr = next;
	}
}

/***************************************************************************/
/***************************************************************************/
CMeshParam* CMeshParam::Register(CMeshInstance* i_mesh, const char* i_context,
		const char* i_varPath)
{
	CMeshParam* curr = d_paramHead;
	while (curr)
	{
		if (curr->HasMesh(i_mesh))
			return(curr);
		curr = curr->d_paramNext;
	}

	curr = d_paramHead;
	while (curr)
	{
		if (!strncmp(curr->d_context, i_context, CONTEXT_NAME_LENGTH))
		{
			curr->AddMesh(i_mesh);
			return(curr);
		}
		curr = curr->d_paramNext;
	}

	curr = new CMeshParam;
	curr->d_paramPrev = NULL;
	curr->d_paramNext = d_paramHead;
	d_paramHead = curr;
	curr->CreateContext(i_context, i_varPath);
	curr->AddMesh(i_mesh);
	return(curr);
}

/***************************************************************************/
/***************************************************************************/
void CMeshParam::Unregister(CMeshInstance* i_mesh)
{
	CMeshParam* curr = d_paramHead;
	while (curr)
	{
		if (curr->HasMesh(i_mesh))
		{
			curr->RemoveMesh(i_mesh);
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

/***************************************************************************/
/***************************************************************************/
void CMeshParam::CreateContext(const char* i_context, const char* i_varPath)
{
	strncpy(d_context, i_context, CONTEXT_NAME_LENGTH);
	d_context[CONTEXT_NAME_LENGTH - 1] = '\0';

	d_controlFlag[CBaseLOD::FOCUS] = MESH_DYNAMIC | MESH_APPLY_GRAVITY |
			MESH_APPLY_DRAG | MESH_CHECK_OBJECT | MESH_COMPUTE_NORMALS |
			MESH_UPDATE_SUB_MESH;
	d_controlFlag[CBaseLOD::HIGH] = MESH_DYNAMIC | MESH_APPLY_GRAVITY |
			MESH_APPLY_DRAG | MESH_CHECK_OBJECT | MESH_COMPUTE_NORMALS |
			MESH_UPDATE_SUB_MESH;
	d_controlFlag[CBaseLOD::MEDIUM] = MESH_DYNAMIC | MESH_APPLY_GRAVITY |
			MESH_APPLY_DRAG | MESH_CHECK_OBJECT | MESH_UPDATE_SUB_MESH;
	d_controlFlag[CBaseLOD::LOW] = MESH_DYNAMIC | MESH_APPLY_GRAVITY |
			MESH_APPLY_DRAG | MESH_UPDATE_SUB_MESH;
	d_controlFlag[CBaseLOD::PERIPHERY] = MESH_DYNAMIC | MESH_APPLY_GRAVITY |
			MESH_APPLY_DRAG;
	d_controlFlag[CBaseLOD::ABSENT] = MESH_NO_UPDATE;
	d_iterationRate = 300.0f;
	d_globalDamping = 1.0f;
	d_decayTimeConstant = 2.0f;
	d_gravity.Set(0.0f, -9.81f, 0.0f);
	d_dragCoefficient = 0.1f;
	d_constraintDir.Set(0.0f, -1.0f, 0.0f);

	char var[128];
	if ((i_varPath != NULL) && (i_varPath[0] != '\0'))
	{
		strcpy (var, i_varPath);
		strcat (var, d_context);
	}
	else
		strcpy (var, d_context);

	g_console.DeleteContext(d_context);
	g_console.BindScript(d_context, var);

	g_console.CreateVar(d_context, "FocusControlFlag",
			d_controlFlag[CBaseLOD::FOCUS], true);
	g_console.CreateVar(d_context, "HighControlFlag",
			d_controlFlag[CBaseLOD::HIGH], true);
	g_console.CreateVar(d_context, "MediumControlFlag",
			d_controlFlag[CBaseLOD::MEDIUM], true);
	g_console.CreateVar(d_context, "LowControlFlag",
			d_controlFlag[CBaseLOD::LOW], true);
	g_console.CreateVar(d_context, "PeripheryControlFlag",
			d_controlFlag[CBaseLOD::PERIPHERY], true);
	g_console.CreateVar(d_context, "AbsentControlFlag",
			d_controlFlag[CBaseLOD::ABSENT], true);
	g_console.CreateVar(d_context, "IterationRate", d_iterationRate);
	g_console.CreateVar(d_context, "GlobalDamping", d_globalDamping);
	g_console.CreateVar(d_context, "DecayTimeConstant", d_decayTimeConstant);
	g_console.CreateVar(d_context, "Gravity", (Vector3Packed*)&d_gravity);
	g_console.CreateVar(d_context, "DragCoefficient", d_dragCoefficient);
	g_console.CreateVar(d_context, "ConstraintDir", (Vector3Packed*)&d_constraintDir);

	g_console.ExecuteContext(d_context);

#ifdef ENABLE_WATCHCLIENT
	WatchClient* instance = WatchClient::Instance();
	instance->RegisterVariable(d_context, &d_iterationRate,
			d_iterationRate, 0.0f, 3000.0f, "IterationRate");
	instance->RegisterVariable(d_context, &d_globalDamping,
			d_globalDamping, 0.0f, 1.0f, "GlobalDamping");
	instance->RegisterVariable(d_context, &d_decayTimeConstant,
			d_decayTimeConstant, 0.0f, 1.0f, "DecayTimeConstant");
	instance->RegisterVariable(d_context, &d_gravity._x,
			d_gravity.x(), -50.0f, 50.0f, "GravityX");
	instance->RegisterVariable(d_context, &d_gravity._y,
			d_gravity.y(), -50.0f, 50.0f, "GravityY");
	instance->RegisterVariable(d_context, &d_gravity._z,
			d_gravity.z(), -50.0f, 50.0f, "GravityZ");
	instance->RegisterVariable(d_context, &d_dragCoefficient,
			d_dragCoefficient, 0.0f, 5.0f, "DragCoefficient");
	instance->RegisterVariable(d_context, &d_constraintDir._x,
			d_constraintDir.x(), -1.0f, 1.0f, "ConstraintDirX");
	instance->RegisterVariable(d_context, &d_constraintDir._y,
			d_constraintDir.y(), -1.0f, 1.0f, "ConstraintDirY");
	instance->RegisterVariable(d_context, &d_constraintDir._z,
			d_constraintDir.z(), -1.0f, 1.0f, "ConstraintDirZ");
#endif //ENABLE_WATCHCLIENT

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(d_context);
#endif //INTERACTIVE_CONSOLE
}

/***************************************************************************/
/***************************************************************************/
bool CMeshParam::HasMesh(CMeshInstance* i_mesh)
{
	CMeshParamInstance* curr = d_instanceHead;
	while (curr)
	{
		if (curr->d_mesh == i_mesh)
			return(true);
		curr = curr->d_instanceNext;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CMeshParam::AddMesh(CMeshInstance* i_mesh)
{
	CMeshParamInstance* instance = new CMeshParamInstance;
	instance->d_instancePrev = NULL;
	instance->d_instanceNext = d_instanceHead;
	d_instanceHead = instance;
	instance->d_mesh = i_mesh;
	d_refCount++;
}

/***************************************************************************/
/***************************************************************************/
void CMeshParam::RemoveMesh(CMeshInstance* i_mesh)
{
	CMeshParamInstance* curr = d_instanceHead;
	while (curr)
	{
		if (curr->d_mesh == i_mesh)
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
