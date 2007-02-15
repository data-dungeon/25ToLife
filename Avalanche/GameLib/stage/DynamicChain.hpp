/*
**
**  File:   DynamicChain.hpp
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   Applies dynamics to chained bones in a model.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/DynamicChain.hpp $
**  $Revision: 26 $
**      $Date: 7/31/03 2:00p $
**    $Author: Bcollard $
**
*/

#ifdef DYNAMIC_CHAIN_HPP
#undef DYNAMIC_CHAIN_HPP

inline void CDynamicChain::LoadPartnershipParams(void)
{
	LoadParams();
	if (d_dynamicPartner)
		d_dynamicPartner->LoadPartnershipParams();
}

inline void CDynamicChain::SetControlMask(int i_link, int i_mask)
{
	if ((i_link >= 0) && (i_link < d_numLinks))
		d_dynamicLink[i_link].d_controlMask = i_mask;
}

inline int CDynamicChain::GetControlMask(int i_link) const
{
	if ((i_link >= 0) && (i_link < d_numLinks))
		 return(d_dynamicLink[i_link].d_controlMask);
	 return(0);
}

inline void CDynamicChain::SetMass(int i_link, float i_mass)
{
	if ((i_link >= 0) && (i_link < d_numLinks) &&
			!Math::DivByZero(1.0f, i_mass))
		d_dynamicLink[i_link].d_baseInvMass = 1.0f / i_mass;
}

inline float CDynamicChain::GetMass(int i_link)
{
	if ((i_link >= 0) && (i_link < d_numLinks) &&
			!Math::DivByZero(1.0f, d_dynamicLink[i_link].d_baseInvMass))
		return(1.0f / d_dynamicLink[i_link].d_baseInvMass);
	return(0.0f);
}

inline void CDynamicChain::SetVelocityInWorld(int i_link, Vector3 &i_velocity)
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	d_dynamicLink[i_link].d_velocityInWorld = i_velocity;
}

inline void CDynamicChain::SetTargetDynamics(int i_link,
		float i_naturalFrequency, float i_dampingFactor)
{
	if ((i_link >= 0) && (i_link < d_numLinks))
	{
		d_dynamicLink[i_link].d_targetNaturalFrequency = i_naturalFrequency;
		d_dynamicLink[i_link].d_targetDampingFactor = i_dampingFactor;
	}
}

inline void CDynamicChain::SetDragCoefficient(int i_link,
		float i_dragCoefficient)
{
	if ((i_link >= 0) && (i_link < d_numLinks))
		d_dynamicLink[i_link].d_dragCoefficient = i_dragCoefficient;
}

inline bool CDynamicChain::GetAttachPoint(int i_link, ts_Bone* &o_attachBone,
		Vector3 &o_attachPoint) const
{
	if ((i_link >= 0) && (i_link < d_numLinks))
	{
		o_attachBone = d_dynamicLink[i_link].d_control;
		o_attachPoint = d_dynamicLink[i_link].d_attachPoint;
		return(true);
	}
	return(false);
}

inline void CDynamicChain::SetAttachPoint(int i_link, ts_Bone* i_attachBone,
		Vector3CRef i_attachPoint)
{
	if ((i_link >= 0) && (i_link < d_numLinks))
	{
		d_dynamicLink[i_link].d_control = i_attachBone;
		d_dynamicLink[i_link].d_attachPoint = i_attachPoint;
	}
}

inline float CDynamicChain::GetLengthSquared(int i_link) const
{
	ASSERT((i_link > 0) && (i_link < d_numLinks));
	return (d_dynamicLink[i_link].d_lengthSquared);
}

inline float CDynamicChain::GetConstraintRadius(int i_link) const
{
	ASSERT((i_link > 0) && (i_link < d_numLinks));
	return (d_dynamicLink[i_link].d_constraintRadius);
}

inline Vector3 CDynamicChain::GetVelocityInWorld(int i_link) const
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	if (Math::DivByZero(1.0f, d_deltaSec))
		return (d_dynamicLink[i_link].d_velocityInWorld);
	return ((1.0f / d_deltaSec) * (d_link[i_link].d_newParticleInWorld -
			d_dynamicLink[i_link].d_particleInWorld));
}

inline void CDynamicChain::AddExternalForce(int i_link, Vector3 &i_force)
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	d_dynamicLink[i_link].d_externalForce += i_force;
}

inline void CDynamicChain::SetExternalForce(int i_link, Vector3CRef i_force)
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	d_dynamicLink[i_link].d_externalForce = i_force;
}

inline Vector3& CDynamicChain::GetExternalForce(int i_link)
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	return (d_dynamicLink[i_link].d_externalForce);
}

inline void CDynamicChain::ClearExternalForces(void)
{
	int i;
	for(i = 0; i < d_numLinks; i++)
		d_dynamicLink[i].d_externalForce.Clear();

	if (d_dynamicPartner)
		d_dynamicPartner->ClearExternalForces();
}

inline void CDynamicChain::SetParticleInWorld(int i_link,
		Vector3CRef i_particleInWorld)
{
	if ((i_link >= 0) && (i_link < d_numLinks) && !RigidAttach(i_link))
		d_link[i_link].d_newParticleInWorld = i_particleInWorld;
}

inline bool CDynamicChain::RigidAttach(int i_link)
{
	return (!(d_dynamicLink[i_link].d_controlMask & LINK_TARGET) &&
			(d_dynamicLink[i_link].d_controlMask & LINK_ATTACH));
}

inline bool CDynamicChain::TargetBase(int i_link)
{
	return ((d_dynamicLink[i_link].d_controlMask & LINK_TARGET) &&
			!(d_dynamicLink[i_link].d_controlMask & LINK_ATTACH));
}

inline bool CDynamicChain::ConstrainToObb(int i_link)
{
	return (((d_dynamicLink[i_link].d_controlMask & LINK_TARGET) ||
			!(d_dynamicLink[i_link].d_controlMask & LINK_ATTACH)) &&
			!(d_dynamicLink[i_link].d_controlMask & LINK_NO_OBB));
}

inline void CDynamicChain::EnforcePartnershipOBBConstraints(
		CObjectList* i_objectList)
{
	EnforceConstraints(i_objectList, 0);

	if (d_dynamicPartner)
		d_dynamicPartner->EnforcePartnershipOBBConstraints(i_objectList);
}

inline bool CDynamicChain::IsInOBB(int i_link, OBB &i_obb)
{
	if (!(d_dynamicLink[i_link].d_controlMask & LINK_TARGET) &&
			(d_dynamicLink[i_link].d_controlMask & LINK_ATTACH))
		return(false);

	Vector3 pointInOBB;
	Vector3 extent;
	ComputeConstraintInfo(&i_obb, d_link[i_link].d_newParticleInWorld,
			d_dynamicLink[i_link].d_constraintRadius, pointInOBB, extent);
	return (InsideOBB(pointInOBB, extent));
}

#endif // DYNAMIC_CHAIN_HPP
