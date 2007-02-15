

// TJC -  this is a float in a vector4... :)

// TJC - there are a TON of these, so we make it easyer with a couple
// of macros...
#define OPERATOR(rettype, rhs)						\
inline rettype operator+=(rhs v);					\
inline rettype operator-=(rhs v);					\
inline rettype operator*=(rhs v);					\
inline rettype operator/=(rhs v);					\
inline rettype operator=(rhs v);

#define OPERATORS(baseret)								\
OPERATOR(baseret, Vector4X);							\
OPERATOR(baseret, Vector4Y);							\
OPERATOR(baseret, Vector4Z);							\
OPERATOR(baseret, Vector4W);							\
OPERATOR(baseret, float);								\
inline Vector4X operator-(void);

class Vector4X
{
public:
	u128 _vec;

	inline Vector4X(u128 v);
	inline Vector4X(float val);
	inline Vector4X(Vector4Y val);
	inline Vector4X(Vector4Z val);
	inline Vector4X(Vector4W val);

	inline operator float();

	OPERATORS(Vector4X);
};

class Vector4Y
{
public:
	u128 _vec;

	inline Vector4Y(u128 v);
	inline Vector4Y(float val);
	inline Vector4Y(Vector4X val);
	inline Vector4Y(Vector4Z val);
	inline Vector4Y(Vector4W val);

	inline operator float();

	OPERATORS(Vector4Y);
};

class Vector4Z
{
public:
	u128 _vec;

	inline Vector4Z(u128 v);
	inline Vector4Z(float val);

	inline Vector4Z(Vector4X val);
	inline Vector4Z(Vector4Y val);
	inline Vector4Z(Vector4W val);

	inline operator float();

	OPERATORS(Vector4Z);
};

class Vector4W
{
public:
	u128 _vec;

	inline Vector4W(u128 v);
	inline Vector4W(float val);

	inline Vector4W(Vector4X val);
	inline Vector4W(Vector4Y val);
	inline Vector4W(Vector4Z val);

	inline operator float();

	OPERATORS(Vector4W);
};

#undef CONSTRUCTORS
#undef OPERATORS
#undef OPERATOR

// TJC -  there are a TON of these!!!  Lets use macros to make it
// easyer to define them...
#define OPERATORS(rettype, subrettype, lhstype, rhstype)	\
inline rettype operator+(lhstype lhs, rhstype rhs);		\
inline subrettype operator-(lhstype lhs, rhstype rhs);	\
inline rettype operator*(lhstype lhs, rhstype rhs); 

//inline Vector4X operator/(lhstype lhs, rhstype rhs);


// TJC -  all those involving Vector4X
OPERATORS(Vector4X, Vector4X, Vector4X, float);
OPERATORS(Vector4X, Vector4X, Vector4X, Vector4X);
OPERATORS(Vector4X, Vector4X, Vector4X, Vector4Y);
OPERATORS(Vector4X, Vector4X, Vector4X, Vector4Z);
OPERATORS(Vector4X, Vector4X, Vector4X, Vector4W);
OPERATORS(Vector4X, Vector4X, float, Vector4X);
OPERATORS(Vector4X, Vector4Y, Vector4Y, Vector4X);
OPERATORS(Vector4X, Vector4Z, Vector4Z, Vector4X);
OPERATORS(Vector4X, Vector4W, Vector4W, Vector4X);

// TJC - those involving Vecto4Y that havent been defined yet..
OPERATORS(Vector4X, Vector4X, Vector4Y, float);
OPERATORS(Vector4Y, Vector4Y, Vector4Y, Vector4Y);
OPERATORS(Vector4Y, Vector4Y, Vector4Y, Vector4Z);
OPERATORS(Vector4Y, Vector4Y, Vector4Y, Vector4W);
OPERATORS(Vector4X, Vector4X, float, Vector4Y);
OPERATORS(Vector4Y, Vector4Z, Vector4Z, Vector4Y);
OPERATORS(Vector4Y, Vector4W, Vector4W, Vector4Y);

// TJC - Those involving vector4Z that havent been defined yet...
OPERATORS(Vector4Z, Vector4Z, Vector4Z, Vector4Z);
OPERATORS(Vector4X, Vector4X, Vector4Z, float);
OPERATORS(Vector4Z, Vector4Z, Vector4Z, Vector4W);
OPERATORS(Vector4X, Vector4X, float, Vector4Z);
OPERATORS(Vector4Z, Vector4W, Vector4W, Vector4Z);

// TJC - and finally those for Vector4W
OPERATORS(Vector4W, Vector4W, Vector4W, Vector4W);
OPERATORS(Vector4X, Vector4X, Vector4W, float);
OPERATORS(Vector4X, Vector4X, float, Vector4W);

#undef OPERATORS

#include "Vector4SNPS2X.hpp"
