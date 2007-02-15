////////////////////////////////////////////////////////////////////////////
//
// TupMatrix
//
// classes to handle matrices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupMatrix.hpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/08/03    Time: 4:59p
 * Updated in $/Avalanche/tools/Tupper3d
 */

template<class TYPE>
TupMatrix4x4<TYPE>::TupMatrix4x4( const TupMatrix4x4<TYPE> &s )  // copy constructor
{
	_11 = s._11; _12 = s._12; _13 = s._13; _14 = s._14; 
	_21 = s._21; _22 = s._22; _23 = s._23; _24 = s._24; 
	_31 = s._31; _32 = s._32; _33 = s._33; _34 = s._34; 
	_41 = s._41; _42 = s._42; _43 = s._43; _44 = s._44; 
}

template<class TYPE>
TupMatrix4x4<TYPE>& TupMatrix4x4<TYPE>::operator=( const TupMatrix4x4<TYPE> &s )  // assignment operator
{
	_11 = s._11; _12 = s._12; _13 = s._13; _14 = s._14; 
	_21 = s._21; _22 = s._22; _23 = s._23; _24 = s._24; 
	_31 = s._31; _32 = s._32; _33 = s._33; _34 = s._34; 
	_41 = s._41; _42 = s._42; _43 = s._43; _44 = s._44; 
	return *this;
}

template<class TYPE>
TupMatrix4x4<TYPE>::TupMatrix4x4(void)
{
	_11=(TYPE)1.0; _21=(TYPE)0.0; _31=(TYPE)0.0; _41=(TYPE)0.0;
	_12=(TYPE)0.0; _22=(TYPE)1.0; _32=(TYPE)0.0; _42=(TYPE)0.0;
	_13=(TYPE)0.0; _23=(TYPE)0.0; _33=(TYPE)1.0; _43=(TYPE)0.0;
	_14=(TYPE)0.0; _24=(TYPE)0.0; _34=(TYPE)0.0; _44=(TYPE)1.0;
};

template<class TYPE>
void TupMatrix4x4<TYPE>::Identity(void)
{
	_11=(TYPE)1.0; _21=(TYPE)0.0; _31=(TYPE)0.0; _41=(TYPE)0.0;
	_12=(TYPE)0.0; _22=(TYPE)1.0; _32=(TYPE)0.0; _42=(TYPE)0.0;
	_13=(TYPE)0.0; _23=(TYPE)0.0; _33=(TYPE)1.0; _43=(TYPE)0.0;
	_14=(TYPE)0.0; _24=(TYPE)0.0; _34=(TYPE)0.0; _44=(TYPE)1.0;
};

template<class TYPE>
void TupMatrix4x4<TYPE>::ResetRotationScale(void)
{
	_11=(TYPE)1.0; _21=(TYPE)0.0; _31=(TYPE)0.0; _41=(TYPE)0.0;
	_12=(TYPE)0.0; _22=(TYPE)1.0; _32=(TYPE)0.0; _42=(TYPE)0.0;
	_13=(TYPE)0.0; _23=(TYPE)0.0; _33=(TYPE)1.0; _43=(TYPE)0.0;
	_44=(TYPE)1.0;
};

//***************************************************************************
// Compute the inverse (FALSE if no inverse, TRUE if computed)
//***************************************************************************
template<class TYPE>
bool TupMatrix4x4<TYPE>::Inverse(TupMatrix4x4 &inv)
{
	TupMatrix4x4 adj;
	TYPE det, invDet;

	// Does the inverse exist
	det = Det4x4();
	if (det == (TYPE)0.0)
		return(false);

	// Now figure it out
	Adj4x4(adj);
	invDet = ((TYPE)1.0) / det;

	inv._11 = adj._11 * invDet;
	inv._12 = adj._12 * invDet;
	inv._13 = adj._13 * invDet;
	inv._14 = adj._14 * invDet;

	inv._21 = adj._21 * invDet;
	inv._22 = adj._22 * invDet;
	inv._23 = adj._23 * invDet;
	inv._24 = adj._24 * invDet;

	inv._31 = adj._31 * invDet;
	inv._32 = adj._32 * invDet;
	inv._33 = adj._33 * invDet;
	inv._34 = adj._34 * invDet;

	inv._41 = adj._41 * invDet;
	inv._42 = adj._42 * invDet;
	inv._43 = adj._43 * invDet;
	inv._44 = adj._44 * invDet;

	return(true);
}

