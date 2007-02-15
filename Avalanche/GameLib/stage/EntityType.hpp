/*
**
**  File:   EntityType.hpp
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A hierarchal bitfield from which the class of an entity can be
**          deduced as well as its base classes. Essentially RTTI for
**          stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/EntityType.hpp $
**  $Revision: 3 $
**      $Date: 6/04/03 9:49a $
**    $Author: Bcollard $
**
*/

#ifdef ENTITY_TYPE_HPP
#undef ENTITY_TYPE_HPP

// Engine includes //
#include "Layers/Assert/Assert.h"

/***************************************************************************/
// Find the number of bits needed for the derived classes. (Define at the
// top so it will be inlined.)
/***************************************************************************/
inline uint CEntityType::DerivedWidth(int i_numDerived)
{

	uint width = 0;
	i_numDerived--;
	while (i_numDerived > 0)
	{
		width++;
		i_numDerived >>= 1;
	}
	return(width);
}

/***************************************************************************/
// Set the given number of least significant bits. (Define at the top so it
// will be inlined.)
/***************************************************************************/
inline uint CEntityType::LSBMask(uint i_width)
{
	uint mask = 0;
	for (uint i = 0; i < i_width; i++)
		mask = (mask << 1) | 1;
	return(mask);
}

/***************************************************************************/
// Construct the base type (only called by CStageEntity).
/***************************************************************************/
inline CEntityType::CEntityType(int i_numDerived)
{
	uint derivedWidth = DerivedWidth(i_numDerived);
	d_derivedShift = 32 - derivedWidth;

	d_field = 0;
	d_baseMask = 0;
	d_derivedMask = LSBMask(derivedWidth) << d_derivedShift;
}

/***************************************************************************/
// Construct a derived type.
/***************************************************************************/
inline CEntityType::CEntityType(const CEntityType &i_baseType,
		uint i_baseDerived, int i_numDerived)
{
	uint derivedWidth = DerivedWidth(i_numDerived);
	ASSERT(derivedWidth <= i_baseType.d_derivedShift);
	d_derivedShift = i_baseType.d_derivedShift - derivedWidth;

	ASSERT(((i_baseDerived << i_baseType.d_derivedShift) &
			~i_baseType.d_derivedMask) == 0);
	d_field = i_baseType.d_field | (i_baseDerived << i_baseType.d_derivedShift);
	d_baseMask = i_baseType.d_baseMask | i_baseType.d_derivedMask;
	d_derivedMask = LSBMask(derivedWidth) << d_derivedShift;
}

/***************************************************************************/
// Copy construct.
/***************************************************************************/
inline CEntityType::CEntityType(const CEntityType &i_src)
{
	d_field = i_src.d_field;
	d_baseMask = i_src.d_baseMask;
	d_derivedMask = i_src.d_derivedMask;
	d_derivedShift = i_src.d_derivedShift;
}

/***************************************************************************/
// See if this type is the passed type or derived from it.
/***************************************************************************/
inline bool CEntityType::IsA(const CEntityType &i_type) const
{
	return((d_field & i_type.d_baseMask) == i_type.d_field);
}

/***************************************************************************/
// See if this type is the passed type.
/***************************************************************************/
inline bool CEntityType::IsABase(const CEntityType &i_type) const
{
	return(((d_field & i_type.d_baseMask) == i_type.d_field) &&
			((d_field & i_type.d_derivedMask) == 0));
}

/***************************************************************************/
// Determine which of the possible types derived from this type the passed
// type is. NOT_DERIVED is returned if this is not derived from the passed type.
/***************************************************************************/
inline uint32 CEntityType::GetDerived(const CEntityType &i_type) const
{
	if ((i_type.d_field & d_baseMask) != d_field)
		return(NOT_DERIVED);

	return((i_type.d_field & d_derivedMask) >> d_derivedShift);
}

#endif // ENTITY_TYPE_HPP
