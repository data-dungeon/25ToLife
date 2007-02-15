/////////////////////////////////////////////////////////////////////////////////////////
// Matrix33 INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline Matrix33::Matrix33( MatrixType type )
{
	m_type = type;
	memset(v,0,sizeof(Vector)*3);
}

inline Matrix33::Matrix33(const double m[3][3],MatrixType type)
{
	m_type = type;
	v[0] = Vector(m[0][0],m[0][1],m[0][2]);
	v[1] = Vector(m[1][0],m[1][1],m[1][2]);
	v[2] = Vector(m[2][0],m[2][1],m[2][2]);
}

inline Matrix33::Matrix33(const Vector v0,const Vector v1,const Vector v2,MatrixType type)
{
	m_type = type;
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
}

inline Matrix33::Matrix33(const Matrix33 &other)
{
	m_type = other.m_type;
	memcpy(v,other.v,sizeof(Vector)*3);
}

inline Matrix33::Matrix33(const double scalar,MatrixType type){
	m_type = type;
	v[0] = Vector(scalar,scalar,scalar);
	v[1] = Vector(scalar,scalar,scalar);
	v[2] = Vector(scalar,scalar,scalar);
}

/*
inline Matrix33::Matrix33(const Quaternion &quat,MatrixType type)
{
	m_type = type;

	SetIdentity();
	
	float X2,  Y2,  Z2;
	float XX2, YY2, ZZ2;
	float XY2, XZ2, XW2;
	float YZ2, YW2, ZW2;

	X2  = 2.0f * (float)quat.v.x;
	XX2 = X2   * (float)quat.v.x;
	XY2 = X2   * (float)quat.v.y;
	XZ2 = X2   * (float)quat.v.z;
	XW2 = X2   * (float)quat.w;

	Y2  = 2.0f * (float)quat.v.y;
	YY2 = Y2   * (float)quat.v.y;
	YZ2 = Y2   * (float)quat.v.z;
	YW2 = Y2   * (float)quat.w;
	
	Z2  = 2.0f * (float)quat.v.z;
	ZZ2 = Z2   * (float)quat.v.z;
	ZW2 = Z2   * (float)quat.w;
	
	(*this)[0][0] = 1.0f - YY2 - ZZ2;
	(*this)[1][0] = XY2  - ZW2;
	(*this)[2][0] = XZ2  + YW2;

	(*this)[0][1] = XY2  + ZW2;
	(*this)[1][1] = 1.0f - XX2 - ZZ2;
	(*this)[2][1] = YZ2  - XW2;

	(*this)[0][2] = XZ2  - YW2;
	(*this)[1][2] = YZ2  + XW2;
	(*this)[2][2] = 1.0f - XX2 - YY2;
}
*/

inline Matrix33 &Matrix33::operator = (const Matrix33 &other) {
	m_type = other.m_type;
	memcpy(v,other.v,sizeof(Vector)*3);
	return *this;
}

