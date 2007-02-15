/*
**
**  File:   XZEntity.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/XZEntity.hpp $
**  $Revision: 6 $
**      $Date: 7/21/03 12:09p $
**    $Author: Nmartin $
**
*/

#ifdef XZ_ENTITY_HPP
#undef XZ_ENTITY_HPP

/**********************************************************************/
/**********************************************************************/
inline const CEntityType &CXZEntity::ClassType(void)
{
	return(s_xzEntityClass);
}

/**********************************************************************/
/**********************************************************************/
inline CLineCollisionObject &CXZEntity::Line(void)
{
	return(d_line);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::Set(float i_x, float i_z)
{
	d_line.Origin().x(i_x);
	d_line.Origin().z(i_z);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::SetDetectionAbility(uint i_detectionAbility)
{
	d_line.SetDetectionAbility(i_detectionAbility);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindHeight(void)
{
	d_line.Origin().y(s_ceiling);
	d_line.Basis().Set(0.0f, -1.0f, 0.0f);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindHeight(float i_x, float i_z)
{
	FindHeight();
	Set(i_x, i_z);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindDepth(void)
{
	d_line.Origin().y(s_floor);
	d_line.Basis().Set(0.0f, 1.0f, 0.0f);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindDepth(float i_x, float i_z)
{
	FindDepth();
	Set(i_x, i_z);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindFloor(float i_y)
{
	d_line.Origin().y(i_y);
	d_line.Basis().Set(0.0f, -1.0f, 0.0f);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindFloor(float i_x, float i_y, float i_z)
{
	FindFloor(i_y);
	Set(i_x, i_z);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindFloor(Vector3CRef i_loc)
{
	FindFloor(i_loc.y());
	Set(i_loc.x(), i_loc.z());
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindCeiling(float i_y)
{
	d_line.Origin().y(i_y);
	d_line.Basis().Set(0.0f, 1.0f, 0.0f);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindCeiling(float i_x, float i_y, float i_z)
{
	FindCeiling(i_y);
	Set(i_x, i_z);
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::FindCeiling(Vector3CRef i_loc)
{
	FindCeiling(i_loc.y());
	Set(i_loc.x(), i_loc.z());
}

/**********************************************************************/
/**********************************************************************/
inline CXZEntity::CXZEntity() :
	CQueryEntity(s_xzEntityClass)
{
 	d_collisionConfiguration = SIMPLE;
	d_collisionObject = &d_line;
	ManageCollisionComplexity(&d_line);

	d_line.SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	d_line.SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX,
			0.0f, s_height);
	d_line.SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	FindHeight(0.0f, 0.0f);
}

/**********************************************************************/
/**********************************************************************/
inline CXZEntity::CXZEntity(float i_x, float i_z) :
	CQueryEntity(s_xzEntityClass)
{
	d_collisionConfiguration = SIMPLE;
	d_collisionObject = &d_line;
	ManageCollisionComplexity(&d_line);

	d_line.SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	d_line.SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX,
			0.0f, s_height);
	d_line.SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	FindHeight(i_x, i_z);
}

/**********************************************************************/
/**********************************************************************/
inline float CXZEntity::YVal(void) const
{
	return(d_intersection.y());
}

/**********************************************************************/
/**********************************************************************/
inline void CXZEntity::Intersection(Vector3 &o_intersection) const
{
	o_intersection = d_intersection;
}

/**********************************************************************/
/**********************************************************************/
inline Vector3CRef CXZEntity::Intersection(void) const
{
	return(d_intersection);
}

#endif // XZ_ENTITY_HPP
