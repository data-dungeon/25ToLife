/*
**
**  File:   RigidBodyTable.cpp
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   A table for rigid body collision data.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/physics/RigidBodyTable.cpp $
**  $Revision: #6 $
**      $Date: 2005/03/22 $
**    $Author: Rob $
**
*/

#include "Components/ComponentsPCH.h"
#include "Components/RigidBodyTable.h"

#define DEFAULT_COLLISION_TYPE  0
#define DEFAULT_COR             0.0f
#define DEFAULT_COG             1.0f

CRigidBodyTable g_rigidBodyTable;

/******************************************************************************/
/******************************************************************************/
CRigidBodyTable::CRigidBodyTable()
{
	d_table = NULL;
}

/******************************************************************************/
/******************************************************************************/
CRigidBodyTable::~CRigidBodyTable()
{
	Unload();
}

/******************************************************************************/
/******************************************************************************/
SRigidBodyData* CRigidBodyTable::GetEntry(int i_surface1, int i_surface2)
{
	ASSERT(d_table && d_lookup);

	int entry = d_lookup->GetEntry(i_surface1, i_surface2);
	ASSERT(entry >= 0);
	if (entry >= 0)
		return(&d_table[entry]);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
SRigidBodyData* CRigidBodyTable::GetValidEntry(int i_surface1, int i_surface2)
{
	ASSERT(d_table && d_lookup);

	int entry = d_lookup->GetValidEntry(i_surface1, i_surface2);
	ASSERT(entry >= 0);

	return(&d_table[entry]);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::Init(int i_size)
{
	delete[] d_table;
	d_table = new SRigidBodyData[i_size];
	for (int i = 0; i < i_size; i++)
	{
		d_table[i].d_cor = DEFAULT_COR;
		d_table[i].d_cog = DEFAULT_COG;
	}
}

#define VARIABLE_LENGTH	 80

/******************************************************************************/
/******************************************************************************/
bool CRigidBodyTable::LoadVariable(const char* i_variable, int i_entry)
{
	ASSERT(d_table && d_lookup);

	if (strcmpi(i_variable, "cor") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_cor);
	else if (strcmpi(i_variable, "cog") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_cog);

	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::Copy(int i_destEntry, int i_srcEntry)
{
	d_table[i_destEntry] = d_table[i_srcEntry];
}

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::SaveEntry(int i_entry)
{
	ASSERT(d_table && d_lookup);

	d_lookup->SaveNamedToken("cor", d_table[i_entry].d_cor);
	d_lookup->SaveNamedToken("cog", d_table[i_entry].d_cog);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::Unload(void)
{
	delete[] d_table;
	d_table = NULL;
}

// Watch stuff - HACK ALERT!!

#include "stage/SurfaceLookup.h"  // To downcast d_lookup!!

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::LoadWatch(void)
{
//yuch-- this is done in UnloadWatch, which should be paired with this call
//	  DeleteLabels();
//	  DeleteData();

	//---------------------------------------------
	//	Header
	//---------------------------------------------
	nRows = ((CSurfaceLookup*)d_lookup)->GetNumRow();
	nCols = ((CSurfaceLookup*)d_lookup)->GetNumCol();

	//---------------------------------------------
	//	Labels
	//---------------------------------------------
	fRowLabels = new char*[nRows];
	fColLabels = new char*[nCols];

	int i;
	for (i = 0; i < nRows; i++)
	{
		char* label = ((CSurfaceLookup*)d_lookup)->GetMappedRow(i);
		if (!label)
			continue;
		fRowLabels[i] = new char[strlen(label) + 1];
		strcpy(fRowLabels[i], label);
	}
	for (i = 0; i < nCols; i++)
	{
		char* label = ((CSurfaceLookup*)d_lookup)->GetMappedCol(i);
		if (!label)
			continue;
		fColLabels[i] = new char[strlen(label) + 1];
		strcpy(fColLabels[i], label);
	}

	//---------------------------------------------
	//	Data
	//---------------------------------------------
	fData = 0;

	Register();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBodyTable::UnloadWatch(void)
{
	DeleteLabels();
	DeleteData();
}

/******************************************************************************/
// Register this table and all it's variables with Watch.
/******************************************************************************/
void CRigidBodyTable::RegisterVariables()
{
#ifdef ENABLE_WATCHCLIENT
	WatchClient* wc = WatchClient::Instance();
	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, -1.0f, 0.0f, 0,
			"Coefficient of Restitution");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, -1.0f, 1.0f, 1,
			"Coefficient of Grip");
#endif //ENABLE_WATCHCLIENT
}

/******************************************************************************/
// The watch program is asking for some table data.
/******************************************************************************/
void CRigidBodyTable::RequestData(int row, int column)
{
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
		return;

	VAR_VAL data[2];
	SetVARVAL(data[0], d_table[entry].d_cor);
	SetVARVAL(data[1], d_table[entry].d_cog);
#ifdef ENABLE_WATCHCLIENT
	WatchClient::Instance()->TableData(this, row, column, data, 2);
#endif //ENABLE_WATCHCLIENT
}

/******************************************************************************/
// Set table data here. This is called from the watch program.
/******************************************************************************/
void CRigidBodyTable::SetData(int row, int column, int dataIndex, VAR_VAL& data)
{
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
		return;

	if (dataIndex == 0)
		d_table[entry].d_cor = GetVARVALFloat(data);
	else if (dataIndex == 1)
		d_table[entry].d_cog = GetVARVALFloat(data);
}

/******************************************************************************/
// Disallow adding rows of columns via watch.
/******************************************************************************/
bool CRigidBodyTable::NewRowColumn(char* name, bool isRow)
{
	return true;
}

/******************************************************************************/
/******************************************************************************/
bool CRigidBodyTable::Save(char* filename)
{
	char filepath[100];
	strcpy(filepath, "tables/");
	strcat(filepath, filename);
	int i = 0;
	while(filepath[i] != '\0')
	{
		if (filepath[i] == '.')
		{
			filepath[i] = '\0';
			break;
		}
		i++;
	}
	strcat(filepath, ".tbd");
	d_lookup->Save(filepath);
	return(true);
}
