/////////////////////////////////////////////////////////////////////////////////////////
// HashMatrix3x3 INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline HashMatrix3x3::HashMatrix3x3(void)
{
	memset(v,0,sizeof(HashVector3)*3);
}

inline HashMatrix3x3::HashMatrix3x3(const double m[3][3])
{
	v[0] = HashVector3(m[0][0],m[0][1],m[0][2]);
	v[1] = HashVector3(m[1][0],m[1][1],m[1][2]);
	v[2] = HashVector3(m[2][0],m[2][1],m[2][2]);
}

inline HashMatrix3x3::HashMatrix3x3(const HashVector3 v0,const HashVector3 v1,const HashVector3 v2)
{
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
}

inline HashMatrix3x3::HashMatrix3x3(const HashMatrix3x3 &other)
{
	memcpy(v,other.v,sizeof(HashVector3)*3);
}

inline HashMatrix3x3::HashMatrix3x3(const double scalar)
{
	v[0] = HashVector3(scalar,scalar,scalar);
	v[1] = HashVector3(scalar,scalar,scalar);
	v[2] = HashVector3(scalar,scalar,scalar);
}

inline HashMatrix3x3 &HashMatrix3x3::operator = (const HashMatrix3x3 &other) 
{
	memcpy(v,other.v,sizeof(HashVector3)*3);
	return *this;
}

inline HashMatrix3x3 &HashMatrix3x3::operator += (const HashMatrix3x3 &other) 
{
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

inline HashMatrix3x3 &HashMatrix3x3::operator -= ( const HashMatrix3x3 &other ) 
{
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

inline HashMatrix3x3 &HashMatrix3x3::operator *= ( const HashMatrix3x3 &other ) 
{
	HashMatrix3x3 temp;
	// For speed reasons, use the unroll-loops option of your compiler
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			for(int k = 0; k < 3; k++)
				temp[i][j] += (*this)[i][k] * other[k][j];			
	*this = temp;
	return *this;
}

inline HashMatrix3x3 &HashMatrix3x3::operator *= ( double scalar ) 
{
	v[0] *= scalar;
	v[1] *= scalar;
	v[2] *= scalar;
	return *this; 
}

inline HashMatrix3x3 HashMatrix3x3::operator + ( const HashMatrix3x3 &other ) const 
{
	return HashMatrix3x3(*this) += other;
}

inline HashMatrix3x3 HashMatrix3x3::operator - ( const HashMatrix3x3 &other ) const 
{
	return HashMatrix3x3(*this) -= other;
}

inline HashMatrix3x3 HashMatrix3x3::operator * ( const HashMatrix3x3 &other ) const 
{
	return HashMatrix3x3(*this) *= other;
}

inline HashMatrix3x3 HashMatrix3x3::operator * ( double scalar ) const 
{
	return HashMatrix3x3(*this) *= scalar;
}

inline bool HashMatrix3x3::operator == ( const HashMatrix3x3 &other ) const 
{
	if (memcmp(v,other.v,sizeof(HashVector3)*3))
	{
		return true;
	}
	return false;
}

inline bool HashMatrix3x3::operator == ( const double scalar ) const 
{
	double temp[3][3] = {{scalar,scalar,scalar},{scalar,scalar,scalar},{scalar,scalar,scalar}};
	if (memcmp(v,temp,sizeof(HashVector3)*3))
		return true;
	return false;;
}

inline bool HashMatrix3x3::operator != ( const HashMatrix3x3 &other ) const 
{
	return !(*this == other);
}

inline void HashMatrix3x3::Set(const double m[3][3])
{
	v[0] = HashVector3(m[0][0],m[0][1],m[0][2]);
	v[1] = HashVector3(m[1][0],m[1][1],m[1][2]);
	v[2] = HashVector3(m[2][0],m[2][1],m[2][2]);
}

inline void HashMatrix3x3::SetIdentity()
{
	double identity[3][3] = {{ 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}};
	(*this) = HashMatrix3x3(identity);
}

inline HashMatrix3x3 HashMatrix3x3::GetIdentity()
{
	double identity[3][3] = {{ 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}};
	return HashMatrix3x3(identity);
}

inline HashVector3 HashMatrix3x3::operator * ( const HashVector3 &other ) const
{
	return HashVector3( v[0]|other, v[1]|other, v[2]|other );
}

inline HashMatrix3x3 HashMatrix3x3::Transpose()
{
	return HashMatrix3x3( HashVector3( v[0][0], v[1][0], v[2][0] ),
			 HashVector3( v[0][1], v[1][1], v[2][1] ),
			 HashVector3( v[0][2], v[1][2], v[2][2] ));
}

inline void HashMatrix3x3::TransposeSelf()
{
	*this = Transpose();
}

inline HashVector3 HashMatrix3x3::GetXAxis() const
{
	return HashVector3(v[0][0],v[1][0],v[2][0]);
}

inline HashVector3 HashMatrix3x3::GetYAxis() const
{
	return HashVector3(v[0][1],v[1][1],v[2][1]);
}

inline HashVector3 HashMatrix3x3::GetZAxis() const
{
	return HashVector3( v[0][2], v[1][2], v[2][2] );
}

inline HashMatrix3x3 HashMatrix3x3::GetRotateXMatrix(double anglerad)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double xrotmatrow[3][3] = {{1, 0, 0}, {0, cosine, -sine}, {0, sine, cosine}};

	return HashMatrix3x3(xrotmatrow);
}

inline HashMatrix3x3 HashMatrix3x3::GetRotateYMatrix(const double anglerad)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double yrotmatrow[3][3] = {{ cosine, 0, sine}, {0, 1, 0}, {-sine ,0, cosine}};

	return HashMatrix3x3(yrotmatrow);
}

