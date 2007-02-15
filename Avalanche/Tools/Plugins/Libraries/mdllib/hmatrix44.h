#ifndef __HMATRIX44_H
#define __HMATRIX44_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif

class HMatrix44
{
public:
	enum mattype {COLUMN_MAJOR,ROW_MAJOR} type;

	HMatrix44();
	HMatrix44(mattype t);
	HMatrix44(const double mat[4][4]);
	HMatrix44(const double mat[4][4],mattype t);
	HMatrix44(const HMatrix44 &other);
	~HMatrix44();

	HMatrix44			&operator = ( const HMatrix44 &other );
	HMatrix44			&operator += ( const HMatrix44 &other );
	HMatrix44			&operator -= ( const HMatrix44 &other );
	HMatrix44			&operator *= ( const HMatrix44 &other );
	HMatrix44			&operator *= ( double scale );
	friend HMatrix44	operator * ( const float s, const HMatrix44 &a );

	HMatrix44			operator + (const HMatrix44 &other) const;
	HMatrix44			operator - (const HMatrix44 &other) const;
	HMatrix44			operator * (const HMatrix44 &other) const;
	HMatrix44			operator * (double d) const;

	bool				operator == (double other) const;
	bool				operator == (const HMatrix44 &other) const;
	bool				operator != (const HMatrix44 &other) const;

	double&				operator()(unsigned row, unsigned col );
	double				operator()(unsigned row, unsigned col ) const;

	void				transpose();

	void				rotateX(double angle);
	void				rotateY(double angle);
	void				rotateZ(double angle);
	void				rotate(const HVector &r);
	void				translate(const HVector &t);
	void				scale(const HVector &s);

	void				setIdentity();
	void				setFloatMatrix(float m[4][4]);

private:
	double matrix[4][4];
};

#include "hmatrix44.hpp"

#endif