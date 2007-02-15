//////////////////////////////////////////////////////////////////////////////////////////
//
// HSPLINE SEGMENT CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HSPLINESEGMENT_H
#define __HSPLINESEGMENT_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HDOUBLEARRAY_H
#include "hdoublearray.h"
#endif
#ifndef __HSPLINE_H
#include "hspline.h"
#endif

class HSplineSegment
{
public:
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS FLAGS
	////////////////////////////////////////////////////////////////////////////////////////

	struct segflags {
		unsigned long	kHasNoLength			: 1,
						kIsFinalized			: 1,
						kHardEdge				: 1,
						k5ptHookShortSegment	: 1,					
						k5ptHookLongSegment		: 1,
						kBorderSegment			: 1,
						kHookShortSegment		: 1,					
						kHookLongSegment		: 1;
	} flags;


	HSplineSegment();
	HSplineSegment(const HSplineSegment &other);

	HSplineSegment(const HVector &sv0, const HVector &scv0, const HVector &scv1, const HVector &sv1);
	HSplineSegment(const HVector &sv0, const HVector &scv0, const HVector &scv1, const HVector &sv1, bool finaltangents);

	~HSplineSegment();

	HVector v0,v1;
	HVector cv0,cv1;
	std::vector<int> adj_patchIndicies;

	HVector			getPointAtParam(double t);
	HVector			getDerivativeAtParam(double t);
	bool			isStraight(double tol);
	void			splitSplineSegment(HSplineSegment &seg1,HSplineSegment &seg2);

	void			reverse();
	void			flipTangents();
	void			finalize();
	
	void			doSubdivide(double tol,HDoubleArray &percents);

	HSplineSegment &operator = ( const HVector &other );
	HSplineSegment &operator = ( const HSplineSegment &other );
	
private:
	void			subdivide(double tol,double start,double end,HDoubleArray &percents);

};

#include "hsplinesegement.hpp"

#endif
