#ifndef __Matrix33_H
#define __Matrix33_H

#ifndef __Vector_H
#include "vector.h"
#endif

#ifndef __Quaternion_H
//#include "quaternion.h"
#endif

#include "memory.h"

class Matrix33
{
public:
	enum MatrixType { COLUMNMAJOR,ROWMAJOR };
	
	Matrix33(MatrixType type = COLUMNMAJOR);
	Matrix33(const double mat[3][3],MatrixType type = COLUMNMAJOR);
	Matrix33(const Vector v0,const Vector v1,const Vector v2,MatrixType type = COLUMNMAJOR );
	Matrix33(const double scalar,MatrixType type = COLUMNMAJOR);
	Matrix33(const Quaternion &quat,MatrixType type = COLUMNMAJOR);
	Matrix33(const Matrix33 &other);
	~Matrix33() {};

	Matrix33		&operator = ( const Matrix33 &other );
	Matrix33		&operator += ( const Matrix33 &other );
	Matrix33		&operator -= ( const Matrix33 &other );
	Matrix33		&operator *= ( const Matrix33 &other );
	Matrix33		&operator *= ( double scalar );

	Matrix33		operator + (const Matrix33 &other) const;
	Matrix33		operator - (const Matrix33 &other) const;
	Matrix33		operator * (const Matrix33 &other) const;
	Matrix33		operator * (double scalar) const;

	bool			operator == (const double scalar) const;
	bool			operator == (const Matrix33 &other) const;
	bool			operator != (const Matrix33 &other) const;

	Vector			operator * ( const Vector &other ) const;

	const Vector	&operator[]( int row ) const { return v[row]; }
	Vector			&operator[]( int row ) { return v[row]; }

	void			Set(const double m[3][3],MatrixType type = COLUMNMAJOR);

	void			SetIdentity();
	Matrix33		GetIdentity();
	
	Matrix33		Transpose();
	void			TransposeSelf();

	Vector			GetXAxis() const;
	Vector			GetYAxis() const;
	Vector			GetZAxis() const;
 
	void			RotateX(const double angle);// degrees
	void			RotateY(const double angle);// degrees
	void			RotateZ(const double angle);// degrees

	static Matrix33	GetRotateXMatrix(const double angle,MatrixType type = COLUMNMAJOR);// degrees
	static Matrix33	GetRotateYMatrix(const double angle,MatrixType type = COLUMNMAJOR);// degrees
	static Matrix33	GetRotateZMatrix(const double angle,MatrixType type = COLUMNMAJOR);// degrees

	static Matrix33	GetRotateXMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);
	static Matrix33	GetRotateYMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);
	static Matrix33	GetRotateZMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);

	static Matrix33	GetRotateYXMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);
	static Matrix33	GetRotateZXMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);
	static Matrix33	GetRotateYXZMatrixRadians(const double anglerad,MatrixType type = COLUMNMAJOR);

	void			Scale(const Vector &other);
	static Matrix33 GetScaleMatrix(const Vector &other,MatrixType type);

	friend void		TransformVector( const Matrix33 &matrix, Vector &vect ) { vect = matrix * vect; }

	static Matrix33 GetRotateAngleAxis(float anglerad,Vector axis,MatrixType type = COLUMNMAJOR);
	static Matrix33 GetRotate2VMatrix(Vector from, Vector to,MatrixType type = COLUMNMAJOR);
	static Matrix33 VectorsToStandard(const Vector &becomezdir,const Vector &becomeydir,MatrixType type = COLUMNMAJOR);

public:
	MatrixType m_type;
	Vector v[3];
};

#include "Matrix33.hpp"

#endif