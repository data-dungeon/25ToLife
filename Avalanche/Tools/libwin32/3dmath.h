#ifndef __3DMATH_H__
#define __3DMATH_H__

#include <windows.h>
#include <math.h>

class vector2 
{
	public:
		double x;
		double y;
};

class vector3
{
	public:
		double x;
		double y;
		double z;
};

class vector4
{
	public:
		double x;
		double y;
		double z;
		double w;
};

class matrix2
{
	public:
		union
		{
			struct
			{
				double m11;
				double m12;
				double m21;
				double m22;
			};
			double m[4];
		};
		void identity( void );
		void multiply( matrix2 *m );
		vector2 transform( double x, double y );
};

class matrix3
{
	public:
		union
		{
			struct
			{
				double m11;
				double m12;
				double m13;
				double m21;
				double m22;
				double m23;
				double m31;
				double m32;
				double m33;
			};
			double m[9];
		};
		void identity( void );
		vector3 transform( double x, double y, double z );
};

class matrix4
{
	public:
		union
		{
			struct
			{
				double m11;
				double m12;
				double m13;
				double m14;
				double m21;
				double m22;
				double m23;
				double m24;
				double m31;
				double m32;
				double m33;
				double m34;
				double m41;
				double m42;
				double m43;
				double m44;
			};
			double m[16];
		};

		void identity( void );
		vector4 transform( double x, double y, double z, double w );
};

#endif // __3DMATH_H__