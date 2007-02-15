
inline Quat::Quat(const Quat &q)
{
	e[0] = q.e[0];
	e[1] = q.e[1];
	e[2] = q.e[2];
	e[3] = q.e[3];
}

inline Quat::Quat(float u0, float u1, float u2, float w)
{
	e[0] = u0;
	e[1] = u1;
	e[2] = u2;
	e[3] = w;
}

inline Quat::Quat(const ts_Quatvw &q)
{
	e[0] = q.V.x();
	e[1] = q.V.y();
	e[2] = q.V.z();
	e[3] = q.W;
}

inline Quat::Quat(const Matrix3x3 &m)
{
	Matrix4x4 m4M;
	m4M.e00 = m.e00;
	m4M.e01 = m.e01;
	m4M.e02 = m.e02;
	m4M.e03 = 0.0f;
	m4M.e10 = m.e10;
	m4M.e11 = m.e11;
	m4M.e12 = m.e12;
	m4M.e13 = 0.0f;
	m4M.e20 = m.e20;
	m4M.e21 = m.e21;
	m4M.e22 = m.e22;
	m4M.e23 = 0.0f;
	m4M.e30 = 0.0f;
	m4M.e31 = 0.0f;
	m4M.e32 = 0.0f;
	m4M.e33 = 1.0f;
	qatFromMat((Graphics4x4 *) &m4M, (ts_Quatvw *)this);
}

inline Quat::Quat(const Matrix4x4 &m)
{
	qatFromMat((Graphics4x4 *) &m, (ts_Quatvw *)this);
}

inline Quat::Quat(float roll, float pitch, float yaw)
{
	qatFromEuler( roll, pitch, yaw, (ts_Quatvw *) this);
}

inline Quat::Quat( Vector3CRef a, Vector3CRef b, const float epsilon )
{
	qatIdentity( ( ts_Quatvw * )this );
	Set( a, b, epsilon );
}

inline void Quat::Set(float u0, float u1, float u2, float w)
{
	e[0] = u0;
	e[1] = u1;
	e[2] = u2;
	e[3] = w;
}

inline void Quat::Set(Vector3 axis, float angle)
{
	qatFromAxisAngle( &axis, angle, ( ts_Quatvw * )this );
}

inline void Quat::Set(const ts_Quatvw &q)
{
	e[0] = q.V.x();
	e[1] = q.V.y();
	e[2] = q.V.z();
	e[3] = q.W;
}

inline void Quat::Set(float roll, float pitch, float yaw)
{
	qatFromEuler( roll, pitch, yaw, (ts_Quatvw *) this);
}

inline void Quat::Set( Vector3CRef a, Vector3CRef b, const float epsilon )
{
	Vector3 u;

	u = a.Cross( b );

	// don't perform any rotation if a and b are close to parallel
	if (
		( u.x() <= epsilon ) &&
		( u.y() <= epsilon ) &&
		( u.z() <= epsilon )
		)
	{
		qatIdentity( ( ts_Quatvw * )this );
		return;
	}

	u.SafeNormalize( );

	float dot = a.Dot( b );
	float sinTheta = Math::Sqrt( ( 1 - dot ) / 2.0f );
	float cosTheta = Math::Sqrt( ( 1 + dot ) / 2.0f );

	u = u * sinTheta;
	e[ 0 ] = u.x();
	e[ 1 ] = u.y();
	e[ 2 ] = u.z();
	e[ 3 ] = cosTheta;
}

inline void Quat::Identity(void)
{
	qatIdentity( ( ts_Quatvw * )this );
}

inline void Quat::Normalize( void )
{
	qatNormalize( ( ts_Quatvw * )this );
}

inline DirCos3x3 Quat::ConvertToD3x3(void) const
{
	DirCos3x3 temp3;
	Cartesian4x4 temp4;
	qatToRotMat((ts_Quatvw *)this, &temp4);
	temp3 = temp4;
	return temp3;
}

inline Cartesian4x4 Quat::ConvertToC4x4(void) const
{
	Cartesian4x4 temp;
	qatToRotMat((ts_Quatvw *)this, &temp);
	return(temp);
}


inline Quat Quat::Slerp(Quat &qTo, float rPercent) const
{
	Quat temp;
	qatSlerpBetter((ts_Quatvw *)this, (ts_Quatvw *)&qTo, rPercent, (ts_Quatvw *)&temp);
	return(temp);
}

inline Quat Quat::Lerp(Quat &qTo, float rPercent) const
{
	Quat temp;
	qatLerpBetter((ts_Quatvw *)this, (ts_Quatvw *)&qTo, rPercent, (ts_Quatvw *)&temp);
	return(temp);
}

inline Quat Quat::operator*(const Quat &q) const
{
	Quat temp;
	qatMul((ts_Quatvw *)this, (ts_Quatvw *)&q, (ts_Quatvw *)&temp);
	return(temp);
}

inline Quat &Quat::operator=(const Quat &q)
{
	e[0] = q.e[0];
	e[1] = q.e[1];
	e[2] = q.e[2];
	e[3] = q.e[3];
	return(*this);
}

inline Quat &Quat::operator=(const ts_Quatvw &q)
{
	e[0] = q.V.x();
	e[1] = q.V.y();
	e[2] = q.V.z();
	e[3] = q.W;
	return(*this);
}

inline Quat &Quat::operator=(const Matrix3x3 &m)
{
	Matrix4x4 m4M;
	m4M.e00 = m.e00;
	m4M.e01 = m.e01;
	m4M.e02 = m.e02;
	m4M.e03 = 0.0f;
	m4M.e10 = m.e10;
	m4M.e11 = m.e11;
	m4M.e12 = m.e12;
	m4M.e13 = 0.0f;
	m4M.e20 = m.e20;
	m4M.e21 = m.e21;
	m4M.e22 = m.e22;
	m4M.e23 = 0.0f;
	m4M.e30 = 0.0f;
	m4M.e31 = 0.0f;
	m4M.e32 = 0.0f;
	m4M.e33 = 1.0f;
	qatFromMat((Graphics4x4 *) &m4M, (ts_Quatvw *)this);
	return(*this);
}

inline Quat &Quat::operator=(const Matrix4x4 &m)
{
	qatFromMat((Graphics4x4 *) &m, (ts_Quatvw *)this);
	return(*this);
}

inline Quat::operator ts_Quatvw*( void ) const
{
	return( ( ts_Quatvw * )&e[0] );
}

inline Quat::operator ts_Quatvw( void ) const
{
	return( *( ( ts_Quatvw * )&e[0] ) );
}

inline Quat::operator Vector3*(void) const
{
	return( ( Vector3 * )&e[0] );
}

inline Quat::operator Vector3(void) const
{
	return( *( ( Vector3 * )&e[0] ) );
}
