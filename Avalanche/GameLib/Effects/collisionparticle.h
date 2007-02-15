/*
**
**  File:   CollisionParticle.h
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   A table for particles emitted by collisions.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/effects/collisionparticle.h $
**  $Revision: 3 $
**      $Date: 1/25/02 3:27p $
**    $Author: Nmartin $
**
*/

#ifndef COLLISION_PARTICLE_H
#define COLLISION_PARTICLE_H

#include "GameHelper/TableLookup.h"
#include "platform/BaseType.h"
#include "WatchClient/DataTable.h"    // To spoof watch.

class SCollisionParticleData
{
public:
	SCollisionParticleData();

	char		d_particle[VARTYPE_STRING_SIZE];
	float		d_particleScale;
	char		d_sound[VARTYPE_STRING_SIZE];
	int		d_decalIndex;
	float		d_decalRadius;
};

class CCollisionParticle : public CTableData, public DataTable<SCollisionParticleData>
{
public:
	CCollisionParticle();
	virtual ~CCollisionParticle();
	SCollisionParticleData* GetEntry(int i_surface1, int i_surface2);

	// Watch stuff.
	void LoadWatch();
	void UnloadWatch();
	void RequestData(int, int);
	void SetData(int, int, int, VAR_VAL&);
	bool NewRowColumn(char*, bool);
	char* TableName() { return "CollPart"; }
	int Version() { return 1; }
	bool Save(char* filename);

private:
	void Init(int i_size);
	bool LoadVariable(const char* i_variable, int i_entry);
	void Copy(int i_destEntry, int i_srcEntry);
	void SaveEntry(int i_entry);
	void Unload();

	SCollisionParticleData* d_table;

	//	Watch stuff.
	void RegisterVariables();
};

extern CCollisionParticle g_collisionParticle;

#endif // COLLISION_PARTICLE_H
