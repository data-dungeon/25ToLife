/***************************************************************************/
// implementation of some utilties for dealing with Euler angles
/***************************************************************************/

/***************************************************************************/
// Return the 3x3
/***************************************************************************/
inline void EulerXYZUtility::Get3x3(
Vector3CRef euler,
DirCos3x3 &m)
{
	m.SetXRotation(Math::deg2rad(euler.x));
	m.CatYRotation(Math::deg2rad(euler.y));
	m.CatZRotation(Math::deg2rad(euler.z));
}

/***************************************************************************/
// Return the 4x4
/***************************************************************************/
inline void EulerXYZUtility::Get4x4(
Vector3CRef euler,
Cartesian4x4 &m)
{
	m.SetXRotation(Math::deg2rad(euler.x));
	m.CatYRotation(Math::deg2rad(euler.y));
	m.CatZRotation(Math::deg2rad(euler.z));
}

/***************************************************************************/
// Get the direction vector
/***************************************************************************/
inline Vector3 EulerXYZUtility::GetDir(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn2(temp);
	return(temp);
}

/***************************************************************************/
// Get the up vector
/***************************************************************************/
inline Vector3 EulerXYZUtility::GetUp(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn1(temp);
	return(temp);
}

/***************************************************************************/
// Get the right vector
/***************************************************************************/
inline Vector3 EulerXYZUtility::GetRight(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn0(temp);
	return(temp);
}

/***************************************************************************/
// Get all the vectors
/***************************************************************************/
inline void EulerXYZUtility::GetRUD(
Vector3CRef euler,
Vector3 &right,
Vector3 &up,
Vector3 &dir)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	m.GetColumn0(right);
	m.GetColumn1(up);
	m.GetColumn2(dir);
}

/***************************************************************************/
// Return the 3x3
/***************************************************************************/
inline void EulerYXZUtility::Get3x3(
Vector3CRef euler,
DirCos3x3 &m)
{
	m.SetYRotation(Math::deg2rad(euler.y));
	m.CatXRotation(Math::deg2rad(euler.x));
	m.CatZRotation(Math::deg2rad(euler.z));
}

/***************************************************************************/
// Return the 4x4
/***************************************************************************/
inline void EulerYXZUtility::Get4x4(
Vector3CRef euler,
Cartesian4x4 &m)
{
	m.SetYRotation(Math::deg2rad(euler.y));
	m.CatXRotation(Math::deg2rad(euler.x));
	m.CatZRotation(Math::deg2rad(euler.z));
}

/***************************************************************************/
// Get the direction vector
/***************************************************************************/
inline Vector3 EulerYXZUtility::GetDir(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn2(temp);
	return(temp);
}

/***************************************************************************/
// Get the up vector
/***************************************************************************/
inline Vector3 EulerYXZUtility::GetUp(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn1(temp);
	return(temp);
}

/***************************************************************************/
// Get the right vector
/***************************************************************************/
inline Vector3 EulerYXZUtility::GetRight(
Vector3CRef euler)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	Vector3 temp;
	m.GetColumn0(temp);
	return(temp);
}

/***************************************************************************/
// Get all the vectors
/***************************************************************************/
inline void EulerYXZUtility::GetRUD(
Vector3CRef euler,
Vector3 &right,
Vector3 &up,
Vector3 &dir)
{
	DirCos3x3 m;
	Get3x3(euler, m);

	m.GetColumn0(right);
	m.GetColumn1(up);
	m.GetColumn2(dir);
}
