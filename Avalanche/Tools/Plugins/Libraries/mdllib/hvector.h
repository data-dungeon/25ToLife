//////////////////////////////////////////////////////////////////////////////////////////
//
//	HVECTOR CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HVECTOR_H
#define __HVECTOR_H

#ifndef __HMACROS_H
#include "hmacros.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif

class HQuaternion;

class HVector
{
public:

	HVector();
	HVector(const double X, const double Y, const double Z);
	HVector(const HVector &other);
	HVector(const double d);
	~HVector();

	HVector				&operator = (const HVector &other);
	HVector				&operator += (const HVector &other);
	HVector				&operator -= (const HVector &other);
	HVector				&operator /= (const HVector &other);
	HVector				&operator *= (const HVector &other);
	HVector				&operator ^= (const HVector &other);
	HVector				&operator *= (const HQuaternion &q);
	HVector				&operator *= (const double scale);
	HVector				&operator /= (const double scale);

	HVector				cross(const HVector &v) const;
	double				dot(const HVector &other) const;
	double				square() const;
	HVector				absolute() const;
	HVector				reciprocal() const;
	void				crossSelf(const HVector &other);

	HVector				&normalize();
	HVector				normalized() const;
	double				normalizeLength();

	double				lengthsq() const;
	double				length() const;

	HVector				&negate();
	HVector				&clamp(const double digits);

	HVector				&degreesToRadians();
	HVector				&radiansToDegrees();

	bool				isParallel(const HVector b, const double tol) const;
	bool				isSameDirection(const HVector b, const double tol) const;
	bool				isParallelAndSameDirection(const HVector b,const double tol);

	double				distanceTo(const HVector &p) const;
	bool				isWithinTolerance(const HVector &other,const double tol);

	friend HVector		cross(const HVector &a, const HVector &b);
	friend double		dot(const HVector &a, const HVector &b);
	friend HVector		operator * (const double s, const HVector &a);

	HVector				operator + (const HVector &other) const;
	HVector				operator - (const HVector &other) const;
	HVector				operator - (void) const;
	HVector				operator * (const HVector &other) const;
	HVector				operator * (const double d) const;
	HVector				operator / (const double d) const;
	HVector				operator / (const HVector &other) const;
	double				operator | (const HVector &other) const;
	HVector				operator ^ (const HVector &other) const;
	
	bool				operator == (const double other) const;
	bool				operator == (const HVector &other) const;
	bool				operator != (const HVector &other) const;
	bool				operator < (const HVector &other) const;

	friend std::ostream		&operator << (std::ostream &os,const HVector &other);
	
	double x,y,z;
	
private:
		void			_clamp( const double digits,double &value );


};

typedef std::vector<HVector> VectorVector;
typedef VectorVector::iterator VectorVectorIt;

class HVectorArray
{
public:

	HVectorArray ();
	HVectorArray(unsigned size);
	~HVectorArray ();

	void				clear();
	unsigned			length() const;
	void				resize(const unsigned size);

	unsigned			append(const HVector &pos);
	unsigned			appendWeld(const HVector &pos,const double tolerance);

	bool				valueExists(const HVector &pos,unsigned &index) const;

	HVector				*get(const unsigned index);
	HVector				*begin();
	HVector				*end();

	HVector				operator[]( const unsigned index ) const;
 	HVector&			operator[]( const unsigned index );
	HVectorArray		&operator = ( const HVectorArray &other );
	friend std::ostream		&operator << (std::ostream &os,const HVectorArray &other);

	std::vector<HVector> classList;
};

class HVectorIterator
{
public:
	HVectorIterator();
	HVectorIterator(HVectorArray &other);
	~HVectorIterator();

	void			reset();
	void			reset(HVectorArray &other);

	bool			isDone();

	void			insert(const HVector &other);
	unsigned		index() const;

	HVectorIterator	operator  ++(int);
	HVectorIterator	operator  --(int);
	HVector			*operator * ();

private:
	HVectorArray *pointTo;
	std::vector<HVector>::iterator cl_iterator;
};

#include "hvector.hpp"

#endif
