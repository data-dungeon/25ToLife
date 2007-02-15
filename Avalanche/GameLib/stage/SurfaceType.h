/*
**
**  File:   SurfaceType.h
**  Date:   February 22, 2001
**  By:     Bryant Collard
**  Desc:   A list of surface types with IDs.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/SurfaceType.h $
**  $Revision: 5 $
**      $Date: 6/29/01 12:33p $
**    $Author: Nmartin $
**
*/

#ifndef SURFACE_TYPE_H
#define SURFACE_TYPE_H

#include "GameHelper/TableLabel.h"
#include "Layers/LayersStruct.h"
#include "EngineHelper/Singleton.h"

class FileStream;

struct SSurfaceEntry
{
	char* d_name;
	enum
	{
		SURFACE_TYPE_READ_ONLY,
		SURFACE_TYPE_SAVE,
		SURFACE_TYPE_DELETE
	} d_storage;
	bool d_dynamic;
	bool d_kineticTypeSpecified;
};

class CSurfaceType : public CTableLabel
{
  public:
	enum EKineticType
	{
		KINETIC_STATIC,
		KINETIC_DYNAMIC,
		KINETIC_UNSPECIFIED
	};

	CSurfaceType();
	~CSurfaceType();
	void Clear(void);
	void Load(t_Handle i_worldHandle, const char* i_file);
	void Save(char* i_file) const;
	int AddEntry(const char* i_name, EKineticType i_kineticType);
	bool DeleteEntry(const char* i_name, int i_testLength = -1);
	bool DeleteEntry(int i_index);
	virtual char* GetName(int i_index) const;
	int GetNumDynamicEntries(void) const {return(d_numDynamicEntries);};
	bool IsDynamic(int i_index) {return(d_entry[i_index].d_dynamic);};
	int GetValidDynamicIndex(char* io_name, int i_defaultIndex,
			int i_nameSize) const;
	virtual bool GetTitle(char* o_title, int i_length) const;
	virtual char* GetInfo(int i_index, char* o_info, int i_length) const;
  private:
	char* ParseToken(char* io_token);
	EKineticType EvaluateKinetic(char* i_kinetic);
	bool AppendEntry(const char* i_name, int i_length, bool i_readOnly,
			EKineticType i_kineticType);
	int d_numDynamicEntries;
	SSurfaceEntry* d_entry;
	int d_maxNameLength;
};

DefineSingleton(CSurfaceType)
#define g_surfaceType GetSingleton(CSurfaceType)

#endif //  SURFACE_TYPE_H
