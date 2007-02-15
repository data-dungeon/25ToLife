//////////////////////////////////////////////////////////////////////////////////////////
//
// HSPLINE ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HSPLINEARRAY_H
#define __HSPLINEARRAY_H

#ifndef __HCP_H
#include "hcp.h"
#endif
#ifndef __HSPLINE_H
#include "hspline.h"
#endif
#ifndef __HINTMAP_H
#include "hintmap.h"
#endif
#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HSPLINESEGMENT_H
#include "hsplinesegment.h"
#endif
#ifndef __HVECTORMAP_H
#include "hvectormap.h"
#endif
#ifndef __HNEIGHBOR_H
#include "hneighbor.h"
#endif
#ifndef __HHOOK_H
#include "hhook.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif

class mdlObject;
class HCP;

class HSplineArray
{
public:
	HSplineArray();
	HSplineArray(const HSplineArray &other);
	~HSplineArray();

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	void				clear(void);
	unsigned			length(void) const;
	void				append(HSpline spline);

	void				setupCPInfo(void);
	void				calculateTangents(void);

	void				findSplineRelativeCP(unsigned absCPIndex, unsigned &relCPIndex, unsigned &splineIndex);

	unsigned			getNewCPindex(void);
	unsigned			getNewCPindex2(void);

	unsigned			HashCPIndexToMyCPIndex(unsigned cpindex);
	unsigned			MyCPIndexToHashCP(unsigned cpindex);

	HVector				getCPPos(unsigned cpindex);
	HVector				getCPPos(HCP *cp);
	unsigned			addVertexToList( const HVector &pos );
	HVector				getHookPos(HHook *hook);
	HVector				getHookPos(unsigned hookindex);
	HVector				getHookPos(HCP *hookcp);

	void				finalizeTangent(HVector p2, HVector &it, HVector &ot);

	HCP					*getCP(int relCPIndex,unsigned splineIndex);
	HCP					*getCP(unsigned current);
	HCP					*getNextCP(unsigned current);
	HCP					*getPreviousCP(unsigned current);
	HCP					*getParentCP(unsigned cpidx);
	HCP					*getChildCP(unsigned cpidx);
	HCP					*getHeadCP(unsigned cpindex);
	HCP					*getHeadCP(HCP *cp);
	HCP					*getTailCP(unsigned cpindex);
	HCP					*getTailCP(HCP *cp);

	void				getChildStack(unsigned cpindex,std::vector<int> &stack);

	int					getSplineDirection(unsigned index1,unsigned index2);
	HSplineSegment		getSplineSegment(unsigned cpi1, unsigned cpi2,std::vector<int> &patchIndicies);
	void				getSegmentPatchAssociations(unsigned cpvindex0,unsigned cpvindex1,std::vector<int> &patchIndicies);

	bool				doesCPHookbasePointToIndex(HCP *cp,unsigned idx);
	bool				doesCPHookbaseNeighborPointToIndex(HCP *cp,unsigned idx);

	HIntMap				*getCPStack(unsigned cpindex);
	void				getParentCPs(unsigned cpindex, HIntMap &stackmap);
	void				getChildCPs(unsigned cpindex, HIntMap &stackmap);
	void				fillCPStack(unsigned cpindex, HIntMap &stackmap);

	void				flagCPStackWithPatchIndex(HIntMap &stackmap,unsigned patchindex);

	HStatus				clearCPIndexTable(void);

	void				setHooks(void);
	void				findHookVertex(HCP *hookcp,HVector &point);
	void				splitHookedSegment(HCP *hookcp,HSplineSegment &seg1,HSplineSegment &seg2);

	bool				isHook(unsigned cpindex);
	bool				isHookbase(unsigned cpindex);
	bool				isHookbaseNeighbor(unsigned cpindex);

	bool				isHeadCP(unsigned cpindex);
	bool				isTailCP(unsigned cpindex);

	bool				drillForHook(unsigned cpindex,unsigned &index);
	bool				drillForHook(HIntMap &stackmap,unsigned &index);
	bool				drillForHookbase(HIntMap &stackmap,unsigned &index,HHook *hook);
	bool				drillForHookbaseNeighbor(HIntMap &stackmap,unsigned &index,HHook *hook);
	bool				drillFor5pointVertex(unsigned cpindex,unsigned &index);

	int					drillForVertexIndex(unsigned cpindex);
	int					drillForVertexIndex(HCP *cp);

	void				analyzeMesh(std::ifstream &fin);
	void				readHSplineArray(std::ifstream &fin);

	HSpline				*get(unsigned index);
	HSpline				*begin();
	HSpline				*end();

	void				updateBackPointers();

	HSplineArray		&operator = ( const HSplineArray &other );
	HSpline				operator[](unsigned index) const;
	HSpline				&operator[](unsigned index);
	friend std::ostream		&operator << (std::ostream &os,HSplineArray &other);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	HVectorArray			vertices;
	HHookArray				hookList;
	HNeighborArray			neighbors;

	// these tables are for numbering cp's to match hash
	unsigned				abscpindex;
	HIntMap					*cpIdxMap;
	std::vector<int>		*hashCPTable;

	// these tables are for getting an absolute cp
	std::vector<int>		splineTable,absCpiTable,relCpiTable;

	HIntMap					*absCpIdxMap;

	// this table tracks of cps being added
	std::vector<int>		*cpIndexTable;

	// fast cp stack lookup
	HNeighborArray			*stackList;

	mdlObject				*model;

	std::vector<HSpline> classList;
};

#include "hsplinearray.hpp"

#endif