//***************************************************************************
// Return the det of a 2x2
//***************************************************************************
template<class TYPE>
TYPE TupMatrix4x4<TYPE>::Det2x2(TYPE _11, TYPE _12,TYPE _21, TYPE _22)
{
	return(_11 * _22 - _21 * _12);
}

//***************************************************************************
// Return the det of a 3x3
//***************************************************************************
template<class TYPE>
TYPE TupMatrix4x4<TYPE>::Det3x3(
	TYPE _11, TYPE _12, TYPE _13,
	TYPE _21, TYPE _22, TYPE _23,
	TYPE _31, TYPE _32, TYPE _33)
{
	return(_11 * Det2x2(_22, _23, _32, _33) -
			 _12 * Det2x2(_21, _23, _31, _33) +
			 _13 * Det2x2(_21, _22, _31, _32));
}

//***************************************************************************
// Return the det of a 4x4
//***************************************************************************
template<class TYPE>
TYPE TupMatrix4x4<TYPE>::Det4x4(void)
{
	return(  _11 * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) -
			 _12 * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) +
			 _13 * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) -
			 _14 * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43));
}

//***************************************************************************
// Make the adjunct matrix
//***************************************************************************
template<class TYPE>
void TupMatrix4x4<TYPE>::Adj4x4(TupMatrix4x4 &out)
{
	out._11 =  Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
	out._21 = -Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
	out._31 =  Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
	out._41 = -Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);

	out._12 = -Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
	out._22 =  Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
	out._32 = -Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
	out._42 =  Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);

	out._13 =  Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
	out._23 = -Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
	out._33 =  Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
	out._43 = -Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);

	out._14 = -Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);
	out._24 =  Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);
	out._34 = -Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);
	out._44 =  Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::Transpose(TupMatrix4x4 &result)
{
	// if we are transposing ourselves
	if (&result == this)
	{
		TupMatrix4x4 tempMatrix;
		TYPE *pt=&tempMatrix._11;
		TYPE *po=&_11;

		for (int i=0;i<4;i++)
			for (int j=0;j<4;j++)
				pt[(j<<2)+i] = po[(i<<2)+j];
		*this = tempMatrix;
	}
	else
	{
		TYPE *pt=&result._11;
		TYPE *po=&_11;

		for (int i=0;i<4;i++)
			for (int j=0;j<4;j++)
				pt[(j<<2)+i] = po[(i<<2)+j];
	}
}

template<class TYPE>
void TupMatrix4x4<TYPE>::SafeMultiply(TupMatrix4x4 &by,TupMatrix4x4 &dest)
{
	TupMatrix4x4 temp;

	temp._11=_11*by._11 + _12*by._21 + _13*by._31;
	temp._12=_11*by._12 + _12*by._22 + _13*by._32;
	temp._13=_11*by._13 + _12*by._23 + _13*by._33;
	temp._14=_11*by._14 + _12*by._24 + _13*by._34 + _14;

	temp._21=_21*by._11 + _22*by._21 + _23*by._31;
	temp._22=_21*by._12 + _22*by._22 + _23*by._32;
	temp._23=_21*by._13 + _22*by._23 + _23*by._33;
	temp._24=_21*by._14 + _22*by._24 + _23*by._34 + _24;

	temp._31=_31*by._11 + _32*by._21 + _33*by._31;
	temp._32=_31*by._12 + _32*by._22 + _33*by._32;
	temp._33=_31*by._13 + _32*by._23 + _33*by._33;
	temp._34=_31*by._14 + _32*by._24 + _33*by._34 + _34;

	temp._41 = temp._42 = temp._43 = 0.0;
	temp._44=1.0;

	memcpy(&dest, &temp, sizeof(TupMatrix4x4));
}

template<class TYPE>
void TupMatrix4x4<TYPE>::Multiply(TupMatrix4x4 &by,TupMatrix4x4 &dest)
{
	if(&dest == &by || &dest == this)
	{
		SafeMultiply(by,dest);
		return;
	}

	dest._11=_11*by._11 + _12*by._21 + _13*by._31;
	dest._12=_11*by._12 + _12*by._22 + _13*by._32;
	dest._13=_11*by._13 + _12*by._23 + _13*by._33;
	dest._14=_11*by._14 + _12*by._24 + _13*by._34 + _14;

	dest._21=_21*by._11 + _22*by._21 + _23*by._31;
	dest._22=_21*by._12 + _22*by._22 + _23*by._32;
	dest._23=_21*by._13 + _22*by._23 + _23*by._33;
	dest._24=_21*by._14 + _22*by._24 + _23*by._34 + _24;

	dest._31=_31*by._11 + _32*by._21 + _33*by._31;
	dest._32=_31*by._12 + _32*by._22 + _33*by._32;
	dest._33=_31*by._13 + _32*by._23 + _33*by._33;
	dest._34=_31*by._14 + _32*by._24 + _33*by._34 + _34;

	dest._41 = dest._42 = dest._43 = 0.0;
	dest._44=1.0;

}