inline Matrix33 &Matrix33::operator += (const Matrix33 &other) {
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

inline Matrix33 &Matrix33::operator -= ( const Matrix33 &other ) {
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

inline Matrix33 &Matrix33::operator *= ( const Matrix33 &other ) {
	Matrix33 temp(this->m_type);
  // For speed reasons, use the unroll-loops option of your compiler
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			for(int k = 0; k < 3; k++)
				temp[i][j] += (*this)[i][k] * other[k][j];			
	*this = temp;
	return *this;
}

inline Matrix33 &Matrix33::operator *= ( double scalar ) {
	v[0] *= scalar;
	v[1] *= scalar;
	v[2] *= scalar;
	return *this; 
}

inline Matrix33 Matrix33::operator + ( const Matrix33 &other ) const {
	return Matrix33(*this) += other;
}

inline Matrix33 Matrix33::operator - ( const Matrix33 &other ) const {
	return Matrix33(*this) -= other;
}

inline Matrix33 Matrix33::operator * ( const Matrix33 &other ) const {
	return Matrix33(*this) *= other;
}

inline Matrix33 Matrix33::operator * ( double scalar ) const {
	return Matrix33(*this) *= scalar;
}

inline bool Matrix33::operator == ( const Matrix33 &other ) const {
	if (memcmp(v,other.v,sizeof(Vector)*3))
		return true;
	return false;
}

inline bool Matrix33::operator == ( const double scalar ) const {
	double temp[3][3] = {{scalar,scalar,scalar},
		{scalar,scalar,scalar},
		{scalar,scalar,scalar}};
	if (memcmp(v,temp,sizeof(Vector)*3))
		return true;
	return false;;
}

inline bool Matrix33::operator != ( const Matrix33 &other ) const {
	return !(*this == other);
}

inline void Matrix33::Set(const double m[3][3],MatrixType type)
{
	m_type = type;
	v[0] = Vector(m[0][0],m[0][1],m[0][2]);
	v[1] = Vector(m[1][0],m[1][1],m[1][2]);
	v[2] = Vector(m[2][0],m[2][1],m[2][2]);
}

inline void Matrix33::SetIdentity()
{
	double identity[3][3] = {{ 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}};
	(*this) = Matrix33(identity,m_type);
}

inline Matrix33 Matrix33::GetIdentity()
{
	double identity[3][3] = {{ 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}};
	return Matrix33(identity,m_type);
}

inline Vector Matrix33::operator * ( const Vector &other ) const
{
   return Vector( v[0]|other, v[1]|other, v[2]|other );
}

inline Matrix33 Matrix33::Transpose()
{
	return Matrix33( Vector( v[0][0], v[1][0], v[2][0] ),
						Vector( v[0][1], v[1][1], v[2][1] ),
						Vector( v[0][2], v[1][2], v[2][2] ),m_type);
}

inline void Matrix33::TransposeSelf()
{
   *this = Transpose();
}

inline Vector Matrix33::GetXAxis() const
{
	if (m_type == ROWMAJOR)
		return Vector(v[0][0],v[1][0],v[2][0]);
	return Vector(v[0][0],v[0][1],v[0][2]);
}

inline Vector Matrix33::GetYAxis() const
{
	if (m_type == ROWMAJOR)
		return Vector(v[0][1],v[1][1],v[2][1]);
	return Vector(v[1][0],v[1][1],v[1][2]);
}

inline Vector Matrix33::GetZAxis() const
{
	if (m_type == ROWMAJOR)
		return Vector( v[0][2], v[1][2], v[2][2] );
	return Vector(v[2][0],v[2][1],v[2][2]);
}

inline void Matrix33::RotateX(const double angle)
{
	(*this) *= GetRotateXMatrix(angle,this->m_type); 
}

inline void Matrix33::RotateY(const double angle)
{
	(*this) *= GetRotateYMatrix(angle,this->m_type);
}

inline void Matrix33::RotateZ(const double angle)
{
	(*this) *= GetRotateZMatrix(angle,this->m_type);
}

inline Matrix33 Matrix33::GetRotateXMatrix(const double angle,MatrixType type)
{
	double radians = DEGTORAD * angle;
	double sine = sin(radians),cosine = cos(radians);
	double xrotmatrow[3][3] = {{1, 0, 0}, {0, cosine, -sine}, {0, sine, cosine}};
	double xrotmatcol[3][3] = {{1, 0, 0}, {0, cosine, sine}, {0, -sine, cosine}};
	
	if (type == Matrix33::ROWMAJOR)
		return Matrix33(xrotmatrow,type);
	else
		return Matrix33(xrotmatcol,type);
}

inline Matrix33 Matrix33::GetRotateYMatrix(const double angle,MatrixType type)
{
	double radians = DEGTORAD * angle;
	double sine = sin(radians),cosine = cos(radians);
	double yrotmatrow[3][3] = {{ cosine, 0, sine}, {0, 1, 0}, {-sine ,0, cosine}};
	double yrotmatcol[3][3] = {{ cosine, 0, -sine}, {0, 1, 0}, {sine ,0, cosine}};

	if (type == Matrix33::ROWMAJOR)
		return Matrix33(yrotmatrow,type);
	else
		return Matrix33(yrotmatcol,type);
}

inline Matrix33 Matrix33::GetRotateZMatrix(const double angle,MatrixType type)
{
	double radians = DEGTORAD * angle;
	double sine = sin(radians),cosine = cos(radians);
	double zrotmatrow[3][3] = {{ cosine, -sine, 0}, {sine, cosine, 0}, {0, 0, 1}};
	double zrotmatcol[3][3] = {{ cosine, sine, 0}, {-sine, cosine, 0}, {0, 0, 1}};

	if (type == Matrix33::ROWMAJOR)
		return Matrix33(zrotmatrow,type);
	else
		return Matrix33(zrotmatcol,type);
}

inline Matrix33 Matrix33::GetRotateXMatrixRadians(double anglerad,MatrixType type)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double xrotmatrow[3][3] = {{1, 0, 0}, {0, cosine, -sine}, {0, sine, cosine}};
	double xrotmatcol[3][3] = {{1, 0, 0}, {0, cosine, sine}, {0, -sine, cosine}};
	
	if (type == Matrix33::ROWMAJOR)
		return Matrix33(xrotmatrow,type);
	else
		return Matrix33(xrotmatcol,type);
}

inline Matrix33 Matrix33::GetRotateYMatrixRadians(const double anglerad,MatrixType type)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double yrotmatrow[3][3] = {{ cosine, 0, sine}, {0, 1, 0}, {-sine ,0, cosine}};
	double yrotmatcol[3][3] = {{ cosine, 0, -sine}, {0, 1, 0}, {sine ,0, cosine}};

	if (type == Matrix33::ROWMAJOR)
		return Matrix33(yrotmatrow,type);
	else
		return Matrix33(yrotmatcol,type);
}

