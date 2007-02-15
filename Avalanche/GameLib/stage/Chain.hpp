/*
**
**  File:   Chain.hpp
**  Date:   December 17, 2002
**  By:     Bryant Collard
**  Desc:   Manages chained bones in a model.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/Chain.hpp $
**  $Revision: 4 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef CHAIN_HPP
#undef CHAIN_HPP

inline void CChain::SetReferenceOrientation(int i_link,
		const DirCos3x3 &i_refOrientation)
{
	if ((i_link > 0) && (i_link < d_numLinks))
		d_link[i_link].d_refLinkToParent = i_refOrientation;
}

inline ts_Bone* CChain::GetBone(int i_link) const
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	return (d_link[i_link].d_bone);
}

inline Vector3& CChain::GetParticleInWorld(int i_link) const
{
	ASSERT((i_link >= 0) && (i_link < d_numLinks));
	return (d_link[i_link].d_newParticleInWorld);
}

inline void CChain::UpdateExtents(Vector3 &i_point, Vector3 &io_min,
		Vector3 &io_max)
{
	io_min = Vector3::Min(i_point, io_min);
	io_max = Vector3::Max(i_point, io_max);
}

inline void CChain::UpdateExtents()
{
	d_minExtent = d_link[0].d_newParticleInWorld;
	d_maxExtent = d_link[0].d_newParticleInWorld;
	for (int i = 1; i < d_numLinks; i++)
		UpdateExtents(d_link[i].d_newParticleInWorld, d_minExtent, d_maxExtent);
}

inline void CChain::GetPartnershipExtents (Vector3 &io_minExtent,
		Vector3 &io_maxExtent, bool i_compare)
{
	if (d_partner)
	{
		d_partner->GetPartnershipExtents(io_minExtent, io_maxExtent,
				i_compare);
		i_compare = true;
	}

	if (i_compare)
	{
		io_minExtent = Vector3::Min(io_minExtent, d_minExtent);
		io_maxExtent = Vector3::Max(io_maxExtent, d_maxExtent);
	}
	else
	{
		io_minExtent = d_minExtent;
		io_maxExtent = d_maxExtent;
	}
}

#endif // CHAIN_HPP