template<class TYPE>
void TupMatrix4x4<TYPE>::TransformVertices(TupArray<TupVertex3<TYPE> > &vertexArray)
{
	TupVertex3<TYPE> tv,*v;
	int total = vertexArray.GetSize();
	for(int i=0;i<total;i++)
	{
		v = &vertexArray[i];
		tv.x = v->x*_11 + v->y*_12 + v->z*_13 + _14;
		tv.y = v->x*_21 + v->y*_22 + v->z*_23 + _24;
		v->z = v->x*_31 + v->y*_32 + v->z*_33 + _34;
		v->x = tv.x;
		v->y = tv.y;
	}
}

template<class TYPE>
void TupMatrix4x4<TYPE>::TransformVertex(TupVertex3<TYPE> &vertex)
{
	TupVertex3<TYPE> tv,*v;
	v = &vertex;
	tv.x = v->x*_11 + v->y*_12 + v->z*_13 + _14;
	tv.y = v->x*_21 + v->y*_22 + v->z*_23 + _24;
	v->z = v->x*_31 + v->y*_32 + v->z*_33 + _34;
	v->x = tv.x;
	v->y = tv.y;
}

template<class TYPE>
void TupMatrix4x4<TYPE>::TransformNormals(TupArray<TupVertex3<TYPE> > &normalsArray)
{
	TupVertex3<TYPE> tv,*v;
	int total = normalsArray.GetSize();
	for(int i=0;i<total;i++)
	{
		v = &normalsArray[i];
		tv.x = v->x*_11 + v->y*_12 + v->z*_13;
		tv.y = v->x*_21 + v->y*_22 + v->z*_23;
		v->z = v->x*_31 + v->y*_32 + v->z*_33;
		v->x = tv.x;
		v->y = tv.y;
		v->Normalize();
	}
}

template<class TYPE>
void TupMatrix4x4<TYPE>::TransformNormal(TupVertex3<TYPE> &vertex)
{
	TupVertex3<TYPE> tv,*v;
	v = &vertex;
	tv.x = v->x*_11 + v->y*_12 + v->z*_13;
	tv.y = v->x*_21 + v->y*_22 + v->z*_23;
	v->z = v->x*_31 + v->y*_32 + v->z*_33;
	v->x = tv.x;
	v->y = tv.y;
	v->Normalize();
}

