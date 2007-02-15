#ifndef __HVECTOR2D_H
#define __HVECTOR2D_H

class HQuaternion;

class HVector2D
{
public:

	HVector2D();
	HVector2D(double X, double Y);
	HVector2D(const HVector2D &other);
	HVector2D(const double other);
	~HVector2D();

	HVector2D			&operator = (const HVector2D &other);
	HVector2D			&operator += (const HVector2D &other);
	HVector2D			&operator -= (const HVector2D &other);
	HVector2D			&operator /= (const HVector2D &other);
	HVector2D			&operator *= (const HVector2D &other);
	HVector2D			&operator *= (const HQuaternion &q);
	HVector2D			&operator *= (const double scale);
	HVector2D			&operator /= (const double scale);

	double				dot(const HVector2D &other) const;
	double				square() const;
	HVector2D			absolute() const;
	HVector2D			reciprocal() const;

	HVector2D			&normalize();
	HVector2D			normalized() const;
//	double				normalizeLength() const;

	double				length() const;

	HVector2D			&negate();

	double				distanceTo(HVector2D p) const;

	friend double		dot(const HVector2D &a, const HVector2D &b);
	
	friend HVector2D	operator * (const double s, const HVector2D &a);
	HVector2D			operator + (const HVector2D &other) const;
	HVector2D			operator - (const HVector2D &other) const;
	HVector2D			operator - (void) const;
	HVector2D			operator * (const HVector2D &other ) const;
	HVector2D			operator * (const double scale) const;
	HVector2D			operator / (const double scale) const;
	HVector2D			operator / (const HVector2D &other) const;
	double				operator | (const HVector2D &other) const;

	bool				operator == (const double other) const;
	bool				operator == (const HVector2D &other) const;
	bool				operator != (const HVector2D &other) const;

	friend std::ostream		&operator << (std::ostream &os,HVector2D &other);

	double x,y;
};

typedef std::vector<HVector2D> Vector2DVector;
typedef Vector2DVector::iterator Vector2DVectorIt;

class HVector2DArray
{
public:

	HVector2DArray ();

	virtual ~HVector2DArray ();

	void				clear();
	unsigned			length() const;
	unsigned			append(const HVector2D &pos);
	void				resize(const unsigned size);

	HVector2D			*get(const unsigned index);

	HVector2D			operator[](const unsigned index ) const;
 	HVector2D&			operator[](const unsigned index );
	HVector2DArray		&operator = ( const HVector2DArray &other );
	friend std::ostream		&operator << (std::ostream &os,HVector2DArray &other);

	std::vector<HVector2D> classList;
};

class HVector2DIterator
{
public:
	HVector2DIterator();
	HVector2DIterator(HVector2DArray &other);
	~HVector2DIterator();

	void			reset(void);
	void 			reset(HVector2DArray &other);

	bool			isDone(void);

	void			insert(const HVector2D pos);
	unsigned		index();

	HVector2DIterator	operator  ++(int);
	HVector2DIterator	operator  --(int);
	HVector2D			*operator * ();

private:
	HVector2DArray *pointTo;
	std::vector<HVector2D>::iterator cl_iterator;
};

#include "hvector2d.hpp"

#endif