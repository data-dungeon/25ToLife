/*
**
**  File:   EntityType.h
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A hierarchal bitfield from which the class of an entity can be
**          deduced as well as its base classes. Essentially RTTI for
**          stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/EntityType.h $
**  $Revision: 3 $
**      $Date: 6/04/03 9:49a $
**    $Author: Bcollard $
**
*/

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

// GameLib includes //
#include "platform/BaseType.h"

class CStageEntity;

class CEntityType
{
  public:
	// Construct a derived type.
	CEntityType(const CEntityType &i_baseType, uint i_baseDerived,
			int i_numDerived);

	// Copy construct.
	CEntityType(const CEntityType &i_src);

	// See if this type is the passed type or derived from it.
	bool IsA(const CEntityType &i_type) const;

	// See if this type is the passed type.
	bool IsABase(const CEntityType &i_type) const;

	// Determine which of the possible types derived from this type the passed
	// type is. NOT_DERIVED is returned if this is not derived from the passed
	// type.
	uint32 GetDerived(const CEntityType &i_type) const;
	static const uint32 NOT_DERIVED;

  private:
	// Prevent default construction.
	CEntityType();

	// Construct the base type (only called by CStageEntity).
	CEntityType(int i_numDerived);

	// Find the number of bits needed for the derived classes.
	uint DerivedWidth(int i_numDerived);

	// Set the given number of least significant bits.
	uint LSBMask(uint i_width);

	// The type bitfield.
	uint32 d_field;

	// A mask with the MSB's including the type set.
	uint32 d_baseMask;

	// A mask with the just the bits for the derived classes set.
	uint32 d_derivedMask;

	// The number of bits to the right of the derived classes.
	uint d_derivedShift;

	// So it can call CEntityType(int i_numDerived).
	friend class CStageEntity;
};

#define ENTITY_TYPE_HPP
#include "stage/EntityType.hpp"

#endif // ENTITY_TYPE_H
