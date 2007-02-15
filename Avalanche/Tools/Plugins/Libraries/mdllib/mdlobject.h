#ifndef __MDLOBJECT_H
#define __MDLOBJECT_H

#ifndef __HSTATUS_H
#include "hstatus.h"
#endif

#ifndef __HSPLINEARRAY_H
#include "hsplinearray.h"
#endif

#ifndef __HPATCHARRAY_H
#include "hpatcharray.h"
#endif

#ifndef __HPOLYARRAY_H
#include "hpolyarray.h"
#endif

#ifndef __UIFLAGS_H
#include "uiflags.h"
#endif

class HPatchArray;

class mdlObject
{
public:
	mdlObject();
	mdlObject(const mdlObject &other);
	~mdlObject();

	void				clear();
	HStatus				load(UString fname);

	HStatus				calculateSplineTangents();
	HStatus				calculatePatchTangents();
	HStatus				calculatePolygons();
	HStatus				triangulatePolygons();
	HStatus				createSkeleton();

	bool				HasBones();

	mdlObject			&operator = ( const mdlObject &other );
	friend std::ostream		&operator << (std::ostream &os,mdlObject &other);

	HSplineArray		splineArray;
	HPatchArray			*patchArray;
	HPolyArray			*polygonArray;

	UString filename;

	unsigned ui_numsubdivisions;
	double ui_displacefactor;
	unsigned ui_normalsamples;
//	double ui_straightnesstolerance;

	float version;

	UIFlags ui_flags;

private:
	void				readModel(std::ifstream &fin);
	void				analyzeModel(std::ifstream &fin);
};

#endif


