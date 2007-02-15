//////////////////////////////////////////////////////////////////////////////////////////
//
// HSPLINE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HSPLINE_H
#define __HSPLINE_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HBIAS_H
#include "hbias.h"
#endif
#ifndef __HQUATERNION_H
#include "hquaternion.h"
#endif
#ifndef __HHOOK_H
#include "hhook.h"
#endif

class HCP;
class HSplineArray;

typedef std::vector<HCP> cpVector;
typedef cpVector::iterator cpVectorIt;

class HSpline
{
public:
	HSpline();
	HSpline(const HSpline &other);
	~HSpline();

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	void				clear(void);
	unsigned			length(void) const;
	unsigned			append(const HCP &cp);
	unsigned			add(HCP &cp);

	void				calculateTangents(void);

	int					getDirection(unsigned cpi1, unsigned cpi2);

	static double		bezier(double t, double p0, double p1, double p2, double p3);
	static double		derivative(double t, double p0, double p1, double p2, double p3);

	HVector				calcPeakedDefaultEndTangent(HVector p2, HVector p3, HVector p4, HVector *gammaplane);
	HVector				calcSmoothDefaultEndTangent(HVector p2, HVector p3, HVector p4, HVector *gammaplane);
	void				calcSmoothDefaultTangent(HVector p1, HVector p2, HVector p3, HVector &in, HVector &out, HVector *gammaplane);
	void				calcPeakedDefaultTangent(HVector p1, HVector p2, HVector p3, HVector &it, HVector &ot, HVector *gammaplane);
	void				calcHBias(HVector &tangent, HBias bias, HVector gammaplane);

	void				calcHeadTangent(HCP *cp);
	void				calcMiddleTangent(HCP *cp);
	void				calcTailTangent(HCP *cp);
	void				calcLoopingTangent(HCP *cp);

	void				readHSpline(std::ifstream &fin);

	HCP					*get(unsigned index);
	HCP					*head();
	HCP					*tail();
	HSpline				*next();
	HSpline				*previous();

	HCP					operator[](unsigned index) const;
	HCP					&operator[](unsigned index);
	HSpline				&operator = ( const HSpline &other );
	friend std::ostream		&operator << (std::ostream &os,HSpline &spline);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	HSplineArray *splineArray;
	unsigned index;
	std::vector<HCP> classList;
};

#include "hspline.hpp"

#endif