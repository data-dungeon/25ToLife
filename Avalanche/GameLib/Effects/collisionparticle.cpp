/*
**
**  File:   CollisionParticle.cpp
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   A table for particles emitted by collisions.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/Effects/collisionparticle.cpp $
**  $Revision: 9 $
**      $Date: 11/21/02 12:24p $
**    $Author: Dross $
**
*/
#include "Effects/EffectsPCH.h"

CCollisionParticle g_collisionParticle;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SCollisionParticleData::SCollisionParticleData()
{
	d_particle[0] = '\0';
	d_particleScale = 1.0f;
	d_sound[0] = '\0';
	d_decalIndex = -1;
	d_decalRadius = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CCollisionParticle::CCollisionParticle()
{
	d_table = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CCollisionParticle::~CCollisionParticle()
{
	Unload();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SCollisionParticleData* CCollisionParticle::GetEntry(
int i_surface1,
int i_surface2)
{
	ASSERT(d_table && d_lookup);

	int entry = d_lookup->GetEntry(i_surface1, i_surface2);
	ASSERT(entry >= 0);
	if (entry >= 0)
	   return &d_table[entry];

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CCollisionParticle::Init(int i_size)
{
	delete[] d_table;
	d_table = new SCollisionParticleData[i_size];
	ASSERT(d_table != NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CCollisionParticle::LoadVariable(const char* i_variable, int i_entry)
{
	ASSERT(d_table && d_lookup);

	if (strcmpi(i_variable, "particle") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_particle, sizeof(d_table[i_entry].d_particle));
	else if (strcmpi(i_variable, "particleScale") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_particleScale);
	else if (strcmpi(i_variable, "sound") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_sound, sizeof(d_table[i_entry].d_sound));
	else if (strcmpi(i_variable, "decalIndex") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_decalIndex);
	else if (strcmpi(i_variable, "decalRadius") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_decalRadius);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CCollisionParticle::Copy(int i_destEntry, int i_srcEntry)
{
	d_table[i_destEntry] = d_table[i_srcEntry];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CCollisionParticle::SaveEntry(int i_entry)
{
	ASSERT(d_table && d_lookup);

	d_lookup->SaveNamedToken("particle", d_table[i_entry].d_particle);
	d_lookup->SaveNamedToken("particleScale", d_table[i_entry].d_particleScale);
	d_lookup->SaveNamedToken("sound", d_table[i_entry].d_sound);
	d_lookup->SaveNamedToken("decalIndex", d_table[i_entry].d_decalIndex);
	d_lookup->SaveNamedToken("decalRadius", d_table[i_entry].d_decalRadius);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CCollisionParticle::Unload()
{
	delete[] d_table;
	d_table = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Watch stuff - HACK ALERT!!
///////////////////////////////////////////////////////////////////////////////

#include "stage/SurfaceLookup.h"  // To downcast d_lookup!!

void CCollisionParticle::LoadWatch()
{
	//yuch-- this is done in UnloadWatch, which should be paired with this call
	//    DeleteLabels();
	//    DeleteData();

	//---------------------------------------------
	//  Header
	//---------------------------------------------
	nRows = ((CSurfaceLookup*)d_lookup)->GetNumRow();
	nCols = ((CSurfaceLookup*)d_lookup)->GetNumCol();

	//---------------------------------------------
	//  Labels
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
	//  Data
	//---------------------------------------------
	fData = 0;

	Register();
}

///////////////////////////////////////////////////////////////////////////////
// clean up
///////////////////////////////////////////////////////////////////////////////
void CCollisionParticle::UnloadWatch()
{
	DeleteLabels();
	DeleteData();
}

//----------------------------------------------------------------------
// CCollisionParticle::Register
//    Register this table and all it's variables with Watch.
//----------------------------------------------------------------------
void CCollisionParticle::RegisterVariables()
{
#ifdef ENABLE_WATCHCLIENT
	WatchClient* wc = WatchClient::Instance();

	wc->RegisterTableVariable(TableName(), this, VARTYPE_STRING, 0.0f, 1.0f, 0, "Particle");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, 0.0f, 10.0f, 0, "Particle Scale");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_STRING, 0.0f, 1.0f, 0, "Sound");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_INT, -1.0f, 100.0f, 0, "Decal");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, 0.0f, 1.0f, 0, "Decal Radius");
#endif //ENABLE_WATCHCLIENT
}

//----------------------------------------------------------------------
// CCollisionParticle::RequestData
//    The watch program is asking for some table data.
//----------------------------------------------------------------------
void CCollisionParticle::RequestData(int row, int column)
{
#ifdef ENABLE_WATCHCLIENT
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
	   return;

	VAR_VAL data[5];
	SetVARVAL(data[0], d_table[entry].d_particle);
	SetVARVAL(data[1], d_table[entry].d_particleScale);
	SetVARVAL(data[2], d_table[entry].d_sound);
	SetVARVAL(data[3], d_table[entry].d_decalIndex);
	SetVARVAL(data[4], d_table[entry].d_decalRadius);

	WatchClient::Instance()->TableData(this, row, column, data, sizeof(data) / sizeof(VAR_VAL));
#endif //ENABLE_WATCHCLIENT
}

//----------------------------------------------------------------------
// CCollisionParticle::SetData
//    Set table data here.  This is called from the watch program.
//----------------------------------------------------------------------
void CCollisionParticle::SetData(int row, int column, int dataIndex, VAR_VAL& data)
{
#ifdef ENABLE_WATCHCLIENT
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
	   return;

	if (dataIndex == 0)
		GetVARVALString(data, d_table[entry].d_particle, sizeof(d_table[entry].d_particle));
	else if (dataIndex == 1)
		d_table[entry].d_particleScale = GetVARVALFloat(data);
	else if (dataIndex == 2)
		GetVARVALString(data, d_table[entry].d_sound, sizeof(d_table[entry].d_sound));
	else if (dataIndex == 3)
		d_table[entry].d_decalIndex = GetVARVALInt(data);
	else if (dataIndex == 4)
		d_table[entry].d_decalRadius = GetVARVALFloat(data);
#endif
}

//----------------------------------------------------------------------
// CCollisionParticle::NewRowColumn
//    Disallow adding rows of columns via watch.
//----------------------------------------------------------------------
bool CCollisionParticle::NewRowColumn(char* name, bool isRow)
{
	return true;
}

bool CCollisionParticle::Save(char* filename)
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
	return true;
}