inline Matrix33 Matrix33::GetRotateZMatrixRadians(const double anglerad,MatrixType type)
{
	double sine = sin(anglerad),cosine = cos(anglerad);
	double zrotmatrow[3][3] = {{ cosine, -sine, 0}, {sine, cosine, 0}, {0, 0, 1}};
	double zrotmatcol[3][3] = {{ cosine, sine, 0}, {-sine, cosine, 0}, {0, 0, 1}};

	if (type == Matrix33::ROWMAJOR)
		return Matrix33(zrotmatrow,type);
	else
		return Matrix33(zrotmatcol,type);
}

inline void Matrix33::Scale(const Vector &other)
{
	double mat[3][3] = {{ other.x, 0, 0}, { 0, other.y, 0}, { 0, 0, other.z}};
	(*this) *= mat;
}

inline Matrix33 Matrix33::GetScaleMatrix(const Vector &other,MatrixType type)
{
	double mat[3][3] = {{ other.x, 0, 0}, { 0, other.y, 0}, { 0, 0, other.z}};
	return Matrix33(mat);
}

inline Vector &Vector::operator *= (const Matrix33 &other)
{
	Vector vec(this->x,this->y,this->z);
	this->x = other.v[0]|vec;
	this->y = other.v[1]|vec;
	this->z = other.v[2]|vec;
	return *this; 
}

inline Matrix33 Matrix33::GetRotateAngleAxis(float anglerad,Vector axis,MatrixType type)
{
	float sine = (float)sin(anglerad*DEGTORAD);
	float cosine = (float)cos(anglerad*DEGTORAD);

	Matrix33 matrix(type);

	matrix.SetIdentity();

	matrix[0][0] = axis.x*axis.x*(1.0f-cosine)+cosine;
	matrix[1][0] = axis.x*axis.y*(1.0f-cosine)-(axis.z*sine);
	matrix[2][0] = axis.x*axis.z*(1.0f-cosine)+(axis.y*sine);

	matrix[0][1] = axis.y*axis.x*(1.0f-cosine)+(axis.z*sine);
	matrix[1][1] = axis.y*axis.y*(1.0f-cosine)+cosine;
	matrix[2][1] = axis.y*axis.z*(1.0f-cosine)-(axis.x*sine);

	matrix[0][2] = axis.x*axis.z*(1.0f-cosine)-(axis.y*sine);
	matrix[1][2] = axis.y*axis.z*(1.0f-cosine)+(axis.x*sine);
	matrix[2][2] = axis.z*axis.z*(1.0f-cosine)+cosine;

	if (type == ROWMAJOR)
		matrix.TransposeSelf();
	return matrix;
}

inline Matrix33 Matrix33::GetRotate2VMatrix(Vector from, Vector to,MatrixType type)
{
	// get the cosing of between the vectors
	from.Normalize();
	to.Normalize();
	double dotProduct = from.Dot(to);
	if (dotProduct > 1.0)
		dotProduct = 1.0;
	if (dotProduct < -1.0)
		dotProduct = -1.0;

	float angle = (float)(acos(dotProduct)*RADTODEG);
	Vector axis = from.Cross(to);
	if (axis.LengthSq()<=0.0001f)
		axis.Set(-from.y,from.x,from.z);
	axis.Normalize();
	return GetRotateAngleAxis(angle,axis,type);
}

inline Matrix33 Matrix33::VectorsToStandard(const Vector &becomezdir,const Vector &becomeydir,MatrixType type)
{
	Matrix33 vectostandard = GetRotate2VMatrix( becomezdir, Vector( 0.0f, 0.0f, 1.0f ),type);
	Vector rollvec = vectostandard * becomeydir;
	float roll = (float)atan2( -rollvec.x, rollvec.y );
	if (roll)
		vectostandard = Matrix33::GetRotateZMatrixRadians(-roll,type) * vectostandard;
	if (type == ROWMAJOR)
		vectostandard.TransposeSelf();
	return vectostandard;
}