inline HashMatrix3x3 HashMatrix3x3::GetRotateZMatrix(const double anglerad)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double zrotmatrow[3][3] = {{ cosine, -sine, 0}, {sine, cosine, 0}, {0, 0, 1}};

	return HashMatrix3x3(zrotmatrow);
}

inline void HashMatrix3x3::Scale(const HashVector3 &other)
{
	double mat[3][3] = {{ other.x, 0, 0}, { 0, other.y, 0}, { 0, 0, other.z}};
	(*this) *= mat;
}

inline HashMatrix3x3 HashMatrix3x3::GetScaleMatrix(const HashVector3 &other)
{
	double mat[3][3] = {{ other.x, 0, 0}, { 0, other.y, 0}, { 0, 0, other.z}};
	return HashMatrix3x3(mat);
}

inline HashVector3 &HashVector3::operator *= (const HashMatrix3x3 &other)
{
	HashVector3 vec(this->x,this->y,this->z);
	this->x = other.v[0]|vec;
	this->y = other.v[1]|vec;
	this->z = other.v[2]|vec;
	return *this; 
}

inline HashMatrix3x3 HashMatrix3x3::GetRotateAngleAxis(double anglerad,HashVector3 axis)
{
	axis.Normalize();
	double sine = (double)sin(anglerad);
	double cosine = (double)cos(anglerad);

	HashMatrix3x3 matrix;

	matrix.SetIdentity();

	matrix[0][0] = axis.x*axis.x*(1.0f-cosine)+cosine;
	matrix[0][1] = axis.x*axis.y*(1.0f-cosine)-(axis.z*sine);
	matrix[0][2] = axis.x*axis.z*(1.0f-cosine)+(axis.y*sine);

	matrix[1][0] = axis.y*axis.x*(1.0f-cosine)+(axis.z*sine);
	matrix[1][1] = axis.y*axis.y*(1.0f-cosine)+cosine;
	matrix[1][2] = axis.y*axis.z*(1.0f-cosine)-(axis.x*sine);

	matrix[2][0] = axis.x*axis.z*(1.0f-cosine)-(axis.y*sine);
	matrix[2][1] = axis.y*axis.z*(1.0f-cosine)+(axis.x*sine);
	matrix[2][2] = axis.z*axis.z*(1.0f-cosine)+cosine;

	return matrix;
}

inline HashMatrix3x3 HashMatrix3x3::GetRotate2VMatrix(HashVector3 from, HashVector3 to)
{
	from.Normalize();
	to.Normalize();
	double numerator = from | to;
	double denominator = from.Length() * to.Length();
	double radangle;
	HashVector3 axis;

	if ( fabs(numerator - denominator) < .00001) // SAME DIR
	{
		HashMatrix3x3 identity;
		identity.SetIdentity();
		return identity;
	}
	if ( fabs(numerator + denominator) < .00001 ) // OPPOSITE DIR
	{
		radangle = 3.141519f;
		if (from.Normalized().Absolute() != HashVector3(1,0,0))
			axis = from ^ HashVector3(1,0,0);
		else
			axis = from ^ HashVector3(0,1,0);
	}
	else 
	{
		double ratio = numerator / denominator;
		if (ratio >= 1.0f)
		{
			HashMatrix3x3 identity;
			identity.SetIdentity();
			return identity;
		}
		radangle = acos( ratio );
		axis = from ^ to;
	}
	return GetRotateAngleAxis( radangle, axis);
}

inline HashMatrix3x3 HashMatrix3x3::VectorsToStandard(const HashVector3 &becomezdir,const HashVector3 &becomeydir)
{
	HashMatrix3x3 vectostandard = GetRotate2VMatrix( becomezdir, HashVector3( 0.0f, 0.0f, 1.0f ));
	HashVector3 rollvec = vectostandard * becomeydir;
	double roll = (double)atan2( -rollvec.x, rollvec.y );
	if (roll)
		vectostandard = HashMatrix3x3::GetRotateZMatrix(-roll) * vectostandard;
	return vectostandard;
}
