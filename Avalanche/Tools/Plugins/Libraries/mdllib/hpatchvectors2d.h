#ifndef __HPATCHVECTORS2D_H
#define __HPATCHVECTORS2D_H

#ifndef __HVECTOR2D_H
#include "hvector2d.h"
#endif
#ifndef __HMACROS_H
#include "hmacros.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif

class HPatchVectors2D
{
public:
	HPatchVectors2D();
	HPatchVectors2D(const HPatchVectors2D &other);
	~HPatchVectors2D();

	void rotateElementsClockwise90();
	void rotateElementsClockwise(unsigned rot);
	void mirrorElementsX();
	void mirrorElementsY();

	HVector2DArray pv;

	HPatchVectors2D &operator = ( const HPatchVectors2D &other );

	friend std::ostream &operator << (std::ostream &os,HPatchVectors2D &v);
};

class HPatchVectors2DArray
{
public:
	HPatchVectors2DArray();
	~HPatchVectors2DArray();

	void					clear();
	unsigned				length() const;
	void					append(HPatchVectors2D &pVectors);

	HPatchVectors2D			operator[](unsigned index) const;
	HPatchVectors2D			&operator[](unsigned index);

	HPatchVectors2DArray	&operator = ( const HPatchVectors2DArray &other );

	std::vector<HPatchVectors2D> classList;
	friend std::ostream &operator << (std::ostream &os,HPatchVectors2DArray &varray);
};

#endif