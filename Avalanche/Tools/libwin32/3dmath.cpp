#include "pch.h"
#include "3dmath.h"

void matrix2::identity( void )
{
	m11 = 1.0; m12 = 0.0;
	m21 = 0.0; m22 = 1.0;
}

void matrix2::multiply( matrix2 *m )
{
	matrix2 temp;

	temp.m11 = m11 * m->m11 + m12 * m->m21;
	temp.m12 = m11 * m->m21 + m12 * m->m22;
	
	temp.m21 = m21 * m->m11 + m22 * m->m21;
	temp.m22 = m21 * m->m21 + m22 * m->m22;

	*this = temp;
}

vector2 matrix2::transform( double x, double y )
{
	vector2 v;

	v.x = x * m11 + y * m21;
	v.y = x * m12 + y * m22;

	return v;
}

void matrix3::identity( void )
{
	m11 = 1.0; m12 = 0.0; m13 = 0.0;
	m21 = 0.0; m22 = 1.0; m23 = 0.0;
	m31 = 0.0; m32 = 0.0; m33 = 1.0;
}

vector3 matrix3::transform( double x, double y, double z )
{
	vector3 v;

	v.x = x * m11 + y * m21 + z * m31;
	v.y = y * m12 + y * m22 + z * m32;
	v.z = z * m13 + y * m32 + z * m33;

	return v;
}

void matrix4::identity( void )
{
	m11 = 1.0; m12 = 0.0; m13 = 0.0; m14 = 0.0;
	m21 = 0.0; m22 = 1.0; m23 = 0.0; m24 = 0.0;
	m31 = 0.0; m32 = 0.0; m33 = 1.0; m34 = 0.0;
	m41 = 0.0; m42 = 0.0; m43 = 0.0; m44 = 1.0;
}

vector4 matrix4::transform( double x, double y, double z, double w )
{
	vector4 v;

	v.x = x * m11 + y * m21 + z * m31 + w * m41;
	v.y = y * m12 + y * m22 + z * m32 + w * m42;
	v.z = z * m13 + y * m32 + z * m33 + w * m43;
	v.w = w * m14 + y * m42 + z * m43 + w * m44;

	return v;
}
