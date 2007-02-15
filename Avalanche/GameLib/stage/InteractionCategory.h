/*
**
**  File:   InteractionCategory.h
**  Date:   March 31, 2003
**  By:     Bryant Collard
**  Desc:   A bitfield where each bit represents a category used to control
**          the interaction of two entities. This is wrapped in a class
**          instead of just being an unsigned int so it can be expanded.
**          In fact, the width of the field should never be considered as
**          known.
**
**          Most of the functions have versions that use an unsigned integer
**          as an argument. This is so categories can be specified in tables,
**          used in scripts, etc. These functions operate on the "dominant"
**          part of the bitfield (that's everything for now).
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/InteractionCategory.h $
**  $Revision: 3 $
**      $Date: 6/25/03 10:26a $
**    $Author: Bcollard $
**
*/

#ifndef INTERACTION_CATEGORY_H
#define INTERACTION_CATEGORY_H

// GameLib includes //
#include "platform/BaseType.h"

class CInteractionCategory;

// Comparison
bool operator==(uint32 i_field, const CInteractionCategory &i_category);
bool operator!=(uint32 i_field, const CInteractionCategory &i_category);

// Bitwise operators
CInteractionCategory operator&(uint32 i_field,
		const CInteractionCategory &i_category);
CInteractionCategory operator|(uint32 i_field,
		const CInteractionCategory &i_category);

class CInteractionCategory
{
  public:
	// Construction
	CInteractionCategory();
	CInteractionCategory(const CInteractionCategory &i_category);
	CInteractionCategory(uint32 i_field);
	CInteractionCategory(uint32 i_field, uint i_shift);

	// Setup
	void Set(const CInteractionCategory &i_category);
	void Set(uint32 i_field);
	void Set(uint32 i_field, uint i_shift);

	// Assignment
	CInteractionCategory &operator=(const CInteractionCategory &i_category);
	CInteractionCategory &operator=(uint32 i_field);

	// Comparison
	bool operator==(const CInteractionCategory &i_category) const;
	bool operator==(uint32 i_field) const;
	bool operator!=(const CInteractionCategory &i_category) const;
	bool operator!=(uint32 i_field) const;

	// Bitwise operators
	CInteractionCategory operator~() const;
	CInteractionCategory operator&(const CInteractionCategory &i_category) const;
	CInteractionCategory operator&(uint32 i_field) const;
	CInteractionCategory operator|(const CInteractionCategory &i_category) const;
	CInteractionCategory operator|(uint32 i_field) const;

	// Bitwise assignment
	CInteractionCategory &operator&=(const CInteractionCategory &i_category);
	CInteractionCategory &operator&=(uint32 i_field);
	CInteractionCategory &operator|=(const CInteractionCategory &i_category);
	CInteractionCategory &operator|=(uint32 i_field);

	// Category matches
	bool PositiveMatch(const CInteractionCategory &i_category) const;
	bool PositiveMatch(uint32 i_field) const;
	bool NegativeMatch(const CInteractionCategory &i_category) const;
	bool NegativeMatch(uint32 i_field) const;

	// Hack
	uint32* Get(void) {return(&d_field);}

  private:
	uint32 d_field;

	friend bool operator==(uint32 i_field,
			const CInteractionCategory &i_category);
	friend bool operator!=(uint32 i_field,
			const CInteractionCategory &i_category);
	friend CInteractionCategory operator&(uint32 i_field,
			const CInteractionCategory &i_category);
	friend CInteractionCategory operator|(uint32 i_field,
			const CInteractionCategory &i_category);
};

#define INTERACTION_CATEGORY_HPP
#include "stage/InteractionCategory.hpp"

#endif // INTERACTION_CATEGORY_H
