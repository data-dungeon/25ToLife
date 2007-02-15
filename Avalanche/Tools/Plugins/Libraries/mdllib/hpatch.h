//////////////////////////////////////////////////////////////////////////////////////////
//
// HPATCH CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HPATCH_H
#define __HPATCH_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HPATCHVECTORS_H
#include "hpatchvectors.h"
#endif
#ifndef __HPATCHVECTORS2D_H
#include "hpatchvectors2D.h"
#endif
#ifndef __UIFLAGS_H
#include "uiflags.h"
#endif
#ifndef __HSPLINESEGMENT_H
#include "hsplinesegment.h"
#endif
#ifndef __HPOLY_H
#include "hpoly.h"
#endif
#ifndef __HMATRIX_H
#include "hmatrix.h"
#endif
#ifndef __HPATCHARRAY_H
#include "hpatcharray.h"
#endif
#ifndef __HMATRIX44_H
#include "hmatrix44.h"
#endif

class HPatchArray;
class HProxyPoly;
class HEdgeArray;
class HPolyArray;
class HIntMap;

class HPatch
{
public:

	struct pflags {
		unsigned long	kIs5point				: 1,
						kIs3point				: 1,
						kGroupimagerotation		: 2;
	} flags;


	/////////////////////////////////////////////////////////////////////////////////////
	// class methods
	/////////////////////////////////////////////////////////////////////////////////////

	HPatch();
	HPatch(const HPatch &other);
	~HPatch();

	void				readHPatch(std::ifstream &fin);
	void				calculateTangents();
	void				calculateInteriorTangents(HVector *pVectors);
	void				calc5PointPatchVectors(HSplineSegment &segA,HSplineSegment &segB,HSplineSegment &segC,
											  HSplineSegment &segD,HSplineSegment &segE);
	std::vector<int>	get4SidedPatchVectorIndicies(unsigned side);
	HVector				get5PointPatchMidPoint(const HSplineSegment &seg,const HSplineSegment &seg1,const HSplineSegment &seg2,
									   const HSplineSegment &seg3,const HSplineSegment &seg4,HVector &planenormal);
	HVector				calcNormalFrom5Vertices(HVector &v0, HVector &v1, HVector &v2, HVector &v3, HVector &v4);
	HVector				calcNormalFrom4Vertices(HVector &v0, HVector &v1, HVector &v2, HVector &v3);
	void				derivePatchSegments();
	bool				findConnection(HIntMap *cpstack0,HIntMap *cpstack1,HCP **cp0,HCP **cp1);
	HSplineSegment		getSegmentFromSide(unsigned side);
	void				setEdgePatchVectors(unsigned side, const HSplineSegment &seg, HVector *pVectors);
	void				raiseMidPoint(const HVector &p1,const HVector &p2,const HVector &p3,const HVector &p4,const HVector &p5,HVector &midpoint,HVector n );
	void				setPatchVectors(unsigned is3point,int subpatchid,const HSplineSegment &segA,const HSplineSegment &segB,const HSplineSegment &segC,const HSplineSegment &segD);
	HVector				calcPointOnPatch(double s, double t, unsigned nPatch);
	HVector				calcNormalOnPatch(double s, double t, unsigned nPatch);
	HVector2D			calcUVOnPatch(double s, double t, unsigned nPatch);
	HVector2D			calcUVOn4Point(double s, double t, HVector2D repeat, HPatchVectors2D patchVectors);
	HVector2D			calcUVOn5Point(double s, double t,unsigned nPatch, HVector2D repeat, HPatchVectors *pvect);
	void				createPatchMatrix(unsigned uspans,unsigned vspans,unsigned nspans,unsigned pvectidx,
							HMatrix &nvectmat,HMatrix &vmat,HMatrix &nmat,HMatrix &uvmat);
	void				tessellate(HPolyArray &polygons,std::vector<int> &shaderIndicies,unsigned numsubdivisions);
	void				calcPolysMidpointSubdivision(HPolyArray &polygons,std::vector<int> &shaderIndicies,unsigned uspans,unsigned vspans);
//	void				calcPolysVariableSubdivision(HPolyArray &polygons,std::vector<int> &shaderIndicies);
	bool				isUStraight(double tol);
	bool				isVStraight(double tol);
	void				getUSubdivideInfo(double tol,HDoubleArray &percent);
	void				getVSubdivideInfo(double tol,HDoubleArray &percent);
	void				getSubdivideInfo(double tol,HDoubleArray &upercent,HDoubleArray &vpercent);
	void				setFlagsFromOldFlags(unsigned long flags);//THESE FLAGS ARE OBSOLETE
	void				setFlags(unsigned long flags);
	void				setFlags(HPatch::pflags flags);
	unsigned long		getFlags();
	void				assignAdjacentPatch(std::vector<int> &patchIndicies,HSplineSegment &seg);
	HSplineSegment		getSplineSegment(unsigned side);
	bool				hasDecals();
	bool				hasGroups();
	void				createEdgeList(unsigned u,unsigned v,unsigned umax,unsigned vmax,unsigned pvectidx,bool is3point,HPoly &poly,
										std::vector<int> &edgeAlist,std::vector<int> &edgeBlist,std::vector<int> &edgeClist,std::vector<int> &edgeDlist);
	bool				getAdjacentEdgeVertices(HPatch *adj_patch,std::vector<int> *vindicies);
	bool				findAdjacentEdgeOnPatch(HPatch *adj_patch,unsigned &index);
	void				findAdjacentEdgesOnPatch(HPatch *adj_patch,std::vector<int> &pvindicies,std::vector<int> &eindicies);
	bool				applyEdgeVerticesToMatrix(unsigned side,unsigned ftype,unsigned cellidx,unsigned pvectidx,
									HMatrix &vmat,HMatrix &nmat,HMatrix &mflags);
	float				getDivisionConstant(const float number);



	void				createPositionMatrix(unsigned pvectidx, HMatrix &pvectmat);
	void				createNormalMatrix(unsigned pvectidx,HMatrix &nvectmat);
	HVector				getNormalFromMatrix(double u,double v,unsigned nspans,double nspank,HMatrix &nvectmat);

	HPatch				*begin();
	HPatch				*end();
	HPatch				*next();
	HPatch				*previous();
	
	HPatch				&operator = ( const HPatch &other );
	friend std::ostream		&operator << (std::ostream &os,HPatch &patch);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	std::vector<int>		cpid1,cpid2;
	std::vector<int>		cpvIndicies;

	unsigned				attrIndex;
	unsigned				patchIndex;

	HPatchVectorsArray		patchVectors;
	HPatchArray				*patchArray;

	std::vector<int>		decalIndicies;
	std::vector<int>		groupIndicies;
};

#include "hpatch.hpp"

#endif