template<class TYPE>
void TupMatrix4x4<TYPE>::Rotate(TYPE angle,TupVertex3<TYPE> axis,TupMatrix4x4<TYPE> &destMatrix)
{
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	TupMatrix4x4<TYPE> byMatrix;
	byMatrix._11 = axis.x*axis.x*(1.0f-cosine)+cosine;
	byMatrix._12 = axis.x*axis.y*(1.0f-cosine)-(axis.z*sine);
	byMatrix._13 = axis.x*axis.z*(1.0f-cosine)+(axis.y*sine);

	byMatrix._21 = axis.y*axis.x*(1.0f-cosine)+(axis.z*sine);
	byMatrix._22 = axis.y*axis.y*(1.0f-cosine)+cosine;
	byMatrix._23 = axis.y*axis.z*(1.0f-cosine)-(axis.x*sine);

	byMatrix._31 = axis.x*axis.z*(1.0f-cosine)-(axis.y*sine);
	byMatrix._32 = axis.y*axis.z*(1.0f-cosine)+(axis.x*sine);
	byMatrix._33 = axis.z*axis.z*(1.0f-cosine)+cosine;

	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::RotateTo(TupVertex3<TYPE> sourceVector,TupVertex3<TYPE> destVector, TupMatrix4x4 &destMatrix)
{
	// get the cosing of between the vectors
	sourceVector.Normalize();
	destVector.Normalize();
	TYPE dotProduct = TupVertex3<TYPE>::DotProduct(sourceVector,destVector);
	if (dotProduct > (TYPE)1.0)
		dotProduct = (TYPE)1.0;
	if (dotProduct < (TYPE)-1.0)
		dotProduct = (TYPE)-1.0;

	TYPE angle = (TYPE)(acos(dotProduct)*RADTODEG);
	TupVertex3<TYPE> axis;
	sourceVector.CrossProduct(destVector,axis);
	if (axis.LengthSq()<=(TYPE)0.0001)
		axis.Set(-sourceVector.y,sourceVector.x,sourceVector.z);
	axis.Normalize();
	Rotate(angle,axis,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::Translate(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix)
{
	TupMatrix4x4 byMatrix;
	byMatrix._14 = vector.x;
	byMatrix._24 = vector.y;
	byMatrix._34 = vector.z;
	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::Scale(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix)
{
	TupMatrix4x4 byMatrix;
	byMatrix._11 = vector.x;
	byMatrix._22 = vector.y;
	byMatrix._33 = vector.z;
	Multiply(byMatrix,destMatrix);
}


// set shear (x coordinate changes as y coordinate changes)
template<class TYPE>
void TupMatrix4x4<TYPE>::Shear(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix)
{
	TupMatrix4x4 byMatrix;
	byMatrix._12 = vector.x;
	byMatrix._13 = vector.y;
	byMatrix._23 = vector.z;
	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::SetXRotation(TYPE angle)
{
	Identity();
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	_22 = cosine;
	_23 = -sine;
	_32 = sine;
	_33 = cosine;
}

template<class TYPE>
void TupMatrix4x4<TYPE>::SetYRotation(TYPE angle)
{
	Identity();
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	_11 = cosine;
	_13 = sine;
	_31 = -sine;
	_33 = cosine;
}

template<class TYPE>
void TupMatrix4x4<TYPE>::SetZRotation(TYPE angle)
{
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	_11 = cosine;
	_12 = -sine;
	_21 = sine;
	_22 = cosine;
}

template<class TYPE>
void TupMatrix4x4<TYPE>::RotateX(TYPE angle,TupMatrix4x4 &destMatrix)
{
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	TupMatrix4x4 byMatrix;

	byMatrix._22 = cosine;
	byMatrix._23 = -sine;
	byMatrix._32 = sine;
	byMatrix._33 = cosine;

	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::RotateY(TYPE angle,TupMatrix4x4 &destMatrix)
{
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	TupMatrix4x4 byMatrix;

	byMatrix._11 = cosine;
	byMatrix._13 = sine;
	byMatrix._31 = -sine;
	byMatrix._33 = cosine;

	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::RotateZ(TYPE angle,TupMatrix4x4 &destMatrix)
{
	TYPE sine = (TYPE)sin(angle*DEGTORAD);
	TYPE cosine = (TYPE)cos(angle*DEGTORAD);
	TupMatrix4x4 byMatrix;

	byMatrix._11 = cosine;
	byMatrix._12 = -sine;
	byMatrix._21 = sine;
	byMatrix._22 = cosine;

	Multiply(byMatrix,destMatrix);
}

template<class TYPE>
void TupMatrix4x4<TYPE>::SetAxes(TupVertex3<TYPE> xAxis,TupVertex3<TYPE> yAxis,TupVertex3<TYPE> zAxis)
{
	_11 = xAxis.x;	_12 = xAxis.y;	_13 = xAxis.z; _14 = (TYPE)0.0;
	_21 = yAxis.x;	_22 = yAxis.y;	_23 = yAxis.z; _24 = (TYPE)0.0;
	_31 = zAxis.x;	_32 = zAxis.y;	_33 = zAxis.z; _34 = (TYPE)0.0;
	_41 = _42 = _43 = (TYPE)0.0;
	_44 = (TYPE)1.0;
}

template<class TYPE>
bool TupMatrix4x4<TYPE>::IsIdentity(void) const
{
	TupMatrix4x4 identity;
	return Compare(identity);
}

template<class TYPE>
bool TupMatrix4x4<TYPE>::Compare( const TupMatrix4x4 &s,TYPE tolerance) const
{
	const TYPE *p1 = (const TYPE *)this;
	const TYPE *p2 = (const TYPE *)&s;
	for (int index=0;index<16;index++)
	{
		TYPE diff = p1[index] - p2[index];
		if (diff>tolerance)
			return false;
		if (diff<-tolerance)
			return false;
	}
	return true;
}

